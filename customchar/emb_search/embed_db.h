#pragma once

#include <string>
#include <vector>
#include "collection.h"
namespace CC {
namespace embeddb {

class EmbedDB {
 private:
  // Database path.
  std::string db_path;

  // HNSW index path.
  std::string hnsw_path;

  // Document database (SQLite).
  std::string doc_db_path;

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
