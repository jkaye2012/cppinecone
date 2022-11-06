#pragma once

#include <sstream>

#include <curl/curl.h>
#include <curl/easy.h>

#include "pinecone/domain/curl_result.hpp"
#include "pinecone/domain/method.hpp"

namespace pinecone::domain
{
/**
 * @brief All operation_types exposed by the Pinecone REST API.
 *
 * @details
 * Operation_Types can be understood within three general categories:
 *
 * - Index operation_types
 * - Collection operation_types
 * - Vector operation_types
 */
enum class operation_type {
  index_create,
  index_configure,
  index_list,
  index_describe,
  index_delete,

  collection_create,
  collection_list,
  collection_describe,
  collection_delete,

  vector_upsert,
  vector_update,
  vector_query,
  vector_fetch,
  vector_describe_all_index_stats,
  vector_describe_filtered_index_stats,
  vector_delete
};

constexpr auto op_url_fragment(operation_type op) -> char const*
{
  switch (op) {
    case operation_type::index_create:
    case operation_type::index_list:
      return "/databases";
    case operation_type::index_configure:
    case operation_type::index_describe:
    case operation_type::index_delete:
      return "/databases/";
    case operation_type::collection_create:
    case operation_type::collection_list:
      return "/collections";
    case operation_type::collection_describe:
    case operation_type::collection_delete:
      return "/collections/";
    case operation_type::vector_upsert:
      return "/vectors/upsert";
    case operation_type::vector_update:
      return "/vectors/update";
    case operation_type::vector_query:
      return "/query";
    case operation_type::vector_fetch:
      return "/vectors/fetch";
    case operation_type::vector_describe_all_index_stats:
    case operation_type::vector_describe_filtered_index_stats:
      return "/describe_index_stats";
    case operation_type::vector_delete:
      return "/vectors/delete";
  }
}

constexpr auto op_method(operation_type op) -> method
{
  switch (op) {
    case operation_type::index_create:
    case operation_type::collection_create:
    case operation_type::vector_upsert:
    case operation_type::vector_update:
    case operation_type::vector_query:
    case operation_type::vector_describe_filtered_index_stats:
    case operation_type::vector_delete:
      return method::post;
    case operation_type::index_list:
    case operation_type::index_describe:
    case operation_type::collection_list:
    case operation_type::collection_describe:
    case operation_type::vector_fetch:
    case operation_type::vector_describe_all_index_stats:
      return method::get;
    case operation_type::index_configure:
      return method::patch;
    case operation_type::index_delete:
    case operation_type::collection_delete:
      return method::del;
  }
}

struct arg_base {
  explicit arg_base(std::string url) : _url(std::move(url)) {}

  [[nodiscard]] constexpr auto url() const noexcept -> char const* { return _url.c_str(); }

 private:
  std::string _url;
};

template <operation_type>
struct operation_args : public arg_base {
  explicit operation_args(std::string_view url_prefix) noexcept : arg_base(build_url(url_prefix)) {}

  [[nodiscard]] static auto build_url(std::string_view url_prefix) noexcept -> std::string
  {
    return std::string(url_prefix);
  }
  // NOLINTNEXTLINE
  constexpr auto set_opts(CURL* curl, curl_slist* headers) noexcept -> domain::curl_result
  {
    return {};
  }
};

template <>
struct operation_args<operation_type::index_describe> : public arg_base {
  operation_args(std::string_view url_prefix, std::string_view index_name) noexcept
      : arg_base(build_url(url_prefix, index_name)), _index_name(index_name)
  {
  }

  [[nodiscard]] static auto build_url(std::string_view url_prefix,
                                      std::string_view index_name) noexcept -> std::string
  {
    std::ostringstream oss;
    oss << url_prefix << index_name;
    return oss.str();
  }

  // NOLINTNEXTLINE
  constexpr auto set_opts(CURL* curl, curl_slist* headers) noexcept -> domain::curl_result
  {
    return {};
  }

 private:
  std::string_view _index_name;
};

static constexpr auto kContentType = "Content-Type: application/json; charset=utf-8";

/**
 * @brief Data common to all Pinecone API operation types.
 */
template <operation_type Op>
struct operation {
  operation(operation_args<Op> args, std::string api_key) noexcept
      : _args(std::move(args)), _api_key(std::move(api_key)), _method(op_method(op_type))
  {
  }

  static constexpr auto op_type = Op;

  ~operation() noexcept { curl_slist_free_all(_headers); }
  operation(operation const&) = delete;
  auto operator=(operation const&) = delete;
  operation(operation&&) noexcept = delete;
  auto operator=(operation&&) noexcept = delete;

  [[nodiscard]] constexpr auto method() const noexcept { return _method; }
  [[nodiscard]] constexpr auto api_key() const noexcept { return _api_key; }

  constexpr auto set_opts(CURL* curl) noexcept -> domain::curl_result
  {
    curl_easy_reset(curl);
    return domain::curl_result(curl_easy_setopt(curl, CURLOPT_URL, _args.url()))
        .and_then([this]() { return set_header_value(_api_key.c_str()); })
        .and_then([this]() { return set_header_value(kContentType); })
        .and_then([this, curl]() { return _args.set_opts(curl, _headers); })
        .and_then([this, curl]() { return curl_easy_setopt(curl, CURLOPT_HTTPHEADER, _headers); });
  }

 private:
  operation_args<Op> _args;
  std::string _api_key;
  domain::method _method;

  curl_slist* _headers{};

  constexpr auto set_header_value(char const* header) noexcept -> curl_result
  {
    _headers = curl_slist_append(_headers, header);
    return _headers;
  }
};
}  // namespace pinecone::domain
