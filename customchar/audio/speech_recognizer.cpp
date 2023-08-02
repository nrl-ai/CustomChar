#include "customchar/audio/speech_recognizer.h"
#include "customchar/common/common.h"

using namespace CC;
using namespace CC::audio;

SpeechRecognizer::SpeechRecognizer(const std::string& model_path_,
                                   const std::string& language,
                                   int32_t audio_ctx, int n_threads,
                                   int max_tokens, bool translate,
                                   bool no_timestamps, bool print_special,
                                   bool speed_up, const std::string& person_,
                                   const std::string& bot_name_)
    : model_path_(model_path_),
      language(language),
      audio_ctx(audio_ctx),
      n_threads(n_threads),
      max_tokens(max_tokens),
      translate(translate),
      no_timestamps(no_timestamps),
      print_special(print_special),
      speed_up(speed_up),
      person_(person_),
      bot_name_(bot_name_) {
  // Init model
  context_ = whisper_init_from_file(model_path_.c_str());

  // Print information about the model
  {
    fprintf(stderr, "\n");

    if (!whisper_is_multilingual(context_)) {
      if (language != "en" || translate) {
        this->language = "en";
        translate = false;
        fprintf(stderr,
                "%s: WARNING: model is not multilingual, ignoring language "
                "and translation options\n",
                __func__);
      }
    }
    fprintf(stderr,
            "%s: processing, %d threads, lang = %s, task = %s, timestamps = "
            "%d ...\n",
            __func__, n_threads, language.c_str(),
            translate ? "translate" : "Transcribe", no_timestamps ? 0 : 1);

    fprintf(stderr, "\n");
  }

  init_prompt();
}

SpeechRecognizer::~SpeechRecognizer() {
  whisper_print_timings(context_);
  whisper_free(context_);
}

void SpeechRecognizer::init_prompt() {
  const std::string bot_name_ = "CustomChar";
  prompt_ = common::replace(k_prompt_whisper_, "{1}", bot_name_);
}

std::string SpeechRecognizer::postprocess(const std::string& text_heard) {
  std::string processed_text = common::trim(text_heard);

  // Remove text between brackets using regex
  {
    std::regex re("\\[.*?\\]");
    processed_text = std::regex_replace(processed_text, re, "");
  }

  // Remove text between brackets using regex
  {
    std::regex re("\\(.*?\\)");
    processed_text = std::regex_replace(processed_text, re, "");
  }

  // Remove all characters, except for letters, numbers, punctuation and ':',
  // '\'', '-', ' '
  processed_text = std::regex_replace(
      processed_text, std::regex("[^a-zA-Z0-9\\.,\\?!\\s\\:\\'\\-]"), "");

  // Take first line
  processed_text = processed_text.substr(0, processed_text.find_first_of('\n'));

  // Remove leading and trailing whitespace
  processed_text = std::regex_replace(processed_text, std::regex("^\\s+"), "");
  processed_text = std::regex_replace(processed_text, std::regex("\\s+$"), "");

  return processed_text;
}

std::string SpeechRecognizer::recognize(const std::vector<float>& pcmf32,
                                        float& prob, int64_t& t_ms) {
  std::string text_heard;
  text_heard = transcribe(pcmf32, prob, t_ms);
  text_heard = postprocess(text_heard);
  return text_heard;
}

std::string SpeechRecognizer::transcribe(const std::vector<float>& pcmf32,
                                         float& prob, int64_t& t_ms) {
  const auto t_start = std::chrono::high_resolution_clock::now();

  prob = 0.0f;
  t_ms = 0;

  std::vector<whisper_token> prompt_tokens;

  whisper_full_params wparams =
      whisper_full_default_params(WHISPER_SAMPLING_GREEDY);

  prompt_tokens.resize(1024);
  prompt_tokens.resize(whisper_tokenize(
      context_, prompt_.c_str(), prompt_tokens.data(), prompt_tokens.size()));

  wparams.print_progress = false;
  wparams.print_special = print_special;
  wparams.print_realtime = false;
  wparams.print_timestamps = !no_timestamps;
  wparams.translate = translate;
  wparams.no_context = true;
  wparams.single_segment = true;
  wparams.max_tokens = max_tokens;
  wparams.language = language.c_str();
  wparams.n_threads = n_threads;

  wparams.prompt_tokens =
      prompt_tokens.empty() ? nullptr : prompt_tokens.data();
  wparams.prompt_n_tokens = prompt_tokens.empty() ? 0 : prompt_tokens.size();

  wparams.audio_ctx = audio_ctx;
  wparams.speed_up = speed_up;

  if (whisper_full(context_, wparams, pcmf32.data(), pcmf32.size()) != 0) {
    return "";
  }

  int prob_n = 0;
  std::string result;

  const int n_segments = whisper_full_n_segments(context_);
  for (int i = 0; i < n_segments; ++i) {
    const char* text = whisper_full_get_segment_text(context_, i);
    result += text;

    const int n_tokens = whisper_full_n_tokens(context_, i);
    for (int j = 0; j < n_tokens; ++j) {
      const auto token = whisper_full_get_token_data(context_, i, j);

      prob += token.p;
      ++prob_n;
    }
  }

  if (prob_n > 0) {
    prob /= prob_n;
  }

  const auto t_end = std::chrono::high_resolution_clock::now();
  t_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start)
             .count();

  return result;
}
