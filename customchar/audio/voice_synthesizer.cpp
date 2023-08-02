#include "customchar/audio/voice_synthesizer.h"
#include "customchar/common/common.h"

using namespace CC;
using namespace CC::audio;

VoiceSynthesizer::VoiceSynthesizer() {
  // Check if the Say command is supported
  std::string command = "which say";
  FILE* pipe = popen(command.c_str(), "r");
  if (pipe == nullptr) {
    printf("Failed to run command: %s\n", command.c_str());
    is_say_supported_ = false;
    return;
  }

  is_say_supported_ = true;
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
  result = common::replace(result, "`", " ");
  result = common::replace(result, "$", " ");
  result = common::replace(result, "&", " ");
  result = common::replace(result, "!", " ");
  result = common::replace(result, "#", " ");
  result = common::replace(result, "@", " ");
  result = common::replace(result, "%", " ");
  result = common::replace(result, ";", " ");

  // Remove multiple spaces
  result = common::replace(result, "  ", " ");

  return result;
}

void VoiceSynthesizer::say(const std::string& text) {
  if (!is_say_supported_) {
    printf(
        "TTS is only supported on macOS at the moment. More OSes will be "
        "supported later.\n");
    return;
  }

  std::string command = "say " + preprocess(text);
  system(command.c_str());
}
