
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

  auto detect_result = gpio.Detect();
  if (!detect_result.second) {
    std::cout << "[ERROR]: Board detection failed with " << detect_result.first
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
      gpio.CreateBinary(kPinNum[gpio.GetBoardType()], jetson::Direction::IN,
                        jetson::Signal::HIGH);
  if (!setup_result.second) {
    std::cout << "[ERROR]: Gpio setup failed with." << setup_result.first
              << std::endl;
    return -1;
  }

  auto printer0 = [](int value) {
    std::cout << "print on both: " << value << std::endl;
  };

  auto printer1 = [](int value) {
    std::cout << "print on high: " << value << std::endl;
  };

  auto printer2 = [](int value) {
    std::cout << "print on low: " << value << std::endl;
  };

  setup_result.second->RegisterCallback(jetson::TriggerEdge::BOTH, printer0);
  setup_result.second->RegisterCallback(jetson::TriggerEdge::RISING, printer1);
  setup_result.second->RegisterCallback(jetson::TriggerEdge::FALLING, printer2);

  std::cout << "Press q for quit" << std::endl;

  while (int c = std::getchar()) {
    if (static_cast<char>(c) == 'q') break;
    if (static_cast<char>(c) == 'r') {
      std::cout << setup_result.second->Read() << std::endl;
    }
  }

  std::cout << "terminating" << std::endl;

  return 0;
}
