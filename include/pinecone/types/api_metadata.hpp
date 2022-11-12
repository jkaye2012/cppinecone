#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include "pinecone/result.hpp"

using json = nlohmann::json;

namespace pinecone::types
{
struct api_metadata {
  [[nodiscard]] static auto build(json api_result) -> result<api_metadata>
  {
    return api_metadata(api_result["project_name"], api_result["user_label"],
                        api_result["user_name"]);
  }

  api_metadata() noexcept = default;

  [[nodiscard]] auto project_name() const noexcept -> std::string const& { return _project_name; }

  [[nodiscard]] auto user_label() const noexcept -> std::string const& { return _user_label; }

  [[nodiscard]] auto user_name() const noexcept -> std::string const& { return _user_name; }

 private:
  std::string _project_name;
  std::string _user_label;
  std::string _user_name;

  api_metadata(std::string project_name, std::string user_label, std::string user_name) noexcept
      : _project_name(std::move(project_name)),
        _user_label(std::move(user_label)),
        _user_name(std::move(user_name))
  {
  }
};
}  // namespace pinecone::types