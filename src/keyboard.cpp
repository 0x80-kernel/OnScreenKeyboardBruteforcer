// src/keyboard.cpp
#include "../include/keyboard.hpp"
#include "../include/log.hpp"
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <atomic>
#include <fstream>
#include <iostream>
#include <thread>
#include <unistd.h>

VirtualKeyboard::VirtualKeyboard()
{
  display = XOpenDisplay(nullptr);
  if (!display) {
    Logger::error("Failed to open X11 display");
    exit(1);
  }
  digitPositions.resize(10, {0, 0});
  f12KeyCode = XKeysymToKeycode(display, XK_F12);
  if (f12KeyCode == 0) {
    Logger::error("Failed to get keycode for F12");
  }
  Window rootWindow = DefaultRootWindow(display);
  XGrabKey(display, f12KeyCode, AnyModifier, rootWindow, False, GrabModeAsync,
           GrabModeAsync);
  XSync(display, False);
}

VirtualKeyboard::~VirtualKeyboard()
{
  if (display) {
    XUngrabKey(display, f12KeyCode, AnyModifier, DefaultRootWindow(display));
    XCloseDisplay(display);
  }
}

void VirtualKeyboard::handleEvents()
{
  XEvent event;
  while (true) {
    if (XPending(display) > 0) {
      XNextEvent(display, &event);
      if (event.type == KeyPress && event.xkey.keycode == f12KeyCode) {
        bool current = running.load();
        running.store(!current);
        Logger::log(current ? "Automation stopped" : "Automation started");
      }
    } else {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
}

void VirtualKeyboard::setRunning(bool running) { this->running = running; }

bool VirtualKeyboard::isRunning() const { return running; }

void VirtualKeyboard::setDelayBetweenPins(unsigned int milliseconds)
{
  delayBetweenPins = milliseconds;
  Logger::log("Delay between PINs set to " + std::to_string(milliseconds) +
              " ms");
}

void VirtualKeyboard::setDelayBetweenDigits(unsigned int milliseconds)
{
  delayBetweenDigits = milliseconds;
  Logger::log("Delay between digits set to " + std::to_string(milliseconds) +
              " ms");
}

unsigned int VirtualKeyboard::getDelayBetweenPins()
const {
  return delayBetweenPins;
}

unsigned int VirtualKeyboard::getDelayBetweenDigits()
const {
  return delayBetweenDigits;
}

void VirtualKeyboard::calibratePositions()
{
  Logger::log("Starting calibration for digits 0-9...");
  Logger::log("Press left mouse button on the position for each digit when "
              "prompted...");
  Logger::log("After clicking, the position will be recorded and you'll move "
              "to the next digit.");
  Window rootWindow = DefaultRootWindow(display);
  int root_x, root_y;
  int win_x, win_y;
  unsigned int mask;
  Window child_win, root_win;
  for (int i = 0; i < 10; ++i) {
    Logger::log("Position cursor and click for digit " + std::to_string(i) +
                "...");
    bool clicked = false;
    while (!clicked) {
      XQueryPointer(display, rootWindow, &root_win, &child_win, &root_x,
                    &root_y, &win_x, &win_y, &mask);
      if (mask & Button1Mask) {
        digitPositions[i] = {root_x, root_y};
        Logger::log("Digit " + std::to_string(i) + " calibrated at: " +
                    std::to_string(root_x) + ", " + std::to_string(root_y));
        clicked = true;
        while (mask & Button1Mask) {
          XQueryPointer(display, rootWindow, &root_win, &child_win, &root_x,
                        &root_y, &win_x, &win_y, &mask);
          std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
  }
  Logger::log("Calibration complete!");
}

bool VirtualKeyboard::ensureButtonReleased()
{
  Window rootWindow = DefaultRootWindow(display);
  int root_x, root_y, win_x, win_y;
  unsigned int mask;
  Window child_win, root_win;
  XQueryPointer(display, rootWindow, &root_win, &child_win, &root_x, &root_y,
                &win_x, &win_y, &mask);
  if (mask & Button1Mask) {
    Logger::log("Detected stuck button - forcing release");
    XTestFakeButtonEvent(display, Button1, False, 0);
    XFlush(display);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return true;
  }
  return false;
}

void VirtualKeyboard::clickDigit(int digit)
{
  if (!running)
    return;
  if (digit < 0 || digit > 9) {
    Logger::error("Invalid digit: " + std::to_string(digit));
    return;
  }
  ensureButtonReleased();
  auto [x, y] = digitPositions[digit];
  Logger::log("Clicking digit " + std::to_string(digit) +
              " at position: " + std::to_string(x) + ", " + std::to_string(y));
  XTestFakeMotionEvent(display, 0, x, y, 0);
  XFlush(display);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  XTestFakeButtonEvent(display, Button1, True, 0); // Press
  XFlush(display);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  XTestFakeButtonEvent(display, Button1, False, 0); // Release
  XFlush(display);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  if (ensureButtonReleased()) {
    Logger::log("Had to force button release after clicking digit " +
                std::to_string(digit));
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(delayBetweenDigits));
}

void VirtualKeyboard::typePin(const std::string &pin)
{
  for (char c : pin) {
    if (!running)
      break;
    if (isdigit(c)) {
      int digit = c - '0';
      clickDigit(digit);
    } else {
      Logger::error("Invalid character in PIN: " + std::string(1, c));
    }
  }
  ensureButtonReleased();
}

bool VirtualKeyboard::saveCalibration(const std::string &filename)
{
  std::ofstream file(filename, std::ios::binary);
  if (!file) {
    Logger::error("Failed to open file for saving calibration: " + filename);
    return false;
  }
  size_t size = digitPositions.size();
  file.write(reinterpret_cast<const char *>(&size), sizeof(size));
  for (const auto &pos : digitPositions) {
    file.write(reinterpret_cast<const char *>(&pos.first), sizeof(pos.first));
    file.write(reinterpret_cast<const char *>(&pos.second), sizeof(pos.second));
  }
  Logger::log("Calibration saved to " + filename);
  return true;
}

bool VirtualKeyboard::loadCalibration(const std::string &filename)
{
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    Logger::error("Failed to open file for loading calibration: " + filename);
    return false;
  }
  size_t size;
  file.read(reinterpret_cast<char *>(&size), sizeof(size));
  if (size != 10) {
    Logger::error("Invalid calibration file: expected 10 positions, got " +
                  std::to_string(size));
    return false;
  }
  digitPositions.resize(size);
  for (auto &pos : digitPositions) {
    file.read(reinterpret_cast<char *>(&pos.first), sizeof(pos.first));
    file.read(reinterpret_cast<char *>(&pos.second), sizeof(pos.second));
  }
  Logger::log("Calibration loaded from " + filename);
  return true;
}