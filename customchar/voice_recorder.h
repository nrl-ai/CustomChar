#pragma once

#include "whisper-cpp/examples/common-sdl.h"  // TODO: Move this to our lib
#include "whisper-cpp/examples/common.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <regex>
#include <string>
#include <thread>
#include <vector>

namespace CC {

class VoiceRecorder {
 private:
  audio_async* audio = nullptr;
  std::vector<float> pcmf32_cur;

 public:
  /// @brief Constructor
  VoiceRecorder();

  /// @brief Clear audio buffer to prepare for new recording
  void clear_audio_buffer();

  /// @brief Sample audio
  void sample_audio();

  /// @brief Check if finished talking
  bool finished_talking();

  /// @brief Get final audio
  void get_audio(std::vector<float>& result);
};  // class VoiceRecorder

}  // namespace CC
