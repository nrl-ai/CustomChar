#ifndef CUSTOMCHAR_EMBEDDB_COLLECTION_H_
#define CUSTOMCHAR_EMBEDDB_COLLECTION_H_

#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <vector>

#include "customchar/embeddb/document.h"
#include "customchar/embeddb/embed_search.h"
#include "customchar/embeddb/types.h"

namespace CC {
namespace embeddb {

class Collection {
 private:
  // Collection name.
  std::string name;
  // Collection path.
  std::string path;

  // HNSW index path.
  std::string hnsw_path;
  uint32_t dim;
  uint32_t max_size;
  EmbedSearch* embed_search;

  // sqlite3 path.
  std::string sqlite3_path;
  SQLite::Database* db;

  void init_hnsw();
  void init_sqlite3();

 public:
  Collection(const std::string& name, const std::string& path,
             const uint32_t dim, const uint32_t max_size);
  u_int32_t get_doc_count();
  int get_dim();
  u_int32_t insert_doc(std::vector<float> doc_embedding,
                       const std::string& content, const std::string& meta,
                       u_int32_t original_doc_id, u_int32_t original_pos_line,
                       u_int32_t original_pos_col);

  void search(std::vector<float> query_embedding, int top_k, float threshold,
              std::vector<u_int32_t>& doc_ids, std::vector<float>& distances);

  std::vector<Document> get_docs_by_ids(std::vector<u_int32_t> doc_ids,
                                        int limit = -1, int offset = 0);

  Document get_doc(u_int32_t doc_id);
  void delete_doc(u_int32_t doc_id);
  void delete_all_docs();

  std::vector<Document> get_docs(int limit = -1, int offset = 0);
};
}  // namespace embeddb
}  // namespace CC
#endif  // COLLECTION_H_
