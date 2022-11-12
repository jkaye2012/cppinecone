#pragma once

#include <functional>
#include <string>
#include <variant>

#include <curl/curl.h>

#include "pinecone/util/visit.hpp"

namespace pinecone::util
{
/**
 * Models the possibly of failure for API operations.
 */
struct [[nodiscard]] curl_result {
  using error_type = std::variant<CURLcode, curl_slist*>;

  constexpr curl_result() noexcept = default;

  // NOLINTNEXTLINE
  constexpr curl_result(CURLcode code) noexcept
  {
    if (code != CURLE_OK) {
      _value = code;
    } else {
      _value = {};
    }
  }

  // NOLINTNEXTLINE
  constexpr curl_result(curl_slist* list) noexcept
  {
    if (list == nullptr) {
      _value = list;
    } else {
      _value = {};
    }
  }

  [[nodiscard]] constexpr auto is_success() const noexcept -> bool { return _value.index() == 0; }

  [[nodiscard]] constexpr auto is_error() const noexcept -> bool { return !is_success(); }

  [[nodiscard]] constexpr auto error() const noexcept -> error_type
  {
    return std::get<error_type>(_value);
  }

  constexpr auto and_then(std::function<curl_result()> const& func) const noexcept -> curl_result
  {
    if (is_error()) {
      return *this;
    }

    return func();
  }

  static auto to_string(error_type const& err) noexcept -> std::string
  {
    return std::visit(
        util::overloaded{[](CURLcode arg) { return std::to_string(arg); },
                         [](curl_slist*) -> std::string { return "Bad header list"; }},
        err);
  }

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
