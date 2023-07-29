#ifndef CUSTOMCHAR_AUDIO_VOICE_RECORDER_H_
#define CUSTOMCHAR_AUDIO_VOICE_RECORDER_H_

#include <cassert>
#include <cstdio>
#include <fstream>
#include <regex>
#include <string>
#include <thread>
#include <vector>

#include "customchar/audio/audio.h"
#include "customchar/audio/sdl.h"

namespace CC {
namespace audio {

class VoiceRecorder {
 private:
  AudioAsync* audio_ = nullptr;
  std::vector<float> pcmf32_cur_;

 public:
  /// @brief Constructor
  VoiceRecorder();

  /// @brief Clear audio_ buffer to prepare for new recording
  void ClearAudioBuffer();

  /// @brief Sample audio_
  void SampleAudio();

  /// @brief Check if finished talking
  bool FinishedTalking();

  /// @brief Get final audio_
  void GetAudio(std::vector<float>& result);
};  // class VoiceRecorder

}  // namespace audio
}  // namespace CC

#endif  // CUSTOMCHAR_AUDIO_SPEECH_RECOGNIZER_H_
