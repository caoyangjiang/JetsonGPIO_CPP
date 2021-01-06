
#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <thread>
#include "gpio.h"
#include "utils.h"

int main() {
  jetson::Gpio gpio;
  if (!gpio.Detect()) {
    std::cout << "[ERROR]: Board detection failed! Can't find board type."
              << std::endl;
    return -1;
  }

  PrintChannelInfo(gpio.GetChannelInfo(jetson::BoardMode::BOARD, "18"));

  std::map<jetson::BoardType, std::string> kPinNum = {
      {jetson::BoardType::JETSON_XAVIER, "18"},
      {jetson::BoardType::JETSON_NANO, "33"}};

  // Board pin-numbering scheme
  if (!gpio.SetMode(jetson::BoardMode::BOARD).second) {
    std::cout << "[ERROR]: Set board mode failed! Can't reassign board mode."
              << std::endl;
    return -1;
  }

  // Set pin as an output pin with optional initial state of HIGH
  auto setup_result =
      gpio.CreateBinary(kPinNum[gpio.GetBoardType()], jetson::Direction::OUT,
                        jetson::Signal::HIGH);
  if (!setup_result.second) {
    std::cout << "[ERROR]: Gpio setup failed with." << setup_result.first
              << std::endl;
    return -1;
  }

  auto signal = setup_result.second;

  std::cout << "Press 0 for low"
            << "\n"
            << "Press 1 for high"
            << "\n"
            << "Press q for quit" << std::endl;
  while (int c = std::getchar()) {
    if (static_cast<char>(c) == 'q') break;
    if (static_cast<char>(c) == '0') signal->Write(0);
    if (static_cast<char>(c) == '1') signal->Write(1);
  }

  return 0;
}
