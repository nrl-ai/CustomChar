#ifndef CUSTOMCHAR_AUDIO_AUDIO_H_
#define CUSTOMCHAR_AUDIO_AUDIO_H_

#include <map>
#include <random>
#include <string>
#include <thread>
#include <vector>

namespace CC {
namespace audio {

#define COMMON_SAMPLE_RATE 16000

/// @brief Read WAV audio file and store the PCM data into pcmf32
/// The sample rate of the audio must be equal to COMMON_SAMPLE_RATE
/// If stereo flag is set and the audio has 2 channels, the pcmf32s will contain
/// 2 channel PCM
bool read_wav(const std::string& fname, std::vector<float>& pcmf32,
              std::vector<std::vector<float>>& pcmf32s, bool stereo);

/// @brief Apply a high-pass frequency filter to PCM audio
/// Suppresses frequencies below cutoff Hz
void high_pass_filter(std::vector<float>& data, float cutoff,
                      float sample_rate);

/// @brief Basic voice activity detection (VAD) using audio energy adaptive
/// threshold
bool vad_simple(std::vector<float>& pcmf32, int sample_rate, int last_ms,
                float vad_thold, float freq_thold, bool verbose);

}  // namespace audio
}  // namespace CC

#endif  // CUSTOMCHAR_AUDIO_AUDIO_H_
