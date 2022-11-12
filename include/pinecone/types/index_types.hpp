#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "pinecone/result.hpp"

using json = nlohmann::json;

namespace pinecone::types
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

  static auto build(json api_result) -> result<database>
  {
    auto status = database_status::build(api_result["status"]);
    auto json_db = api_result["database"];
    return database(json_db["name"], json_db["dimension"], json_db["metric"], json_db["pod_type"],
                    json_db["pods"], json_db["replicas"], json_db["shards"], status);
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

struct collection {
  [[nodiscard]] auto name() const noexcept -> std::string { return _name; }
  [[nodiscard]] auto size() const noexcept -> uint64_t { return _size; }
  [[nodiscard]] auto status() const noexcept -> std::string { return _status; }

  static auto build(json api_result) -> result<collection>
  {
    return collection(api_result["name"], api_result["size"], api_result["status"]);
  }

 private:
  std::string _name;
  std::string _status;
  uint64_t _size;

  collection(std::string name, uint64_t size, std::string status) noexcept
      : _name(std::move(name)), _status(std::move(status)), _size(size)
  {
  }
};

struct list {
  static auto build(json api_result) -> result<list>
  {
    std::vector<std::string> names;
    names.reserve(api_result.size());

    for (auto& index : api_result) {
      names.emplace_back(std::move(index));
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

struct index_configuration {
  index_configuration(uint16_t replicas, std::string pod_type) noexcept
      : _replicas(replicas), _pod_type(std::move(pod_type))
  {
  }

  [[nodiscard]] auto serialize() const noexcept -> std::string
  {
    json repr = {{"replicas", _replicas}, {"pod_type", _pod_type}};

    return repr.dump();
  }

 private:
  uint16_t _replicas;
  std::string _pod_type;  // TODO: could be made enum
};

struct new_collection {
  new_collection(std::string name, std::string source) noexcept
      : _name(std::move(name)), _source(std::move(source))
  {
  }

  [[nodiscard]] auto serialize() const noexcept -> std::string
  {
    json repr = {{"name", _name}, {"source", _source}};

    return repr.dump();
  }

 private:
  std::string _name;
  std::string _source;
};

struct new_index {
  struct builder {
    builder(std::string name, uint32_t dimension) noexcept
        : _name(std::move(name)), _dimension(dimension)
    {
    }

    auto with_metric(std::string metric) noexcept -> builder&
    {
      _metric = std::move(metric);
      return *this;
    }

    auto with_pods(uint16_t pods) noexcept -> builder&
    {
      _pods = pods;
      return *this;
    }

    auto with_pod_type(std::string pod_type) noexcept -> builder&
    {
      _pod_type = std::move(pod_type);
      return *this;
    }

    auto with_shards(uint16_t shards) noexcept -> builder&
    {
      _shards = shards;
      return *this;
    }

    auto with_replicas(uint16_t replicas) noexcept -> builder&
    {
      _replicas = replicas;
      return *this;
    }

    auto with_metadata_config(std::vector<std::string> indexed) noexcept -> builder&
    {
      _metadata_config = std::move(indexed);
      return *this;
    }

    auto with_source_collection(std::string source_collection) noexcept -> builder&
    {
      _source_collection = std::move(source_collection);
      return *this;
    }

    [[nodiscard]] auto build() noexcept -> new_index
    {
      return {std::move(_name),
              _dimension,
              std::move(_metric),
              _pods,
              std::move(_pod_type),
              _shards,
              _replicas,
              std::move(_metadata_config),
              std::move(_source_collection)};
    }

   private:
    std::string _name;
    uint32_t _dimension;
    std::optional<std::string> _metric;
    std::optional<uint16_t> _pods;
    std::optional<std::string> _pod_type;
    std::optional<uint16_t> _shards;
    std::optional<uint16_t> _replicas;
    std::optional<std::vector<std::string>> _metadata_config;
    std::optional<std::string> _source_collection;
  };

  [[nodiscard]] auto serialize() const noexcept -> std::string
  {
    json repr = {{"name", _name}, {"dimension", _dimension}};
    if (_metric) {
      repr["metric"] = *_metric;
    }
    if (_pods) {
      repr["pods"] = *_pods;
    }
    if (_pod_type) {
      repr["pod_type"] = *_pod_type;
    }
    if (_shards) {
      repr["shards"] = *_shards;
    }
    if (_replicas) {
      repr["replicas"] = *_replicas;
    }
    if (_metadata_config) {
      repr["metadata_config"] = {{"indexed", *_metadata_config}};
    }
    if (_source_collection) {
      repr["source_collection"] = *_source_collection;
    }

    return repr.dump();
  };

 private:
  std::string _name;
  uint16_t _dimension;
  std::optional<std::string> _metric;  // TODO: could be made enum
  std::optional<uint16_t> _pods;
  std::optional<std::string> _pod_type;  // TODO: could be made enum
  std::optional<uint16_t> _shards;
  std::optional<uint16_t> _replicas;
  std::optional<std::vector<std::string>> _metadata_config;
  std::optional<std::string> _source_collection;

  new_index(std::string name, uint32_t dimension, std::optional<std::string> metric,
            std::optional<uint16_t> pods, std::optional<std::string> pod_type,
            std::optional<uint16_t> shards, std::optional<uint16_t> replicas,
            std::optional<std::vector<std::string>> metadata_config,
            std::optional<std::string> source_collection) noexcept
      : _name(std::move(name)),
        _dimension(dimension),
        _metric(std::move(metric)),
        _pods(pods),
        _pod_type(std::move(pod_type)),
        _shards(shards),
        _replicas(replicas),
        _metadata_config(std::move(metadata_config)),
        _source_collection(std::move(source_collection))
  {
  }
};
}  // namespace pinecone::types
