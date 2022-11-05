#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace pinecone
{
/**
 * @brief Pod states for individual databases.
 */
enum class database_state {
  initializing,
  scaling_up,
  scaling_down,
  terminating,
  ready
};

/**
 * @brief The status of an individual database.
 */
struct database_status {
 private:
  bool _ready;
  database_state _state;
};

/**
 * @brief The complete description of an individual database.
 */
struct database {
  [[nodiscard]] constexpr auto name() const noexcept -> std::string const& { return _name; }

  [[nodiscard]] constexpr auto dimensions() const noexcept -> std::string const&
  {
    return _dimensions;
  }

  static auto build(json api_result) noexcept -> std::optional<database> { return {}; }

 private:
  std::string _name;
  std::string _dimensions;
  // std::string _index_type;
  // std::string _metric;
  // std::string _pod_type;
  // uint16_t _pods;
  // uint16_t _replicas;
  // uint16_t _shards;
  // database_status _status;

  database(std::string name, std::string dimensions) noexcept
      : _name(std::move(name)), _dimensions(std::move(dimensions))
  {
  }
};

// TODO: properly model failure, std::optional is nowhere near sufficient

struct indexes {
  static auto build(json api_result) noexcept -> std::optional<indexes>
  {
    if (!api_result.is_array()) {
      return std::nullopt;
    }
    std::vector<std::string> names;
    names.reserve(api_result.size());
    for (auto& index : api_result) {
      if (!index.is_string()) {
        return std::nullopt;
      }

      names.emplace_back(std::move(index));
    }

    return indexes(std::move(names));
  }

  [[nodiscard]] auto names() const noexcept -> std::vector<std::string> const& { return _names; }

 private:
  std::vector<std::string> _names;

  explicit indexes(std::vector<std::string> names) noexcept : _names(std::move(names)) {}
};
}  // namespace pinecone
