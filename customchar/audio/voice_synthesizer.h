#ifndef CUSTOMCHAR_AUDIO_SYNTHESIZER_H_
#define CUSTOMCHAR_AUDIO_SYNTHESIZER_H_

#include <cassert>
#include <cstdio>
#include <fstream>
#include <regex>
#include <string>
#include <thread>
#include <vector>

namespace CC {
namespace audio {

class VoiceSynthesizer {
 private:
  bool is_say_supported_ = false;

  /// @brief Preprocess text
  std::string PreProcess(const std::string& text);

 public:
  /// @brief Constructor
  VoiceSynthesizer();

  /// @brief Say text
  void Say(const std::string& text);
};

}  // namespace audio
}  // namespace CC

#endif  // CUSTOMCHAR_AUDIO_SYNTHESIZER_H_
