#pragma once

#include <cassert>
#include <cstdlib>
#include <memory>

#include "pinecone/pinecone.hpp"

namespace pinecone::tests
{
static inline auto build_sync_client() -> pinecone::synchronous_client
{
  auto const* api_key = std::getenv("PINECONE_TEST_API_KEY");
  assert(api_key != nullptr);
  auto client = pinecone::synchronous_client::build({"us-west1-gcp", api_key});
  assert(client);
  return std::move(*client);
}
}  // namespace pinecone::tests