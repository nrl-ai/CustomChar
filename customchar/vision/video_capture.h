#ifndef CUSTOMCHAR_VISION_VIDEO_CAPTURE_H_
#define CUSTOMCHAR_VISION_VIDEO_CAPTURE_H_

#include <unistd.h>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include <opencv2/opencv.hpp>

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
    if (mode_ != VideoCaptureMode::kOpenCV) return;
    if (capture_.isOpened()) capture_.release();
  }

  /// @brief Capture frames from camera
  void CaptureOpenCV() {
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

  void CaptureFFmpeg() {
    std::stringstream ss;
    ss << "ffmpeg -y -f avfoundation -ac 2 -framerate 30 -i \"0:0\" -c:a aac "
          "-ab "
          "96k -f matroska "
       << save_video_path_ << " -vcodec mjpeg -f image2pipe -framerate 30 -";
    auto ffmpeg_pipe = popen(ss.str().c_str(), "r");
    SET_BINARY_MODE(fileno(ffmpeg_pipe));

    // Prevent ffmpeg from buffering
    // setvbuf(ffmpeg_pipe, NULL, _IONBF, 0);

    // // Prevent ffmpeg from outputting to stdout
    // dup2(fileno(ffmpeg_pipe), fileno(stdout));
    // dup2(fileno(ffmpeg_pipe), fileno(stderr));

    std::vector<char> data;
    bool skip = true;
    bool imgready = false;
    bool ff = false;
    int readbytes = -1;
    while (is_capturing_) {
      char ca[BUFSIZE];
      uchar c;
      if (readbytes != 0) {
        readbytes = read(fileno(ffmpeg_pipe), ca, BUFSIZE);
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
        std::cerr << "zero byte read" << std::endl;
      }
    }

    // Close ffmpeg pipe
    pclose(ffmpeg_pipe);
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

  /// @brief Start recording frames to file
  /// @param output_path
  void Record(const std::string& output_path) {
    if (is_capturing_) Stop();
    is_capturing_ = true;
    save_video_path_ = output_path;
    capture_thread_ = std::thread(&VideoCapture::CaptureFFmpeg, this);
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
    if (!StartDevice()) return;
    // Get first frame to initialize frame size
    std::lock_guard<std::mutex> frame_lock(frame_mutex_);
    capture_ >> frame_;
    std::cout << "Frame size: " << frame_.cols << "x" << frame_.rows
              << std::endl;
    is_capturing_ = true;
    mode_ = VideoCaptureMode::kOpenCV;
    // Random a saved video path from time
    save_video_path_ = "";
    capture_thread_ = std::thread(&VideoCapture::CaptureOpenCV, this);
  }

  /// @brief Stop capturing frames
  void Stop() {
    {
      std::lock_guard<std::mutex> lock(capture_mutex_);
      if (!is_capturing_) {
        return;
      }
    }
    StopDevice();
    is_capturing_ = false;
    mode_ = VideoCaptureMode::kNone;
    capture_thread_.join();
  }

  /// @brief Get frame from queue
  /// @return cv::Mat. Empty if queue is empty
  cv::Mat GetFrame() {
    cv::Mat frame;
    std::lock_guard<std::mutex> lock(frame_mutex_);
    frame = frame_.clone();
    return frame;
  }

  /// @brief Check if is recording
  bool IsRecording() {
    return is_capturing_ && mode_ == VideoCaptureMode::kFFmpeg;
  }
};

}  // namespace vision
}  // namespace CC

#endif  // CUSTOMCHAR_SESSION_CHAT_HISTORY_H_
