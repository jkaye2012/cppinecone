#pragma once
/**
 * @file pinecone.hpp
 * @brief The Pinecone API client (start here!)
 * @example metadata_filtering.hpp
 * @example main.cpp
 * @example CMakeLists.txt
 */

#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "pinecone/domain/index_operations.hpp"
#include "pinecone/domain/meta_operations.hpp"
#include "pinecone/domain/operation.hpp"
#include "pinecone/domain/operation_type.hpp"
#include "pinecone/domain/vector_operations.hpp"
#include "pinecone/net/arguments.hpp"
#include "pinecone/net/http_client.hpp"
#include "pinecone/net/url_builder.hpp"
#include "pinecone/types/accepted.hpp"
#include "pinecone/types/api_metadata.hpp"
#include "pinecone/types/filters.hpp"
#include "pinecone/types/index_types.hpp"
#include "pinecone/types/vector_metadata.hpp"
#include "pinecone/types/vector_types.hpp"
#include "pinecone/util/logging.hpp"
#include "pinecone/util/result.hpp"

namespace pinecone
{
using type = domain::operation_type;
template <type t>
using args = domain::operation_args<t>;
template <type t, typename f>
using vec_args = domain::operation_args<t, f>;

/**
 * @brief The Pinecone REST API client.
 *
 * @tparam Mode the threading mode for the client
 *
 * @details All functions on this class mirror Pinecone API operations directly; as such,
 * function-level documentation is provided only when Cppinecone functionality somehow diverges
 * from Pinecone's provided API functionality.
 *
 * Most API operations that require specifically-formatted inputs follow the Builder pattern to
 * easily support optional fields in each message. See the documentation for the @p builder
 * class contained within each custom type for details on how to construct instances of the type.
 * The values accepted by these builders follow the semantics of the Pinecone API as described
 * in the documentation linked below.
 *
 * All operations that accept a @p filter argument make use of the vector metadata filtering API.
 * The API supports an arbitrary number of filters via filter combination. See the linked
 * documentation below for more details.
 *
 * @see https://docs.pinecone.io/docs/overview
 * @see filters.hpp
 */
template <net::threading_mode Mode>
struct pinecone_client {
  pinecone_client() = default;
  /**
   * @brief Attempts to initialize a new Pinecone API client.
   *
   * @param args arguments to initialize the client connection
   * @returns a Pinecone client instance initialized according to the provided arguments, or an
   * error message if client construction fails
   */
  static auto build(net::connection_args args) -> std::variant<pinecone_client<Mode>, std::string>
  {
    net::url_builder url_builder(args.environment());
    auto client = net::http_client<Mode>::build(std::move(args));
    if (client) {
      auto api_metadata = client->request(
          domain::operation_args<domain::operation_type::actions_whoami>(url_builder));
      if (api_metadata.is_failed()) {
        std::ostringstream err;
        err << "Failed to construct Pinecone client due to API metadata retrieval failure: "
            << api_metadata.to_string() << std::endl;
        return {err.str()};
      }
      url_builder.set_metadata(std::move(*api_metadata));
      return pinecone_client(std::move(url_builder), std::move(client));
    }

    return {"Failed to construct HTTP client; CURL seems to be somehow misconfigured"};
  }

  [[nodiscard]] auto get_api_metdata() const noexcept -> util::result<types::api_metadata>
  {
    util::logger()->info("Retrieving API metadata");
    return _http_client->request(args<type::actions_whoami>{_url_builder});
  }

  [[nodiscard]] auto create_index(types::new_index index) const noexcept
      -> util::result<types::accepted>
  {
    util::logger()->info("Creating index");
    return _http_client->request(args<type::index_create>{_url_builder, std::move(index)});
  }

  [[nodiscard]] auto list_indexes() const noexcept -> util::result<std::vector<std::string>>
  {
    util::logger()->info("Listing indices");
    return _http_client->request(args<type::index_list>{_url_builder});
  }

  [[nodiscard]] auto describe_index(std::string_view name) const noexcept
      -> util::result<types::database>
  {
    util::logger()->info("Describing index");
    return _http_client->request(args<type::index_describe>{_url_builder, name});
  }

  [[nodiscard]] auto configure_index(std::string_view name,
                                     types::index_configuration config) const noexcept
      -> util::result<types::accepted>
  {
    util::logger()->info("Configuring index");
    return _http_client->request(args<type::index_configure>(_url_builder, name, config));
  }

  [[nodiscard]] auto delete_index(std::string_view name) const noexcept
      -> util::result<types::accepted>
  {
    util::logger()->info("Deleting index");
    return _http_client->request(args<type::index_delete>{_url_builder, name});
  }

  [[nodiscard]] auto list_collections() const noexcept -> util::result<std::vector<std::string>>
  {
    util::logger()->info("Listing collections");
    return _http_client->request(args<type::collection_list>{_url_builder});
  }

  [[nodiscard]] auto describe_collection(std::string_view name) const noexcept
      -> util::result<types::collection>
  {
    util::logger()->info("Describing collection");
    return _http_client->request(args<type::collection_describe>{_url_builder, name});
  }

  [[nodiscard]] auto delete_collection(std::string_view name) const noexcept
      -> util::result<types::accepted>
  {
    util::logger()->info("Deleting collection");
    return _http_client->request(args<type::collection_delete>{_url_builder, name});
  }

  [[nodiscard]] auto create_collection(types::new_collection collection) const noexcept
      -> util::result<types::accepted>
  {
    util::logger()->info("Creating collection");
    return _http_client->request(
        args<type::collection_create>(_url_builder, std::move(collection)));
  }

  [[nodiscard]] auto describe_index_stats(std::string_view name) const noexcept
      -> util::result<types::index_stats>
  {
    util::logger()->info("Descriving index stats");
    return _http_client->request(vec_args<type::vector_describe_index_stats, types::no_filter>{
        _url_builder, name, types::filters::none()});
  }

  template <typename filter>
  [[nodiscard]] auto query(std::string_view name, types::query<filter> query) const noexcept
      -> util::result<types::query_result>
  {
    util::logger()->info("Querying index");
    return _http_client->request(
        vec_args<type::vector_query, filter>{_url_builder, name, std::move(query)});
  }

  template <typename filter>
  [[nodiscard]] auto delete_vectors(std::string_view name,
                                    types::delete_request<filter> req) const noexcept
      -> util::result<types::accepted>
  {
    util::logger()->info("Deleting vectors");
    return _http_client->request(
        vec_args<type::vector_delete, filter>{_url_builder, name, std::move(req)});
  }

  [[nodiscard]] auto upsert_vectors(std::string_view name, types::upsert_request req) const noexcept
      -> util::result<types::accepted>
  {
    util::logger()->info("Upserting vectors");
    return _http_client->request(args<type::vector_upsert>{_url_builder, name, std::move(req)});
  }

  [[nodiscard]] auto update_vector(std::string_view name, types::update_request req) const noexcept
      -> util::result<types::accepted>
  {
    util::logger()->info("Updating vectors");
    return _http_client->request(args<type::vector_update>{_url_builder, name, std::move(req)});
  }

 private:
  net::url_builder _url_builder;
  std::unique_ptr<net::http_client<Mode>> _http_client;

  pinecone_client(net::url_builder url_builder,
                  std::unique_ptr<net::http_client<Mode>> client) noexcept
      : _url_builder(std::move(url_builder)), _http_client(std::move(client))
  {
    util::logger()->info("Client construction completed successfully");
  }

  template <typename filter>
  [[nodiscard]] auto describe_index_stats(std::string_view name, filter f) const noexcept
      -> util::result<types::index_stats>
  {
    return _http_client->request(
        vec_args<type::vector_describe_index_stats, filter>{_url_builder, name, std::move(f)});
  }
};

using synchronous_client = pinecone_client<net::threading_mode::sync>;
}  // namespace pinecone
