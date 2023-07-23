#pragma once

#include "whisper-cpp/examples/common-sdl.h"  // TODO: Move this to our lib

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
  audio_async *audio = nullptr;
  std::vector<float> pcmf32_cur;

 public:
  VoiceRecoder() {
    audio = new audio_async(30 * 1000);
    int capture_id = 0; // TODO: Make this configurable
    if (!audio->init(capture_id, WHISPER_SAMPLE_RATE)) {
      fprintf(stderr, "%s: audio->init() failed!\n", __func__);
      exit(1);
    }

    audio->resume();
  }

  void clear_audio_buffer() {
    audio->clear();
  }

  void sample_audio() {
    audio->get(2000, pcmf32_cur);
  }

  bool finished_talking() {
    float vad_thold = 0.6f;
    float freq_thold = 100.0f;
    bool print_energy = false;
    return vad_simple(pcmf32_cur, WHISPER_SAMPLE_RATE, 1250, vad_thold, freq_thold, print_energy);
  }

  void get_audio(std::vector<float> & result) {
    int32_t voice_ms   = 10000;
    audio->get(voice_ms, pcmf32_cur);
    result = pcmf32_cur;
  }
}; // class VoiceRecoder

}  // namespace customchar