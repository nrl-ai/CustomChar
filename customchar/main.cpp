#include "customchar/common.h"
#include "customchar/helpers.h"
#include "customchar/llm.h"
#include "customchar/speech_recognizer.h"
#include "customchar/voice_recorder.h"
#include "customchar/voice_synthesizer.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <regex>
#include <string>
#include <thread>
#include <vector>

using namespace CC;

int main(int argc, char** argv) {
  // Parse command line arguments
  CCParams params;
  if (cc_params_parse(argc, argv, params) == false) {
    exit(1);
  }
  if (whisper_lang_id(params.language.c_str()) == -1) {
    fprintf(stderr, "error: unknown language '%s'\n", params.language.c_str());
    cc_print_usage(argc, argv, params);
    exit(1);
  }

  // CC components
  SpeechRecognizer speech_recognizer(
      params.sr_model_path, params.language, params.audio_ctx, params.n_threads,
      params.max_tokens, params.translate, params.no_timestamps,
      params.print_special, params.speed_up);
  VoiceRecorder voice_recoder;
  VoiceSynthesizer voice_synthesizer;

  // Load LLM
  LLM llm(params.llm_model_path, params.path_session, params.person,
          params.bot_name);
  llm.eval_model();

  // Start talking
  printf("Start speaking in the microphone\n");
  printf("%s%s", params.person.c_str(), params.chat_symb.c_str());
  fflush(stdout);

  // Clear audio buffer to avoid processing old audio
  voice_recoder.clear_audio_buffer();

  std::vector<llama_token> embd;
  int n_iter = 0;
  bool is_running = true;
  while (is_running) {
    // Handle Ctrl + C
    is_running = sdl_poll_events();
    if (!is_running) {
      break;
    }

    // Delay
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    float prob = 0.0f;
    int64_t t_ms = 0;

    // Sample audio
    voice_recoder.sample_audio();
    if (!voice_recoder.finished_talking()) {
      continue;
    }

    // Get recorded audio
    std::vector<float> audio_buff;
    voice_recoder.get_audio(audio_buff);

    // Recognize speech
    std::string text_heard =
        speech_recognizer.recognize(audio_buff, prob, t_ms);

    // Tokenize user input
    auto tokens = llm.tokenize(text_heard, false);

    // Skip if nothing was heard
    if (text_heard.empty() || tokens.empty()) {
      printf("Heard nothing, skipping ...\n");
      voice_recoder.clear_audio_buffer();
      continue;
    }

    // Append the new input tokens to the session_tokens vector
    llm.add_tokens_to_current_session(tokens);

    // Print user input
    text_heard.insert(0, 1, ' ');
    text_heard += "\n" + params.bot_name + params.chat_symb;
    printf("%s%s%s", "\033[1m", text_heard.c_str(), "\033[0m");
    fflush(stdout);

    // Get answer from LLM
    embd = llm.tokenize(text_heard, false);

    // Get answer from LLM
    std::string text_to_speak = llm.get_answer(embd);

    // Play speak
    voice_synthesizer.say(text_to_speak);

    // Clean up
    voice_recoder.clear_audio_buffer();
    ++n_iter;
  }

  return 0;
}