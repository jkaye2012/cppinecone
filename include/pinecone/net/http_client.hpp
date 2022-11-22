#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include <curl/curl.h>
#include <curl/easy.h>
#include <nlohmann/json.hpp>

#include "pinecone/domain/method.hpp"
#include "pinecone/domain/operation.hpp"
#include "pinecone/domain/operation_type.hpp"
#include "pinecone/net/arguments.hpp"
#include "pinecone/util/result.hpp"

using json = nlohmann::json;

namespace pinecone::net
{
static constexpr size_t kInitialDataSize = 1024;
static constexpr int64_t kHttpOk = 200;
static constexpr int64_t kHttpCreated = 201;
static constexpr int64_t kHttpAccepted = 202;

/**
 * @brief An HTTP client responsible for making requests to a Pinecone
 * instance.
 */
template <threading_mode Mode>
struct http_client;

static constexpr auto kContentType = "Content-Type: application/json; charset=utf-8";

template <>
struct http_client<threading_mode::sync> {
  static auto build(connection_args args) noexcept -> std::unique_ptr<http_client>
  {
    auto* curl_handle = curl_easy_init();
    curl_slist* headers{};
    headers = curl_slist_append(headers, kContentType);
    headers = curl_slist_append(headers, args.api_key_header());
    if (headers == nullptr) {
      return nullptr;
    }
    return curl_handle != nullptr ? std::unique_ptr<http_client>(new http_client(
                                        std::move(args), curl_handle, headers))  // NOLINT
                                  : nullptr;
  }

  ~http_client() noexcept
  {
    curl_easy_cleanup(_curl_handle);
    curl_slist_free_all(_headers);
  }

  http_client(http_client const&) = delete;
  auto operator=(http_client const&) -> http_client& = delete;
  http_client(http_client&&) noexcept = default;
  auto operator=(http_client&&) noexcept -> http_client& = default;

  template <domain::operation_type Op, typename Dep = bool>
  auto request(domain::operation_args<Op, Dep> op_args) const noexcept
      -> util::result<typename domain::operation_args<Op, Dep>::parsed_type>
  {
    domain::operation<Op, Dep> operation(std::move(op_args));
    _data.clear();
    auto result =
        operation
            .set_opts(_curl_handle, _headers)
            // .and_then([this]() { return curl_easy_setopt(_curl_handle, CURLOPT_VERBOSE, 1L); })
            // // TODO: dynamically enable?
            .and_then([this]() {
              return curl_easy_setopt(_curl_handle, CURLOPT_WRITEFUNCTION, http_client::read);
            })
            .and_then([this]() { return curl_easy_setopt(_curl_handle, CURLOPT_WRITEDATA, this); })
            .and_then([this]() { return curl_easy_perform(_curl_handle); });
    if (result.is_error()) {
      return {result.error()};
    }

    int64_t http_code = 0;
    curl_easy_getinfo(_curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
    switch (http_code) {
      case kHttpOk:
      case kHttpCreated:
      case kHttpAccepted:
        try {
          return op_args.parse(_data);
        } catch (json::exception& ex) {
          return {std::move(ex)};
        }
      default:  // TODO: parse errors when possible
        return {http_code, std::string(_data.begin(), _data.end())};
    }
  }

 private:
  auto receive_chunk(void* buffer, size_t n) noexcept -> size_t
  {
    auto* bytes = static_cast<uint8_t*>(buffer);
    for (size_t i = 0; i < n; ++i) {
      _data.emplace_back(*(bytes + i));  // NOLINT
    }

    return n;
  }

 public:
  static auto read(void* buffer, size_t sz, size_t n, void* f) noexcept -> size_t
  {
    return static_cast<http_client*>(f)->receive_chunk(buffer, n * sz);
  }

 private:
  connection_args _args;
  CURL* _curl_handle;
  curl_slist* _headers;
  mutable std::vector<uint8_t> _data;

  http_client(connection_args args, CURL* curl_handle, curl_slist* headers) noexcept
      : _args(std::move(args)), _curl_handle(curl_handle), _headers(headers)
  {
    assert(_curl_handle != nullptr);
    _data.reserve(kInitialDataSize);
  }
};
}  // namespace pinecone::net
