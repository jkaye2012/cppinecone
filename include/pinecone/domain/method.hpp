#pragma once
/**
 * @brief HTTP method definitions.
 */

namespace pinecone::domain
{
/**
 * @brief The API methods supported by the Pinecone API.
 *
 * @details
 * Each `operation` is associated with exactly one `method`.
 *
 */
enum class method {
  get,
  post,
  patch,
  del
};
}  // namespace pinecone::domain
