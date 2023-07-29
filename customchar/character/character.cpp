#include "customchar/character/character.h"

using namespace CC;

Character::Character(CCParams init_params) {
  params_ = init_params;

  // CC components
  speech_recognizer_ = std::make_shared<SpeechRecognizer>(
      params_.sr_model_path, params_.language, params_.audio_ctx,
      params_.n_threads, params_.max_tokens, params_.translate,
      params_.no_timestamps, params_.print_special, params_.speed_up);
  voice_recoder_ = std::make_shared<VoiceRecorder>();
  voice_synthesizer_ = std::make_shared<VoiceSynthesizer>();

  // Load LLM
  llm_ = std::make_shared<LLM>(params_.llm_model_path, params_.path_session,
                               params_.person, params_.bot_name);
  llm_->EvalModel();

  // Load plugin executor
  plugin_executor_ = std::make_shared<executors::PluginExecutor>();
}

void Character::SetOnUserMessage(
    std::function<void(std::string)> on_user_message_) {
  this->on_user_message_ = on_user_message_;
}

void Character::SetOnBotMessage(
    std::function<void(std::string)> on_bot_message_) {
  this->on_bot_message_ = on_bot_message_;
}

void Character::Run() {
  // Start talking
  printf("Start speaking in the microphone\n");
  printf("%s%s", params_.person.c_str(), params_.chat_symb.c_str());
  fflush(stdout);

  // Clear audio buffer to avoid processing old audio
  voice_recoder_->ClearAudioBuffer();

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
    voice_recoder_->SampleAudio();
    if (!voice_recoder_->FinishedTalking()) {
      continue;
    }

    // Get recorded audio
    std::vector<float> audio_buff;
    voice_recoder_->GetAudio(audio_buff);

    // Recognize speech
    std::string text_heard =
        speech_recognizer_->Recognize(audio_buff, prob, t_ms);

    // Tokenize user input
    auto tokens = llm_->Tokenize(text_heard, false);

    // Skip if nothing was heard
    if (text_heard.empty() || tokens.empty()) {
      printf("Heard nothing, skipping ...\n");
      voice_recoder_->ClearAudioBuffer();
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
    if (!plugin_executor_->ParseAndExecute(text_heard, response)) {
      // Append the new input tokens to the session_tokens vector
      llm_->AddTokensToCurrentSession(tokens);
      // Get answer from LLM
      embd = llm_->Tokenize(formated_text_heard, false);
      // Get answer from LLM
      response = llm_->GetAnswer(embd);
    } else {
      // TODO: Add plugin executor response to LLM session
    }

    // Callback for bot message
    if (on_bot_message_) {
      on_bot_message_(response);
    }

    // Play speak
    voice_synthesizer_->Say(response);

    // Clean up
    voice_recoder_->ClearAudioBuffer();
    ++n_iter;
  }
}
