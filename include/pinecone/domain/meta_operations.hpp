#pragma once

#include "pinecone/domain/operation.hpp"
#include "pinecone/domain/operation_type.hpp"
#include "pinecone/types/api_metadata.hpp"
#include "pinecone/types/parser.hpp"

namespace pinecone::domain
{
template <>
struct operation_args<operation_type::actions_whoami>
    : public list_operation_args<operation_type::actions_whoami>,
      public types::parser<types::api_metadata> {
  using list_operation_args::list_operation_args;
};
}  // namespace pinecone::domain