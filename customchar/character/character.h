#ifndef CUSTOMCHAR_CHARACTER_CHARACTER_H_
#define CUSTOMCHAR_CHARACTER_CHARACTER_H_

#include <cassert>
#include <cstdio>
#include <fstream>
#include <functional>
#include <regex>
#include <string>
#include <thread>
#include <vector>

#include <opencv2/opencv.hpp>

#include "customchar/audio/speech_recognizer.h"
#include "customchar/audio/voice_recorder.h"
#include "customchar/audio/voice_synthesizer.h"
#include "customchar/common/common.h"
#include "customchar/common/helpers.h"
#include "customchar/executors/plugin_executor.h"
#include "customchar/llm/llm.h"
#include "customchar/vision/video_capture.h"

namespace CC {
namespace character {

class Character {
 private:
  common::CCParams params_;
  std::shared_ptr<audio::SpeechRecognizer> speech_recognizer_;
  std::shared_ptr<audio::VoiceRecorder> voice_recoder_;
  std::shared_ptr<audio::VoiceSynthesizer> voice_synthesizer_;
  std::shared_ptr<llm::LLM> llm_;
  std::shared_ptr<executors::PluginExecutor> plugin_executor_;

  std::function<void(std::string)> on_user_message_;
  std::function<void(std::string)> on_bot_message_;
  std::function<void()> open_camera_view_;
  std::function<void()> close_camera_view_;

  bool is_muted_ = false;

  vision::VideoCapture video_capture_;

 public:
  /// @brief Constructor
  /// @param init_params
  Character(common::CCParams init_params);

  /// @brief Set mute (do not speak)
  void set_mute(bool is_muted);

  /// @brief Get mute status
  bool is_muted();

  /// @brief Set on user message callback
  /// @param on_user_message_
  void set_on_user_message(std::function<void(std::string)> on_user_message_);

  /// @brief Set on bot message callback
  /// @param on_bot_message_
  void set_on_bot_message(std::function<void(std::string)> on_bot_message_);

  /// @brief Set open camera view callback
  void set_open_camera_view(std::function<void()> func) {
    open_camera_view_ = func;
  }

  /// @brief Set close camera view callback
  void set_close_camera_view(std::function<void()> func) {
    close_camera_view_ = func;
  }

  /// @brief Run character in a loop
  void Run();

  /// @brief Get video capture
  vision::VideoCapture& get_video_capture() { return video_capture_; }

  /// @brief Start video capture
  void start_video_capture() {
    if (open_camera_view_) open_camera_view_();
    video_capture_.Start();
    std::cout << "Started video capture" << std::endl;
  }

  /// @brief Stop video capture
  void stop_video_capture() {
    if (close_camera_view_) close_camera_view_();
    video_capture_.Stop();
  }

  /// @brief Record video
  void start_video_recording(const std::string& filename) {
    if (open_camera_view_) open_camera_view_();
    video_capture_.Record(filename);
  }

  /// @brief Stop recording video
  bool is_recording() { return video_capture_.is_recording(); }

  /// @brief Get visualized frame
  cv::Mat get_visualized_frame() {
    cv::Mat frame = video_capture_.get_frame();

    // If frame is empty, return empty frame
    if (frame.empty()) {
      return frame;
    }

    // Draw recoding status
    if (video_capture_.is_recording()) {
      cv::putText(frame, "Recording", cv::Point(10, 30),
                  cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);
    }

    return frame;
  }
};

}  // namespace character
}  // namespace CC

#endif  // CUSTOMCHAR_CHARACTER_CHARACTER_H_
