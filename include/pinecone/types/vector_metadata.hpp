#pragma once

#include <cstdint>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <variant>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace pinecone::types
{
enum class binary_operator {
  eq,
  ne,
  gt,
  gte,
  lt,
  lte,
};

constexpr auto to_string(binary_operator binop) noexcept -> std::string_view
{
  switch (binop) {
    case binary_operator::eq:
      return "$eq";
    case binary_operator::ne:
      return "$ne";
    case binary_operator::gt:
      return "$gt";
    case binary_operator::gte:
      return "$gte";
    case binary_operator::lt:
      return "$lt";
    case binary_operator::lte:
      return "$lte";
  }
}

enum class array_operator {
  in,
  nin,
};

constexpr auto to_string(array_operator arrop) noexcept -> std::string_view
{
  switch (arrop) {
    case array_operator::in:
      return "$in";
    case array_operator::nin:
      return "$nin";
  }
}

enum class combination_operator {
  and_,
  or_,
};

constexpr auto to_string(combination_operator combop) noexcept -> std::string_view
{
  switch (combop) {
    case combination_operator::and_:
      return "$and";
    case combination_operator::or_:
      return "$or";
  }
}

struct metadata_value {
  using value_type = std::variant<bool, int64_t, double, std::string_view>;
  // NOLINTNEXTLINE
  constexpr metadata_value(char const* value) noexcept : _var(std::string_view(value)) {}
  // NOLINTNEXTLINE
  constexpr metadata_value(std::string_view value) noexcept : _var(value) {}
  // NOLINTNEXTLINE
  constexpr metadata_value(bool value) noexcept : _var(value) {}
  // NOLINTNEXTLINE
  constexpr metadata_value(int64_t value) noexcept : _var(value) {}
  // NOLINTNEXTLINE
  constexpr metadata_value(double value) noexcept : _var(value) {}

  [[nodiscard]] auto var() const noexcept -> value_type const& { return _var; }

 private:
  value_type _var;
};

constexpr auto to_json(json& j, metadata_value const& value) -> void
{
  std::visit([&j](auto const& v) -> void { j = v; }, value.var());
}

template <typename Derived>
struct filter_base {
  [[nodiscard]] auto serialize() const noexcept -> std::string
  {
    json result{{"filter", json::object()}};
    static_cast<Derived const*>(this)->serialize_impl(result["filter"]);
    return result.dump();
  }
};

struct binary_filter : public filter_base<binary_filter> {
  auto serialize_impl(json& obj) const noexcept -> void { obj[_key] = {{to_string(_op), _value}}; }

  constexpr binary_filter(std::string_view key, binary_operator op, metadata_value value) noexcept
      : _key(key), _op(op), _value(value)
  {
  }

 private:
  std::string_view _key;
  binary_operator _op;
  metadata_value _value;
};

// iter must support .begin() and .end(), and the value type must be convertible to metadata_value
template <typename iter>
struct array_filter : public filter_base<array_filter<iter>> {
  auto serialize_impl(json& obj) const noexcept -> void
  {
    auto opstr = to_string(_op);
    obj[_key] = {{opstr, json::array()}};
    auto& arr = obj[_key][opstr];
    for (auto const& v : _values) {
      arr.emplace_back(v);
    }
  }

  constexpr array_filter(std::string_view key, array_operator op, iter values) noexcept
      : _key(key), _op(op), _values(std::move(values))
  {
  }

 private:
  std::string_view _key;
  array_operator _op;
  iter _values;
};

template <typename filter>
auto serialize_expand(json& arr, filter const& f) -> void
{
  f.serialize_impl(arr.emplace_back());
}

template <typename filter, typename... filters>
auto serialize_expand(json& arr, filter const& f, filters const&... fs) -> void
{
  serialize_expand(arr, f);
  serialize_expand(arr, fs...);
}

// each ts must be one of binary_filter, array_filter, or combination_filter
template <typename... ts>
struct combination_filter : public filter_base<combination_filter<ts...>> {
  auto serialize_impl(json& obj) const noexcept -> void
  {
    auto opstr = to_string(_op);
    obj[opstr] = json::array();
    auto& arr = obj[opstr];
    std::apply([&arr](auto const&... filter) -> void { serialize_expand(arr, filter...); },
               _filters);
  }

  // NOLINTNEXTLINE
  constexpr combination_filter(combination_operator op, ts... filters) noexcept
      : _op(op), _filters(std::move(filters)...)
  {
  }

 private:
  combination_operator _op;
  std::tuple<ts...> _filters;
};

struct no_filter : public filter_base<no_filter> {
  auto serialize_impl(json& obj) const noexcept -> void {}
};

template <typename filter>
struct metadata_filter {
  explicit constexpr metadata_filter(filter f) : _filter(std::move(f)) {}

  [[nodiscard]] auto serialize() const noexcept -> std::string
  {
    json result{{"filter", json::object()}};
    _filter.serialize(result["filter"]);
    return result.dump();
  }

 private:
  filter _filter;
};
}  // namespace pinecone::types