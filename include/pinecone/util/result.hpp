#pragma once

#include <sstream>
#include <string>
#include <variant>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include "pinecone/util/curl_result.hpp"

using json = nlohmann::json;

namespace pinecone::util
{
/**
 * @brief The possible ways in which a Pinecone API call can fail.
 * @details
 * While there are many different reasons why any one of these failure modes may occur, API users
 * are likely to handle failures depending on which of these categories the failure belongs to. For
 * example, there is likely little that a client can do about
 * `parsing_failed`, but retrying a `request_rejected` in the future could be a reasonable course of
 * action.
 */
enum class failure {
  /**
   * @brief No failure; the request was processed and succeeded as expected.
   */
  none,
  /**
   * @brief The API request was sent successfully, but the server did not respond. This
   * usually indicates a Pinecone outage or client-side network misconfiguration, but could
   * potentially also be caused by a bug in cppinecone depending on the error code.
   */
  request_rejected,
  /**
   * @brief The API request was sent successfully, but the server returned a non-200 HTTP response.
   * This usually indicates a bug within the client code, such as providing an invalid index name or
   * invalid api key.
   */
  request_failed,
  /**
   * @brief The API request was sent and processed successfully by the server, but the server's
   * response could not be parsed. This usually indicates a bug in cppinecone and should not be
   * expected during standard use.
   */
  parsing_failed
};

template <failure F>
struct failure_reason {
};

template <>
struct failure_reason<failure::request_rejected> {
  explicit constexpr failure_reason(curl_result::error_type err) noexcept : _curl_err(err) {}

  [[nodiscard]] constexpr auto curl_error() const noexcept -> curl_result::error_type
  {
    return _curl_err;
  }

 private:
  curl_result::error_type _curl_err;
};
using request_rejected = failure_reason<failure::request_rejected>;

template <>
struct failure_reason<failure::request_failed> {
  failure_reason(int64_t code, std::string body) noexcept
      : _response_code(code), _body(std::move(body))
  {
  }

  [[nodiscard]] constexpr auto response_code() const noexcept -> int64_t { return _response_code; }
  [[nodiscard]] constexpr auto body() const noexcept -> std::string const& { return _body; }

 private:
  int64_t _response_code;
  std::string _body;
};
using request_failed = failure_reason<failure::request_failed>;

template <>
struct failure_reason<failure::parsing_failed> {
  explicit failure_reason(json::exception const& ex) noexcept : _message(ex.what()) {}
  explicit failure_reason(char const* message) noexcept : _message(message) {}

  [[nodiscard]] constexpr auto message() const noexcept -> char const* { return _message; }

 private:
  const char* _message;
};
using parsing_failed = failure_reason<failure::parsing_failed>;

template <typename T>
struct [[nodiscard]] result {
  using error_type = std::variant<request_rejected, request_failed, parsing_failed>;
  using value_type = std::variant<T, error_type>;

  // NOLINTNEXTLINE
  result(T value) noexcept : _value(std::move(value)) {}
  // NOLINTNEXTLINE
  result(curl_result::error_type err) noexcept : _value(request_rejected(err)) {}
  // NOLINTNEXTLINE
  result(int64_t code, std::string body) noexcept : _value(request_failed(code, std::move(body))) {}
  // NOLINTNEXTLINE
  result(json::exception const& ex) noexcept : _value(parsing_failed(ex)) {}
  // NOLINTNEXTLINE
  result(char const* message) noexcept : _value(parsing_failed(message)) {}
  // NOLINTNEXTLINE
  result(error_type err) noexcept : _value(std::move(err)) {}

  [[nodiscard]] constexpr auto failure_reason() const noexcept -> failure
  {
    if (_value.index() == 0) {
      return failure::none;
    }

    auto const& err = std::get<error_type>(_value);
    switch (err.index()) {
      case 0:
        return failure::request_rejected;
      case 1:
        return failure::request_failed;
      case 2:
        return failure::parsing_failed;
    }
  }

  [[nodiscard]] auto to_string() const noexcept -> std::string
  {
    if (_value.index() == 0) {
      return "success";
    }

    std::ostringstream oss;
    auto const& err = std::get<error_type>(_value);
    switch (err.index()) {
      case 0:
        oss << "Request rejected: "
            << curl_result::to_string(std::get<request_rejected>(err).curl_error());
        break;
      case 1:
        oss << "Request failed: " << std::get<request_failed>(err).response_code() << " "
            << std::get<request_failed>(err).body();
        break;
      case 2:
        oss << "Parsing failed: " << std::get<parsing_failed>(err).message();
        break;
      default:
        return "unknown";
    }

    return oss.str();
  }

  [[nodiscard]] constexpr auto is_successful() const noexcept -> bool
  {
    return _value.index() == 0;
  }

  [[nodiscard]] constexpr auto is_failed() const noexcept -> bool { return !is_successful(); }

  template <typename U>
  [[nodiscard]] constexpr auto propagate() noexcept -> result<U>
  {
    return {std::move(std::get<error_type>(_value))};
  }

  template <typename U>
  constexpr auto and_then(std::function<result<U>(T&)> const& func) noexcept -> result<U>
  {
    if (is_failed()) {
      return propagate<U>();
    }

    return func(std::get<T>(_value));
  }

  [[nodiscard]] constexpr auto operator->() noexcept -> T* { return &std::get<T>(_value); }
  [[nodiscard]] constexpr auto operator*() noexcept -> T& { return std::get<T>(_value); }

  [[nodiscard]] constexpr auto value() noexcept -> value_type& { return _value; }

 private:
  value_type _value;
};
}  // namespace pinecone::util
