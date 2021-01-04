
#include <atomic>
#include <chrono>
#include <functional>
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

  // Set pin as an PWM pin
  auto pwm = gpio.CreatePWMController(kPinNum[gpio.GetBoardType()], 50, 50);

  // Set pin as an output pin with optional initial state of HIGH
  // auto setup_result = gpio.Setup(kPinNum[gpio.GetBoardType()],
  //                                jetson::Direction::OUT,
  //                                jetson::Signal::HIGH);
  // if (!setup_result.second) {
  //   std::cout << "[ERROR]: Gpio setup failed with." << setup_result.first
  //             << std::endl;
  //   return -1;
  // }

  pwm->Start();
  std::this_thread::sleep_for(std::chrono::seconds(5));

  pwm->Stop();

  // std::atomic_bool stop = false;
  // auto result = std::async([&](){
  //   // Create PWM signal
  //   auto pwm_ctl = Gpio.CreatePWMController(kPinNum[Gpio.GetBoardType()], 10,
  //   50); int value = 100; int increment = 5;
  //
  //   pwm_ctl.Start();
  //   #ifdef DEBUG
  //     std::cout << "PWM started." << std::endl
  //   #endif
  //   while(!stop) {
  //     std::this_thread::sleep_for(std::chrono::milliseconds(10));
  //     if (value >= 100) increment = -increment;
  //     if (value <= 0) increment = -increment;
  //     value += increment;
  //     pwm_ctl.ResetDutyCycle(value);
  //   }
  //   pwm_ctl.Stop();
  //   #ifdef DEBUG
  //     std::cout << "PWM stopped." << std::endl
  //   #endif
  // }
  // );
  //
  // std::cout << "Press any key to stop: ";
  // auto c = std::getchar();
  // std::cout << c << std::endl;
  // stop = true;
  // result.get();
  // Gpio.Cleanup();

  // gpio.TearDown();
  return 0;
}
