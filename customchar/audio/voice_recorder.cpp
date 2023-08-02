#include "customchar/audio/voice_recorder.h"
#include "whisper-cpp/whisper.h"

using namespace CC;
using namespace CC::audio;

VoiceRecorder::VoiceRecorder() {
  audio_ = new AudioAsync(30 * 1000);
  int capture_id = 0;  // TODO: Make this configurable
  if (!audio_->initialize(capture_id, WHISPER_SAMPLE_RATE)) {
    fprintf(stderr, "%s: audio_->init() failed!\n", __func__);
    exit(1);
  }

  audio_->resume();
}

void VoiceRecorder::clear_audio_buffer() { audio_->clear(); }

void VoiceRecorder::sample_audio() { audio_->get(2000, pcmf32_cur_); }

bool VoiceRecorder::finished_talking() {
  float vad_thold = 0.6f;
  float freq_thold = 100.0f;
  bool print_energy = false;
  return vad_simple(pcmf32_cur_, WHISPER_SAMPLE_RATE, 1250, vad_thold,
                    freq_thold, print_energy);
}

void VoiceRecorder::get_audio(std::vector<float>& result) {
  int32_t voice_ms = 10000;
  audio_->get(voice_ms, pcmf32_cur_);
  result = pcmf32_cur_;
}

std::vector<float> VoiceRecorder::record_speech() {
  bool is_running;
  std::vector<float> audio_buff;
  while (true) {
    // Handle Ctrl + C
    is_running = audio::sdl_poll_events();
    if (!is_running) {
      break;
    }

    // Delay
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Sample audio
    sample_audio();
    if (!finished_talking()) {
      continue;
    }

    // Get recorded audio
    get_audio(audio_buff);
    break;
  }

  return audio_buff;
};
