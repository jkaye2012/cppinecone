#pragma once

namespace pinecone::domain
{
/**
 * @brief The API methods supported by the Pinecone API.
 *
 * @detail
 * Each `operation` is associated with exacty one `method`.
 *
 */
enum class method {
  get,
  post,
  patch,
  del
};
}  // namespace pinecone::domain
