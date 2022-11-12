#pragma once

#include "pinecone/domain/operation.hpp"
#include "pinecone/domain/operation_type.hpp"
#include "pinecone/types/accepted.hpp"
#include "pinecone/types/parser.hpp"
#include "pinecone/types/vector_types.hpp"

namespace pinecone::domain
{
template <>
struct operation_args<operation_type::vector_describe_index_stats>
    : public vector_operation_args<operation_type::vector_describe_index_stats,
                                   types::metadata_filter>,
      public types::parser<types::index_stats> {
  using vector_operation_args::vector_operation_args;
};

}  // namespace pinecone::domain