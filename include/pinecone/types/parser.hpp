#pragma once
/**
 * @file parser.hpp
 * @brief Parses HTTP responses to domain types
 */

#include <cstdint>
#include <type_traits>
#include <vector>

#include <nlohmann/json.hpp>

#include "pinecone/types/accepted.hpp"
#include "pinecone/util/result.hpp"

using json = nlohmann::json;

namespace pinecone::types
{
/**
 * @brief Parses raw data from the Pinecone API into native C++ types.
 *
 * @tparam T the type to parse
 * @tparam Json whether the type in question should be parsed using JSON
 */
template <typename T, bool Json = !std::is_same_v<T, accepted>>
struct parser {
  using parsed_type = T;

  /**
   * @brief Attempts to parse a stream of bytes into the provided type.
   *
   * @param data the data to parse
   * @return the result of the operation
   */
  [[nodiscard]] auto parse(std::vector<uint8_t>& data) const -> util::result<T>
  {
    if constexpr (Json) {
      T parsed = json::parse(data);
      return parsed;
    } else {
      return accepted{data};
    }
  }
};

}  // namespace pinecone::types