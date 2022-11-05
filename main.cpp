#include <iostream>

#include "pinecone/net/http_client.hpp"
#include "pinecone/pinecone.hpp"

int main(int argc, char** argv)
{
  auto client = pinecone::synchronous_client::build({"us-west1-gcp", argv[1]});

  auto indexes = client->list_indexes();
  std::cout << "Num indexes: " << indexes->names().size() << std::endl;
  for (auto const& idx : indexes->names()) {
    std::cout << idx << std::endl;
  }
}
