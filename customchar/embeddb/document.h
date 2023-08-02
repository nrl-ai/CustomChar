#ifndef CUSTOMCHAR_EMBEDDB_DOCUMENT_H_
#define CUSTOMCHAR_EMBEDDB_DOCUMENT_H_

#include <string>
#include <vector>

namespace CC {
namespace embeddb {

class Document {
 private:
  // Document ID.
  u_int32_t id_;

  // Document embedding.
  std::vector<float> embedding_;

  // Document content.
  std::string content_;

  // Document metadata (JSON).
  std::string meta_;

  // Original document ID.
  u_int32_t original_doc_id_;

  // Position of the document in the original file (line).
  u_int32_t original_pos_line_;

  // Position of the document in the original file (column).
  u_int32_t original_pos_col_;

 public:
  Document();
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
}  // namespace embeddb
}  // namespace CC

#endif  // CUSTOMCHAR_EMBED_SEARCH_COLLECTION_H_
