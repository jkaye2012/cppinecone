#pragma once
/**
 * @file operation_type.hpp
 * @brief Operations made available by the Pinecone API.
 */

#include "pinecone/domain/method.hpp"

namespace pinecone::domain
{
/**
 * @brief All operation types exposed by the Pinecone REST API.
 *
 * @details
 * Operation types can be understood within three general categories:
 *
 * - Index operations
 * - Collection operations
 * - Vector operations
 */
enum class operation_type {
  actions_whoami,

  index_create,
  index_configure,
  index_list,
  index_describe,
  index_delete,

  collection_create,
  collection_list,
  collection_describe,
  collection_delete,

  vector_upsert,
  vector_update,
  vector_query,
  vector_fetch,
  vector_describe_index_stats,
  vector_delete
};

/**
 * @brief The API types supported by the Pinecone REST API.
 *
 * @details
 * The Pinecone API is structured as two basic APIs:
 *
 * - The controller API, which allows users to create, configure, and delete indexes and collections
 * - The vector API, which allows users to upsert, update, query, and delete vectors
 *
 * Each `operation_type` is associated with one of these two APIs.
 */
enum class api_type {
  controller,
  service
};

/**
 * @brief Returns the API type of an operation.
 *
 * @param op the operation to query
 * @return the API type for the operation
 */
constexpr auto op_api_type(operation_type op) -> api_type
{
  switch (op) {
    case operation_type::actions_whoami:
    case operation_type::index_create:
    case operation_type::index_list:
    case operation_type::index_configure:
    case operation_type::index_describe:
    case operation_type::index_delete:
    case operation_type::collection_create:
    case operation_type::collection_list:
    case operation_type::collection_describe:
    case operation_type::collection_delete:
      return api_type::controller;
    case operation_type::vector_upsert:
    case operation_type::vector_update:
    case operation_type::vector_query:
    case operation_type::vector_fetch:
    case operation_type::vector_describe_index_stats:
    case operation_type::vector_delete:
      return api_type::service;
  }
}

/**
 * @brief Returns the URL specialization for an operation
 *
 * @param op the operation to query
 * @return the URL suffix for the operation
 */
constexpr auto op_url_fragment(operation_type op) -> char const*
{
  switch (op) {
    case operation_type::actions_whoami:
      return "/actions/whoami";
    case operation_type::index_create:
    case operation_type::index_list:
      return "/databases";
    case operation_type::index_configure:
    case operation_type::index_describe:
    case operation_type::index_delete:
      return "/databases/";
    case operation_type::collection_create:
    case operation_type::collection_list:
      return "/collections";
    case operation_type::collection_describe:
    case operation_type::collection_delete:
      return "/collections/";
    case operation_type::vector_upsert:
      return "/vectors/upsert";
    case operation_type::vector_update:
      return "/vectors/update";
    case operation_type::vector_query:
      return "/query";
    case operation_type::vector_fetch:
      return "/vectors/fetch";
    case operation_type::vector_describe_index_stats:
      return "/describe_index_stats";
    case operation_type::vector_delete:
      return "/vectors/delete";
  }
}

/**
 * @brief Returns the HTTP method for an operation.
 *
 * @param op the operation to query
 * @return the HTTP method for the operation
 */
constexpr auto op_method(operation_type op) -> method
{
  switch (op) {
    case operation_type::index_create:
    case operation_type::collection_create:
    case operation_type::vector_upsert:
    case operation_type::vector_update:
    case operation_type::vector_query:
    case operation_type::vector_describe_index_stats:
    case operation_type::vector_delete:
      return method::post;
    case operation_type::actions_whoami:
    case operation_type::index_list:
    case operation_type::index_describe:
    case operation_type::collection_list:
    case operation_type::collection_describe:
    case operation_type::vector_fetch:
      return method::get;
    case operation_type::index_configure:
      return method::patch;
    case operation_type::index_delete:
    case operation_type::collection_delete:
      return method::del;
  }
}
}  // namespace pinecone::domain