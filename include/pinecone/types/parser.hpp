#pragma once

#include <cstdint>
#include <type_traits>
#include <vector>

#include <nlohmann/json.hpp>

#include "pinecone/result.hpp"
#include "pinecone/types/accepted.hpp"

using json = nlohmann::json;

namespace pinecone::types
{
template <typename T, bool Json = !std::is_same_v<T, accepted>>
struct parser {
  using parsed_type = T;

  [[nodiscard]] auto parse(std::vector<uint8_t>& data) const -> result<T>
  {
    if constexpr (Json) {
      return parsed_type::build(json::parse(data));
    }

    return parsed_type::build(data);
  }
};

}  // namespace pinecone::types