#include "customchar/embeddb/document.h"

using namespace CC::embeddb;

Document::Document(u_int32_t id, std::vector<float> embedding,
                   const std::string& content, const std::string& meta,
                   u_int32_t original_doc_id, u_int32_t original_pos_line,
                   u_int32_t original_pos_col)
    : id(id),
      embedding(embedding),
      content(content),
      meta(meta),
      original_doc_id(original_doc_id),
      original_pos_line(original_pos_line),
      original_pos_col(original_pos_col) {}

u_int32_t Document::get_id() { return id; }
std::vector<float> Document::get_embedding() { return embedding; }
std::string Document::get_content() { return content; }
std::string Document::get_meta() { return meta; }
u_int32_t Document::get_original_doc_id() { return original_doc_id; }
u_int32_t Document::get_original_pos_line() { return original_pos_line; }
u_int32_t Document::get_original_pos_col() { return original_pos_col; }
