#pragma once
/**
 * @file api_metadata.hpp
 * @brief Metadata required by the Pinecone API
 */

#include <string>

#include <nlohmann/json.hpp>

#include "pinecone/util/result.hpp"

namespace pinecone::types
{
/**
 * @brief Pinecone API metadata.
 * @details Many Pinecone API operations require data that can be retrieved only from the API
 * itself. Cppinecone refers to this data as "API metadata". All metadata is managed for the user by
 * Cppinecone.
 *
 */
struct api_metadata {
  /**
   * @return the project name used by the API
   */
  [[nodiscard]] auto md_project_name() const noexcept -> std::string const& { return project_name; }

  /**
   * @return the user label used by the API
   */
  [[nodiscard]] auto md_user_label() const noexcept -> std::string const& { return user_label; }

  /**
   * @return the user name used by the API
   */
  [[nodiscard]] auto md_user_name() const noexcept -> std::string const& { return user_name; }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(api_metadata, project_name, user_label, user_name)

 private:
  std::string project_name;
  std::string user_label;
  std::string user_name;
};
}  // namespace pinecone::types