#pragma once
/**
 * @file curl_result.hpp
 * @brief Wraps CURL results for type and exception safety
 */

#include <functional>
#include <string>
#include <variant>

#include <curl/curl.h>

#include "pinecone/util/visit.hpp"

namespace pinecone::util
{
/**
 * @brief Models the possibly of failure for remote HTTP operations.
 */
struct [[nodiscard]] curl_result {
  using error_type = std::variant<CURLcode, curl_slist*>;

  constexpr curl_result() noexcept = default;

  /**
   * @brief Constructs a result from a CURL error code.
   * @details This constructor is intentionally non-explicit to facilitate converting construction.
   */
  // NOLINTNEXTLINE
  constexpr curl_result(CURLcode code) noexcept
  {
    if (code != CURLE_OK) {
      _value = code;
    } else {
      _value = {};
    }
  }

  /**
   * @brief Constructs a result from a CURL header list.
   * @details This constructor is intentionally non-explicit to facilitate converting construction.
   */
  // NOLINTNEXTLINE
  constexpr curl_result(curl_slist* list) noexcept
  {
    if (list == nullptr) {
      _value = list;
    } else {
      _value = {};
    }
  }

  /**
   * @returns whether the current instance represents a successful operation
   */
  [[nodiscard]] constexpr auto is_success() const noexcept -> bool { return _value.index() == 0; }

  /**
   * @returns whether the current instance represents a failed operation
   */
  [[nodiscard]] constexpr auto is_error() const noexcept -> bool { return !is_success(); }

  /**
   * @brief Retrieves the error type associated with a failed operation.
   * Cannot be called on a successful result.
   *
   * @returns the associated error
   */
  [[nodiscard]] constexpr auto error() const noexcept -> error_type
  {
    return std::get<error_type>(_value);
  }

  /**
   * @brief Runs a transformation function on a successful result; does nothing on a failed result.
   *
   * @param func the transformation function to apply on a successful result
   * @return the result of the transformation (or the propagated error)
   */
  constexpr auto and_then(std::function<curl_result()> const& func) const noexcept -> curl_result
  {
    if (is_error()) {
      return *this;
    }

    return func();
  }

  /**
   * @brief Retrieves the string representation of a failed operation.
   *
   * @param err the error to stringify
   * @return the string representation of the error
   */
  static auto to_string(error_type const& err) noexcept -> std::string
  {
    return std::visit(
        util::overloaded{[](CURLcode arg) { return std::to_string(arg); },
                         [](curl_slist*) -> std::string { return "Bad header list"; }},
        err);
  }

  /**
   * @brief Retrieves the string representation of the current instance.

   * @details The format of the string returned by this function is _not_ specified
   *   and should not be relied upon programmatically.
   * @returns the string representation of the operation result
   */
  [[nodiscard]] auto to_string() const noexcept -> std::string
  {
    switch (_value.index()) {
      case 0:
        return "Success";
      case 1:
        return to_string(error());
      default:
        return "Unknown";
    }
  }

 private:
  std::variant<std::monostate, error_type> _value;
};

}  // namespace pinecone::util
