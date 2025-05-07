// src/log.cpp
#include "../include/log.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>

void Logger::log(const std::string &message)
{
  auto now = std::chrono::system_clock::now();
  auto now_time = std::chrono::system_clock::to_time_t(now);
  std::cout << "["
            << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S")
            << "] " << message << std::endl;
}

void Logger::error(const std::string &message)
{
  auto now = std::chrono::system_clock::now();
  auto now_time = std::chrono::system_clock::to_time_t(now);
  std::cerr << "["
            << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S")
            << "] ERROR: " << message << std::endl;
}