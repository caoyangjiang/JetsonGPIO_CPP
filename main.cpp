
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
  // auto setup_result = gpio.Setup(kPinNum[gpio.GetBoardType()],
  //                                jetson::Direction::OUT,
  //                                jetson::Signal::HIGH);
  // if (!setup_result.second) {
  //   std::cout << "[ERROR]: Gpio setup failed with." << setup_result.first
  //             << std::endl;
  //   return -1;
  // }
  std::atomic_bool stop = false;
  auto result = std::async([&]() {
    // Create PWM signal
    auto pwm = gpio.CreatePWMController(kPinNum[gpio.GetBoardType()], 50, 50);
    double value = 7.5;
    double increment = 0.5;
    pwm->Start();
#ifdef DEBUG
    std::cout << "PWM started." << std::endl;
#endif
    while (!stop) {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      if (value >= 10) increment = -increment;
      if (value <= 5) increment = -increment;
      value += increment;
      pwm->ResetDutyCycle(value);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    pwm->ResetDutyCycle(7.5f);
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    pwm->Stop();
#ifdef DEBUG
    std::cout << "PWM stopped." << std::endl;
#endif
  });

  std::cout << "Press any key to stop: ";
  auto c = std::getchar();
  std::cout << c << std::endl;
  stop = true;
  result.get();

  // gpio.TearDown();
  return 0;
}
