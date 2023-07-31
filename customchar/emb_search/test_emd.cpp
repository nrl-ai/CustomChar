#include "embed_search.h"
using namespace CC::embeddb;
int main() {
  EmbedSearch* es = new EmbedSearch("test.hnsw", 10, 1000);
  std::vector<std::vector<float>> embeds;
  // init embeds 5x10
  for (int i = 0; i < 5; i++) {
    std::vector<float> embed;
    for (int j = 0; j < 10; j++) {
      embed.push_back(i * 10 + j);
    }
    embeds.push_back(embed);
  }
  // insert embeds
  std::vector<uint32_t> ids;
  es->insert_embeds(embeds, ids);
  for (int i = 0; i < ids.size(); i++) {
    std::cout << ids[i] << std::endl;
  }

  // search embed
  std::vector<SearchEmbedResult> results;
  std::vector<float> embed;
  for (int i = 0; i < 10; i++) {
    embed.push_back(i);
  }
  es->search_embed(embed, results, 10, 10);
  for (int i = 0; i < results.size(); i++) {
    std::cout << results[i].id << " " << results[i].score << std::endl;
  }

  return 0;
}