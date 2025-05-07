// src/main.cpp
#include "../include/keyboard.hpp"
#include "../include/log.hpp"
#include "../include/utils.hpp"
#include <iostream>
#include <limits>
#include <thread>
#include <vector>

int main()
{
  Logger::log("Starting PIN automation program");
  VirtualKeyboard keyboard;
  std::vector<std::string> pins;
  bool calibrated = false;
  std::thread eventThread([&keyboard]() { keyboard.handleEvents(); });
  eventThread.detach();
  int choice = 0;
  bool running = true;
  while (running) {
    printMenu();
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    switch (choice) {
    case 1: { // Run calibration
      keyboard.calibratePositions();
      calibrated = true;
      break;
    }
    case 2: { // Load calibration
      std::string filename;
      Logger::log("Enter calibration filename (default: calibration.dat): ");
      std::getline(std::cin, filename);
      if (filename.empty())
        filename = "calibration.dat";
      if (keyboard.loadCalibration(filename)) {
        calibrated = true;
      }
      break;
    }
    case 3: { // Save calibration
      if (!calibrated) {
        Logger::error("Cannot save: No calibration data available");
        break;
      }
      std::string filename;
      Logger::log("Enter calibration filename (default: calibration.dat): ");
      std::getline(std::cin, filename);
      if (filename.empty())
        filename = "calibration.dat";

      keyboard.saveCalibration(filename);
      break;
    }
    case 4: { // Configure delays
      unsigned int pinDelay, digitDelay;
      Logger::log("Current delay between PINs: " +
                  std::to_string(keyboard.getDelayBetweenPins()) + " ms");
      Logger::log(
          "Enter new delay between PINs (in milliseconds, default 1000): ");
      std::string input;
      std::getline(std::cin, input);
      if (!input.empty()) {
        try {
          pinDelay = std::stoi(input);
          keyboard.setDelayBetweenPins(pinDelay);
        } catch (const std::exception &e) {
          Logger::error("Invalid input. Using default value.");
        }
      }

      Logger::log("Current delay between digits: " +
                  std::to_string(keyboard.getDelayBetweenDigits()) + " ms");
      Logger::log(
          "Enter new delay between digits (in milliseconds, default 100): ");
      input.clear();
      std::getline(std::cin, input);
      if (!input.empty()) {
        try {
          digitDelay = std::stoi(input);
          keyboard.setDelayBetweenDigits(digitDelay);
        } catch (const std::exception &e) {
          Logger::error("Invalid input. Using default value.");
        }
      }

      Logger::log("Delays configured successfully.");
      break;
    }
    case 5: { // Start automation
      if (!calibrated) {
        Logger::error("Cannot start: No calibration data available");
        break;
      }
      if (pins.empty()) {
        std::string pinFilePath = getPinFileLocation();
        pins = readPinsFromFile(pinFilePath);
        if (pins.empty()) {
          Logger::error("No valid PINs found in the file");
          break;
        }
      }
      Logger::log("Found " + std::to_string(pins.size()) + " valid PINs");
      Logger::log("Using delay between PINs: " +
                  std::to_string(keyboard.getDelayBetweenPins()) + " ms");
      Logger::log("Using delay between digits: " +
                  std::to_string(keyboard.getDelayBetweenDigits()) + " ms");
      Logger::log("Press F12 to start/stop the automation");
      std::thread automationThread(
          [&keyboard, &pins]() { runAutomationLoop(keyboard, pins); });
      automationThread.detach();
      keyboard.setRunning(true);
      break;
    }
    case 6:
      running = false;
      break;
    default:
      Logger::error("Invalid choice");
      break;
    }
  }
  Logger::log("Exiting PIN automation program");
  return 0;
}