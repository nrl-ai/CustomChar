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
  int device_id_;
  bool is_capturing_;

  cv::VideoCapture capture_;
  std::mutex capture_mutex_;

  cv::Mat frame_;
  std::mutex frame_mutex_;
  std::thread capture_thread_;

  /// @brief Start capturing frames from device
  bool StartDevice() {
    capture_ = cv::VideoCapture(device_id_);
    if (!capture_.isOpened()) {
      std::cerr << "Error opening video stream or file" << std::endl;
      return false;
    } else {
      return true;
    }
  }

  /// @brief Stop capturing frames from device
  void StopDevice() {
    if (capture_.isOpened()) capture_.release();
  }

 public:
  VideoCapture() {}

  /// @brief Get frame width
  /// @return int
  int GetFrameWidth() {
    std::lock_guard<std::mutex> lock(frame_mutex_);
    return frame_.cols;
  }

  /// @brief Get frame height
  /// @return int
  int GetFrameHeight() {
    std::lock_guard<std::mutex> lock(frame_mutex_);
    return frame_.rows;
  }

  /// @brief Capture frames from camera
  void Capture() {
    cv::Mat frame;
    while (true) {
      {
        std::lock_guard<std::mutex> lock(capture_mutex_);
        if (!is_capturing_) {
          break;
        }
        capture_ >> frame;
      }
      if (frame.empty()) {
        break;
      }
      std::lock_guard<std::mutex> lock(frame_mutex_);
      frame_ = frame.clone();
    }
  }

  /// @brief Start capturing frames
  void Start() {
    {
      std::lock_guard<std::mutex> lock(capture_mutex_);
      if (is_capturing_) {
        return;
      }
      if (!StartDevice()) return;
      // Get first frame to initialize frame size
      std::lock_guard<std::mutex> frame_lock(frame_mutex_);
      capture_ >> frame_;
      is_capturing_ = true;
    }
    capture_thread_ = std::thread(&VideoCapture::Capture, this);
  }

  /// @brief Stop capturing frames
  void Stop() {
    std::lock_guard<std::mutex> lock(capture_mutex_);
    if (!is_capturing_) {
      return;
    }
    StopDevice();
  }

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
