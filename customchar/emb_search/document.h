#pragma once

#include <string>
#include <vector>

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
}  // namespace embeddb
}  // namespace CC