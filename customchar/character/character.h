#ifndef CUSTOMCHAR_CHARACTER_CHARACTER_H_
#define CUSTOMCHAR_CHARACTER_CHARACTER_H_

#include "customchar/audio/speech_recognizer.h"
#include "customchar/audio/voice_recorder.h"
#include "customchar/audio/voice_synthesizer.h"
#include "customchar/common/common.h"
#include "customchar/common/helpers.h"
#include "customchar/executors/plugin_executor.h"
#include "customchar/llm/llm.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <functional>
#include <regex>
#include <string>
#include <thread>
#include <vector>

namespace CC {
namespace character {

class Character {
 private:
  common::CCParams params_;
  std::shared_ptr<audio::SpeechRecognizer> speech_recognizer_;
  std::shared_ptr<audio::VoiceRecorder> voice_recoder_;
  std::shared_ptr<audio::VoiceSynthesizer> voice_synthesizer_;
  std::shared_ptr<llm::LLM> llm_;
  std::shared_ptr<executors::PluginExecutor> plugin_executor_;

  std::function<void(std::string)> on_user_message_;
  std::function<void(std::string)> on_bot_message_;

  bool is_muted_ = false;

 public:
  /// @brief Constructor
  /// @param init_params
  Character(common::CCParams init_params);

  /// @brief Set mute (do not speak)
  void SetMute(bool is_muted);

  /// @brief Get mute status
  bool IsMuted();

  /// @brief Set on user message callback
  /// @param on_user_message_
  void SetOnUserMessage(std::function<void(std::string)> on_user_message_);

  /// @brief Set on bot message callback
  /// @param on_bot_message_
  void SetOnBotMessage(std::function<void(std::string)> on_bot_message_);

  /// @brief Run character in a loop
  void Run();
};

}  // namespace character
}  // namespace CC

#endif  // CUSTOMCHAR_CHARACTER_CHARACTER_H_
