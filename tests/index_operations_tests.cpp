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

  auto index = pinecone::tests::test_index(
      client, pinecone::types::new_index::builder(kTestIndex, kTestDimension)
                  .with_pod_type({pinecone::types::pod_type::s1, pinecone::types::pod_size::x1})
                  .build());

  CHECK(index.index().db_detail().db_dimension() == kTestDimension);
  CHECK(index.index().db_detail().db_replicas() == 1);
  CHECK(index.index().db_detail().db_shards() == 1);

  auto list_result = client.list_indexes();
  INFO(list_result.to_string());
  REQUIRE(list_result.is_successful());
  CHECK(std::any_of(list_result->begin(), list_result->end(),
                    [](auto const& ind) { return ind == kTestIndex; }));

  auto configure_result = client.configure_index(
      kTestIndex, pinecone::types::index_configuration(
                      2, {pinecone::types::pod_type::s1, pinecone::types::pod_size::x2}));
  INFO(configure_result.to_string());
  CHECK(configure_result.is_successful());

  auto describe_result = client.describe_index(kTestIndex);
  INFO(describe_result.to_string());
  REQUIRE(describe_result.is_successful());
  CHECK(describe_result->db_detail().db_replicas() == 2);
  CHECK(describe_result->db_detail().db_pod_type().size() == pinecone::types::pod_size::x2);
}