#ifndef CUSTOMCHAR_VISION_VIDEO_CAPTURE_H_
#define CUSTOMCHAR_VISION_VIDEO_CAPTURE_H_

#include <queue>
#include <string>
#include <thread>
#include <vector>

#include <opencv2/opencv.hpp>

namespace CC {
namespace vision {

class VideoCapture {
 private:
  cv::VideoCapture capture_;
  cv::Mat frame_;
  std::mutex frame_mutex_;
  std::thread capture_thread_;

 public:
  VideoCapture() {
    capture_ = cv::VideoCapture(0);
    if (!capture_.isOpened()) {
      std::cout << "Error opening video stream or file" << std::endl;
      exit(-1);
    }
  }

  /// @brief Capture frames from camera
  void Capture() {
    cv::Mat frame;
    while (true) {
      capture_ >> frame;
      if (frame.empty()) {
        break;
      }
      std::lock_guard<std::mutex> lock(frame_mutex_);
      frame_ = frame.clone();
    }
  }

  /// @brief Start capturing frames
  void Start() { capture_thread_ = std::thread(&VideoCapture::Capture, this); }

  /// @brief  Stop capturing frames
  void Stop() { capture_thread_.join(); }

  /// @brief Get frame from queue
  /// @return cv::Mat. Empty if queue is empty
  cv::Mat GetFrame() {
    cv::Mat frame;
    std::lock_guard<std::mutex> lock(frame_mutex_);
    frame = frame_.clone();
    return frame;
  }
};

}  // namespace vision
}  // namespace CC

#endif  // CUSTOMCHAR_SESSION_CHAT_HISTORY_H_
