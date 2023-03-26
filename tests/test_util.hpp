#pragma once

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <string>
#include <thread>

#include "catch2/catch_message.hpp"
#include "catch2/catch_test_macros.hpp"
#include "pinecone/pinecone.hpp"
#include "pinecone/types/index_types.hpp"
#include "pinecone/util/result.hpp"

namespace pinecone::tests
{
static inline auto build_sync_client() -> synchronous_client
{
  auto const* api_key = std::getenv("PINECONE_TEST_API_KEY");
  assert(api_key != nullptr);
  auto client = synchronous_client::build({"us-west1-gcp", api_key});
  assert(client);
  return std::move(*client);
}

static constexpr size_t kDefaultMaxAttempts = 300;

template <size_t MaxAttempts = kDefaultMaxAttempts>
static inline auto ensure_index(synchronous_client const& client, types::new_index index)
    -> types::database
{
  using namespace std::chrono_literals;

  auto index_name = index.index_name();
  auto create_index_result = client.create_index(std::move(index));
  INFO(create_index_result.to_string());
  REQUIRE(create_index_result.is_successful());

  size_t attempts = 0;
  pinecone::util::result<pinecone::types::database> index_result;
  while (attempts < MaxAttempts) {
    index_result = client.describe_index(index_name);
    REQUIRE(index_result.is_successful());
    if (index_result->db_status().db_ready()) {
      return *index_result;
    }
    attempts++;
    std::this_thread::sleep_for(1s);
  }

  FAIL("Index still not ready after reaching maximum description attempts");
  return *index_result;
}

struct test_index {
  test_index(synchronous_client const& client, types::new_index index) noexcept : _client(client)
  {
    using namespace std::chrono_literals;
    auto _ = _client.delete_index(index.index_name());
    pinecone::util::result<pinecone::types::database> index_result;
    do {
      index_result = client.describe_index(index.index_name());
      std::this_thread::sleep_for(1s);
    } while (index_result.is_successful());

    _index = ensure_index(client, std::move(index));
    // This is dumb, but occasionally Pinecone 503's if we query too quickly after indices report
    // they're ready
    std::this_thread::sleep_for(2s);
  }

  ~test_index() noexcept
  {
    auto delete_index_result = _client.delete_index(_index.db_detail().db_name());
    INFO(delete_index_result.to_string());
    REQUIRE(delete_index_result.is_successful());
  }

  test_index(test_index const&) = delete;
  test_index(test_index&&) = delete;
  auto operator=(test_index const&) -> test_index& = delete;
  auto operator=(test_index&&) -> test_index& = delete;

  [[nodiscard]] auto index() const noexcept -> types::database const& { return _index; }

 private:
  synchronous_client const& _client;
  types::database _index;
};
}  // namespace pinecone::tests