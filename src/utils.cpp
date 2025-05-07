// src/utils.cpp
#include "../include/utils.hpp"
#include "../include/log.hpp"
#include <fstream>
#include <iostream>
#include <thread>

void printMenu()
{
  Logger::log("=== PIN Automation Menu ===");
  Logger::log("1. Run calibration");
  Logger::log("2. Load saved calibration");
  Logger::log("3. Save current calibration");
  Logger::log("4. Configure delays");
  Logger::log("5. Start automation");
  Logger::log("6. Exit");
  Logger::log("Enter your choice: ");
}

std::string getPinFileLocation()
{
  std::string path;
  Logger::log("Enter path to PIN file: ");
  std::getline(std::cin, path);
  return path;
}

std::vector<std::string> readPinsFromFile(const std::string &filePath)
{
  std::vector<std::string> pins;
  std::ifstream pinFile(filePath);
  if (!pinFile) {
    Logger::error("Failed to open file: " + filePath);
    return pins;
  }
  std::string pin;
  while (std::getline(pinFile, pin)) {
    pin.erase(0, pin.find_first_not_of(" \t\r\n"));
    pin.erase(pin.find_last_not_of(" \t\r\n") + 1);
    if (!pin.empty() &&
        pin.find_first_not_of("0123456789") == std::string::npos) {
      pins.push_back(pin);
      Logger::log("Read PIN: " + pin);
    } else if (!pin.empty()) {
      Logger::error("Skipping invalid PIN: " + pin);
    }
  }
  return pins;
}

void runAutomationLoop(VirtualKeyboard &keyboard,
                       const std::vector<std::string> &pins) 
{
  while (true) {
    if (keyboard.isRunning()) {
      for (const auto &pin : pins) {
        if (!keyboard.isRunning())
          break;
        Logger::log("Typing PIN: " + pin);
        keyboard.typePin(pin);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::this_thread::sleep_for(
            std::chrono::milliseconds(keyboard.getDelayBetweenPins()));
      }
      if (keyboard.isRunning()) {
        Logger::log("Finished all PINs. Press F12 to start again.");
        keyboard.setRunning(false);
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}