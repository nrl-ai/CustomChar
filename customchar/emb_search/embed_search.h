#ifndef EMBED_SEARCH_H_
#define EMBED_SEARCH_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "hnswlib.h"
#include "types.h"
namespace CC {
namespace embeddb {

class EmbedSearch {
 private:
  // HNSW index path.
  std::string hnsw_path;
  uint32_t dim;
  uint32_t max_size;
  std::shared_ptr<hnswlib::L2Space> space;
  hnswlib::HierarchicalNSW<float>* alg_hnsw;

  void save_data();
  bool load_data();
  bool check_exists_file(const std::string& name);
  uint32_t get_num_datas();

 public:
  // + HNSW index path.
  EmbedSearch(const std::string& hnsw_path, const uint32_t dim,
              const uint32_t max_size);
  ~EmbedSearch();

  void insert_embeds(const std::vector<std::vector<float>>& embeds,
                     std::vector<uint32_t>& ids);
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

#endif  // EMBED_SEARCH_H_