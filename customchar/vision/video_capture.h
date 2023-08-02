#ifndef CUSTOMCHAR_VISION_VIDEO_CAPTURE_H_
#define CUSTOMCHAR_VISION_VIDEO_CAPTURE_H_

#include <unistd.h>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include <opencv2/opencv.hpp>
#include <subprocess.hpp>

namespace CC {
namespace vision {

#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) ||  \
    defined(__WIN32__) || defined(WIN64) || defined(_WIN64) || \
    defined(__WIN64__)
#include <fcntl.h>
#include <io.h>
#define SET_BINARY_MODE(handle) setmode(handle, O_BINARY)
#else
#define SET_BINARY_MODE(handle) ((void)0)
#endif
#define BUFSIZE 10240

using subprocess::CompletedProcess;
using subprocess::PipeOption;
using subprocess::Popen;
using subprocess::RunBuilder;

enum class VideoCaptureMode {
  kNone,
  kOpenCV,
  kFFmpeg,
};

/// @brief VideoCapture class
class VideoCapture {
 private:
  int device_id_ = 0;
  bool is_capturing_;

  cv::VideoCapture capture_;
  std::mutex capture_mutex_;

  cv::Mat frame_;
  std::mutex frame_mutex_;
  std::thread capture_thread_;

  VideoCaptureMode mode_;
  std::string save_video_path_;

  /// @brief Start capturing frames from device
  bool start_device() {
    capture_ = cv::VideoCapture(device_id_);
    if (!capture_.isOpened()) {
      std::cerr << "Error opening video stream or file" << std::endl;
      return false;
    } else {
      return true;
    }
  }

  /// @brief Stop capturing frames from device
  void stop_device() {
    if (mode_ != VideoCaptureMode::kOpenCV) return;
    if (capture_.isOpened()) capture_.release();
  }

  /// @brief Capture frames from camera
  void capture_opencv() {
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
        std::cout << "Empty frame" << std::endl;
        break;
      }
      std::lock_guard<std::mutex> lock(frame_mutex_);
      frame_ = frame.clone();
    }
  }

  void capture_ffmpeg() {
    std::stringstream ss;
    ss << "ffmpeg -y -f avfoundation -ac 2 -framerate 30 -i 0:0 -c:a aac -ab "
          "96k -f matroska "
       << save_video_path_ << " -vcodec mjpeg -f image2pipe -framerate 30 -";

    std::vector<std::string> args;
    std::string arg;
    while (ss >> arg) {
      args.push_back(arg);
    }
    Popen popen = subprocess::RunBuilder(args)
                      .cout(PipeOption::pipe)
                      .cin(PipeOption::pipe)
                      .popen();

    std::vector<char> data;
    bool skip = true;
    bool imgready = false;
    bool ff = false;
    int readbytes = -1;
    while (is_capturing_) {
      char ca[BUFSIZE];
      uchar c;
      if (readbytes != 0) {
        readbytes = subprocess::pipe_read(popen.cout, ca, BUFSIZE);
        for (int i = 0; i < readbytes; i++) {
          c = ca[i];
          if (ff && c == (uchar)0xd8) {
            skip = false;
            data.push_back((uchar)0xff);
          }
          if (ff && c == 0xd9) {
            imgready = true;
            data.push_back((uchar)0xd9);
            skip = true;
          }
          ff = c == 0xff;
          if (!skip) {
            data.push_back(c);
          }
          if (imgready) {
            if (data.size() != 0) {
              cv::Mat data_mat(data);
              cv::Mat frame(imdecode(data_mat, 1));
              {
                std::lock_guard<std::mutex> lock(frame_mutex_);
                frame_ = frame.clone();
              }
            }
            imgready = false;
            skip = true;
            data.clear();
          }
        }
      } else {
        // std::cerr << "zero byte read" << std::endl;
      }
    }

    // Close ffmpeg pipe
    subprocess::pipe_write(popen.cin, "q\n", std::strlen("q\n"));
    popen.terminate();
  }

 public:
  VideoCapture() {}

  /// @brief Get frame width
  /// @return int
  int get_frame_width() {
    std::lock_guard<std::mutex> lock(frame_mutex_);
    return frame_.cols;
  }

  /// @brief Get frame height
  /// @return int
  int get_frame_height() {
    std::lock_guard<std::mutex> lock(frame_mutex_);
    return frame_.rows;
  }

  /// @brief Start recording frames to file
  /// @param output_path
  void Record(const std::string& output_path) {
    if (is_capturing_) Stop();
    is_capturing_ = true;
    save_video_path_ = output_path;
    mode_ = VideoCaptureMode::kFFmpeg;
    capture_thread_ = std::thread(&VideoCapture::capture_ffmpeg, this);
  }

  /// @brief Start capturing frames
  void Start() {
    {
      std::lock_guard<std::mutex> lock(capture_mutex_);
      if (is_capturing_) {
        std::cerr << "Already capturing" << std::endl;
        return;
      }
    }
    if (!start_device()) return;
    // Get first frame to initialize frame size
    std::lock_guard<std::mutex> frame_lock(frame_mutex_);
    capture_ >> frame_;
    std::cout << "Frame size: " << frame_.cols << "x" << frame_.rows
              << std::endl;
    is_capturing_ = true;
    mode_ = VideoCaptureMode::kOpenCV;
    // Random a saved video path from time
    save_video_path_ = "";
    capture_thread_ = std::thread(&VideoCapture::capture_opencv, this);
  }

  /// @brief Stop capturing frames
  void Stop() {
    {
      std::lock_guard<std::mutex> lock(capture_mutex_);
      if (!is_capturing_) {
        return;
      }
    }
    stop_device();
    is_capturing_ = false;
    mode_ = VideoCaptureMode::kNone;
    capture_thread_.join();
  }

  /// @brief Get frame from queue
  /// @return cv::Mat. Empty if queue is empty
  cv::Mat get_frame() {
    cv::Mat frame;
    std::lock_guard<std::mutex> lock(frame_mutex_);
    frame = frame_.clone();
    return frame;
  }

  /// @brief Check if is recording
  bool is_recording() {
    return is_capturing_ && mode_ == VideoCaptureMode::kFFmpeg;
  }
};

}  // namespace vision
}  // namespace CC

#endif  // CUSTOMCHAR_SESSION_CHAT_HISTORY_H_
