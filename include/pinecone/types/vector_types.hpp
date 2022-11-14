#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

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
}  // namespace pinecone::types