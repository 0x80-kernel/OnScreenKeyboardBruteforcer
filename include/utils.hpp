// include/utils.hpp
#pragma once
#include "keyboard.hpp"
#include <string>
#include <vector>

void printMenu();
std::string getPinFileLocation();
std::vector<std::string> readPinsFromFile(const std::string &filePath);
void runAutomationLoop(VirtualKeyboard &keyboard,
                       const std::vector<std::string> &pins);