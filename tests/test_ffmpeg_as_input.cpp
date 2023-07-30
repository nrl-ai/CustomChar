#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <stdio.h>
#include <unistd.h>

using namespace cv;

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

int main(int argc, char** argv) {
  SET_BINARY_MODE(fileno(stdin));
  auto ffmpeg_pipe = popen(
      "ffmpeg -y -f avfoundation -ac 2 -framerate 30 -i \"0:0\" -c:a aac -ab "
      "96k -f matroska output.mkv -vcodec mjpeg -f image2pipe -framerate 30 -",
      "r");
  SET_BINARY_MODE(fileno(ffmpeg_pipe));

  // Prevent ffmpeg from buffering
  setvbuf(ffmpeg_pipe, NULL, _IONBF, 0);

  // Prevent ffmpeg from outputting to stdout
  dup2(fileno(ffmpeg_pipe), fileno(stdout));
  dup2(fileno(ffmpeg_pipe), fileno(stderr));

  std::vector<char> data;
  bool skip = true;
  bool imgready = false;
  bool ff = false;
  int readbytes = -1;
  while (1) {
    char ca[BUFSIZE];
    uchar c;
    if (readbytes != 0) {
      readbytes = read(fileno(ffmpeg_pipe), ca, BUFSIZE);
      //   readbytes = read(0, ca, BUFSIZE);
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

            imshow("frame", frame);
            waitKey(1);
          } else {
            printf("warning");
          }
          imgready = false;
          skip = true;
          data.clear();
        }
      }
    } else {
      throw std::string("zero byte read");
    }
  }
}
