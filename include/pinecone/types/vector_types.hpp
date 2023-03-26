#pragma once
/**
 * @file vector_types.hpp
 * @brief Native C++ types modeling the results of vector operations
 */

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include <nlohmann/json.hpp>

#include "pinecone/types/filters.hpp"
#include "pinecone/types/vector_metadata.hpp"
#include "pinecone/util/result.hpp"

using json = nlohmann::json;

namespace pinecone::types
{
struct index_stats {
  struct namespace_summary {
    [[nodiscard]] auto vector_count() const noexcept -> uint64_t { return vectorCount; }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(namespace_summary, vectorCount)

   private:
    uint64_t vectorCount;
  };

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(index_stats, namespaces, dimension, indexFullness,
                                 totalVectorCount)

  [[nodiscard]] auto stat_namespaces() const noexcept
      -> std::unordered_map<std::string, namespace_summary> const&
  {
    return namespaces;
  }

  [[nodiscard]] auto stat_dimension() const noexcept -> uint64_t { return dimension; }

  [[nodiscard]] auto stat_index_fullness() const noexcept -> double { return indexFullness; }

  [[nodiscard]] auto stat_total_vector_count() const noexcept -> uint64_t
  {
    return totalVectorCount;
  }

 private:
  std::unordered_map<std::string, namespace_summary> namespaces;
  uint64_t dimension;
  double indexFullness;
  uint64_t totalVectorCount;
};

template <typename filter>
struct query {
  struct builder {
    builder(uint64_t top_k, std::vector<double> vector) noexcept
        : _top_k(top_k), _query(std::move(vector)), _filter(filters::none())
    {
    }
    builder(uint64_t top_k, std::string_view vector_id) noexcept
        : _top_k(top_k), _query(vector_id), _filter(filters::none())
    {
    }
    builder(filter f, uint64_t top_k, std::vector<double> vector) noexcept
        : _top_k(top_k), _query(std::move(vector)), _filter(std::move(f))
    {
    }
    builder(filter f, uint64_t top_k, std::string_view vector_id) noexcept
        : _top_k(top_k), _query(vector_id), _filter(std::move(f))
    {
    }

    [[nodiscard]] auto with_namespace(std::string_view ns) noexcept -> builder&
    {
      _namespace = ns;
      return *this;
    }

    [[nodiscard]] auto with_include_values(bool inc) noexcept -> builder&
    {
      _include_values = inc;
      return *this;
    }

    [[nodiscard]] auto with_include_metadata(bool inc) noexcept -> builder&
    {
      _include_metadata = inc;
      return *this;
    }

    [[nodiscard]] auto build() const noexcept -> query
    {
      return query(_namespace, _top_k, _filter, _include_values, _include_metadata, _query);
    }

   private:
    uint64_t _top_k;
    std::variant<std::vector<double>, std::string_view> _query;
    filter _filter;
    std::optional<std::string_view> _namespace;
    std::optional<bool> _include_values;
    std::optional<bool> _include_metadata;
  };

  friend void to_json(nlohmann ::json& nlohmann_json_j, const query& nlohmann_json_t)
  {
    to_json(nlohmann_json_j["filter"], nlohmann_json_t._filter);
    nlohmann_json_j["topK"] = nlohmann_json_t._top_k;

    if (std::holds_alternative<std::vector<double>>(nlohmann_json_t._query)) {
      nlohmann_json_j["vector"] = std::get<std::vector<double>>(nlohmann_json_t._query);
    } else if (std::holds_alternative<std::string_view>(nlohmann_json_t._query)) {
      nlohmann_json_j["id"] = std::get<std::string_view>(nlohmann_json_t._query);
    }

    if (nlohmann_json_t._namespace) {
      nlohmann_json_j["namespace"] = *nlohmann_json_t._namespace;
    }
    if (nlohmann_json_t._include_values) {
      nlohmann_json_j["includeValues"] = *nlohmann_json_t._include_values;
    }
    if (nlohmann_json_t._include_metadata) {
      nlohmann_json_j["includeMetadata"] = *nlohmann_json_t._include_metadata;
    }
  }

 private:
  uint64_t _top_k;
  std::variant<std::vector<double>, std::string_view> _query;
  std::optional<std::string_view> _namespace;
  filter _filter;
  std::optional<bool> _include_values;
  std::optional<bool> _include_metadata;

  query(std::optional<std::string_view> ns, uint64_t top_k, filter f,
        std::optional<bool> include_values, std::optional<bool> include_metadata,
        std::variant<std::vector<double>, std::string_view> query) noexcept
      : _top_k(top_k),
        _query(std::move(query)),
        _namespace(ns),
        _filter(std::move(f)),
        _include_values(include_values),
        _include_metadata(include_metadata)
  {
  }
};

template <typename filter = no_filter>
inline auto query_builder(uint64_t top_k, std::vector<double> vector) noexcept
{
  return typename query<filter>::builder(top_k, std::move(vector));
}

template <typename filter = no_filter>
inline auto query_builder(uint64_t top_k, std::string_view vector_id) noexcept
{
  return typename query<filter>::builder(top_k, vector_id);
}

template <typename filter>
inline auto query_builder(filter f, uint64_t top_k, std::vector<double> vector) noexcept
{
  return typename query<filter>::builder(f, top_k, std::move(vector));
}

template <typename filter>
inline auto query_builder(filter f, uint64_t top_k, std::string_view vector_id) noexcept
{
  return typename query<filter>::builder(f, top_k, vector_id);
}

struct query_result {
  struct scored_vector {
    [[nodiscard]] auto id() const noexcept -> std::string const& { return _id; }

    [[nodiscard]] auto score() const noexcept -> double { return _score; }

    [[nodiscard]] auto values() const noexcept -> std::optional<std::vector<double>> const&
    {
      return _values;
    }

    [[nodiscard]] auto md() const noexcept -> std::optional<metadata> const& { return _metadata; }

    friend void from_json(const nlohmann ::json& nlohmann_json_j, scored_vector& nlohmann_json_t)
    {
      if (nlohmann_json_j.contains("values")) {
        nlohmann_json_t._values = nlohmann_json_j["values"];
      }

      if (nlohmann_json_j.contains("metadata")) {
        metadata m;
        from_json(nlohmann_json_j["metadata"], m);
        nlohmann_json_t._metadata = std::move(m);
      }

      nlohmann_json_j.at("id").get_to(nlohmann_json_t._id);
      nlohmann_json_j.at("score").get_to(nlohmann_json_t._score);
    }

   private:
    std::string _id;
    double _score;
    std::optional<std::vector<double>> _values;
    std::optional<metadata> _metadata;
  };

  friend void from_json(const nlohmann ::json& nlohmann_json_j, query_result& nlohmann_json_t)
  {
    nlohmann_json_j.at("namespace").get_to(nlohmann_json_t._namespace);
    nlohmann_json_j.at("matches").get_to(nlohmann_json_t.matches);
  }

  [[nodiscard]] auto query_ns() const noexcept -> std::string const& { return _namespace; }

  [[nodiscard]] auto query_matches() const noexcept -> std::vector<scored_vector> const&
  {
    return matches;
  }

 private:
  std::string _namespace;
  std::vector<scored_vector> matches;
};

using ids = std::vector<std::string_view>;

template <typename filter = no_filter>
struct delete_request {
  using delete_mode = std::variant<ids, bool, filter>;

  struct builder {
    explicit builder(ids ids) noexcept : _mode(std::move(ids)) {}
    builder() noexcept : _mode(true) {}
    explicit builder(filter f) noexcept : _mode(std::move(f)) {}

    [[nodiscard]] auto build() const noexcept -> delete_request
    {
      return delete_request(_mode, _namespace);
    }

    auto with_namespace(std::string_view ns) noexcept -> builder&
    {
      _namespace = ns;
      return *this;
    }

   private:
    delete_mode _mode;
    std::optional<std::string_view> _namespace;
  };

  friend void to_json(nlohmann ::json& nlohmann_json_j, const delete_request& nlohmann_json_t)
  {
    if (nlohmann_json_t._namespace) {
      nlohmann_json_j["namespace"] = *nlohmann_json_t._namespace;
    }

    if (std::holds_alternative<ids>(nlohmann_json_t._mode)) {
      nlohmann_json_j["ids"] = json::array({});
      for (auto const& id : std::get<ids>(nlohmann_json_t._mode)) {
        nlohmann_json_j["ids"].emplace_back(id);
      }
    } else if (std::holds_alternative<bool>(nlohmann_json_t._mode)) {
      nlohmann_json_j["deleteAll"] = std::get<bool>(nlohmann_json_t._mode);
    } else if (std::holds_alternative<filter>(nlohmann_json_t._mode)) {
      to_json(nlohmann_json_j, std::get<filter>(nlohmann_json_t._mode));
    }
  }

 private:
  delete_mode _mode;
  std::optional<std::string_view> _namespace;

  delete_request(delete_mode mode, std::optional<std::string_view> ns) noexcept
      : _mode(std::move(mode)), _namespace(ns)
  {
  }
};

struct vector {
  vector() = default;
  vector(std::string id, std::vector<double> values) noexcept
      : _id(std::move(id)), _values(std::move(values)), _metadata(std::nullopt)
  {
  }

  vector(std::string id, std::vector<double> values, metadata md) noexcept
      : _id(std::move(id)), _values(std::move(values)), _metadata(std::move(md))
  {
  }

  [[nodiscard]] auto id() const noexcept -> std::string const& { return _id; }

  [[nodiscard]] auto values() const noexcept -> std::vector<double> const& { return _values; }

  [[nodiscard]] auto md() const noexcept -> std::optional<metadata> const& { return _metadata; }

  friend void to_json(nlohmann ::json& nlohmann_json_j, const vector& nlohmann_json_t)
  {
    nlohmann_json_j["id"] = nlohmann_json_t._id;
    nlohmann_json_j["values"] = nlohmann_json_t._values;
    if (nlohmann_json_t._metadata) {
      nlohmann_json_j["metadata"] = *nlohmann_json_t._metadata;
    }
  }

  friend void from_json(const nlohmann ::json& nlohmann_json_j, vector& nlohmann_json_t)
  {
    if (nlohmann_json_j.contains("metadata")) {
      metadata m;
      nlohmann_json_j.at("metadata").get_to(m);
      nlohmann_json_t._metadata = std::move(m);
    }

    nlohmann_json_j.at("values").get_to(nlohmann_json_t._values);
    nlohmann_json_j.at("id").get_to(nlohmann_json_t._id);
  }

 private:
  std::string _id;
  std::vector<double> _values;
  std::optional<metadata> _metadata;
};

struct upsert_request {
  struct builder {
    explicit builder(std::vector<vector> vectors) noexcept : _vectors(std::move(vectors)) {}

    [[nodiscard]] auto build() const noexcept -> upsert_request { return {_vectors, _namespace}; }

    auto with_namespace(std::string_view ns) noexcept -> builder&
    {
      _namespace = ns;
      return *this;
    }

   private:
    std::vector<vector> _vectors;
    std::optional<std::string_view> _namespace;
  };

  friend void to_json(nlohmann ::json& nlohmann_json_j, const upsert_request& nlohmann_json_t)
  {
    nlohmann_json_j["vectors"] = nlohmann_json_t._vectors;

    if (nlohmann_json_t._namespace) {
      nlohmann_json_j["namespace"] = *nlohmann_json_t._namespace;
    }
  }

 private:
  std::vector<vector> _vectors;
  std::optional<std::string_view> _namespace;

  upsert_request(std::vector<vector> vectors, std::optional<std::string_view> ns) noexcept
      : _vectors(std::move(vectors)), _namespace(ns)
  {
  }
};

struct update_request {
  struct builder {
    explicit builder(std::string_view id) noexcept : _id(id) {}

    [[nodiscard]] auto build() const noexcept -> update_request
    {
      return {_id, _values, _metadata, _namespace};
    }

    auto with_namespace(std::string_view ns) noexcept -> builder&
    {
      _namespace = ns;
      return *this;
    }

    auto with_values(std::vector<double> values) noexcept -> builder&
    {
      _values = std::move(values);
      return *this;
    }

    auto with_metadata(metadata md) noexcept -> builder&
    {
      _metadata = md;
      return *this;
    }

   private:
    std::string_view _id;
    std::optional<std::vector<double>> _values;
    std::optional<metadata> _metadata;
    std::optional<std::string_view> _namespace;
  };

  friend void to_json(nlohmann ::json& nlohmann_json_j, const update_request& nlohmann_json_t)
  {
    nlohmann_json_j["id"] = nlohmann_json_t._id;
    if (nlohmann_json_t._values) {
      nlohmann_json_j["values"] = *nlohmann_json_t._values;
    }
    if (nlohmann_json_t._metadata) {
      to_json(nlohmann_json_j["setMetadata"], *nlohmann_json_t._metadata);
    }
    if (nlohmann_json_t._namespace) {
      nlohmann_json_j["namespace"] = *nlohmann_json_t._namespace;
    }
  }

 private:
  std::string_view _id;
  std::optional<std::vector<double>> _values;
  std::optional<metadata> _metadata;
  std::optional<std::string_view> _namespace;

  update_request(std::string_view id, std::optional<std::vector<double>> values,
                 std::optional<metadata> md, std::optional<std::string_view> ns) noexcept
      : _id(id), _values(std::move(values)), _metadata(std::move(md)), _namespace(ns)
  {
  }
};
}  // namespace pinecone::types