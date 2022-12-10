#pragma once

#include <cstdint>
#include <type_traits>
#include <vector>

#include <nlohmann/json.hpp>

#include "pinecone/types/accepted.hpp"
#include "pinecone/util/result.hpp"

using json = nlohmann::json;

namespace pinecone::types
{
template <typename T, bool Json = !std::is_same_v<T, accepted>>
struct parser {
  using parsed_type = T;

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