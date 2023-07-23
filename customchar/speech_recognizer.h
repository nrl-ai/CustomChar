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

class SpeechRecognizer {
 private:
  std::string person = "User";
  std::string bot_name = "CustomChar";
  const std::string chat_symb = ":";
  const std::string k_prompt_whisper =
      R"(A conversation with a person called {1}.)";
  struct whisper_context* context;
  std::string prompt;

  /// @brief Initialize prompt
  void init_prompt();

  std::string model_path;
  std::string language;
  int32_t audio_ctx;
  int n_threads;
  int max_tokens;
  bool translate;
  bool no_timestamps;
  bool print_special;
  bool speed_up;

  /// @brief  Postprocess text
  std::string postprocess(const std::string& text_heard);

  /// @brief Transcribe speech
  std::string transcribe(const std::vector<float>& pcmf32, float& prob,
                         int64_t& t_ms);

 public:
  /// @brief Constructor
  SpeechRecognizer(const std::string& model_path, const std::string& language,
                   int32_t audio_ctx = 0, int n_threads = 4,
                   int max_tokens = 32, bool translate = false,
                   bool no_timestamps = true, bool print_special = false,
                   bool speed_up = false, const std::string& person = "User",
                   const std::string& bot_name = "CustomChar");
  ~SpeechRecognizer();

  /// @brief Recognize speech
  /// @param audio_buff Input audio buffer
  /// @param prob Output probability
  /// @param t_ms Output time
  /// @return Recognized text
  std::string recognize(const std::vector<float>& audio_buff, float& prob,
                        int64_t& t_ms);

};  // class SpeechRecognizer

}  // namespace CC
