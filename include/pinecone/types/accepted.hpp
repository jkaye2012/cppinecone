#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

#include "pinecone/result.hpp"
namespace pinecone::types
{
struct accepted {
  static auto build(std::vector<uint8_t>& data) noexcept -> result<accepted>
  {
    return {accepted(data)};
  }

  [[nodiscard]] auto result() const noexcept -> std::string_view { return _result; }

 private:
  std::string _result;

  explicit accepted(std::vector<uint8_t>& data) noexcept : _result(data.begin(), data.end()) {}
};
}  // namespace pinecone::types