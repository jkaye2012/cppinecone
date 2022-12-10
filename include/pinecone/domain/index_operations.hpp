#pragma once

#include <string>
#include <vector>

#include "pinecone/domain/operation.hpp"
#include "pinecone/domain/operation_type.hpp"
#include "pinecone/types/accepted.hpp"
#include "pinecone/types/index_types.hpp"
#include "pinecone/types/parser.hpp"

namespace pinecone::domain
{
template <>
struct operation_args<operation_type::index_list>
    : public list_operation_args<operation_type::index_list>,
      public types::parser<std::vector<std::string>> {
  using list_operation_args::list_operation_args;
};

template <>
struct operation_args<operation_type::index_describe>
    : public describe_delete_operation_args<operation_type::index_describe>,
      public types::parser<types::database> {
  using describe_delete_operation_args::describe_delete_operation_args;
};

template <>
struct operation_args<operation_type::index_delete>
    : public describe_delete_operation_args<operation_type::index_delete>,
      public types::parser<types::accepted> {
  using describe_delete_operation_args::describe_delete_operation_args;
};

template <>
struct operation_args<operation_type::collection_list>
    : public list_operation_args<operation_type::collection_list>,
      public types::parser<std::vector<std::string>> {
  using list_operation_args::list_operation_args;
};

template <>
struct operation_args<operation_type::collection_describe>
    : public describe_delete_operation_args<operation_type::collection_describe>,
      public types::parser<types::collection> {
  using describe_delete_operation_args::describe_delete_operation_args;
};

template <>
struct operation_args<operation_type::collection_delete>
    : public describe_delete_operation_args<operation_type::collection_delete>,
      public types::parser<types::accepted> {
  using describe_delete_operation_args::describe_delete_operation_args;
};

template <>
struct operation_args<operation_type::index_configure>
    : public patch_operation_args<operation_type::index_configure, types::index_configuration>,
      public types::parser<types::accepted> {
  using patch_operation_args::patch_operation_args;
};

template <>
struct operation_args<operation_type::collection_create>
    : public create_operation_args<operation_type::collection_create, types::new_collection>,
      public types::parser<types::accepted> {
  using create_operation_args::create_operation_args;
};

template <>
struct operation_args<operation_type::index_create>
    : public create_operation_args<operation_type::index_create, types::new_index>,
      public types::parser<types::accepted> {
  using create_operation_args::create_operation_args;
};
}  // namespace pinecone::domain