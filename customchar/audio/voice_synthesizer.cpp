#include "customchar/audio/voice_synthesizer.h"
#include "customchar/common/common.h"

using namespace CC;
using namespace CC::audio;

VoiceSynthesizer::VoiceSynthesizer(const std::string& voice) : voice_(voice) {
// Check if the Say command is supported
#ifdef __APPLE__
  std::string command = "which say";
  FILE* pipe = popen(command.c_str(), "r");
  if (pipe == nullptr) {
    printf("Failed to run command: %s\n", command.c_str());
    is_say_supported_ = false;
    return;
  }
#elif __linux__
  std::string command = "which espeak";
  FILE* pipe = popen(command.c_str(), "r");
  if (pipe == nullptr) {
    printf("Failed to run command: %s\n", command.c_str());
    printf("Please install espeak\n");
    is_say_supported_ = false;
    return;
  }
#endif

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

#ifdef __APPLE__
  std::string os_voice;
  if (voice_ == "common-voice-male") {
    os_voice = "-v Daniel";
  } else {
    os_voice = "";
  }
  std::string command = "say " + os_voice + " \"" + preprocess(text) + "\"";
  system(command.c_str());
#elif _WIN32
  std::string os_voice;
  if (voice_ == "common-voice-male") {
    os_voice = "David";
  } else {
    os_voice = "Zira";
  }
  std::string command =
      "powershell.exe -ExecutionPolicy Bypass -Command "
      "\"Add-Type -AssemblyName System.Speech; "
      "$speak = New-Object "
      "System.Speech.Synthesis.SpeechSynthesizer; "
      "$speak.SelectVoice('Microsoft " +
      os_voice +
      " Desktop'); "
      "$speak.Rate='0'; "
      "$speak.Speak('" +
      preprocess(text) + "');\"";
  system(command.c_str());
#elif __linux__
  std::string command = "espeak \"" + preprocess(text) + "\"";
#endif
}
