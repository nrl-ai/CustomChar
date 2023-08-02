#ifndef CUSTOMCHAR_EMBEDDB_EMBEDDB_H_
#define CUSTOMCHAR_EMBEDDB_EMBEDDB_H_

#include <string>
#include <vector>

#include "customchar/embeddb/collection.h"

namespace CC {
namespace embeddb {

class EmbedDB {
 private:
  // Database path.
  std::string db_path_;

  // HNSW index path.
  std::string hnsw_path_;

  // Document database (SQLite).
  std::string doc_db_path_;

 public:
  // Database path. Containing:
  // + HNSW index path.
  // + Document database (SQLite).
  EmbedDB(const std::string& db_path);

  void create_collection(const std::string& collection_name);
  Collection get_collection(const std::string& collection_name);
  void delete_collection(const std::string& collection_name);
  bool has_collection(const std::string& collection_name);
};

}  // namespace embeddb
}  // namespace CC

#endif  // CUSTOMCHAR_EMBEDDB_EMBED_DB_H_
