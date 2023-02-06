#pragma once
/**
 * @file operation.hpp
 * @brief Metaprogamming infrastructure for generation of per-operation code
 */

#include <sstream>
#include <string>
#include <variant>

#include <curl/curl.h>
#include <curl/easy.h>

#include "pinecone/domain/method.hpp"
#include "pinecone/domain/operation_type.hpp"
#include "pinecone/net/url_builder.hpp"
#include "pinecone/util/curl_result.hpp"

namespace pinecone::domain
{
/**
 * @brief All operations require a common subset of information; this information forms the base
 * arguments.
 */
struct arg_base {
  explicit arg_base(std::string url) : _url(std::move(url)) {}
  virtual ~arg_base() = default;
  arg_base(arg_base const&) = default;
  arg_base(arg_base&&) noexcept = default;
  auto operator=(arg_base const&) -> arg_base& = default;
  auto operator=(arg_base&&) noexcept -> arg_base& = default;

  [[nodiscard]] constexpr auto url() const noexcept -> char const* { return _url.c_str(); }

 private:
  std::string _url;
};

/**
 * @brief List operations are simple and construct themselves using only a URL.
 * @tparam op the operation to specialize as a list operation
 */
template <operation_type op>
struct list_operation_args : public arg_base {
  explicit list_operation_args(net::url_builder const& url_builder) noexcept
      : arg_base(url_builder.build<op>())
  {
  }
};

/**
 * @brief Constructs a URL for an operation that represents a unary resource.
 *
 * @tparam op the operation
 * @param url_builder a url_builder instance
 * @param resource_name the name of the resource that @p op will run on
 * @return the URL identifying the resource
 */
template <operation_type op>
[[nodiscard]] inline auto build_url(net::url_builder const& url_builder,
                                    std::string_view resource_name) noexcept -> std::string
{
  if constexpr (op_api_type(op) == api_type::service) {
    return url_builder.build<op>(resource_name);
  } else if constexpr (op_api_type(op) == api_type::controller) {
    std::ostringstream oss;
    oss << url_builder.build<op>() << resource_name;
    return oss.str();
  }
}

/**
 * @brief Describe operations construct themselves using a unique resource name.
 * @tparam op the operation to specialize as a describe operation
 */
template <operation_type op>
struct describe_delete_operation_args : public arg_base {
  describe_delete_operation_args(net::url_builder const& url_builder,
                                 std::string_view resource_name) noexcept
      : arg_base(build_url<op>(url_builder, resource_name))
  {
  }
};

/**
 * @brief Patch operations construct themselves using a unique resource name and an
 * operation-dependent body payload.
 *
 * @tparam op the operation to specialize as a patch operation
 * @tparam Body the operation-dependent payload type
 */
template <operation_type op, typename Body>
struct patch_operation_args : public arg_base {
  patch_operation_args(net::url_builder const& url_builder, std::string_view resource_name,
                       Body body) noexcept
      : arg_base(build_url<op>(url_builder, resource_name))
  {
    json j;
    to_json(j, body);
    _body = j.dump();
  }

  /**
   * @return the json-formatted body payload
   */
  [[nodiscard]] auto body() noexcept -> char const* { return _body.c_str(); }

 private:
  std::string _body;
};

/**
 * @brief Create operations construct themselves using a unique resource name and an
 * operation-dependent body payload.
 *
 * @tparam op the operation to specialize as a create operation
 * @tparam Body the operation-dependent payload type
 */
template <operation_type op, typename Body>
struct create_operation_args : public arg_base {
  create_operation_args(net::url_builder const& url_builder, Body body) noexcept
      : arg_base(url_builder.build<op>())
  {
    json j;
    to_json(j, body);
    _body = j.dump();
  }

  /**
   * @return the json-formatted body payload
   */
  [[nodiscard]] auto body() noexcept -> char const* { return _body.c_str(); }

 private:
  std::string _body;
};

/**
 * @brief Vector operations construct themselves using a unique resource name and an
 * operation-dependent body payload.
 *
 * @tparam op the operation to specialize as a vector operation
 * @tparam Body the operation-dependent payload type
 */
template <operation_type op, typename Body>
struct vector_operation_args : public arg_base {
  vector_operation_args(net::url_builder const& url_builder, std::string_view resource_name,
                        Body body) noexcept
      : arg_base(build_url<op>(url_builder, resource_name))
  {
    json j;
    to_json(j, body);
    _body = j.dump();
  }

  /**
   * @return the json-formatted body payload
   */
  [[nodiscard]] auto body() noexcept -> char const* { return _body.c_str(); }

 private:
  std::string _body;
};

/**
 * @brief Operation-specific arguments.
 *
 * @tparam operation_type the operation type
 * @tparam Body when relevant, the expected response type of the operation
 */
template <operation_type, typename Body = std::monostate>
struct operation_args;

static constexpr auto kDelete = "DELETE";
static constexpr auto kPatch = "PATCH";

/**
 * @brief Data common to all Pinecone API operation types.
 */
template <operation_type Op, typename Dep = std::monostate>
struct operation {
  explicit operation(operation_args<Op, Dep> args) noexcept
      : _args(std::move(args)), _method(op_method(op_type))
  {
  }

  /**
   * @brief The type of the operation.
   */
  static constexpr auto op_type = Op;

  /**
   * @return the HTTP method associated with the operation
   */
  [[nodiscard]] constexpr auto method() const noexcept { return _method; }

  /**
   * @brief Set CURL options for the operation.
   *
   * @param curl the initialized curl object
   * @param headers headers common to all API operations
   * @return the result
   */
  constexpr auto set_opts(CURL* curl, curl_slist* headers) noexcept -> util::curl_result
  {
    curl_easy_reset(curl);

    return set_method_opts(curl)
        .and_then([this, curl]() { return curl_easy_setopt(curl, CURLOPT_URL, _args.url()); })
        .and_then([this, curl, headers]() {
          return curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        });
  }

 private:
  constexpr auto set_method_opts(CURL* curl) noexcept -> util::curl_result
  {
    if constexpr (op_method(op_type) == method::del) {
      return {curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, kDelete)};
    }
    if constexpr (op_method(op_type) == method::patch) {
      return util::curl_result(curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, kPatch))
          .and_then(
              [this, curl]() { return curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _args.body()); });
    }
    if constexpr (op_method(op_type) == method::post) {
      return util::curl_result(curl_easy_setopt(curl, CURLOPT_POST, 1L)).and_then([this, curl]() {
        return curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _args.body());
      });
    }

    return {};
  }

  operation_args<Op, Dep> _args;
  domain::method _method;
};
}  // namespace pinecone::domain
