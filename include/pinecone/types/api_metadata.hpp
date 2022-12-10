#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include "pinecone/util/result.hpp"

namespace pinecone::types
{
struct api_metadata {
  [[nodiscard]] auto md_project_name() const noexcept -> std::string const& { return project_name; }

  [[nodiscard]] auto md_user_label() const noexcept -> std::string const& { return user_label; }

  [[nodiscard]] auto md_user_name() const noexcept -> std::string const& { return user_name; }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(api_metadata, project_name, user_label, user_name)

 private:
  std::string project_name;
  std::string user_label;
  std::string user_name;
};
}  // namespace pinecone::types