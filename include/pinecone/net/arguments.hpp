#pragma once

#include <string>

namespace pinecone::net
{
/**
   @brief Threading behaviors supported by `http_client`.
*/
enum class threading_mode {
  /** @brief Synchronous operation; in this mode, `http_client` instances are
   *   _not_ thread-safe.
   */
  sync
};

struct connection_args {
  connection_args(std::string environment, std::string api_key) noexcept
      : _environment(std::move(environment)),
        _api_key(std::move(api_key)),
        _api_key_header("Api-Key: " + _api_key)
  {
  }

  [[nodiscard]] auto environment() const noexcept -> std::string const& { return _environment; }

  [[nodiscard]] auto api_key() const noexcept -> std::string const& { return _api_key; }

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