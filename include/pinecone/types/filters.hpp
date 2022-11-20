#pragma once

#include <string>

#include "pinecone/types/vector_metadata.hpp"

namespace pinecone::types::filters
{
inline auto eq(std::string key, metadata_value value) noexcept -> binary_filter
{
  return {std::move(key), binary_operator::eq, std::move(value)};
}

inline auto ne(std::string key, metadata_value value) noexcept -> binary_filter
{
  return {std::move(key), binary_operator::ne, std::move(value)};
};

inline auto gt(std::string key, metadata_value value) noexcept -> binary_filter
{
  return {std::move(key), binary_operator::gt, std::move(value)};
};

inline auto gte(std::string key, metadata_value value) noexcept -> binary_filter
{
  return {std::move(key), binary_operator::gte, std::move(value)};
};

inline auto lt(std::string key, metadata_value value) noexcept -> binary_filter
{
  return {std::move(key), binary_operator::lt, std::move(value)};
};

inline auto lte(std::string key, metadata_value value) noexcept -> binary_filter
{
  return {std::move(key), binary_operator::lte, std::move(value)};
};

template <typename iter>
inline auto in(std::string key, iter values) noexcept -> array_filter<iter>
{
  return {std::move(key), array_operator::in, std::move(values)};
};

template <typename iter>
inline auto nin(std::string key, iter values) noexcept -> array_filter<iter>
{
  return {std::move(key), array_operator::nin, std::move(values)};
};

template <typename... ts>
inline auto and_(ts... filters) noexcept -> combination_filter<ts...>
{
  return {combination_operator::and_, std::move(filters)...};
};

template <typename... ts>
inline auto or_(ts... filters) noexcept -> combination_filter<ts...>
{
  return {combination_operator::or_, std::move(filters)...};
};

inline auto none() noexcept -> no_filter { return {}; };
}  // namespace pinecone::types::filters