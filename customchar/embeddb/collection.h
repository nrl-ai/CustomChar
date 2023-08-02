#ifndef CUSTOMCHAR_EMBEDDB_COLLECTION_H_
#define CUSTOMCHAR_EMBEDDB_COLLECTION_H_

#include <string>
#include <vector>
#include "customchar/embeddb/document.h"

namespace CC {
namespace embeddb {

class Collection {
 private:
  // Collection name.
  std::string name_;

  // Collection path.
  std::string path_;

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
}  // namespace embeddb
}  // namespace CC

#endif  // CUSTOMCHAR_EMBED_SEARCH_COLLECTION_H_