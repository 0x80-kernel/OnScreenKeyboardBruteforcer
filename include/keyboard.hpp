// include/keyboard.hpp
#pragma once
#include <X11/Xlib.h>
#include <atomic>
#include <string>
#include <utility>
#include <vector>

class VirtualKeyboard {
public:

  VirtualKeyboard();
  ~VirtualKeyboard();

  void calibratePositions();
  void clickDigit(int digit);
  void typePin(const std::string &pin);
  void setRunning(bool running);
  bool isRunning() const;
  void handleEvents();
  bool saveCalibration(const std::string &filename = "calibration.dat");
  bool loadCalibration(const std::string &filename = "calibration.dat");
  void setDelayBetweenPins(unsigned int milliseconds);
  void setDelayBetweenDigits(unsigned int milliseconds);
  unsigned int getDelayBetweenPins() const;
  unsigned int getDelayBetweenDigits() const;

private:
  Display *display;
  std::vector<std::pair<int, int>> digitPositions;
  std::atomic<bool> running{false};
  KeyCode f12KeyCode;
  unsigned int delayBetweenPins{1000};    // Default: 1 second between PINs
  unsigned int delayBetweenDigits{100};   // Default: 100ms between digits
  bool ensureButtonReleased();
};