#ifndef CUSTOMCHAR_EXECUTORS_VIDEO_RECORD_PLUGIN_H_
#define CUSTOMCHAR_EXECUTORS_VIDEO_RECORD_PLUGIN_H_

#include <functional>
#include <iostream>
#include <sstream>
#include <string>

#include "customchar/executors/plugin.h"

namespace CC {
namespace executors {

/// @brief Plugin for record videos
class VideoRecordPlugin : public Plugin {
 private:
  std::function<bool()> IsRecording_;
  std::function<void(const std::string& filename)> StartVideoRecoding_;
  std::function<void()> StopVideoCapture_;

 public:
  VideoRecordPlugin(
      const std::string& name, std::function<bool()> is_recording,
      std::function<void(const std::string& filename)> start_video_recording,
      std::function<void()> stop_video_capture)
      : Plugin(name),
        IsRecording_(is_recording),
        StartVideoRecoding_(start_video_recording),
        StopVideoCapture_(stop_video_capture) {}
  bool handle(const std::string& input, std::string& response,
              bool& finished) override {
    // Normalize app name
    std::string normalized_input = input;
    std::transform(normalized_input.begin(), normalized_input.end(),
                   normalized_input.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    std::cout << normalized_input << std::endl;
    bool contains_stop = normalized_input.find("stop") != std::string::npos ||
                         normalized_input.find("end") != std::string::npos ||
                         normalized_input.find("finish") != std::string::npos;
    if (contains_stop) {
      if (!IsRecording_()) {
        finished = true;
        return false;
      }
      StopVideoCapture_();
      response = "Stopping video recording";
      finished = true;
      return true;
    }

    if (IsRecording_()) {
      finished = true;
      return false;
    }

    // Random a video name
    std::stringstream ss;
    ss << "video_" << std::time(nullptr) << ".mkv";

    StartVideoRecoding_(ss.str());
    response = "Starting video recording...";
    finished = true;
    return true;
  }
};

}  // namespace executors
}  // namespace CC

#endif  // CUSTOMCHAR_EXECUTORS_VIDEO_RECORD_PLUGIN_H_
