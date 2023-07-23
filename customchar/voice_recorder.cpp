#include "customchar/voice_recorder.h"
#include "whisper-cpp/whisper.h"

using namespace CC;

VoiceRecoder::VoiceRecoder() {
  audio = new audio_async(30 * 1000);
  int capture_id = 0;  // TODO: Make this configurable
  if (!audio->init(capture_id, WHISPER_SAMPLE_RATE)) {
    fprintf(stderr, "%s: audio->init() failed!\n", __func__);
    exit(1);
  }

  audio->resume();
}

void VoiceRecoder::clear_audio_buffer() { audio->clear(); }

void VoiceRecoder::sample_audio() { audio->get(2000, pcmf32_cur); }

bool VoiceRecoder::finished_talking() {
  float vad_thold = 0.6f;
  float freq_thold = 100.0f;
  bool print_energy = false;
  return vad_simple(pcmf32_cur, WHISPER_SAMPLE_RATE, 1250, vad_thold,
                    freq_thold, print_energy);
}

void VoiceRecoder::get_audio(std::vector<float>& result) {
  int32_t voice_ms = 10000;
  audio->get(voice_ms, pcmf32_cur);
  result = pcmf32_cur;
}
