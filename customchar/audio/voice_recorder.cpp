#include "customchar/audio/voice_recorder.h"
#include "whisper-cpp/whisper.h"

using namespace CC;
using namespace CC::audio;

VoiceRecorder::VoiceRecorder() {
  audio_ = new AudioAsync(30 * 1000);
  int capture_id = 0;  // TODO: Make this configurable
  if (!audio_->Init(capture_id, WHISPER_SAMPLE_RATE)) {
    fprintf(stderr, "%s: audio_->init() failed!\n", __func__);
    exit(1);
  }

  audio_->Resume();
}

void VoiceRecorder::ClearAudioBuffer() { audio_->Clear(); }

void VoiceRecorder::SampleAudio() { audio_->Get(2000, pcmf32_cur_); }

bool VoiceRecorder::FinishedTalking() {
  float vad_thold = 0.6f;
  float freq_thold = 100.0f;
  bool print_energy = false;
  return VADSimple(pcmf32_cur_, WHISPER_SAMPLE_RATE, 1250, vad_thold,
                   freq_thold, print_energy);
}

void VoiceRecorder::GetAudio(std::vector<float>& result) {
  int32_t voice_ms = 10000;
  audio_->Get(voice_ms, pcmf32_cur_);
  result = pcmf32_cur_;
}

std::vector<float> VoiceRecorder::RecordSpeech() {
  bool is_running;
  std::vector<float> audio_buff;
  while (true) {
    // Handle Ctrl + C
    is_running = audio::SDLPollEvents();
    if (!is_running) {
      break;
    }

    // Delay
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Sample audio
    SampleAudio();
    if (!FinishedTalking()) {
      continue;
    }

    // Get recorded audio
    GetAudio(audio_buff);
    break;
  }

  return audio_buff;
};
