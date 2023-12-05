#ifndef CUSTOMCHAR_COMMON_HELPERS_H_
#define CUSTOMCHAR_COMMON_HELPERS_H_

#include <cassert>
#include <cstdio>
#include <fstream>
#include <regex>
#include <string>
#include <thread>
#include <vector>

#include "nlohmann/json.hpp"

namespace CC {
namespace common {

using json = nlohmann::json;

/// @brief Parameters for CustomChar
struct CCParams {
  float prob0;

  int32_t n_threads = std::min(4, (int32_t)std::thread::hardware_concurrency());
  int32_t voice_ms = 10000;
  int32_t capture_id = -1;
  int32_t max_tokens = 32;
  int32_t audio_ctx = 0;

  float vad_thold = 0.6f;
  float freq_thold = 100.0f;

  bool speed_up = false;
  bool translate = false;
  bool print_special = false;
  bool print_energy = false;
  bool no_timestamps = true;
  bool verbose_prompt = false;

  std::string person = "User";
  std::string bot_name = "JARVIS";
  std::string chat_symb = ":";
  std::string language = "en";
  std::string tts_model_path = "../models/ggml-base.bin";
  std::string llm_model_path = "../models/nous-hermes-llama-2-7b.Q4_0.gguf";
  std::string voice = "common-voice-male";
  std::string prompt = "";
  std::string path_session = "";
};

/// @brief Create CCParams from a character config file
/// @param fname File name
/// @param params Parsed parameters
bool cc_params_from_config(const std::string& fname, CCParams& params);

/// @brief Parse command line arguments
/// @param argc Number of arguments
/// @param argv Arguments
/// @param params Parsed parameters
/// @return True if parsing was successful
bool cc_params_parse(int argc, char** argv, CCParams& params);

/// @brief Print usage
/// @param argc Number of arguments
/// @param argv Arguments
/// @param params Parsed parameters
void cc_print_params_usage(int /*argc*/, char** argv, const CCParams& params);

}  // namespace common
}  // namespace CC

#endif  // CUSTOMCHAR_COMMON_HELPERS_H_
