#pragma once

#include <cassert>
#include <cstdint>
#include <iostream>

#include <pinecone/pinecone.hpp>

namespace examples
{
static inline void run_metadata_example(pinecone::synchronous_client const& client) noexcept
{
  auto comic_filter = pinecone::types::filters::eq("title", "Marvel_Comics");
  auto result =
      client.query("squad", pinecone::types::query_builder(comic_filter, 1000, "9833").build());
  assert(result.is_successful());
  std::cout << "Marvel comic results: " << result->query_matches().size() << std::endl;

  auto ps_filter = pinecone::types::filters::eq("title", "PlayStation_3");
  auto combined_filter = pinecone::types::filters::or_(comic_filter, ps_filter);
  auto combined_result =
      client.query("squad", pinecone::types::query_builder(combined_filter, 1000, "9833").build());
  assert(combined_result.is_successful());
  std::cout << "Marvel comic or playstation results: " << combined_result->query_matches().size()
            << std::endl;
}
}  // namespace examples