#pragma once
/**
 * @file visit.hpp
 * @brief Simple visitor implementation.
 */

namespace pinecone::util
{
/**
 * @brief Basic visitor operations.
 *
 * @tparam Ts types that operations can be applied to
 */
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
}  // namespace pinecone::util
