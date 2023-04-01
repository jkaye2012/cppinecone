#include <cassert>
#include <variant>

#include <pinecone/pinecone.hpp>

#include "metadata_filtering.hpp"

int main(int argc, char** argv)
{
  assert(argc == 3);
  auto client = std::get<0>(pinecone::synchronous_client::build({argv[1], argv[2]}));
  examples::run_metadata_example(client);
  return 0;
}