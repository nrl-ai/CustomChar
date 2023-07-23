#pragma once

#include <cassert>
#include <cstdio>
#include <fstream>
#include <regex>
#include <string>
#include <thread>
#include <vector>

namespace CC {

class VoiceSynthesizer {
 private:
  bool is_say_supported = false;

  /// @brief Preprocess text
  std::string preprocess(const std::string& text);

 public:
  /// @brief Constructor
  VoiceSynthesizer();

  /// @brief Say text
  void say(const std::string& text);
};

}  // namespace CC
