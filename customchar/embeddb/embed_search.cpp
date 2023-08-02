#include "customchar/embeddb/embed_search.h"

using namespace CC::embeddb;

EmbedSearch::EmbedSearch(const std::string& hnsw_path, const uint32_t dim,
                         const uint32_t max_size)
    : hnsw_path(hnsw_path), dim(dim), max_size(max_size) {
  space = std::make_shared<hnswlib::L2Space>(dim);
  alg_hnsw = new hnswlib::HierarchicalNSW<float>(&*space, max_size);
  load_data();
}

EmbedSearch::~EmbedSearch() {
  save_data();
  delete alg_hnsw;
}

void EmbedSearch::save_data() { alg_hnsw->saveIndex(hnsw_path); }

bool EmbedSearch::load_data() {
  if (!check_exists_file(hnsw_path)) return false;
  alg_hnsw->loadIndex(hnsw_path, &*space, max_size);
  std::cout << "Load data success with size: " << get_num_datas() << std::endl;
  return true;
}

bool EmbedSearch::check_exists_file(const std::string& name) {
  std::ifstream f(name.c_str());
  return f.good();
}

uint32_t EmbedSearch::get_num_datas() { return alg_hnsw->cur_element_count; }

void EmbedSearch::insert_embeds(const std::vector<std::vector<float>>& embeds,
                                std::vector<uint32_t>& ids) {
  uint32_t num_datas = get_num_datas();
  ids.resize(embeds.size());
  for (uint32_t i = 0; i < embeds.size(); i++) {
    ids[i] = num_datas + i;
  }
  for (uint32_t i = 0; i < embeds.size(); i++) {
    alg_hnsw->addPoint((void*)&embeds[i][0], ids[i]);
  }
  save_data();
}
void EmbedSearch::remove_embeds(const std::vector<uint32_t>& ids) {
  for (uint32_t i = 0; i < ids.size(); i++) {
    alg_hnsw->markDelete(ids[i]);
  }
  save_data();
}

void EmbedSearch::search_embed(const std::vector<float>& embed,
                               std::vector<SearchEmbedResult>& results,
                               uint32_t top_k, float threshold) {
  results.clear();
  uint32_t num_datas = get_num_datas();
  if (num_datas == 0) return;
  if (num_datas < top_k) top_k = num_datas;
  const void* p = embed.data();

  auto gd = alg_hnsw->searchKnn(p, top_k);
  while (!gd.empty()) {
    if (gd.top().first <= threshold) {
      SearchEmbedResult result;
      result.id = gd.top().second;
      result.score = gd.top().first;
      results.emplace_back(result);
    }
    gd.pop();
  }
}

void EmbedSearch::search_embeds(
    const std::vector<std::vector<float>>& embeds,
    std::vector<std::vector<SearchEmbedResult>>& results, uint32_t top_k,
    float threshold) {
  results.resize(embeds.size());
  uint32_t num_datas = get_num_datas();
  if (num_datas == 0) return;
  if (num_datas < top_k) top_k = num_datas;

  for (uint32_t i = 0; i < embeds.size(); i++) {
    search_embed(embeds[i], results[i], top_k, threshold);
  }
}
