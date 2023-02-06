#pragma once
/**
 * @file arguments.hpp
 * @brief User-facing arguments for networking operations.
 */

#include <string>

namespace pinecone::net
{
/**
 * @brief Threading behaviors supported by `http_client`.
 */
enum class threading_mode {
  /**
   * @brief Synchronous operation; in this mode, `http_client` instances are
   *   _not_ thread-safe.
   */
  sync
};

/**
 * @brief Arguments required to connect to a Pinecone server.
 */
struct connection_args {
  /**
   * @brief Construct a new connection args object
   *
   * @param environment the cloud environment for the connection (e.g., us-west1-gcp)
   * @param api_key a Pinecone API key
   */
  connection_args(std::string environment, std::string api_key) noexcept
      : _environment(std::move(environment)),
        _api_key(std::move(api_key)),
        _api_key_header("Api-Key: " + _api_key)
  {
  }

  [[nodiscard]] auto environment() const noexcept -> std::string const& { return _environment; }

  [[nodiscard]] auto api_key() const noexcept -> std::string const& { return _api_key; }

  /**
   * @return a CURL compatible header
   */
  [[nodiscard]] auto api_key_header() const noexcept -> char const*
  {
    return _api_key_header.c_str();
  }

 private:
  std::string _environment;
  std::string _api_key;
  std::string _api_key_header;
};
}  // namespace pinecone::net