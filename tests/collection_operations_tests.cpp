#include <algorithm>
#include <chrono>
#include <thread>

#include <catch2/catch_all.hpp>

#include "pinecone/pinecone.hpp"
#include "test_util.hpp"

static constexpr auto kTestIndex = "cppinecone-test-collection-index";
static constexpr auto kTestCollection = "cppinecone-test-collection";
static constexpr auto kSourceCollection = "SQuAD";
static constexpr auto kTestDimension = 5;

using namespace std::chrono_literals;

TEST_CASE("Synchronous client collection operations", "[collection][synchronous]")
{
  auto client = pinecone::tests::build_sync_client();
  auto _ = pinecone::tests::test_index(
      client, pinecone::types::new_index::builder(kTestIndex, kTestDimension)
                  .with_pod_type({pinecone::types::pod_type::s1, pinecone::types::pod_size::x1})
                  .build());

  auto create_result = client.create_collection({kTestCollection, kTestIndex});
  INFO(create_result.to_string());
  REQUIRE(create_result.is_successful());

  auto list_result = client.list_collections();
  INFO(list_result.to_string());
  REQUIRE(list_result.is_successful());
  CHECK(std::any_of(list_result->begin(), list_result->end(),
                    [](auto const& col) { return col == kTestCollection; }));

  pinecone::util::result<pinecone::types::collection> collection_result;
  while (true) {
    collection_result = client.describe_collection(kTestCollection);
    // May fail until the collection is ready; bug in the Pinecone API itself (size field not sent)
    if (collection_result.is_successful() && collection_result->col_status() == "Ready") {
      break;
    }
    std::this_thread::sleep_for(1s);
  }
  CHECK(collection_result->col_name() == kTestCollection);
  CHECK(collection_result->col_size() > 0);

  auto delete_collection_result = client.delete_collection(kTestCollection);
  INFO(delete_collection_result.to_string());
  REQUIRE(delete_collection_result.is_successful());
}