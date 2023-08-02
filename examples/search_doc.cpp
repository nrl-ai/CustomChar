#include <iostream>
#include <vector>

#include "customchar/embeddb/collection.h"
#include "customchar/embeddb/document.h"
#include "customchar/embeddb/embed_search.h"
#include "customchar/embeddb/types.h"
#include "customchar/llm/llm.h"

using namespace CC;
using namespace CC::embeddb;

std::vector<std::string> read_lines(const std::string& file_path) {
  std::vector<std::string> lines;

  // Open the file
  std::ifstream file(file_path);

  // Check if the file was opened successfully
  if (!file.is_open()) {
    std::cerr << "Error: Unable to open the file." << std::endl;
    return lines;
  }

  // Read line by line and add non-empty lines to the vector
  std::string line;
  while (std::getline(file, line)) {
    if (!line.empty()) {
      lines.push_back(line);
    }
  }

  // Close the file after reading
  file.close();

  return lines;
}

int main() {
  std::string model_path = "../models/llama-2-7b-chat.ggmlv3.q4_0.bin";
  llm::LLM embedding_model(model_path);
  embedding_model.eval_model();

  std::string connection_name = "test_collection";
  std::string path = "test_collection";
  int dim = 4096;
  int max_size = 1000;
  Collection* collection = new Collection(connection_name, path, dim, max_size);

  // Read the document from file
  std::string file_path = "../data/jarvis.txt";
  std::vector<std::string> lines = read_lines(file_path);

  // Insert all documents
  for (int i = 0; i < lines.size(); i++) {
    std::string content = lines[i];
    std::vector<float> embed = embedding_model.get_embedding(lines[i]);
    std::string meta = "test meta";
    std::cout << "Inserting document " << i << std::endl;
    std::cout << "Embedding size " << embed.size() << std::endl;
    std::cout << "Content size " << content.size() << std::endl;
    std::cout << "Content: " << content << std::endl;
    u_int32_t id = collection->insert_doc(embed, content, meta, 1, 1, 1);
  }

  while (true) {
    // Test Search
    std::string query_str;
    std::cout << "Enter query: ";
    std::getline(std::cin, query_str);
    std::vector<float> query = embedding_model.get_embedding(query_str);

    std::vector<u_int32_t> doc_ids;
    std::vector<float> distances;
    int top_k = 2;
    float threshold = 100000000;
    collection->search(query, top_k, threshold, doc_ids, distances);
    for (int i = 0; i < 10; ++i) {
      std::cout << query[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Search result: " << std::endl;
    for (int i = 0; i < doc_ids.size(); i++) {
      std::cout << "Doc id: " << doc_ids[i] << std::endl;
      std::cout << "Distance: " << distances[i] << std::endl;
      Document doc = collection->get_doc(doc_ids[i]);
      std::cout << "Content: " << doc.get_content() << std::endl;
    }
  }
  return 0;
}
