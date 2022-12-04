#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace pinecone::types
{

/**
 * @brief Pinecone API error details. Should only be accessed via api_error.
 */
struct error_detail {
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(error_detail, typeUrl, value)

  [[nodiscard]] auto type_url() const noexcept -> std::string const& { return typeUrl; }

  [[nodiscard]] auto detail_value() const noexcept -> std::string const& { return value; }

 private:
  std::string typeUrl;
  std::string value;
};

/**
 * @brief An error response returned by the Pinecone API.
 *
 * @details This is _not_ an implementation detail of Cppinecone; rather, the Pinecone API defines
 * the structure that errors are expected to take when returned by the server. Upon receiving an
 * error response from the server, Cppinecone will attempt to parse the result into this format. If
 * parsing fails, the error will fall back to its string representation, in which case no formatting
 * guarantees are made.
 *
 */
struct api_error {
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(api_error, code, message, details)

  [[nodiscard]] auto error_code() const noexcept -> int32_t { return code; }

  [[nodiscard]] auto error_message() const noexcept -> std::string const& { return message; }

  [[nodiscard]] auto error_details() const noexcept -> std::vector<error_detail> const&
  {
    return details;
  }

 private:
  int32_t code;
  std::string message;
  std::vector<error_detail> details;
};
}  // namespace pinecone::types