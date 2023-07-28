#include "customchar/audio/voice_recorder.h"
#include "whisper-cpp/whisper.h"

using namespace CC;

VoiceRecorder::VoiceRecorder() {
  audio_ = new audio_async(30 * 1000);
  int capture_id = 0;  // TODO: Make this configurable
  if (!audio_->init(capture_id, WHISPER_SAMPLE_RATE)) {
    fprintf(stderr, "%s: audio_->init() failed!\n", __func__);
    exit(1);
  }

  audio_->resume();
}

void VoiceRecorder::ClearAudioBuffer() { audio_->clear(); }

void VoiceRecorder::SampleAudio() { audio_->get(2000, pcmf32_cur_); }

bool VoiceRecorder::FinishedTalking() {
  float vad_thold = 0.6f;
  float freq_thold = 100.0f;
  bool print_energy = false;
  return vad_simple(pcmf32_cur_, WHISPER_SAMPLE_RATE, 1250, vad_thold,
                    freq_thold, print_energy);
}

void VoiceRecorder::GetAudio(std::vector<float>& result) {
  int32_t voice_ms = 10000;
  audio_->get(voice_ms, pcmf32_cur_);
  result = pcmf32_cur_;
}
