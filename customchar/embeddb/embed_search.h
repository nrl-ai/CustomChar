#ifndef CUSTOMCHAR_EMBEDDB_EMBED_SEARCH_H_
#define CUSTOMCHAR_EMBEDDB_EMBED_SEARCH_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "customchar/embeddb/types.h"
#include "hnswlib/hnswlib.h"

namespace CC {
namespace embeddb {

class EmbedSearch {
 private:
  // HNSW index path.
  std::string hnsw_path_;
  uint32_t dim_;
  uint32_t max_size_;
  std::shared_ptr<hnswlib::L2Space> space_;
  hnswlib::HierarchicalNSW<float>* alg_hnsw_;

  void save_data();
  bool load_data();
  bool check_exists_file(const std::string& name);

 public:
  // + HNSW index path.
  EmbedSearch(const std::string& hnsw_path, const uint32_t dim,
              const uint32_t max_size);
  ~EmbedSearch();
  uint32_t get_current_index();

  void insert_embeds(const std::vector<std::vector<float>>& embeds,
                     std::vector<uint32_t>& ids);
  void insert_embed(const std::vector<float>& embed, uint32_t& id);
  std::vector<float> get_embed(uint32_t id);
  void remove_embed(uint32_t id);
  void remove_embeds(const std::vector<uint32_t>& ids);
  void search_embed(const std::vector<float>& embed,
                    std::vector<SearchEmbedResult>& results, uint32_t top_k,
                    float threshold);
  void search_embeds(const std::vector<std::vector<float>>& embeds,
                     std::vector<std::vector<SearchEmbedResult>>& results,
                     uint32_t top_k, float threshold);
};

}  // namespace embeddb
}  // namespace CC

#endif
