#include <cassert>
#include <variant>

#include <pinecone/pinecone.hpp>
#include <pinecone/util/logging.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "metadata_filtering.hpp"

int main(int argc, char** argv)
{
  assert(argc == 3);
  pinecone::util::setup_logger([](auto const& name) { return spdlog::stdout_color_mt(name); });

  auto client = std::get<0>(pinecone::synchronous_client::build({argv[1], argv[2]}));
  examples::run_metadata_example(client);
  return 0;
}