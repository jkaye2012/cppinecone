#pragma once
/**
 * @file accepted.hpp
 * @brief A monostate sentinel type for operations that have no associated response
 */

#include <cstdint>
#include <string_view>
#include <vector>

#include "pinecone/util/result.hpp"
namespace pinecone::types
{
/**
 * @brief Some Pinecone API operations have no explicitly expected result. Cppinecone models all of
 * these operations by noting that they were simply "accepted" when they succeed.
 */
struct accepted {
  explicit accepted(std::vector<uint8_t>& data) noexcept : _result(data.begin(), data.end()) {}

  /**
   * @return the string representation of the result
   */
  [[nodiscard]] auto result() const noexcept -> std::string_view { return _result; }

 private:
  std::string _result;
};
}  // namespace pinecone::types