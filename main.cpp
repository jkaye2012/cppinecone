#include <iostream>

#include "pinecone/pinecone.hpp"

int main(int argc, char** argv)
{
  auto client = pinecone::synchronous_client::build({"us-west1-gcp", argv[1]});

  auto indexes = client->list_indexes();
  std::cout << "Num indexes: " << indexes->names().size() << std::endl;

  auto collections = client->list_collections();
  std::cout << "Num collections: " << collections->names().size() << std::endl;

  auto deleted = client->delete_collection("squad-test-collection");
  std::cout << "Deleted: " << *deleted << std::endl;

  for (auto const& idx : indexes->names()) {
    std::cout << idx << std::endl;
  }

  auto db = client->describe_index("squad");
  std::cout << db->dimension() << std::endl;
}
