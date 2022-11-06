#pragma once

#include <functional>
#include <string>
#include <variant>

#include <curl/curl.h>

namespace pinecone::domain
{
/**
 * Models the possibly of failure for API operations.
 */
struct [[nodiscard]] curl_result {
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

  constexpr auto and_then(std::function<curl_result()> const& func) const noexcept -> curl_result
  {
    if (is_error()) {
      return *this;
    }

    return func();
  }

  [[nodiscard]] auto to_string() const noexcept -> std::string
  {
    switch (_value.index()) {
      case 0:
        return "Success";
      case 1:
        return std::to_string(std::get<CURLcode>(_value));
      case 2:
        return "Bad header list";
      default:
        return "Unknown";
    }
  }

 private:
  std::variant<std::monostate, CURLcode, curl_slist*> _value;
};
}  // namespace pinecone::domain
