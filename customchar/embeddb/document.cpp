#include "customchar/embeddb/document.h"

using namespace CC::embeddb;

Document::Document(u_int32_t id, std::vector<float> embedding,
                   const std::string& content, const std::string& meta,
                   u_int32_t original_doc_id, u_int32_t original_pos_line,
                   u_int32_t original_pos_col)
    : id_(id),
      embedding_(embedding),
      content_(content),
      meta_(meta),
      original_doc_id_(original_doc_id),
      original_pos_line_(original_pos_line),
      original_pos_col_(original_pos_col) {}

u_int32_t Document::get_id() { return id_; }
std::vector<float> Document::get_embedding() { return embedding_; }
std::string Document::get_content() { return content_; }
std::string Document::get_meta() { return meta_; }
u_int32_t Document::get_original_doc_id() { return original_doc_id_; }
u_int32_t Document::get_original_pos_line() { return original_pos_line_; }
u_int32_t Document::get_original_pos_col() { return original_pos_col_; }
