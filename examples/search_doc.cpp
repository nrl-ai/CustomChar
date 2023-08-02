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

  std::vector<float> embed;
  for (int i = 0; i < 10; i++) {
    embed.push_back(i);
  }
  std::string content = "test content";
  std::string meta = "test meta";
  u_int32_t id = collection->insert_doc(embed, content, meta, 1, 1, 1);
  std::cout << "Inserted document id: " << id << std::endl;

  std::vector<u_int32_t> ids{0, 1};
  std::vector<Document> docs = collection->get_docs_by_ids(ids, 2);
  std::cout << docs.size() << std::endl;

  return 0;
}
