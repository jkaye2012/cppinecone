#pragma once

#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "pinecone/result.hpp"

using json = nlohmann::json;

namespace pinecone
{
/**
 * @brief Pod states for individual databases.
 */
enum class database_state {
  unknown,
  initializing,
  scaling_up,
  scaling_down,
  terminating,
  ready
};

[[nodiscard]] constexpr auto from_string(std::string_view str) noexcept -> database_state
{
  if (str == "Initializing") {
    return database_state::initializing;
  }
  if (str == "ScalingUp") {
    return database_state::scaling_up;
  }
  if (str == "ScalingDown") {
    return database_state::scaling_down;
  }
  if (str == "Terminating") {
    return database_state::terminating;
  }
  if (str == "Ready") {
    return database_state::ready;
  }

  return database_state::unknown;
}

/**
 * @brief The status of an individual database.
 */
struct database_status {
  /**
   * @brief Constructs a database status from its json representation.
   * @details
   * This function is allowed to throw to allow for easier composition in higher-order parsers; as
   * such, it should only be called from within other parsers to ensure that exceptions do not
   * escape.
   */
  [[nodiscard]] static auto build(json api_result) -> database_status
  {
    std::string const& state(api_result["state"]);
    return {database_status(api_result["ready"], from_string(state))};
  }

 private:
  bool _ready;
  database_state _state;

  constexpr database_status(bool ready, database_state state) noexcept
      : _ready(ready), _state(state)
  {
  }
};

// TODO: metric could probably be made an enum

/**
 * @brief The complete description of an individual database.
 */
struct database {
  [[nodiscard]] constexpr auto name() const noexcept -> std::string const& { return _name; }
  [[nodiscard]] constexpr auto dimension() const noexcept -> uint32_t { return _dimension; }
  [[nodiscard]] constexpr auto metric() const noexcept -> std::string const& { return _metric; }
  [[nodiscard]] constexpr auto pod_type() const noexcept -> std::string const& { return _pod_type; }
  [[nodiscard]] constexpr auto pods() const noexcept -> uint16_t { return _pods; }
  [[nodiscard]] constexpr auto replicas() const noexcept -> uint16_t { return _replicas; }
  [[nodiscard]] constexpr auto shards() const noexcept -> uint16_t { return _shards; }
  [[nodiscard]] constexpr auto status() const noexcept -> database_status const& { return _status; }

  static auto build(json api_result) noexcept -> result<database>
  {
    try {
      auto status = database_status::build(api_result["status"]);
      auto json_db = api_result["database"];
      return database(json_db["name"], json_db["dimension"], json_db["metric"], json_db["pod_type"],
                      json_db["pods"], json_db["replicas"], json_db["shards"], status);
    } catch (json::exception& ex) {
      return {std::move(ex)};
    }
  }

 private:
  std::string _name;
  uint32_t _dimension;
  std::string _metric;
  std::string _pod_type;
  uint16_t _pods;
  uint16_t _replicas;
  uint16_t _shards;
  database_status _status;

  database(std::string name, uint32_t dimension, std::string metric, std::string pod_type,
           uint16_t pods, uint16_t replicas, uint16_t shards, database_status status) noexcept
      : _name(std::move(name)),
        _dimension(dimension),
        _metric(std::move(metric)),
        _pod_type(std::move(pod_type)),
        _pods(pods),
        _replicas(replicas),
        _shards(shards),
        _status(status)
  {
  }
};

struct list {
  static auto build(json api_result) noexcept -> result<list>
  {
    std::vector<std::string> names;
    names.reserve(api_result.size());

    try {
      for (auto& index : api_result) {
        names.emplace_back(std::move(index));
      }
    } catch (json::exception& ex) {
      return {std::move(ex)};
    }

    return list(std::move(names));
  }

  [[nodiscard]] auto names() const noexcept -> std::vector<std::string> const& { return _names; }

 private:
  std::vector<std::string> _names;

  explicit list(std::vector<std::string> names) noexcept : _names(std::move(names)) {}
};
using indexes = list;
using collections = list;
}  // namespace pinecone
