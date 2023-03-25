#include <cstdlib>
#include <vector>

#include <catch2/catch_all.hpp>

#include "pinecone/pinecone.hpp"
#include "test_util.hpp"

static constexpr auto kTestIndex = "cppinecone-test-vector-index";
static constexpr auto kTestDimension = 3;

TEST_CASE("Synchronous client basic vector operations", "[vector][basic][synchronous]")
{
  auto client = pinecone::tests::build_sync_client();
  auto _ = pinecone::tests::test_index(
      client, pinecone::types::new_index::builder(kTestIndex, kTestDimension).build());

  std::vector<pinecone::types::vector> vecs = {
      {"1", {0, 0, 0}},
      {"2", {1, 1, 1}},
      {"3", {.5, .5, .5}},
  };
  auto upsert_result = client.upsert_vectors(
      kTestIndex, pinecone::types::upsert_request::builder(std::move(vecs)).build());
  INFO(upsert_result.to_string());
  CHECK(upsert_result.is_successful());
}