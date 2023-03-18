#include <cstdlib>

#include <catch2/catch_all.hpp>

#include "catch2/catch_message.hpp"
#include "pinecone/pinecone.hpp"
#include "pinecone/types/index_types.hpp"
#include "test_util.hpp"

TEST_CASE("Synchronous client API metadata operations", "[metadata][synchronous]")
{
  auto client = pinecone::tests::build_sync_client();
  auto metadata = client.get_api_metdata();
  CHECK(metadata.is_successful());
  CHECK(metadata->md_project_name() == "7c6ab97");
  CHECK(metadata->md_user_name() == "e2ff7f9");
  CHECK(metadata->md_user_label() == "default");
}

TEST_CASE("Create and delete a new index", "[metadata][synchronous]")
{
  auto client = pinecone::tests::build_sync_client();
  auto _ = client.delete_index("test_index").is_successful();
  auto result = client.create_index(pinecone::types::new_index::builder("test-index", 10).build());

  INFO(result.to_string());
  CHECK(result.is_successful());
  REQUIRE(client.delete_index("test_index").is_successful());
}