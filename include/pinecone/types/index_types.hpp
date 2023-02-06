#pragma once
/**
 * @file index_types.hpp
 * @brief Native C++ type modeling the results of index operations
 */

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

// NOLINTNEXTLINE
NLOHMANN_JSON_SERIALIZE_ENUM(database_state, {{database_state::unknown, nullptr},
                                              {database_state::initializing, "initializing"},
                                              {database_state::scaling_up, "scaling_up"},
                                              {database_state::scaling_down, "scaling_down"},
                                              {database_state::terminating, "terminating"},
                                              {database_state::ready, "ready"}})

/**
 * @brief Pod types for individual databases.
 */
enum class pod_type {
  unknown,
  s1,
  p1,
  p2
};

// NOLINTNEXTLINE
NLOHMANN_JSON_SERIALIZE_ENUM(pod_type, {{pod_type::unknown, nullptr},
                                        {pod_type::s1, "s1"},
                                        {pod_type::p1, "p1"},
                                        {pod_type::p2, "p2"}})

/**
 * @brief Pod sizes for individual databases.
 */
enum class pod_size {
  unknown,
  x1,
  x2,
  x4,
  x8
};

// NOLINTNEXTLINE
NLOHMANN_JSON_SERIALIZE_ENUM(pod_size, {{pod_size::unknown, nullptr},
                                        {pod_size::x1, "x1"},
                                        {pod_size::x2, "x2"},
                                        {pod_size::x4, "x4"},
                                        {pod_size::x8, "x8"}})

/**
 * @brief Metric types for individual databases.
 */
enum class metric_type {
  unknown,
  euclidean,
  cosine,
  dotproduct
};

// NOLINTNEXTLINE
NLOHMANN_JSON_SERIALIZE_ENUM(metric_type, {{metric_type::unknown, nullptr},
                                           {metric_type::euclidean, "euclidean"},
                                           {metric_type::cosine, "cosine"},
                                           {metric_type::dotproduct, "dotproduct"}})

/**
 * @brief Pod configuration for an individual index.
 */
struct pod_configuration {
  pod_configuration() = default;
  pod_configuration(pod_type type, pod_size size) noexcept : _type(type), _size(size) {}

  [[nodiscard]] auto type() const noexcept -> pod_type { return _type; }
  [[nodiscard]] auto size() const noexcept -> pod_size { return _size; }

  friend void to_json(nlohmann ::json& nlohmann_json_j, const pod_configuration& nlohmann_json_t)
  {
    json typestr = nlohmann_json_t._type;
    json sizestr = nlohmann_json_t._size;
    nlohmann_json_j = typestr.get<std::string>() + "." + sizestr.get<std::string>();
  }

  friend void from_json(const nlohmann ::json& nlohmann_json_j, pod_configuration& nlohmann_json_t)
  {
    std::string const& pod_type = nlohmann_json_j.get<std::string>();
    json type_str = pod_type.substr(0, 2);
    json size_str = pod_type.substr(3);
    type_str.get_to(nlohmann_json_t._type);
    size_str.get_to(nlohmann_json_t._size);
  }

 private:
  pod_type _type;
  pod_size _size;
};

/**
 * @brief The status of an individual database.
 */
struct database_status {
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(database_status, ready, state)

 private:
  bool ready;
  database_state state;
};

/**
 * @brief The complete description of an individual database.
 */
struct database_detail {
  [[nodiscard]] constexpr auto db_name() const noexcept -> std::string const& { return name; }
  [[nodiscard]] constexpr auto db_dimension() const noexcept -> uint32_t { return dimension; }
  [[nodiscard]] constexpr auto db_metric() const noexcept -> metric_type { return metric; }
  [[nodiscard]] constexpr auto db_pod_type() const noexcept -> pod_configuration const&
  {
    return pod_type;
  }
  [[nodiscard]] constexpr auto db_pods() const noexcept -> uint16_t { return pods; }
  [[nodiscard]] constexpr auto db_replicas() const noexcept -> uint16_t { return replicas; }
  [[nodiscard]] constexpr auto db_shards() const noexcept -> uint16_t { return shards; }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(database_detail, name, dimension, metric, pod_type, pods, replicas,
                                 shards)

 private:
  std::string name;
  uint32_t dimension;
  metric_type metric;
  pod_configuration pod_type;
  uint16_t pods;
  uint16_t replicas;
  uint16_t shards;
};

struct database {
  [[nodiscard]] auto db_status() const noexcept -> database_status { return status; }
  [[nodiscard]] auto db_detail() const noexcept -> database_detail { return database; }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(database, status, database);

 private:
  database_status status{};
  database_detail database;
};

struct collection {
  [[nodiscard]] auto col_name() const noexcept -> std::string { return name; }
  [[nodiscard]] auto col_size() const noexcept -> uint64_t { return size; }
  [[nodiscard]] auto col_status() const noexcept -> std::string { return status; }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(collection, name, status, size)

 private:
  std::string name;
  std::string status;
  uint64_t size;
};

struct index_configuration {
  index_configuration() = default;
  index_configuration(uint16_t replicas, pod_configuration pod_type)
      : replicas(replicas), pod_type(pod_type)
  {
  }

  [[nodiscard]] auto index_replicas() const noexcept -> uint16_t { return replicas; }
  [[nodiscard]] auto index_pod_config() const noexcept -> pod_configuration { return pod_type; }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(index_configuration, replicas, pod_type)

 private:
  uint16_t replicas;
  pod_configuration pod_type;
};

struct new_collection {
  new_collection() = default;
  new_collection(std::string name, std::string source) noexcept
      : name(std::move(name)), source(std::move(source))
  {
  }

  [[nodiscard]] auto col_name() const noexcept -> std::string const& { return name; }
  [[nodiscard]] auto col_source() const noexcept -> std::string const& { return source; }

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(new_collection, name, source)

 private:
  std::string name;
  std::string source;
};

struct new_index {
  struct builder {
    builder(std::string name, uint32_t dimension) noexcept
        : _name(std::move(name)), _dimension(dimension)
    {
    }

    auto with_metric(metric_type metric) noexcept -> builder&
    {
      _metric = metric;
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
              _metric,
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
    std::optional<metric_type> _metric;
    std::optional<uint16_t> _pods;
    std::optional<pod_configuration> _pod_config;
    std::optional<uint16_t> _shards;
    std::optional<uint16_t> _replicas;
    std::optional<std::vector<std::string>> _metadata_config;
    std::optional<std::string> _source_collection;
  };

  friend void to_json(nlohmann ::json& nlohmann_json_j, const new_index& nlohmann_json_t)
  {
    nlohmann_json_j["name"] = nlohmann_json_t.name;
    nlohmann_json_j["dimension"] = nlohmann_json_t.dimension;
    if (nlohmann_json_t.metric) {
      nlohmann_json_j["metric"] = *nlohmann_json_t.metric;
    }
    if (nlohmann_json_t.pods) {
      nlohmann_json_j["pods"] = *nlohmann_json_t.pods;
    }
    if (nlohmann_json_t.pod_type) {
      nlohmann_json_j["podConfig"] = *nlohmann_json_t.pod_type;
    }
    if (nlohmann_json_t.shards) {
      nlohmann_json_j["shards"] = *nlohmann_json_t.shards;
    }
    if (nlohmann_json_t.replicas) {
      nlohmann_json_j["replicas"] = *nlohmann_json_t.replicas;
    }
    if (nlohmann_json_t.metadata_config) {
      nlohmann_json_j["metadataConfig"] = *nlohmann_json_t.metadata_config;
    }
    if (nlohmann_json_t.source_collection) {
      nlohmann_json_j["sourceCollection"] = *nlohmann_json_t.source_collection;
    }
  }

 private:
  std::string name;
  uint16_t dimension{};
  std::optional<metric_type> metric;
  std::optional<uint16_t> pods;
  std::optional<pod_configuration> pod_type;
  std::optional<uint16_t> shards;
  std::optional<uint16_t> replicas;
  std::optional<std::vector<std::string>> metadata_config;
  std::optional<std::string> source_collection;

  new_index() = default;
  new_index(std::string name, uint32_t dimension, std::optional<metric_type> metric,
            std::optional<uint16_t> pods, std::optional<pod_configuration> pod_config,
            std::optional<uint16_t> shards, std::optional<uint16_t> replicas,
            std::optional<std::vector<std::string>> metadata_config,
            std::optional<std::string> source_collection) noexcept
      : name(std::move(name)),
        dimension(dimension),
        metric(metric),
        pods(pods),
        pod_type(pod_config),
        shards(shards),
        replicas(replicas),
        metadata_config(std::move(metadata_config)),
        source_collection(std::move(source_collection))
  {
  }
};
}  // namespace pinecone::types
