#include "customchar/character/character.h"

using namespace CC;
using namespace CC::character;

Character::Character(common::CCParams params) {
  params_ = params;

  // CC components
  speech_recognizer_ = std::make_shared<audio::SpeechRecognizer>(
      params_.tts_model_path, params_.language, params_.audio_ctx,
      params_.n_threads, params_.max_tokens, params_.translate,
      params_.no_timestamps, params_.print_special, params_.speed_up);
  voice_recoder_ = std::make_shared<audio::VoiceRecorder>();
  voice_synthesizer_ = std::make_shared<audio::VoiceSynthesizer>();

  // Load LLM
  llm_ = std::make_shared<llm::LLM>(params.llm_model_path, params_.path_session,
                                    params_.person, params_.bot_name,
                                    params.prompt);
  llm_->eval_model();

  // Load plugin executor
  plugin_executor_ = std::make_shared<executors::PluginExecutor>(
      std::bind(&Character::is_recording, this),
      std::bind(&Character::start_video_recording, this, std::placeholders::_1),
      std::bind(&Character::stop_video_capture, this));
}

void Character::set_on_user_message(
    std::function<void(std::string)> on_user_message_) {
  this->on_user_message_ = on_user_message_;
}

void Character::set_on_bot_message(
    std::function<void(std::string)> on_bot_message_) {
  this->on_bot_message_ = on_bot_message_;
}

void Character::set_mute(bool is_muted) { is_muted_ = is_muted; }

bool Character::is_muted() { return is_muted_; }

void Character::Run() {
  // Start talking
  printf("Start speaking in the microphone\n");
  printf("%s%s", params_.person.c_str(), params_.chat_symb.c_str());
  fflush(stdout);

  // Clear audio buffer to avoid processing old audio
  voice_recoder_->clear_audio_buffer();

  std::vector<llama_token> embd;
  int n_iter = 0;
  bool is_running = true;
  while (is_running) {
    // Handle Ctrl + C
    is_running = audio::sdl_poll_events();
    if (!is_running) {
      break;
    }

    // Record speech from user
    std::vector<float> audio_buff = voice_recoder_->record_speech();

    // Recognize speech
    float prob;
    int64_t t_ms;
    std::string text_heard =
        speech_recognizer_->recognize(audio_buff, prob, t_ms);

    // Tokenize user input
    auto tokens = llm_->tokenize(text_heard, false);

    // Start over if nothing was heard
    if (text_heard.empty() || tokens.empty()) {
      voice_recoder_->clear_audio_buffer();
      continue;
    }

    // Callback for user message
    if (on_user_message_) {
      on_user_message_(text_heard);
    }

    // Print user input
    std::string formated_text_heard = text_heard;
    formated_text_heard.insert(0, 1, ' ');
    formated_text_heard += "\n" + params_.bot_name + params_.chat_symb;
    printf("%s%s%s", "\033[1m", formated_text_heard.c_str(), "\033[0m");
    fflush(stdout);

    // Response from character
    // If plugin executor returns true, then it handled the user input
    // Otherwise, LLM will handle
    std::string response;
    if (!plugin_executor_->parse_and_execute(text_heard, response)) {
      // Get answer from LLM
      response = llm_->get_answer(text_heard);
    } else {
      // TODO: Add plugin executor response to LLM session
    }

    // Callback for bot message
    if (on_bot_message_) {
      on_bot_message_(response);
    }

    // Play speak
    if (!is_muted_) voice_synthesizer_->say(response);

    // Clean up
    voice_recoder_->clear_audio_buffer();
    ++n_iter;
  }
}
