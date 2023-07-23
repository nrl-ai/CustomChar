#include "customchar/voice_synthesizer.h"
#include "customchar/common.h"

using namespace CC;

VoiceSynthesizer::VoiceSynthesizer() {
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

std::string VoiceSynthesizer::preprocess(const std::string& text) {
  std::string result = text;

  // Replace all \n with space
  result = common::replace(result, "\n", " ");

  // Remove quotes and special characters to put into command line string
  result = common::replace(result, "\"", " ");
  result = common::replace(result, "\\", " ");
  result = common::replace(result, "\'", " ");
  result = common::replace(result, "(", " ");
  result = common::replace(result, ")", " ");

  // Remove multiple spaces
  result = common::replace(result, "  ", " ");

  return result;
}

void VoiceSynthesizer::say(const std::string& text) {
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
