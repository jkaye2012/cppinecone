#pragma once

#include <memory>
#include <optional>
#include <string>

#include "pinecone/domain/index_operations.hpp"
#include "pinecone/domain/operation.hpp"
#include "pinecone/domain/operation_type.hpp"
#include "pinecone/net/http_client.hpp"
#include "pinecone/net/url_builder.hpp"
#include "pinecone/result.hpp"
#include "pinecone/types/accepted.hpp"
#include "pinecone/types/index_types.hpp"

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
      return pinecone_client(std::move(url_builder), std::move(client));
    }

    return std::nullopt;
  }

  [[nodiscard]] auto list_indexes() const noexcept -> result<types::indexes>
  {
    return _http_client->request(args<type::index_list>{_url_builder});
  }

  // TODO: pass all strings by value
  [[nodiscard]] auto describe_index(std::string const& name) const noexcept
      -> result<types::database>
  {
    return _http_client->request(args<type::index_describe>{_url_builder, name});
  }

  [[nodiscard]] auto configure_index(std::string const& name,
                                     types::index_configuration config) const noexcept
      -> result<types::accepted>
  {
    return _http_client->request(
        args<type::index_configure>(_url_builder, name, std::move(config)));
  }

  [[nodiscard]] auto delete_index(std::string const& name) const noexcept -> result<types::accepted>
  {
    return _http_client->request(args<type::index_delete>{_url_builder, name});
  }

  [[nodiscard]] auto list_collections() const noexcept -> result<types::collections>
  {
    return _http_client->request(args<type::collection_list>{_url_builder});
  }

  [[nodiscard]] auto describe_collection(std::string const& name) const noexcept
      -> result<types::collection>
  {
    return _http_client->request(args<type::collection_describe>{_url_builder, name});
  }

  [[nodiscard]] auto delete_collection(std::string const& name) const noexcept
      -> result<types::accepted>
  {
    return _http_client->request(args<type::collection_delete>{_url_builder, name});
  }

 private:
  mutable net::url_builder _url_builder;
  mutable std::unique_ptr<net::http_client<Mode>> _http_client;

  pinecone_client(net::url_builder url_builder,
                  std::unique_ptr<net::http_client<Mode>> client) noexcept
      : _url_builder(std::move(url_builder)), _http_client(std::move(client))
  {
  }
};

using synchronous_client = pinecone_client<net::threading_mode::sync>;
}  // namespace pinecone
