#pragma once
/**
 * @file filters.hpp
 * @brief Filters supported by complex vector API operations
 */

#include <string>

#include "pinecone/types/vector_metadata.hpp"

namespace pinecone::types::filters
{
/**
 * @brief Equality filter, tests if a key is == to a value.
 *
 * @param key
 * @param value
 * @return binary_filter
 */
inline auto eq(std::string key, metadata_value value) noexcept -> binary_filter
{
  return {std::move(key), binary_operator::eq, std::move(value)};
}

/**
 * @brief Not-equal filter, tests if a key is != to a value.
 *
 * @param key
 * @param value
 * @return binary_filter
 */
inline auto ne(std::string key, metadata_value value) noexcept -> binary_filter
{
  return {std::move(key), binary_operator::ne, std::move(value)};
};

/**
 * @brief Greater-than filter, tests if a key is > than a value.
 *
 * @param key
 * @param value
 * @return binary_filter
 */
inline auto gt(std::string key, metadata_value value) noexcept -> binary_filter
{
  return {std::move(key), binary_operator::gt, std::move(value)};
};

/**
 * @brief Greater-than-or-equal filter, tests if a key is >= a value.
 *
 * @param key
 * @param value
 * @return binary_filter
 */
inline auto gte(std::string key, metadata_value value) noexcept -> binary_filter
{
  return {std::move(key), binary_operator::gte, std::move(value)};
};

/**
 * @brief Less-than filter, tests if a key is < a value.
 *
 * @param key
 * @param value
 * @return binary_filter
 */
inline auto lt(std::string key, metadata_value value) noexcept -> binary_filter
{
  return {std::move(key), binary_operator::lt, std::move(value)};
};

/**
 * @brief Less-than-or-equal filter, tests if a key is <= a value.
 *
 * @param key
 * @param value
 * @return binary_filter
 */
inline auto lte(std::string key, metadata_value value) noexcept -> binary_filter
{
  return {std::move(key), binary_operator::lte, std::move(value)};
};

/**
 * @brief In filter, tests if a key is in a range of values.
 *
 * @tparam iter
 * @param key
 * @param values
 * @return array_filter<iter>
 */
template <typename iter>
inline auto in(std::string key, iter values) noexcept -> array_filter<iter>
{
  return {std::move(key), array_operator::in, std::move(values)};
};

/**
 * @brief Not-in filter, tests is a key is not in a range of values.
 *
 * @tparam iter
 * @param key
 * @param values
 * @return array_filter<iter>
 */
template <typename iter>
inline auto nin(std::string key, iter values) noexcept -> array_filter<iter>
{
  return {std::move(key), array_operator::nin, std::move(values)};
};

/**
 * @brief And filter, tests if all contained filters are true.
 *
 * @tparam ts
 * @param filters
 * @return combination_filter<ts...>
 */
template <typename... ts>
inline auto and_(ts... filters) noexcept -> combination_filter<ts...>
{
  return {combination_operator::and_, std::move(filters)...};
};

/**
 * @brief Or filter, tests is any contained filter is true.
 *
 * @tparam ts
 * @param filters
 * @return combination_filter<ts...>
 */
template <typename... ts>
inline auto or_(ts... filters) noexcept -> combination_filter<ts...>
{
  return {combination_operator::or_, std::move(filters)...};
};

/**
 * @brief No filter, always true.
 *
 * @return no_filter
 */
inline auto none() noexcept -> no_filter { return {}; };
}  // namespace pinecone::types::filters