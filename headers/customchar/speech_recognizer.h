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

  void init_prompt() {
    prob0 = 0.0f;
    const std::string chat_symb = ":";
    const std::string bot_name = "CustomChar";
    std::string prompt_whisper = replace(k_prompt_whisper, "{1}", bot_name);
  }

 public:
  void SpeechRecognizer(int argc, char** argv) {
    if (whisper_params_parse(argc, argv, params) == false) {
      return 1;
    }

    if (whisper_lang_id(params.language.c_str()) == -1) {
      fprintf(stderr, "error: unknown language '%s'\n",
              params.language.c_str());
      whisper_print_usage(argc, argv, params);
      exit(1);
    }

    // Init model
    ctx_wsp = whisper_init_from_file(params.model_wsp.c_str());

    // Print information about the model
    {
      fprintf(stderr, "\n");

      if (!whisper_is_multilingual(ctx_wsp)) {
        if (params.language != "en" || params.translate) {
          params.language = "en";
          params.translate = false;
          fprintf(stderr,
                  "%s: WARNING: model is not multilingual, ignoring language "
                  "and translation options\n",
                  __func__);
        }
      }
      fprintf(stderr,
              "%s: processing, %d threads, lang = %s, task = %s, timestamps = "
              "%d ...\n",
              __func__, params.n_threads, params.language.c_str(),
              params.translate ? "translate" : "transcribe",
              params.no_timestamps ? 0 : 1);

      fprintf(stderr, "\n");
    }

    init_prompt();
  }

  ~SpeechRecognizer() {
    whisper_print_timings(ctx_wsp);
    whisper_free(ctx_wsp);
  }

  static bool whisper_params_parse(int argc, char** argv,
                                   whisper_params& params);
  static void whisper_print_usage(int /*argc*/, char** argv,
                                  const whisper_params& params);

  std::string transcribe(const std::vector<float>& pcmf32, float& prob,
                         int64_t& t_ms);

  std::string postprocess(const std::string& text_heard) {
    text_heard = trim(text_heard);

    // Remove text between brackets using regex
    {
      std::regex re("\\[.*?\\]");
      text_heard = std::regex_replace(text_heard, re, "");
    }

    // Remove text between brackets using regex
    {
      std::regex re("\\(.*?\\)");
      text_heard = std::regex_replace(text_heard, re, "");
    }

    // Remove all characters, except for letters, numbers, punctuation and ':',
    // '\'', '-', ' '
    text_heard = std::regex_replace(
        text_heard, std::regex("[^a-zA-Z0-9\\.,\\?!\\s\\:\\'\\-]"), "");

    // Take first line
    text_heard = text_heard.substr(0, text_heard.find_first_of('\n'));

    // Remove leading and trailing whitespace
    text_heard = std::regex_replace(text_heard, std::regex("^\\s+"), "");
    text_heard = std::regex_replace(text_heard, std::regex("\\s+$"), "");
  }

  std::string recognize(const std::vector<float>& pcmf32, float& prob,
                        int64_t& t_ms) {
    std::string text_heard;
    text_heard = transcribe(pcmf32, prob, t_ms);
    text_heard = postprocess(text_heard);
    return text_heard;
  }

};  // class SpeechRecognizer

}  // namespace CC
