#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>

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
#include "pinecone/types/index_types.hpp"
#include "pinecone/types/vector_metadata.hpp"
#include "pinecone/types/vector_types.hpp"
#include "pinecone/util/result.hpp"

namespace pinecone
{
using type = domain::operation_type;
template <type t>
using args = domain::operation_args<t>;

/**
 * @brief The Pinecone REST API client.
 */
template <net::threading_mode Mode>
struct pinecone_client {
  static auto build(net::connection_args args) -> std::optional<pinecone_client<Mode>>
  {
    net::url_builder url_builder(args.environment());
    auto client = net::http_client<Mode>::build(std::move(args));
    if (client) {
      auto api_metadata = client->request(
          domain::operation_args<domain::operation_type::actions_whoami>(url_builder));
      if (api_metadata.is_failed()) {
        return std::nullopt;
      }
      url_builder.set_metadata(std::move(*api_metadata));
      return pinecone_client(std::move(url_builder), std::move(client));
    }

    return std::nullopt;
  }

  [[nodiscard]] auto get_api_metdata() const noexcept -> util::result<types::api_metadata>
  {
    return _http_client->request(args<type::actions_whoami>{_url_builder});
  }

  [[nodiscard]] auto create_index(types::new_index index) const noexcept
      -> util::result<types::accepted>
  {
    return _http_client->request(args<type::index_create>{_url_builder, std::move(index)});
  }

  [[nodiscard]] auto list_indexes() const noexcept -> util::result<types::indexes>
  {
    return _http_client->request(args<type::index_list>{_url_builder});
  }

  [[nodiscard]] auto describe_index(std::string_view name) const noexcept
      -> util::result<types::database>
  {
    return _http_client->request(args<type::index_describe>{_url_builder, name});
  }

  [[nodiscard]] auto configure_index(std::string_view name,
                                     types::index_configuration config) const noexcept
      -> util::result<types::accepted>
  {
    return _http_client->request(
        args<type::index_configure>(_url_builder, name, std::move(config)));
  }

  [[nodiscard]] auto delete_index(std::string_view name) const noexcept
      -> util::result<types::accepted>
  {
    return _http_client->request(args<type::index_delete>{_url_builder, name});
  }

  [[nodiscard]] auto list_collections() const noexcept -> util::result<types::collections>
  {
    return _http_client->request(args<type::collection_list>{_url_builder});
  }

  [[nodiscard]] auto describe_collection(std::string_view name) const noexcept
      -> util::result<types::collection>
  {
    return _http_client->request(args<type::collection_describe>{_url_builder, name});
  }

  [[nodiscard]] auto delete_collection(std::string_view name) const noexcept
      -> util::result<types::accepted>
  {
    return _http_client->request(args<type::collection_delete>{_url_builder, name});
  }

  [[nodiscard]] auto create_collection(types::new_collection collection) const noexcept
      -> util::result<types::accepted>
  {
    return _http_client->request(
        args<type::collection_create>(_url_builder, std::move(collection)));
  }

  template <typename filter>
  [[nodiscard]] auto describe_index_stats(std::string_view name, filter f) const noexcept
  {
    return _http_client->request(domain::operation_args<type::vector_describe_index_stats, filter>{
        _url_builder, name, std::move(f)});
  }

 private:
  net::url_builder _url_builder;
  std::unique_ptr<net::http_client<Mode>> _http_client;

  pinecone_client(net::url_builder url_builder,
                  std::unique_ptr<net::http_client<Mode>> client) noexcept
      : _url_builder(std::move(url_builder)), _http_client(std::move(client))
  {
  }
};

using synchronous_client = pinecone_client<net::threading_mode::sync>;
}  // namespace pinecone
