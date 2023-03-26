#pragma once
/**
 * @file result.hpp
 * @brief Models the possibility of failure for Pinecone operations
 */

#include <sstream>
#include <string>
#include <variant>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include "pinecone/types/error.hpp"
#include "pinecone/util/curl_result.hpp"

using json = nlohmann::json;

namespace pinecone::util
{
/**
 * @brief The possible ways in which a Pinecone API call can fail.
 *
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

/**
 * @brief Data associated with the possible failure modes for Cppinecone operations.
 *
 * @tparam F the failure mode
 */
template <failure F>
struct failure_reason {
};

/**
 * @brief A request that was rejected by the remote API.
 */
template <>
struct failure_reason<failure::request_rejected> {
  explicit constexpr failure_reason(curl_result::error_type err) noexcept : _curl_err(err) {}

  /**
   * @returns the CURL error resulting from the failed request
   */
  [[nodiscard]] constexpr auto curl_error() const noexcept -> curl_result::error_type
  {
    return _curl_err;
  }

 private:
  curl_result::error_type _curl_err;
};
using request_rejected = failure_reason<failure::request_rejected>;

/**
 * @brief A request that was processed by the remote API, but failed during processing.
 */
template <>
struct failure_reason<failure::request_failed> {
  failure_reason(int64_t code, std::string body) noexcept
      : _response_code(code), _body(std::move(body))
  {
  }

  /**
   * @returns the HTTP response code returned by the API operation
   */
  [[nodiscard]] constexpr auto response_code() const noexcept -> int64_t { return _response_code; }

  /**
   * @returns the raw HTTP body returned by the API operation
   */
  [[nodiscard]] constexpr auto body() const noexcept -> std::string const& { return _body; }

  /**
   * @brief Attempts to parse the body as a structured API error.
   *
   * @returns the parsed error, or the JSON exception resulting from parsing failure
   */
  [[nodiscard]] auto api_error() const noexcept -> std::variant<types::api_error, json::exception>
  {
    try {
      types::api_error err = json::parse(_body);
      return err;
    } catch (json::exception const& ex) {
      return ex;
    }
  }

 private:
  int64_t _response_code;
  std::string _body;
};
using request_failed = failure_reason<failure::request_failed>;

/**
 * @brief A request that succeeded on the remote server, but whose response could not be parsed.
 *
 * @details This response normally indicates either a bug in Cppinecone or a divergence in
 * API/client expectations (e.g., the API has been upgraded but the client has not).
 */
template <>
struct failure_reason<failure::parsing_failed> {
  explicit failure_reason(json::exception const& ex) noexcept : _message(ex.what()) {}
  explicit failure_reason(char const* message) noexcept : _message(message) {}

  /**
   * @returns the exception message associated with the parsing failure
   */
  [[nodiscard]] constexpr auto message() const noexcept -> char const* { return _message; }

 private:
  const char* _message;
};
using parsing_failed = failure_reason<failure::parsing_failed>;

/**
 * @brief Models the possibility of failure for all Cppinecone public API operations.
 *
 * @details Cppinecone heavily constrains the types of errors that it allows to be propagated to
 * clients. All operations return a result, allowing the library fine-grained control over failure
 * modes and error handling. Usage of this class can be modified with client-level policies.
 *
 * @tparam T the type held in the event of operation success
 */
template <typename T>
struct [[nodiscard]] result {
  using error_type = std::variant<request_rejected, request_failed, parsing_failed>;
  using value_type = std::variant<T, error_type>;

  result() noexcept = default;
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

  /**
   * @returns the reason for operation failure (or a sentinel value for successful operations)
   */
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

  /**
   * @brief Retrieves a string representation of the operation result.
   *
   * @details The format of this string is _not_ specified and should not be relied upon
   * programmatically.
   *
   * @returns the string representation of the result
   */
  [[nodiscard]] auto to_string() const noexcept -> std::string
  {
    if (_value.index() == 0) {
      return "success";
    }

    std::ostringstream oss;
    auto const& err = std::get<error_type>(_value);
    switch (err.index()) {
      case 0:
        oss << "Request rejected. CURL error code: "
            << curl_result::to_string(std::get<request_rejected>(err).curl_error());
        break;
      case 1:
        oss << "Request failed. HTTP response code: "
            << std::get<request_failed>(err).response_code() << " "
            << std::get<request_failed>(err).body();
        break;
      case 2:
        oss << "Parsing failed. Exception message: " << std::get<parsing_failed>(err).message();
        break;
      default:
        return "Unknown. This is a bug, please report it!";
    }

    return oss.str();
  }

  /**
   * @returns whether the operation succeeded
   */
  [[nodiscard]] constexpr auto is_successful() const noexcept -> bool
  {
    return _value.index() == 0;
  }

  /**
   * @returns whether the operation failed
   */
  [[nodiscard]] constexpr auto is_failed() const noexcept -> bool { return !is_successful(); }

  /**
   * @brief Runs a transformation function on a successful result; does nothing on a failed result.
   *
   * @tparam U the result type of the transformation function
   * @param func the transformation function to apply on a successful result
   * @return the result of the transformation (or the propagated error)
   */
  template <typename U>
  constexpr auto and_then(std::function<result<U>(T&)> const& func) noexcept -> result<U>
  {
    if (is_failed()) {
      return propagate<U>();
    }

    return func(std::get<T>(_value));
  }

  /**
   * @brief Can be used only on successful results.
   */
  [[nodiscard]] constexpr auto operator->() noexcept -> T* { return &std::get<T>(_value); }

  /**
   * @brief Can be used only on successful results.
   */
  [[nodiscard]] constexpr auto operator*() noexcept -> T& { return std::get<T>(_value); }

  /**
   * @returns the raw value contained within the result
   */
  [[nodiscard]] constexpr auto value() noexcept -> value_type& { return _value; }

 private:
  template <typename U>
  [[nodiscard]] constexpr auto propagate() noexcept -> result<U>
  {
    return {std::move(std::get<error_type>(_value))};
  }

  value_type _value;
};
}  // namespace pinecone::util
