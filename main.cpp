#include <opencv2/opencv.hpp>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>

#ifndef _WIN32
#include <sys/ioctl.h>
#include <unistd.h>
#else
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <io.h>
#include <stdio.h>
#include <windows.h>
#endif

#ifndef _WIN32
//works in macOS Terminal/iTerm2 and VS Code integrated terminal (PTY).
//returns {0,0} if stdout isn't a terminal (e.g., piped to a file).
//finds the current terminal size
bool getTerminalSize(int &rows, int &cols)
{
  rows = 0;
  cols = 0;

  if (!isatty(STDOUT_FILENO))
  {
    std::cerr << "error with terminal" << std::endl;
    return false;
  }
  
  winsize w{};
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != 0)
  {
    std::cerr << "error with terminal" << std::endl;
    return false;
  }
  
  //leave row for the cursor
  rows = w.ws_row - 1;
  cols = w.ws_col;

  return (w.ws_row > 0) && (w.ws_col > 0);
}
#else
// Win32 implementation was produced with AI, modeled after the macOS/POSIX version above.
bool getTerminalSize(int &rows, int &cols)
{
  rows = 0;
  cols = 0;

  if (!_isatty(_fileno(stdout)))
  {
    std::cerr << "error with terminal" << std::endl;
    return false;
  }

  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE)
  {
    std::cerr << "error with terminal" << std::endl;
    return false;
  }

  CONSOLE_SCREEN_BUFFER_INFO csbi{};
  if (!GetConsoleScreenBufferInfo(hOut, &csbi))
  {
    std::cerr << "error with terminal" << std::endl;
    return false;
  }

  const LONG winRows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
  const LONG winCols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  // leave row for the cursor
  rows = static_cast<int>(winRows) - 1;
  cols = static_cast<int>(winCols);

  return (winRows > 0) && (winCols > 0);
}
#endif

//converts a bgr pixel value to greyscale value (0-255)
int convertGreyscale(const cv::Vec3b &pixel) 
{
  //apply ITU-R BT.601 standard to calculate luminance (BGR order).
  int greyPixel = (29 * pixel[0]) + (150 * pixel[1]) + (77 * pixel[2]);
  greyPixel = greyPixel >> 8;

  return greyPixel;
}

//calculates the minimum scale to downsample the result so it fits within the terminal
//each character has a 2x1 ratio (height x width)
void findScale(const cv::Mat &input, int &frows, int &fcols, int &scale) 
{
  int trows = 0;
  int tcols = 0;
  frows = 0;
  fcols = 0;
  scale = 0;

  //if there was an error with the terminal
  if (!getTerminalSize(trows, tcols)) 
  {
    std::cerr << "error with terminal" << std::endl;
    return;
  }

  if (trows <= 0 || tcols <= 0)
  {
    std::cerr << "error with terminal" << std::endl;
    return;
  }

  //use round up integer math to find the scale
  scale = std::max((input.cols + tcols - 1)/tcols, (input.rows + (2*trows) - 1)/(2*trows));
  frows = input.rows / (2 * scale);
  fcols = input.cols / scale;
}

//converts the pixels to characters based on luminance and returns the entire image as a single string
void ImageToASCII(cv::Mat &image, int scale, int frows, int fcols, std::string &output) 
{
#ifndef _WIN32
  //estimated using the number of active pixels in IBM VGA 8x16 bitmap font
  static const std::string ascii = R"(   ..'',:_-^";i~=r(f+vcx[sneahwkZUXSGDKARNWQ&%MM88@@@)";
#else
  //calibrated with Windows 11's Cascadia Mono font
  static const std::string ascii = R"( .':,_;^~"+=vxi[Shk%8&RNW@)";
#endif
  static const int asciiLast = ascii.length() - 1;

  output.clear();

  if (scale == 0) 
  {
    std::cerr << "scale cannot be 0" << std::endl;
    return;
  } 
  
  for (int row = 0; row < frows; row++) 
  {
    cv::Vec3b *ptr = image.ptr<cv::Vec3b>(row * (scale * 2));

    for (int col = 0; col < fcols; col++) 
    {
      //take the pixel, convert it to greyscale, then find the correct char
      output += ascii[convertGreyscale(ptr[col * scale]) * asciiLast / 255];
    }
    output += '\n';
  }
}

//main; determines whether the input is video or image 
//runs the whole process of printing ascii versions
int main(int argc, char **argv) 
{
  if (argc < 2) 
  {
    std::cerr << "usage: " << argv[0] << " <file_path>" << std::endl;
    return 1;
  }
  const char *path = argv[1];

  //clears terminal, sets cursor to top-left
  std::cout << "\033[2J\033[H";

  int frows = 0;
  int fcols = 0;
  int scale = 0;
  std::string outFrame;

  //for image input
  cv::Mat input = cv::imread(path, cv::IMREAD_COLOR);
  if (!input.empty()) 
  {
    findScale(input, frows, fcols, scale); //set frows, fcols, and scale
    outFrame.reserve( (frows) * (fcols + 1) );
    ImageToASCII(input, scale, frows, fcols, outFrame);
    std::cout << outFrame << std::endl;
    return 0;
  }

  //video input
  cv::VideoCapture video = cv::VideoCapture(path);
  if (!video.isOpened()) 
  {
    std::cerr << "failed to open as image or video: " << path << std::endl;
    return 1;
  }

  cv::Mat frame; 
  
  using clock = std::chrono::steady_clock;
  clock::time_point start;
  
  int frameCount = 0;
  
  double fps = video.get(cv::CAP_PROP_FPS);
  if (fps <= 0.0)
  {
    std::cerr << "error: fps cannot be 0" << std::endl;
    return 1;
  }
  double frameTime = 1.0/fps;
  double endTime = 0.0;
  
  //converting through the video frame by frame
  while(video.grab()) 
  {
    video.retrieve(frame);

    //only runs the first time
    if (scale == 0)
    {
      //set frows, fcols, and scale
      findScale(frame, frows, fcols, scale);
      if (scale == 0)
      {
        return 1;
      }
      outFrame.reserve( frows * (fcols + 1) );
    }

    ImageToASCII(frame, scale, frows, fcols, outFrame);

    endTime = frameTime * frameCount;
    //starts counting right before the first frame is printed
    if (start == clock::time_point{})
    {
      start = clock::now();
    }

    //stalls until the designated time to print
    while (std::chrono::duration<double>(clock::now() - start).count() < endTime)
    {
      std::this_thread::yield();
    }

    // move cursor to top-left
    std::cout << "\033[H";
    //prints the frame
    std::cout << outFrame;
    frameCount++;
  }
  return 0;
}
