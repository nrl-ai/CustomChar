#include <iostream>
#include <vector>

#include "customchar/embeddb/collection.h"
#include "customchar/embeddb/document.h"
#include "customchar/embeddb/embed_search.h"
#include "customchar/embeddb/types.h"

using namespace CC::embeddb;

int main() {
  std::string connection_name = "test_collection";
  std::string path = "test_collection";
  int dim = 10;
  int max_size = 1000;
  Collection* collection = new Collection(connection_name, path, dim, max_size);

  // Test Insert
  std::vector<float> embed;
  for (int i = 0; i < 10; i++) {
    embed.push_back(i);
  }
  std::string content = "test content";
  std::string meta = "test meta";
  u_int32_t id = collection->insert_doc(embed, content, meta, 1, 1, 1);
  std::cout << "Inserted document id: " << id << std::endl;

  // Test Get Doc From Ids
  std::vector<u_int32_t> ids{0, 1};
  std::vector<Document> docs = collection->get_docs_by_ids(ids, 2);
  std::cout << docs.size() << std::endl;

  // Test Search
  std::vector<float> query;
  for (int i = 0; i < 10; i++) {
    query.push_back(i);
  }
  std::vector<u_int32_t> doc_ids;
  std::vector<float> distances;
  int top_k = 2;
  float threshold = 100;
  collection->search(query, top_k, threshold, doc_ids, distances);
  for (int i = 0; i < doc_ids.size(); i++) {
    std::cout << doc_ids[i] << " " << distances[i] << std::endl;
  }

  return 0;
}
