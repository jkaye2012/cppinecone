#include <cstdlib>

#include <catch2/catch_all.hpp>

#include "pinecone/pinecone.hpp"
#include "test_util.hpp"

TEST_CASE("Synchronous client API metadata operations", "[metadata][synchronous]")
{
  auto client = pinecone::tests::build_sync_client();
}
