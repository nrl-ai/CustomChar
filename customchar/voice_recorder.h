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

class VoiceRecoder {
 private:
  audio_async* audio = nullptr;
  std::vector<float> pcmf32_cur;

 public:
  VoiceRecoder();

  void clear_audio_buffer();

  void sample_audio();

  bool finished_talking();

  void get_audio(std::vector<float>& result);
};  // class VoiceRecoder

}  // namespace CC
