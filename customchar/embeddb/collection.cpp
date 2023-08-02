#include "customchar/embeddb/collection.h"

using namespace CC::embeddb;

Collection::Collection(const std::string& name, const std::string& path,
                       const uint32_t dim, const uint32_t max_size)
    : name(name), path(path), dim(dim), max_size(max_size) {
  std::cout << "Collection::Collection" << std::endl;
  hnsw_path = path + "/" + name + ".hnsw";
  sqlite3_path = path + "/" + name + ".sqlite3";
  init_hnsw();
  init_sqlite3();
}

void Collection::init_hnsw() {
  embed_search = new EmbedSearch(hnsw_path, dim, max_size);
  std::cout << "HNSW index file '" << hnsw_path.c_str()
            << "' opened successfully\n";
}
void Collection::init_sqlite3() {
  db = new SQLite::Database(sqlite3_path, SQLite::OPEN_READWRITE |
                                              SQLite::OPEN_CREATE |
                                              SQLite::OPEN_FULLMUTEX);
  std::cout << "SQLite database file '" << db->getFilename().c_str()
            << "' opened successfully\n";
  db->exec(
      "CREATE TABLE IF NOT EXISTS documents ("
      "id INTEGER PRIMARY KEY,"
      "content TEXT,"
      "meta TEXT,"
      "original_doc_id INTEGER,"
      "original_pos_line INTEGER,"
      "original_pos_col INTEGER"
      ")");
}
int Collection::get_dim() { return dim; }
u_int32_t Collection::get_doc_count() {
  SQLite::Statement query(*db, "SELECT COUNT(*) FROM documents");
  query.executeStep();
  return query.getColumn(0);
}

u_int32_t Collection::insert_doc(std::vector<float> doc_embedding,
                                 const std::string& content,
                                 const std::string& meta,
                                 u_int32_t original_doc_id,
                                 u_int32_t original_pos_line,
                                 u_int32_t original_pos_col) {
  u_int32_t id_search;
  embed_search->insert_embed(doc_embedding, id_search);
  SQLite::Statement query(*db,
                          "INSERT INTO documents (id, content, meta, "
                          "original_doc_id, original_pos_line, "
                          "original_pos_col) VALUES (?, ?, ?, ?, ?, ?)");
  query.bind(1, id_search);
  query.bind(2, content);
  query.bind(3, meta);
  query.bind(4, original_doc_id);
  query.bind(5, original_pos_line);
  query.bind(6, original_pos_col);
  query.exec();
  return id_search;
}

Document Collection::get_doc(u_int32_t doc_id) {
  SQLite::Statement query(*db, "SELECT * FROM documents WHERE id = ?");
  query.bind(1, doc_id);
  query.executeStep();
  Document doc(query.getColumn(0), embed_search->get_embed(doc_id),
               query.getColumn(1), query.getColumn(2), query.getColumn(3),
               query.getColumn(4), query.getColumn(5));
  return doc;
}
std::vector<Document> Collection::get_docs_by_ids(
    std::vector<u_int32_t> doc_ids, int limit, int offset) {
  std::vector<Document> docs;
  std::string query_str = "SELECT * FROM documents WHERE id IN (";
  for (int i = 0; i < doc_ids.size(); i++) {
    query_str += std::to_string(doc_ids[i]);
    if (i < doc_ids.size() - 1) {
      query_str += ", ";
    }
  }
  query_str += ")";
  if (limit > 0) {
    query_str += " LIMIT " + std::to_string(limit);
  }
  if (offset > 0) {
    query_str += " OFFSET " + std::to_string(offset);
  }
  SQLite::Statement query(*db, query_str);
  while (query.executeStep()) {
    Document doc(query.getColumn(0),
                 embed_search->get_embed(query.getColumn(0)),
                 query.getColumn(1), query.getColumn(2), query.getColumn(3),
                 query.getColumn(4), query.getColumn(5));
    docs.push_back(doc);
  }
  return docs;
}

void Collection::delete_doc(u_int32_t doc_id) {
  embed_search->remove_embed(doc_id);
  SQLite::Statement query(*db, "DELETE FROM documents WHERE id = ?");
  query.bind(1, doc_id);
  query.exec();
}

void Collection::delete_all_docs() {
  std::vector<u_int32_t> ids;
  SQLite::Statement query(*db, "SELECT id FROM documents");
  while (query.executeStep()) {
    ids.push_back(query.getColumn(0));
  }
  embed_search->remove_embeds(ids);
  SQLite::Statement remove_query(*db, "DELETE FROM documents");
  remove_query.exec();
}

void Collection::search(std::vector<float> query_embedding, int top_k,
                        float threshold, std::vector<u_int32_t>& doc_ids,
                        std::vector<float>& distances) {
  std::vector<SearchEmbedResult> results;
  embed_search->search_embed(query_embedding, results, top_k, threshold);
  doc_ids.clear();
  distances.clear();
  doc_ids.resize(results.size());
  distances.resize(results.size());
  for (int i = 0; i < results.size(); i++) {
    doc_ids[i] = results[i].id;
    distances[i] = results[i].score;
  }
}
