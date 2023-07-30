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
      const std::string& name, std::function<bool()> IsRecording,
      std::function<void(const std::string& filename)> StartVideoRecoding,
      std::function<void()> StopVideoCapture)
      : Plugin(name),
        IsRecording_(IsRecording),
        StartVideoRecoding_(StartVideoRecoding),
        StopVideoCapture_(StopVideoCapture) {}
  bool Handle(const std::string& input, std::string& response,
              bool& finished) override {
    bool contains_stop = input.find("stop") != std::string::npos;
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
    response = "Starting video recording to " + ss.str();
    finished = true;
    return true;
  }
};

}  // namespace executors
}  // namespace CC

#endif  // CUSTOMCHAR_EXECUTORS_VIDEO_RECORD_PLUGIN_H_
