#pragma once

#include <string>

namespace CC {
namespace embeddb {

class Document {
 private:
  // Document ID.
  u_int32_t id;

  // Document embedding.
  std::vector<float> embedding;

  // Document content.
  std::string content;

  // Document metadata (JSON).
  std::string meta;

  // Original document ID.
  u_int32_t original_doc_id;

  // Position of the document in the original file (line).
  u_int32_t original_pos_line;

  // Position of the document in the original file (column).
  u_int32_t original_pos_col;

 public:
  Document(u_int32_t id, std::vector<float> embedding,
           const std::string& content, const std::string& meta,
           u_int32_t original_doc_id, u_int32_t original_pos_line,
           u_int32_t original_pos_col);
  u_int32_t get_id();
  std::vector<float> get_embedding();
  std::string get_content();
  std::string get_meta();
  u_int32_t get_original_doc_id();
  u_int32_t get_original_pos_line();
  u_int32_t get_original_pos_col();
};

class Collection {
 private:
  // Collection name.
  std::string name;

  // Collection path.
  std::string path;

 public:
  u_int32_t get_doc_count();
  int get_dim();

  /// @brief Insert a document into the collection.
  /// @param doc_embedding Document embedding.
  /// @param content Document content.
  /// @param meta Document metadata (JSON).
  /// @param original_doc_id Original document ID.
  /// @param original_pos_line Position of the document in the original file
  /// (line).
  /// @param original_pos_col Position of the document in the original file
  /// (column).
  /// @return Document ID.
  u_int32_t insert_doc(std::vector<float> doc_embedding,
                       const std::string& content, const std::string& meta,
                       u_int32_t original_doc_id, u_int32_t original_pos_line,
                       u_int32_t original_pos_col);

  /// @brief Search for similar documents.
  /// @param query_embedding Query embedding.
  /// @param k Maximum number of results.
  /// @param doc_ids Found document IDs.
  /// @param distances Distances.
  bool search(std::vector<float> query_embedding, int k,
              std::vector<u_int32_t>& doc_ids, std::vector<float>& distances);

  /// @brief Get documents by IDs.
  /// @param doc_ids Document IDs.
  /// @return Documents.
  std::vector<Document> get_docs_by_ids(std::vector<u_int32_t> doc_ids,
                                        int limit = -1, int offset = 0);

  /// @brief Get document by ID.
  /// @param doc_id Document ID.
  /// @return Document.
  Document get_doc(u_int32_t doc_id);

  /// @brief Delete document by ID.
  /// @param doc_id Document ID.
  void delete_doc(u_int32_t doc_id);

  /// @brief Delete all documents.
  void delete_all_docs();

  /// @brief Get documents.
  /// @return Documents.
  std::vector<Document> get_docs(int limit = -1, int offset = 0);
};

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
