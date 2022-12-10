#include <array>
#include <iostream>

#include "pinecone/pinecone.hpp"
#include "pinecone/types/filters.hpp"
#include "pinecone/types/index_types.hpp"
#include "pinecone/types/vector_metadata.hpp"
#include "pinecone/types/vector_types.hpp"

namespace pf = pinecone::types::filters;

auto main(int /*argc*/, char** argv) -> int
{
  auto client = pinecone::synchronous_client::build({"us-west1-gcp", argv[1]});
  if (!client) {
    std::cerr << "Failed to build client" << std::endl;
    return 1;
  }

  auto bin =
      pinecone::types::binary_filter("title", pinecone::types::binary_operator::eq, "Physics");
  auto arr = pinecone::types::array_filter<std::array<pinecone::types::metadata_value, 2>>(
      "title", pinecone::types::array_operator::in, {"Nutrition", "Health"});
  auto and_ =
      pinecone::types::combination_filter(pinecone::types::combination_operator::and_, bin, arr);

  json j = and_;
  std::cout << j.dump() << std::endl;

  auto example2 = pf::and_(pf::eq("title", "Physics"), pf::eq("author", "Bob"));
  json je = example2;
  std::cout << je.dump() << std::endl;

  auto indexes = client->list_indexes();
  std::cout << "Num indexes: " << indexes->size() << std::endl;

  auto collections = client->list_collections();
  std::cout << "Num collections: " << collections->size() << std::endl;

  pinecone::types::index_configuration config{
      1, pinecone::types::pod_configuration(pinecone::types::pod_type::s1,
                                            pinecone::types::pod_size::x1)};
  json jc = config;
  auto configure_result = client->configure_index("squad", config);
  std::cout << jc.dump() << std::endl;
  std::cout << configure_result.to_string() << std::endl;

  auto unfiltered_stats = client->describe_index_stats("squad", pf::none());
  std::cout << unfiltered_stats.to_string() << std::endl;
  std::cout << "Unfiltered: " << unfiltered_stats->stat_namespaces().at("").vector_count()
            << std::endl;

  auto filtered_stats = client->describe_index_stats("squad", pf::eq("title", "Nutrition"));
  std::cout << "Filtered: " << filtered_stats->stat_namespaces().at("").vector_count() << std::endl;

  pinecone::types::metadata update_md{{{"Testing", "A thing"}}};
  auto update_req =
      pinecone::types::update_request::builder("11113").with_metadata(std::move(update_md)).build();
  json ju = update_req;
  std::cout << ju.dump() << std::endl;
  auto updated = client->update_vector("squad", std::move(update_req));
  std::cout << "Updated: " << updated.to_string() << std::endl;

  auto q = pinecone::types::query_builder(1, "11113").with_include_metadata(true).build();
  auto search_result = client->query("squad", q);
  json jq = q;
  std::cout << "Query request: " << jq.dump() << std::endl;
  std::cout << "Query result: " << search_result.to_string() << std::endl;
  std::cout << "Query result metdata:" << std::endl;
  for (auto const& [k, v] : search_result->query_matches().at(0).md()->values()) {
    json jv = v;
    std::cout << k << ": " << jv.dump() << std::endl;
  }

  // auto delete_result = client->delete_index("squad");
  // std::cout << delete_result.to_string() << std::endl;

  // pinecone::types::new_index::builder index_builder("squad-new", 128);
  // auto create_result = client->create_index(index_builder.build());
  // std::cout << create_result.to_string() << std::endl;

  // auto deleted = client->delete_collection("squad-test-collection");
  // std::cout << "Deleted: " << deleted.to_string() << std::endl;

  // auto created = client->create_collection({"squad-test-collection", "squad"});
  // std::cout << "Created: " << created.to_string() << std::endl;

  for (auto const& idx : *indexes) {
    std::cout << idx << std::endl;
  }

  auto db = client->describe_index("squad");
  std::cout << db.to_string() << std::endl;
  std::cout << db->db_detail().db_dimension() << std::endl;

  // auto del_req = pinecone::types::delete_request<>::builder().build();
  // auto del_res = client->delete_vectors("squad", del_req);
  // std::cout << del_res.to_string() << std::endl;
}
