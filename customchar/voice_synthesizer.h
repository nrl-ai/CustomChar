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
  bool is_say_supported = false;

 public:
  VoiceSynthesizer();

  std::string preprocess(const std::string& text);

  void say(const std::string& text);
};

}  // namespace CC
