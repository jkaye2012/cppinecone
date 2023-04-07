#pragma once
/**
 * @file logging.hpp
 * @brief Provides logging capability.
 */

#include <functional>
#include <memory>
#include <string>

#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

namespace pinecone::util
{
static constexpr auto* kCppineconeLoggerName = "cppinecone";
using Logger = std::shared_ptr<spdlog::logger>;

/**
 * @brief Retrieves the logger to use for Cppinecone logging.
 * @details setup_logger should be called before this function is used; otherwise, no logger will be
 * configured, and written logs will not be persisted.
 *
 * @return a logger
 */
[[nodiscard]] inline auto logger() noexcept -> Logger { return spdlog::get(kCppineconeLoggerName); }

/**
 * @brief Configure the logger to user for Cppinecone logging.
 * @details Should be called once before Cppinecone functionality is utilized. Applies for the
 * lifetime of the application.
 *
 * @param builder a function to use for instrumentation of the Cppinecone logger.
 * @return a logger
 */
inline auto setup_logger(std::function<Logger(std::string const&)> const& builder) noexcept
    -> Logger
{
  return builder(kCppineconeLoggerName);
}
}  // namespace pinecone::util