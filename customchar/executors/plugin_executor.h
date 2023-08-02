#ifndef CUSTOMCHAR_EXECUTORS_PLUGIN_EXECUTOR_H_
#define CUSTOMCHAR_EXECUTORS_PLUGIN_EXECUTOR_H_

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "customchar/executors/plugin.h"
#include "customchar/executors/plugins/open_app_plugin.h"
#include "customchar/executors/plugins/video_record_plugin.h"

namespace CC {
namespace executors {

class PluginExecutor {
 private:
  std::map<std::string, std::string> reflections_ = {
      {"i am", "you are"},   {"i was", "you were"}, {"i", "you"},
      {"i'm", "you are"},    {"i'd", "you would"},  {"i've", "you have"},
      {"i'll", "you will"},  {"my", "your"},        {"you are", "I am"},
      {"you were", "I was"}, {"you've", "I have"},  {"you'll", "I will"},
      {"your", "my"},        {"yours", "mine"},     {"you", "me"},
      {"me", "you"}};
  std::vector<std::vector<std::string>> pairs_ = {
      {"open (.*)", "OPEN_APP"},
      {"open (.*) app", "OPEN_APP"},
      {"(.*)record video(.*)", "VIDEO_RECORD"},
      {"(.*)stop recording(.*)", "VIDEO_RECORD"},
      {"(.*)stop video recording(.*)", "VIDEO_RECORD"},
      {"(.*)record (a|some) video(.*)", "VIDEO_RECORD"}};

  std::function<bool()> IsRecording_;
  std::function<void(const std::string& filename)> StartVideoRecoding_;
  std::function<void()> StopVideoCapture_;

  std::vector<std::shared_ptr<Plugin>> plugins_ = {
      std::make_shared<OpenAppPlugin>("OPEN_APP"),
      std::make_shared<VideoRecordPlugin>("VIDEO_RECORD", IsRecording_,
                                          StartVideoRecoding_,
                                          StopVideoCapture_)};
  std::shared_ptr<Plugin> current_plugin_;

  std::string reflect(const std::string& word);

 public:
  /// @brief Constructor
  PluginExecutor(
      std::function<bool()> is_recording,
      std::function<void(const std::string& filename)> start_video_recording,
      std::function<void()> stop_video_capture)
      : IsRecording_(is_recording),
        StartVideoRecoding_(start_video_recording),
        StopVideoCapture_(stop_video_capture) {}

  /// @brief Parse and execute input
  /// @param input user input string
  /// @param response response string
  /// @return true if handled, false otherwise
  bool parse_and_execute(const std::string& input, std::string& response);
};  // namespace executors

}  // namespace executors
}  // namespace CC

#endif  // CUSTOMCHAR_EXECUTORS_COMMAND_PARSER_H_
