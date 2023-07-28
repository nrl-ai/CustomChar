#ifndef CUSTOMCHAR_AUDIO_SPEECH_RECOGNIZER_H_
#define CUSTOMCHAR_AUDIO_SPEECH_RECOGNIZER_H_

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
  std::string person_ = "User";
  std::string bot_name_ = "CustomChar";
  const std::string chat_symb_ = ":";
  const std::string k_prompt_whisper_ =
      R"(A conversation with a person_ called {1}.)";
  struct whisper_context* context_;
  std::string prompt_;

  /// @brief Initialize prompt
  void InitPrompt();

  std::string model_path_;
  std::string language;
  int32_t audio_ctx;
  int n_threads;
  int max_tokens;
  bool translate;
  bool no_timestamps;
  bool print_special;
  bool speed_up;

  /// @brief  Postprocess text
  std::string PostProcess(const std::string& text_heard);

  /// @brief Transcribe speech
  std::string Transcribe(const std::vector<float>& pcmf32, float& prob,
                         int64_t& t_ms);

 public:
  /// @brief Constructor
  SpeechRecognizer(const std::string& model_path_, const std::string& language,
                   int32_t audio_ctx = 0, int n_threads = 4,
                   int max_tokens = 32, bool translate = false,
                   bool no_timestamps = true, bool print_special = false,
                   bool speed_up = false, const std::string& person_ = "User",
                   const std::string& bot_name_ = "CustomChar");
  ~SpeechRecognizer();

  /// @brief Recognize speech
  /// @param audio_buff Input audio_ buffer
  /// @param prob Output probability
  /// @param t_ms Output time
  /// @return Recognized text
  std::string Recognize(const std::vector<float>& audio_buff, float& prob,
                        int64_t& t_ms);

};  // class SpeechRecognizer

}  // namespace CC

#endif  // CUSTOMCHAR_AUDIO_SPEECH_RECOGNIZER_H_
