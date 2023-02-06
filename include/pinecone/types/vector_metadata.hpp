#pragma once
/**
 * @file vector_metadata.hpp
 * @brief Encoding for Pinecone's vector metadata API
 */

#include <cstdint>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <variant>

#include <nlohmann/json.hpp>

#include "pinecone/util/result.hpp"

using json = nlohmann::json;

namespace pinecone::types
{
/**
 * @brief The binary operators supported by the metadata filtering API.
 */
enum class binary_operator {
  unknown,
  eq,
  ne,
  gt,
  gte,
  lt,
  lte,
};

// NOLINTNEXTLINE
NLOHMANN_JSON_SERIALIZE_ENUM(binary_operator, {{binary_operator::unknown, nullptr},
                                               {binary_operator::eq, "$eq"},
                                               {binary_operator::ne, "$ne"},
                                               {binary_operator::gt, "$gt"},
                                               {binary_operator::gte, "$gte"},
                                               {binary_operator::lt, "$lt"},
                                               {binary_operator::lte, "$lte"}})

/**
 * @brief The array operators supported by the metadata filtering API.
 */
enum class array_operator {
  unknown,
  in,
  nin,
};

// NOLINTNEXTLINE
NLOHMANN_JSON_SERIALIZE_ENUM(array_operator, {{array_operator::unknown, nullptr},
                                              {array_operator::in, "$in"},
                                              {array_operator::nin, "$nin"}})

/**
 * @brief The combination operators supported by the metadata filtering API.
 */
enum class combination_operator {
  unknown,
  and_,
  or_,
};

// NOLINTNEXTLINE
NLOHMANN_JSON_SERIALIZE_ENUM(combination_operator, {{combination_operator::unknown, nullptr},
                                                    {combination_operator::and_, "$and"},
                                                    {combination_operator::or_, "$or"}})

/**
 * @brief Value types supported by the metadata filtering API.
 */
struct metadata_value {
  using value_type = std::variant<bool, int64_t, double, std::string>;
  // NOLINTNEXTLINE
  constexpr metadata_value(char const* value) noexcept : _var(value) {}
  // NOLINTNEXTLINE
  metadata_value(std::string value) noexcept : _var(value) {}
  // NOLINTNEXTLINE
  constexpr metadata_value(bool value) noexcept : _var(value) {}
  // NOLINTNEXTLINE
  constexpr metadata_value(int64_t value) noexcept : _var(value) {}
  // NOLINTNEXTLINE
  constexpr metadata_value(double value) noexcept : _var(value) {}

  [[nodiscard]] auto var() const noexcept -> value_type const& { return _var; }

  friend void to_json(nlohmann ::json& nlohmann_json_j, const metadata_value& nlohmann_json_t)
  {
    std::visit([&nlohmann_json_j](auto const& v) { nlohmann_json_j = v; }, nlohmann_json_t._var);
  }

 private:
  value_type _var;
};

/**
 * @brief Metadata for a single Pinecone vector.
 * @details Metadata is essentially a map of key:value pairs where each key is a string and each
 * value is a metadata_value (essentially a variant over a few supported value types). This metadata
 * can be used in multiple API operations as a filter to restrict the operation to run on a subset
 * of vectors within an index. Due to the complexity of this API, Cppinecone encodes metadata
 * filtering with a special set of operations that make it easier to interact with from C++ code.
 * @see filters.hpp
 */
struct metadata {
  metadata() = default;
  explicit metadata(std::unordered_map<std::string, metadata_value> values) noexcept
      : _values(std::move(values))
  {
  }

  /**
   * @return the raw metadata values
   */
  [[nodiscard]] auto values() const noexcept
      -> std::unordered_map<std::string, metadata_value> const&
  {
    return _values;
  }

  friend void to_json(nlohmann ::json& nlohmann_json_j, const metadata& nlohmann_json_t)
  {
    for (auto const& [key, value] : nlohmann_json_t._values) {
      std::visit([&, k = key](auto const& v) { nlohmann_json_j[k] = v; }, value.var());
    }
  }

  friend void from_json(const nlohmann ::json& nlohmann_json_j, metadata& nlohmann_json_t)
  {
    std::unordered_map<std::string, metadata_value> values;
    for (auto const& [key, value] : nlohmann_json_j.items()) {
      if (value.is_boolean()) {
        values.emplace(key, value.get<bool>());
      } else if (value.is_number_integer()) {
        values.emplace(key, value.get<int64_t>());
      } else if (value.is_number_float()) {
        values.emplace(key, value.get<double>());
      } else if (value.is_string()) {
        values.emplace(key, value.get<std::string>());
      } else {
        // TODO: handle failure
        // return {"Metadata value was not a boolean, integer, float, or string"};
      }
    }

    nlohmann_json_t._values = std::move(values);
  }

 private:
  std::unordered_map<std::string, metadata_value> _values;
};

template <typename Derived>
struct filter_base {
  friend void to_json(nlohmann ::json& nlohmann_json_j, const filter_base& nlohmann_json_t)
  {
    nlohmann_json_j["filter"] = json::object();
    to_json(nlohmann_json_j["filter"], static_cast<Derived const&>(nlohmann_json_t));
  }
};

/**
 * @brief Binary filters are simple predicates; they compare a single metadata value to a provided
 * operand.
 */
struct binary_filter : public filter_base<binary_filter> {
  friend void to_json(nlohmann ::json& nlohmann_json_j, const binary_filter& nlohmann_json_t)
  {
    json j = nlohmann_json_t._op;
    nlohmann_json_j[nlohmann_json_t._key][j] = nlohmann_json_t._value;
  }

  binary_filter(std::string key, binary_operator op, metadata_value value) noexcept
      : _key(std::move(key)), _op(op), _value(std::move(value))
  {
  }

 private:
  std::string _key;
  binary_operator _op;
  metadata_value _value;
};

/**
 * @brief Array filters test a single metadata value against multiple operands.
 *
 * @tparam iter the iterator type, must support .begin() and .end(), and must be implicitly
 * convertiable to metadata_value
 */
template <typename iter>
struct array_filter : public filter_base<array_filter<iter>> {
  friend void to_json(nlohmann ::json& nlohmann_json_j, const array_filter& nlohmann_json_t)
  {
    json j = nlohmann_json_t._op;
    nlohmann_json_j[nlohmann_json_t._key][j] = json::array();
    auto& arr = nlohmann_json_j[nlohmann_json_t._key][j];
    for (auto const& v : nlohmann_json_t._values) {
      arr.emplace_back(v);
    }
  }

  array_filter(std::string key, array_operator op, iter values) noexcept
      : _key(std::move(key)), _op(op), _values(std::move(values))
  {
  }

 private:
  std::string _key;
  array_operator _op;
  iter _values;
};

template <typename filter>
auto serialize_expand(json& arr, filter const& f) -> void
{
  to_json(arr.emplace_back(), f);
}

template <typename filter, typename... filters>
auto serialize_expand(json& arr, filter const& f, filters const&... fs) -> void
{
  serialize_expand(arr, f);
  serialize_expand(arr, fs...);
}

/**
 * @brief Combination filters apply boolean logic to the other filter types.
 *
 * @tparam ts the filter types to combine; each entry must be one of binary_filter, array_filter, or
 * combination_filter
 */
template <typename... ts>
struct combination_filter : public filter_base<combination_filter<ts...>> {
  friend void to_json(nlohmann ::json& nlohmann_json_j, const combination_filter& nlohmann_json_t)
  {
    json j = nlohmann_json_t._op;
    nlohmann_json_j[j] = json::array();
    auto& arr = nlohmann_json_j[j];
    std::apply([&arr](auto const&... filter) -> void { serialize_expand(arr, filter...); },
               nlohmann_json_t._filters);
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

/**
 * @brief No filter applied; always returns true.
 */
struct no_filter : public filter_base<no_filter> {
  friend void to_json(nlohmann ::json& nlohmann_json_j, const no_filter& /*nlohmann_json_t*/)
  {
    nlohmann_json_j = json::object({});
  }
};
}  // namespace pinecone::types