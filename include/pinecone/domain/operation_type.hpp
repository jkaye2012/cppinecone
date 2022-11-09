#pragma once

#include "method.hpp"
namespace pinecone::domain
{ /**
   * @brief All operation_types exposed by the Pinecone REST API.
   *
   * @details
   * Operation_Types can be understood within three general categories:
   *
   * - Index operation_types
   * - Collection operation_types
   * - Vector operation_types
   */
enum class operation_type {
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
  vector_describe_all_index_stats,
  vector_describe_filtered_index_stats,
  vector_delete
};

constexpr auto op_url_fragment(operation_type op) -> char const*
{
  switch (op) {
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
    case operation_type::vector_describe_all_index_stats:
    case operation_type::vector_describe_filtered_index_stats:
      return "/describe_index_stats";
    case operation_type::vector_delete:
      return "/vectors/delete";
  }
}

constexpr auto op_method(operation_type op) -> method
{
  switch (op) {
    case operation_type::index_create:
    case operation_type::collection_create:
    case operation_type::vector_upsert:
    case operation_type::vector_update:
    case operation_type::vector_query:
    case operation_type::vector_describe_filtered_index_stats:
    case operation_type::vector_delete:
      return method::post;
    case operation_type::index_list:
    case operation_type::index_describe:
    case operation_type::collection_list:
    case operation_type::collection_describe:
    case operation_type::vector_fetch:
    case operation_type::vector_describe_all_index_stats:
      return method::get;
    case operation_type::index_configure:
      return method::patch;
    case operation_type::index_delete:
    case operation_type::collection_delete:
      return method::del;
  }
}
}  // namespace pinecone::domain