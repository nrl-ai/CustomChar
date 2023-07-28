#include "customchar/audio/voice_synthesizer.h"
#include "customchar/common/common.h"

using namespace CC;

VoiceSynthesizer::VoiceSynthesizer() {
  // Check if the Say command is supported
  std::string command = "Say --version";
  FILE* pipe = popen(command.c_str(), "r");
  if (pipe == nullptr) {
    printf("Failed to run command: %s\n", command.c_str());
    is_say_supported_ = false;
    return;
  }

  is_say_supported_ = true;
}

std::string VoiceSynthesizer::PreProcess(const std::string& text) {
  std::string result = text;

  // Replace all \n with space
  result = common::Replace(result, "\n", " ");

  // Remove quotes and special characters to put into command line string
  result = common::Replace(result, "\"", " ");
  result = common::Replace(result, "\\", " ");
  result = common::Replace(result, "\'", " ");
  result = common::Replace(result, "(", " ");
  result = common::Replace(result, ")", " ");
  result = common::Replace(result, "`", " ");
  result = common::Replace(result, "$", " ");
  result = common::Replace(result, "&", " ");
  result = common::Replace(result, "!", " ");
  result = common::Replace(result, "#", " ");
  result = common::Replace(result, "@", " ");
  result = common::Replace(result, "%", " ");
  result = common::Replace(result, ";", " ");

  // Remove multiple spaces
  result = common::Replace(result, "  ", " ");

  return result;
}

void VoiceSynthesizer::Say(const std::string& text) {
  if (!is_say_supported_) {
    printf(
        "TTS is only supported on macOS at the moment. More OSes will be "
        "supported later.\n");
    return;
  }

  const int voice_id = 2;
  std::string command =
      "Say " + std::to_string(voice_id) + " \"" + PreProcess(text) + "\"";
  system(command.c_str());
}
