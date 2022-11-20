#pragma once

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
    explicit namespace_summary(uint64_t vector_count) noexcept : _vector_count(vector_count) {}

    [[nodiscard]] auto vector_count() const noexcept -> uint64_t { return _vector_count; }

   private:
    uint64_t _vector_count;
  };

  static auto build(json api_result) -> util::result<index_stats>
  {
    std::unordered_map<std::string, namespace_summary> namespaces;
    for (auto const& ns : api_result["namespaces"].items()) {
      namespaces.emplace(ns.key(), namespace_summary(std::move(ns.value()["vectorCount"])));
    }

    return index_stats(std::move(namespaces), api_result["dimension"], api_result["indexFullness"],
                       api_result["totalVectorCount"]);
  }

  [[nodiscard]] auto namespaces() const noexcept
      -> std::unordered_map<std::string, namespace_summary> const&
  {
    return _namespaces;
  }

  [[nodiscard]] auto dimension() const noexcept -> uint64_t { return _dimension; }

  [[nodiscard]] auto index_fullness() const noexcept -> double { return _index_fullness; }

  [[nodiscard]] auto total_vector_count() const noexcept -> uint64_t { return _total_vector_count; }

 private:
  std::unordered_map<std::string, namespace_summary> _namespaces;
  uint64_t _dimension;
  double _index_fullness;
  uint64_t _total_vector_count;

  index_stats(std::unordered_map<std::string, namespace_summary> namespaces, uint64_t dimension,
              double index_fullness, uint64_t total_vector_count) noexcept
      : _namespaces(std::move(namespaces)),
        _dimension(dimension),
        _index_fullness(index_fullness),
        _total_vector_count(total_vector_count)
  {
  }
};

template <typename filter = no_filter>
struct query {
  struct builder {
    builder(uint64_t top_k, double vector) noexcept
        : _top_k(top_k), _query(vector), _filter(filters::none())
    {
    }

    builder(uint64_t top_k, std::string_view vector_id) noexcept
        : _top_k(top_k), _query(vector_id), _filter(filters::none())
    {
    }
    builder(filter f, uint64_t top_k, double vector) noexcept
        : _top_k(top_k), _query(vector), _filter(std::move(f))
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
    std::variant<double, std::string_view> _query;
    filter _filter;
    std::optional<std::string_view> _namespace;
    std::optional<bool> _include_values;
    std::optional<bool> _include_metadata;
  };

  [[nodiscard]] auto serialize() const noexcept -> std::string
  {
    json repr = {{"topK", _top_k}};

    if (std::holds_alternative<double>(_query)) {
      repr["vector"] = std::get<double>(_query);
    } else if (std::holds_alternative<std::string_view>(_query)) {
      repr["id"] = std::get<std::string_view>(_query);
    }

    _filter.serialize(repr);
    if (_namespace) {
      repr["namespace"] = *_namespace;
    }
    if (_include_values) {
      repr["includeValues"] = *_include_values;
    }
    if (_include_metadata) {
      repr["includeMetadata"] = *_include_metadata;
    }

    return repr.dump();
  }

 private:
  uint64_t _top_k;
  std::variant<double, std::string_view> _query;
  std::optional<std::string_view> _namespace;
  filter _filter;
  std::optional<bool> _include_values;
  std::optional<bool> _include_metadata;

  query(std::optional<std::string_view> ns, uint64_t top_k, filter f,
        std::optional<bool> include_values, std::optional<bool> include_metadata,
        std::variant<double, std::string_view> query) noexcept
      : _top_k(top_k),
        _query(query),
        _namespace(ns),
        _filter(std::move(f)),
        _include_values(include_values),
        _include_metadata(include_metadata)
  {
  }
};

struct query_result {
  struct scored_vector {
    [[nodiscard]] auto id() const noexcept -> std::string const& { return _id; }

    [[nodiscard]] auto score() const noexcept -> double { return _score; }

    [[nodiscard]] auto values() const noexcept -> std::optional<std::vector<double>> const&
    {
      return _values;
    }

    [[nodiscard]] auto md() const noexcept -> std::optional<metadata> const& { return _metadata; }

    static auto build(json& api_result) -> util::result<scored_vector>
    {
      std::optional<std::vector<double>> values;
      if (auto vals = api_result["values"]; !vals.is_null()) {
        values = std::vector<double>();
        for (auto val : vals) {
          values->emplace_back(val);
        }
      }

      std::optional<metadata> md;
      if (api_result.contains("metadata")) {
        auto md_result = metadata::build(api_result["metadata"]);
        if (md_result.is_failed()) {
          return md_result.propagate<scored_vector>();
        }
        md = *md_result;
      }

      return scored_vector(std::move(api_result["id"]), api_result["score"], std::move(values),
                           std::move(md));
    }

   private:
    std::string _id;
    double _score;
    std::optional<std::vector<double>> _values;
    std::optional<metadata> _metadata;

    scored_vector(std::string id, double score, std::optional<std::vector<double>> values,
                  std::optional<metadata> md) noexcept
        : _id(std::move(id)), _score(score), _values(std::move(values)), _metadata(std::move(md))
    {
    }
  };

  static auto build(json api_result) -> util::result<query_result>
  {
    std::vector<scored_vector> results;
    for (auto& result : api_result["matches"]) {
      auto sv = scored_vector::build(result);
      if (sv.is_failed()) {
        return sv.propagate<query_result>();
      }
      results.emplace_back(*sv);
    }

    return query_result{api_result["namespace"], std::move(results)};
  }

  [[nodiscard]] auto ns() const noexcept -> std::string const& { return _namespace; }

  [[nodiscard]] auto matches() const noexcept -> std::vector<scored_vector> const&
  {
    return _matches;
  }

 private:
  std::string _namespace;
  std::vector<scored_vector> _matches;

  query_result(std::string ns, std::vector<scored_vector> matches) noexcept
      : _namespace(std::move(ns)), _matches(std::move(matches))
  {
  }
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

  [[nodiscard]] auto serialize() const noexcept -> std::string
  {
    json repr = {};

    if (_namespace) {
      repr["namespace"] = *_namespace;
    }

    if (std::holds_alternative<ids>(_mode)) {
      repr["ids"] = json::array({});
      for (auto const& id : std::get<ids>(_mode)) {
        repr["ids"].emplace_back(id);
      }
    } else if (std::holds_alternative<bool>(_mode)) {
      repr["deleteAll"] = std::get<bool>(_mode);
    } else if (std::holds_alternative<filter>(_mode)) {
      std::get<filter>(_mode).serialize(repr);
    }

    return repr.dump();
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
 private:
  std::string _id;
  std::vector<double> _values;
  std::optional<metadata> _metadata;
};
}  // namespace pinecone::types