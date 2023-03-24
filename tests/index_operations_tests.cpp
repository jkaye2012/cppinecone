#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <thread>

#include <catch2/catch_all.hpp>

#include "catch2/catch_message.hpp"
#include "pinecone/pinecone.hpp"
#include "pinecone/types/index_types.hpp"
#include "test_util.hpp"

static constexpr uint32_t kTestDimension = 4;
static constexpr auto* kTestIndex = "cppinecone-test-index";

using namespace std::chrono_literals;

TEST_CASE("Synchronous client index operations", "[index][synchronous]")
{
  auto client = pinecone::tests::build_sync_client();

  auto create_result = client.create_index(
      pinecone::types::new_index::builder(kTestIndex, kTestDimension)
          .with_pod_type({pinecone::types::pod_type::s1, pinecone::types::pod_size::x1})
          .build());
  INFO(create_result.to_string());
  REQUIRE(create_result.is_successful());

  auto list_result = client.list_indexes();
  INFO(list_result.to_string());
  CHECK(list_result.is_successful());
  CHECK(std::any_of(list_result->begin(), list_result->end(),
                    [](auto const& ind) { return ind == kTestIndex; }));

  pinecone::util::result<pinecone::types::database> describe_result;
  while (true) {
    describe_result = client.describe_index(kTestIndex);
    REQUIRE(describe_result.is_successful());
    if (describe_result->db_status().db_ready()) {
      break;
    }
    std::this_thread::sleep_for(1s);
  }
  CHECK(describe_result->db_detail().db_dimension() == kTestDimension);
  CHECK(describe_result->db_detail().db_replicas() == 1);
  CHECK(describe_result->db_detail().db_shards() == 1);

  auto configure_result = client.configure_index(
      kTestIndex, pinecone::types::index_configuration(
                      2, {pinecone::types::pod_type::s1, pinecone::types::pod_size::x2}));
  INFO(configure_result.to_string());
  CHECK(configure_result.is_successful());
  describe_result = client.describe_index(kTestIndex);
  INFO(describe_result.to_string());
  CHECK(describe_result.is_successful());
  CHECK(describe_result->db_detail().db_replicas() == 2);
  CHECK(describe_result->db_detail().db_pod_type().size() == pinecone::types::pod_size::x2);

  auto delete_result = client.delete_index(kTestIndex);
  INFO(delete_result.to_string());
  REQUIRE(delete_result.is_successful());
}