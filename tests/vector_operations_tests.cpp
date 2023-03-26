#include <cstdlib>
#include <string_view>
#include <vector>

#include <catch2/catch_all.hpp>

#include "pinecone/pinecone.hpp"
#include "pinecone/types/vector_types.hpp"
#include "test_util.hpp"

static constexpr auto kTestIndex = "cppinecone-test-vector-index";
static constexpr auto kTestMdIndex = "cppinecone-test-vector-metadata-index";
static constexpr auto kTestDimension = 3;

namespace
{
inline void test_query(pinecone::synchronous_client const& client,
                       std::string_view expected) noexcept
{
  static const std::vector<double> query_vec{-1, -1, -1};
  auto query_result =
      client.query(kTestIndex, pinecone::types::query_builder(1, query_vec).build());
  INFO(query_result.to_string());
  CHECK(query_result.is_successful());
  CHECK(query_result->query_matches().size() == 1);
  CHECK(query_result->query_matches().front().id() == expected);
}

inline void test_md_query(pinecone::synchronous_client const& client,
                          std::string_view expected) noexcept
{
  static const std::vector<double> query_vec{-1, -1, -1};
  auto query_result = client.query(
      kTestMdIndex,
      pinecone::types::query_builder(pinecone::types::filters::ne("skipme", "true"), 1, query_vec)
          .build());
  INFO(query_result.to_string());
  CHECK(query_result.is_successful());
  CHECK(query_result->query_matches().size() == 1);
  CHECK(query_result->query_matches().front().id() == expected);
}
}  // namespace

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

  auto stats_result = client.describe_index_stats(kTestIndex);
  INFO(stats_result.to_string());
  CHECK(stats_result.is_successful());
  CHECK(stats_result->stat_total_vector_count() == 3);

  test_query(client, "1");

  auto delete_result =
      client.delete_vectors(kTestIndex, pinecone::types::delete_request<>::builder({"1"}).build());
  INFO(delete_result.to_string());
  CHECK(delete_result.is_successful());

  test_query(client, "3");

  auto update_result = client.update_vector(
      kTestIndex, pinecone::types::update_request::builder("2").with_values({0, 0, 0}).build());

  test_query(client, "2");
}

TEST_CASE("Synchronous client metadata vector operations", "[vector][metadata][synchronous]")
{
  auto client = pinecone::tests::build_sync_client();
  auto _ = pinecone::tests::test_index(
      client, pinecone::types::new_index::builder(kTestMdIndex, kTestDimension).build());

  std::vector<pinecone::types::vector> vecs = {
      {"1", {-1, -1, -1}},
      {"2", {1, 1, 1}},
      {"3", {.5, .5, .5}},
      {"4", {0, 0, 0}, pinecone::types::metadata({{"skipme", "true"}})},
  };
  auto upsert_result = client.upsert_vectors(
      kTestMdIndex, pinecone::types::upsert_request::builder(std::move(vecs)).build());
  INFO(upsert_result.to_string());
  CHECK(upsert_result.is_successful());

  test_md_query(client, "1");

  auto delete_result = client.delete_vectors(
      kTestMdIndex, pinecone::types::delete_request<>::builder({"1"}).build());
  INFO(delete_result.to_string());
  CHECK(delete_result.is_successful());

  test_md_query(client, "3");

  auto update_result = client.update_vector(
      kTestMdIndex, pinecone::types::update_request::builder("2").with_values({0, 0, 0}).build());

  test_md_query(client, "2");
}