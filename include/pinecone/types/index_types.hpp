#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "pinecone/util/result.hpp"

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

/**
 * @brief Pod types for individual databases.
 */
enum class pod_type {
  s1,
  p1,
  p2
};

inline auto to_string(pod_type type) noexcept -> std::string
{
  switch (type) {
    case pod_type::s1:
      return "s1";
    case pod_type::p1:
      return "p1";
    case pod_type::p2:
      return "p2";
  }
}

/**
 * @brief Pod sizes for individual databases.
 */
enum class pod_size {
  x1,
  x2,
  x4,
  x8
};

inline auto to_string(pod_size size) noexcept -> std::string
{
  switch (size) {
    case pod_size::x1:
      return "x1";
    case pod_size::x2:
      return "x2";
    case pod_size::x4:
      return "x4";
    case pod_size::x8:
      return "x8";
  }
}

/**
 * @brief Pod configuration for an individual index.
 */
struct pod_configuration {
  /**
   * @brief Constructs a new pod configuration.
   *
   * @param type the pod type
   * @param size the pod size
   */
  pod_configuration(pod_type type, pod_size size) noexcept : _type(type), _size(size) {}

  /**
   * @return the pod type
   */
  [[nodiscard]] auto type() const noexcept -> pod_type { return _type; }

  /**
   * @return the pod size
   */
  [[nodiscard]] auto size() const noexcept -> pod_size { return _size; }

  [[nodiscard]] static auto build(std::string const& config) -> util::result<pod_configuration>
  {
    auto type_str = config.substr(0, 2);
    pod_type type{};
    if (type_str == "s1") {
      type = pod_type::s1;
    } else if (type_str == "p1") {
      type = pod_type::p1;
    } else if (type_str == "p2") {
      type = pod_type::p2;
    } else {
      return {"failed to parse pod type"};
    }

    auto size_str = config.substr(3);
    pod_size size{};
    if (size_str == "x1") {
      size = pod_size::x1;
    } else if (size_str == "x2") {
      size = pod_size::x2;
    } else if (size_str == "x4") {
      size = pod_size::x4;
    } else if (size_str == "x8") {
      size = pod_size::x8;
    } else {
      return {"failed to parse pod size"};
    }

    return pod_configuration(type, size);
  }

  [[nodiscard]] auto serialize() const noexcept -> std::string
  {
    return to_string(_type) + "." + to_string(_size);
  }

 private:
  pod_type _type;
  pod_size _size;
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
  [[nodiscard]] constexpr auto pod_type() const noexcept -> pod_configuration const&
  {
    return _pod_config;
  }
  [[nodiscard]] constexpr auto pods() const noexcept -> uint16_t { return _pods; }
  [[nodiscard]] constexpr auto replicas() const noexcept -> uint16_t { return _replicas; }
  [[nodiscard]] constexpr auto shards() const noexcept -> uint16_t { return _shards; }
  [[nodiscard]] constexpr auto status() const noexcept -> database_status const& { return _status; }

  static auto build(json api_result) -> util::result<database>
  {
    auto status = database_status::build(api_result["status"]);
    auto json_db = api_result["database"];
    auto pod_config = pod_configuration::build(json_db["pod_type"]);
    if (pod_config.is_failed()) {
      return pod_config.propagate<database>();
    }
    return database(json_db["name"], json_db["dimension"], json_db["metric"], *pod_config,
                    json_db["pods"], json_db["replicas"], json_db["shards"], status);
  }

 private:
  std::string _name;
  uint32_t _dimension;
  std::string _metric;
  pod_configuration _pod_config;
  uint16_t _pods;
  uint16_t _replicas;
  uint16_t _shards;
  database_status _status;

  database(std::string name, uint32_t dimension, std::string metric, pod_configuration pod_config,
           uint16_t pods, uint16_t replicas, uint16_t shards, database_status status) noexcept
      : _name(std::move(name)),
        _dimension(dimension),
        _metric(std::move(metric)),
        _pod_config(pod_config),
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

  static auto build(json api_result) -> util::result<collection>
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
  static auto build(json api_result) -> util::result<list>
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
  index_configuration(uint16_t replicas, pod_configuration pod_config) noexcept
      : _replicas(replicas), _pod_config(pod_config)
  {
  }

  [[nodiscard]] auto serialize() const noexcept -> std::string
  {
    json repr = {{"replicas", _replicas}, {"pod_type", _pod_config.serialize()}};

    return repr.dump();
  }

 private:
  uint16_t _replicas;
  pod_configuration _pod_config;
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

    auto with_pod_type(pod_configuration pod_config) noexcept -> builder&
    {
      _pod_config = pod_config;
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
              _pod_config,
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
    std::optional<pod_configuration> _pod_config;
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
    if (_pod_config) {
      repr["pod_type"] = _pod_config->serialize();
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
  std::optional<pod_configuration> _pod_config;
  std::optional<uint16_t> _shards;
  std::optional<uint16_t> _replicas;
  std::optional<std::vector<std::string>> _metadata_config;
  std::optional<std::string> _source_collection;

  new_index(std::string name, uint32_t dimension, std::optional<std::string> metric,
            std::optional<uint16_t> pods, std::optional<pod_configuration> pod_config,
            std::optional<uint16_t> shards, std::optional<uint16_t> replicas,
            std::optional<std::vector<std::string>> metadata_config,
            std::optional<std::string> source_collection) noexcept
      : _name(std::move(name)),
        _dimension(dimension),
        _metric(std::move(metric)),
        _pods(pods),
        _pod_config(pod_config),
        _shards(shards),
        _replicas(replicas),
        _metadata_config(std::move(metadata_config)),
        _source_collection(std::move(source_collection))
  {
  }
};
}  // namespace pinecone::types
