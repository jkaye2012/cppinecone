#pragma once
/**
 * @file url_builder.hpp
 * @brief Constructs Pinecone API URLs
 */

#include <map>
#include <sstream>
#include <string>
#include <string_view>

#include "pinecone/domain/operation_type.hpp"
#include "pinecone/types/api_metadata.hpp"

namespace pinecone::net
{
/**
 * @brief Constructs Pinecone API URLs.
 */
struct url_builder {
  /**
   * @brief Construct a new url builder object
   *
   * @param environment the cloud environment for the URLs
   */
  explicit url_builder(std::string environment) noexcept : _environment(std::move(environment))
  {
    std::ostringstream oss;
    oss << "https://controller." << _environment << ".pinecone.io";
    _controller_prefix = oss.str();
  }

  /**
   * @brief Set the API metadata
   * @details This function is required due to the construction of the Pinecone API. Some API URLs
   * require information that can only be obtained by making an _initial_ API call; thus, Cppinecone
   * makes this request for the user when a new connection is created, then uses the response to
   * populate the requisite metadata for subsequent operations.
   *
   * @param metadata the metadata returned by the Pinecone API server
   */
  auto set_metadata(types::api_metadata metadata) noexcept { _metadata = std::move(metadata); }

  /**
   * @return the prefix for all API operations
   */
  [[nodiscard]] auto prefix() const noexcept -> std::string_view { return _controller_prefix; }

  /**
   * @brief Constructs a URL for a specific operation_type
   *
   * @tparam op the operation
   * @return the URL
   */
  template <domain::operation_type op>
  [[nodiscard]] auto build() const noexcept -> std::string
  {
    if constexpr (domain::op_api_type(op) == domain::api_type::controller) {
      return _controller_prefix + domain::op_url_fragment(op);
    }
  }

  /**
   * @brief Constructs a URL for a specific operation_type requiring an index name
   *
   * @tparam op the operation
   * @param index_name the index name
   * @return the URL
   */
  template <domain::operation_type op>
  [[nodiscard]] auto build(std::string_view index_name) const noexcept -> std::string
  {
    if constexpr (domain::op_api_type(op) == domain::api_type::service) {
      std::ostringstream oss;
      oss << "https://" << index_name << "-" << _metadata.md_project_name() << ".svc."
          << _environment << ".pinecone.io" << domain::op_url_fragment(op);
      return oss.str();
    }
  }

 private:
  std::string _environment;
  std::string _controller_prefix;
  types::api_metadata _metadata;
};
}  // namespace pinecone::net
