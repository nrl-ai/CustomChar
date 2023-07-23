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
  VoiceSynthesizer() {
    // Check if the say command is supported
    std::string command = "say --version";
    FILE* pipe = popen(command.c_str(), "r");
    if (pipe == nullptr) {
      printf("Failed to run command: %s\n", command.c_str());
      is_say_supported = false;
      return;
    }

    is_say_supported = true;
  }

  std::string preprocess(const std::string& text) {
    std::string result = text;

    // Replace all \n with space
    result = ::replace(result, "\n", " ");

    // Remove quotes and special characters to put into command line string
    result = ::replace(result, "\"", " ");
    result = ::replace(result, "\\", " ");
    result = ::replace(result, "\'", " ");
    result = ::replace(result, "(", " ");
    result = ::replace(result, ")", " ");

    // Remove multiple spaces
    result = ::replace(result, "  ", " ");

    return result;
  }

  void say(const std::string& text) {
    if (!is_say_supported) {
      printf(
          "TTS is only supported on macOS at the moment. More OSes will be "
          "supported later.\n");
      return;
    }

    const int voice_id = 2;
    std::string command =
        "say " + std::to_string(voice_id) + " \"" + preprocess(text) + "\"";
    system(command.c_str());
  }
};

}  // namespace CC
