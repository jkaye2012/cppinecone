#pragma once

#include <string_view>

#include "pinecone/types/vector_metadata.hpp"

namespace pinecone::types::filters
{
constexpr auto eq(std::string_view key, metadata_value value) noexcept -> binary_filter
{
  return {key, binary_operator::eq, value};
}

constexpr auto ne(std::string_view key, metadata_value value) noexcept -> binary_filter
{
  return {key, binary_operator::ne, value};
};

constexpr auto gt(std::string_view key, metadata_value value) noexcept -> binary_filter
{
  return {key, binary_operator::gt, value};
};

constexpr auto gte(std::string_view key, metadata_value value) noexcept -> binary_filter
{
  return {key, binary_operator::gte, value};
};

constexpr auto lt(std::string_view key, metadata_value value) noexcept -> binary_filter
{
  return {key, binary_operator::lt, value};
};

constexpr auto lte(std::string_view key, metadata_value value) noexcept -> binary_filter
{
  return {key, binary_operator::lte, value};
};

template <typename iter>
constexpr auto in(std::string_view key, iter values) noexcept -> array_filter<iter>
{
  return {key, array_operator::in, std::move(values)};
};

template <typename iter>
constexpr auto nin(std::string_view key, iter values) noexcept -> array_filter<iter>
{
  return {key, array_operator::nin, std::move(values)};
};

template <typename... ts>
constexpr auto and_(ts... filters) noexcept -> combination_filter<ts...>
{
  return {combination_operator::and_, std::move(filters)...};
};

template <typename... ts>
constexpr auto or_(ts... filters) noexcept -> combination_filter<ts...>
{
  return {combination_operator::or_, std::move(filters)...};
};

constexpr auto none() noexcept -> no_filter { return {}; };
}  // namespace pinecone::types::filters