#pragma once

#include <string>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace pinecone::types
{
/**
 * @brief Booleans operators supported by the metadata query language.
 * @see https://www.pinecone.io/docs/metadata-filtering/#metadata-query-language
 */
enum class metadata_filter_operator {
  eq,
  ne,
  gt,
  gte,
  lt,
  lte,
  in,
  nin
};

struct metadata_filter {
  [[nodiscard]] auto serialize() const noexcept -> std::string
  {
    json result;
    result["filter"] = {};
    return result.dump();
  }
};

}  // namespace pinecone::types