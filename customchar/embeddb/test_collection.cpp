#include <iostream>
#include <vector>
#include "collection.h"
#include "document.h"
#include "embed_search.h"
using namespace CC::embeddb;
int main() {
  Collection* collection = new Collection("test", "test", 10, 1000);

  // std::vector<float> embed;
  // for (int i = 0; i < 10; i++) {
  //   embed.push_back(i);
  // }
  // std::string content = "test content";
  // std::string meta = "test meta";
  // u_int32_t id = collection->insert_doc(embed, content, meta, 1, 1, 1);
  // std::cout << "insert id: " << id << std::endl;

  // Document doc = collection->get_doc(1);
  // std::vector<float> embed = doc.get_embedding();
  // for (int i = 0; i < 10; i++) {
  //   std::cout << embed[i] << std::endl;
  // }

  std::vector<u_int32_t> ids{0, 1};

  std::vector<Document> docs = collection->get_docs_by_ids(ids, 2);
  std::cout << docs.size() << std::endl;
  return 0;
}
