#include <iostream>

#include "pinecone/pinecone.hpp"
#include "pinecone/types/index_types.hpp"

int main(int argc, char** argv)
{
  auto client = pinecone::synchronous_client::build({"us-west1-gcp", argv[1]});
  if (!client) {
    std::cerr << "Failed to build client" << std::endl;
    return 1;
  }

  auto indexes = client->list_indexes();
  std::cout << "Num indexes: " << indexes->names().size() << std::endl;

  auto collections = client->list_collections();
  std::cout << "Num collections: " << collections->names().size() << std::endl;

  auto configure_result = client->configure_index("squad", {1, "s1"});
  std::cout << configure_result.to_string() << std::endl;

  auto stats = client->describe_index_stats("squad");
  std::cout << "Described stats: " << stats.to_string() << std::endl;

  // auto delete_result = client->delete_index("squad");
  // std::cout << delete_result.to_string() << std::endl;

  // pinecone::types::new_index::builder index_builder("squad-new", 128);
  // auto create_result = client->create_index(index_builder.build());
  // std::cout << create_result.to_string() << std::endl;

  // auto deleted = client->delete_collection("squad-test-collection");
  // std::cout << "Deleted: " << deleted.to_string() << std::endl;

  // auto created = client->create_collection({"squad-test-collection", "squad"});
  // std::cout << "Created: " << created.to_string() << std::endl;

  for (auto const& idx : indexes->names()) {
    std::cout << idx << std::endl;
  }

  auto db = client->describe_index("squad");
  std::cout << db->dimension() << std::endl;
}
