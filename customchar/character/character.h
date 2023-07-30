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

  std::function<void(std::string)> OnUserMessage_;
  std::function<void(std::string)> OnBotMessage_;
  std::function<void()> OpenCameraView_;
  std::function<void()> CloseCameraView_;

  bool is_muted_ = false;

  vision::VideoCapture video_capture_;

 public:
  /// @brief Constructor
  /// @param init_params
  Character(common::CCParams init_params);

  /// @brief Set mute (do not speak)
  void SetMute(bool is_muted);

  /// @brief Get mute status
  bool IsMuted();

  /// @brief Set on user message callback
  /// @param OnUserMessage_
  void SetOnUserMessage(std::function<void(std::string)> OnUserMessage_);

  /// @brief Set on bot message callback
  /// @param OnBotMessage_
  void SetOnBotMessage(std::function<void(std::string)> OnBotMessage_);

  /// @brief Set open camera view callback
  void SetOpenCameraView(std::function<void()> func) { OpenCameraView_ = func; }

  /// @brief Set close camera view callback
  void SetCloseCameraView(std::function<void()> func) {
    CloseCameraView_ = func;
  }

  /// @brief Run character in a loop
  void Run();

  /// @brief Get video capture
  vision::VideoCapture& GetVideoCapture() { return video_capture_; }

  /// @brief Start video capture
  void StartVideoCapture() {
    if (OpenCameraView_) OpenCameraView_();
    video_capture_.Start();
    std::cout << "Started video capture" << std::endl;
  }

  /// @brief Stop video capture
  void StopVideoCapture() {
    if (CloseCameraView_) CloseCameraView_();
    video_capture_.Stop();
  }

  /// @brief Record video
  void StartVideoRecoding(const std::string& filename) {
    if (OpenCameraView_) OpenCameraView_();
    video_capture_.Record(filename);
  }

  /// @brief Stop recording video
  bool IsRecording() { return video_capture_.IsRecording(); }

  /// @brief Get visualized frame
  cv::Mat GetVisualizedFrame() {
    cv::Mat frame = video_capture_.GetFrame();

    // If frame is empty, return empty frame
    if (frame.empty()) {
      return frame;
    }

    // Draw recoding status
    if (video_capture_.IsRecording()) {
      cv::putText(frame, "Recording", cv::Point(10, 30),
                  cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);
    }

    return frame;
  }
};

}  // namespace character
}  // namespace CC

#endif  // CUSTOMCHAR_CHARACTER_CHARACTER_H_
