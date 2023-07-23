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

const std::string person = "User";
const std::string bot_name = "CustomChar";
const std::string chat_symb = ":";

int main(int argc, char** argv) {
  SpeechRecognizer speech_recognizer(argc, argv);
  VoiceRecoder voice_recorder;
  VoiceSynthesizer voice_synthesizer;

  // Init LLM session
  LLM llm;
  std::string path_session = "";
  if (!path_session.empty()) {
    llm.load_session(path_session);
  }
  llm.eval_model();

  // Start talking
  printf("Start speaking in the microphone\n");
  printf("%s%s", person.c_str(), chat_symb.c_str());
  fflush(stdout);

  // Clear audio buffer to avoid processing old audio
  voice_recorder.clear_audio_buffer();

  std::vector<llama_token> embd;

  // Reverse prompts for detecting when it's time to stop speaking
  std::vector<std::string> antiprompts = {
      person + chat_symb,
  };

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
    voice_recorder.sample_audio();
    if (!voice_recorder.finished_talking()) {
      continue;
    }

    // Get recorded audio
    std::vector<float> audio_buff;
    voice_recorder.get_audio(audio_buff);

    // Recognize speech
    std::string text_heard =
        speech_recognizer.recognize(audio_buff, prob, t_ms);

    // Tokenize user input
    auto tokens = llm.tokenize(text_heard, false);

    // Skip if nothing was heard
    if (text_heard.empty() || tokens.empty()) {
      printf("Heard nothing, skipping ...\n");
      voice_recorder.clear_audio_buffer();
      continue;
    }

    // Append the new input tokens to the session_tokens vector
    llm.add_tokens_to_current_session(tokens);

    // Print user input
    text_heard.insert(0, 1, ' ');
    text_heard += "\n" + bot_name + chat_symb;
    printf("%s%s%s", "\033[1m", text_heard.c_str(), "\033[0m");
    fflush(stdout);

    // Get answer from LLM
    embd = llm.tokenize(text_heard, false);

    // Get answer from LLM
    std::string text_to_speak = llm.get_answer(embd);

    // Play speak
    voice_synthesizer.say(text_to_speak);

    // Clean up
    voice_recorder.clear_audio_buffer();
    ++n_iter;
  }

  return 0;
}
