#include "customchar/embeddb/embed_search.h"

using namespace CC::embeddb;

EmbedSearch::EmbedSearch(const std::string& hnsw_path, const uint32_t dim,
                         const uint32_t max_size)
    : hnsw_path_(hnsw_path), dim_(dim), max_size_(max_size) {
  space_ = std::make_shared<hnswlib::L2Space>(dim);
  alg_hnsw_ = new hnswlib::HierarchicalNSW<float>(&*space_, max_size);
  load_data();
  save_data();
}

EmbedSearch::~EmbedSearch() {
  save_data();
  delete alg_hnsw_;
}

void EmbedSearch::save_data() { alg_hnsw_->saveIndex(hnsw_path_); }

bool EmbedSearch::load_data() {
  if (!check_exists_file(hnsw_path_)) return false;
  alg_hnsw_->loadIndex(hnsw_path_, &*space_, max_size_);
  std::cout << "Load data success with current index" << get_current_index()
            << std::endl;
  return true;
}

bool EmbedSearch::check_exists_file(const std::string& name) {
  std::ifstream f(name.c_str());
  return f.good();
}
uint32_t EmbedSearch::get_current_index() {
  return alg_hnsw_->cur_element_count;
}

void EmbedSearch::insert_embed(const std::vector<float>& embed, uint32_t& id) {
  id = get_current_index();
  alg_hnsw_->addPoint((void*)&embed[0], id);
  save_data();
}

std::vector<float> EmbedSearch::get_embed(uint32_t id) {
  std::vector<float> embed;
  embed.resize(dim_);
  char* data = alg_hnsw_->getDataByInternalId(id);
  memcpy(&embed[0], data, dim_ * sizeof(float));
  return embed;
}

void EmbedSearch::insert_embeds(const std::vector<std::vector<float>>& embeds,
                                std::vector<uint32_t>& ids) {
  uint32_t current_index = get_current_index();
  ids.resize(embeds.size());
  for (uint32_t i = 0; i < embeds.size(); i++) {
    ids[i] = current_index + i;
  }
  for (uint32_t i = 0; i < embeds.size(); i++) {
    alg_hnsw_->addPoint((void*)&embeds[i][0], ids[i]);
  }
  save_data();
}
void EmbedSearch::remove_embeds(const std::vector<uint32_t>& ids) {
  for (uint32_t i = 0; i < ids.size(); i++) {
    alg_hnsw_->markDelete(ids[i]);
  }
  save_data();
}

void EmbedSearch::remove_embed(uint32_t id) {
  alg_hnsw_->markDelete(id);
  save_data();
}
void EmbedSearch::search_embed(const std::vector<float>& embed,
                               std::vector<SearchEmbedResult>& results,
                               uint32_t top_k, float threshold) {
  results.clear();
  uint32_t current_index = get_current_index();
  if (current_index == 0) return;
  if (current_index < top_k) top_k = current_index;
  const void* p = embed.data();

  auto gd = alg_hnsw_->searchKnn(p, top_k);
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
  uint32_t current_index = get_current_index();
  if (current_index == 0) return;
  if (current_index < top_k) top_k = current_index;

  for (uint32_t i = 0; i < embeds.size(); i++) {
    search_embed(embeds[i], results[i], top_k, threshold);
  }
}
