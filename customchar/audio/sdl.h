#ifndef CUSTOMCHAR_AUDIO_SDL_H_
#define CUSTOMCHAR_AUDIO_SDL_H_

#include <SDL.h>
#include <SDL_audio.h>

#include <atomic>
#include <cstdint>
#include <mutex>
#include <vector>

namespace CC {
namespace audio {

/// @brief Audio capture class
class AudioAsync {
 public:
  AudioAsync(int len_ms);
  ~AudioAsync();

  bool initialize(int capture_id, int sample_rate);

  /// Start capturing audio via the provided SDL callback
  /// keep last len_ms seconds of audio in a circular buffer
  bool resume();
  bool pause();
  bool clear();

  /// @brief Callback function for SDL
  /// @param stream Audio stream
  /// @param len Length of the stream
  void callback(uint8_t* stream, int len);

  /// @brief  Get audio from the circular buffer
  /// @param ms Number of milliseconds to get
  /// @param audio Output audio
  void get(int ms, std::vector<float>& audio);

 private:
  SDL_AudioDeviceID m_dev_id_in_ = 0;

  int m_len_ms_ = 0;
  int m_sample_rate_ = 0;

  std::atomic_bool m_running_;
  std::mutex m_mutex_;

  std::vector<float> m_audio_;
  std::vector<float> m_audio_new_;
  size_t m_audio_pos_ = 0;
  size_t m_audio_len_ = 0;
};

// Return false if need to quit
bool sdl_poll_events();

}  // namespace audio
}  // namespace CC

#endif  // CUSTOMCHAR_AUDIO_SDL_H_
