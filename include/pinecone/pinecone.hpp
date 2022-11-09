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
#include "pinecone/types/index_types.hpp"

namespace pinecone
{
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
    domain::operation_args<domain::operation_type::index_list> args(_url_builder);
    return _http_client->request(std::move(args));
  }

  [[nodiscard]] auto describe_index(std::string const& name) const noexcept
      -> result<types::database>
  {
    domain::operation_args<domain::operation_type::index_describe> args(_url_builder, name);
    return _http_client->request(std::move(args));
  }

  [[nodiscard]] auto list_collections() const noexcept -> result<types::collections>
  {
    domain::operation_args<domain::operation_type::collection_list> args(_url_builder);
    return _http_client->request(std::move(args));
  }

  [[nodiscard]] auto delete_collection(std::string const& name) const noexcept -> result<json>
  {
    domain::operation_args<domain::operation_type::collection_delete> args(_url_builder, name);
    return _http_client->request(std::move(args));
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
