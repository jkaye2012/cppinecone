#include <cstdlib>

#include <catch2/catch_all.hpp>

#include "pinecone/pinecone.hpp"
#include "test_util.hpp"

TEST_CASE("Synchronous client API metadata operations", "[meta][synchronous]")
{
  auto client = pinecone::tests::build_sync_client();
  auto metadata = client.get_api_metdata();
  CHECK(metadata.is_successful());
  CHECK(metadata->md_project_name() == "7c6ab97");
  CHECK(metadata->md_user_name() == "e2ff7f9");
  CHECK(metadata->md_user_label() == "default");
}