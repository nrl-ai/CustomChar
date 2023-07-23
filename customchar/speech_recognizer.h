#pragma once

#include "whisper-cpp/examples/common-sdl.h"  // TODO: Move this to our lib
#include "whisper-cpp/examples/common.h"      // TODO: Move this to our lib
#include "whisper-cpp/whisper.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <regex>
#include <string>
#include <thread>
#include <vector>

namespace CC {

struct whisper_params {
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
  std::string language = "en";
  std::string model_wsp = "../models/ggml-base.en.bin";
  std::string model_llama = "../models/llm.bin";
  std::string speak = "say";
  std::string prompt = "";
  std::string fname_out;
  std::string path_session =
      "";  // path to file for saving/loading model eval state
};

class SpeechRecognizer {
 private:
  const std::string k_prompt_whisper =
      R"(A conversation with a person called {1}.)";
  whisper_params params;
  struct whisper_context* ctx_wsp;
  std::string prompt_whisper;
  std::string prompt = "";

  void init_prompt();

 public:
  SpeechRecognizer(int argc, char** argv);
  ~SpeechRecognizer();

  std::string postprocess(const std::string& text_heard);

  std::string recognize(const std::vector<float>& pcmf32, float& prob,
                        int64_t& t_ms);

  bool whisper_params_parse(int argc, char** argv, whisper_params& params);

  void whisper_print_usage(int /*argc*/, char** argv,
                           const whisper_params& params);

  std::string transcribe(const std::vector<float>& pcmf32, float& prob,
                         int64_t& t_ms);

};  // class SpeechRecognizer

}  // namespace CC
