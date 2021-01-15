
#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <thread>
#include "gpio.h"
#include "utils.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct js_event {
  unsigned int time;
  short value;
  unsigned char type;
  unsigned char number;
};

#define JS_EVENT_BUTTON 0x01
#define JS_EVENT_AXIS 0x02
#define JS_EVENT_INIT 0x80

class ControlMap {
 public:
  void SetDeadZone(int16_t value);
  void SetStepSize(int16_t value);
  void CalculateParameters() {
    float x_start = (js_end_points_[0] - dead_zone_) / step_size_;
    float x_end = (js_end_points_[1] / step_size_);

    float y_start = pwm_delta_endpoints_[0];
    float y_end = pwm_delta_endpoints_[1];

    slope_ = (y_end - y_start) / (x_end - x_start);
    intercept_ = ((y_end + y_start) - slope_ * (x_end + x_start)) / 2;
  }

  float Map(int16_t value) {
    float abs_value = std::abs(value);
    if (abs_value <= dead_zone_) return 0.0f;

    float y = slope_ * (abs_value - dead_zone_) / step_size_ + intercept_;
    return value < 0 ? -y : y;
  }

 private:
  float slope_ = 0;
  float intercept_ = 0;
  float pwm_delta_endpoints_[2] = {0, 2.5};
  int16_t js_end_points_[2] = {0, 32767};
  int16_t dead_zone_ = 3000;
  int16_t step_size_ = 30;
};

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

  ControlMap map;

  map.CalculateParameters();

  std::cout << map.Map(32767) << " " << map.Map(0) << map.Map(-32767)
            << std::endl;

  std::atomic_bool stop = false;
  std::atomic<int> delta = 0;
  auto result = std::async([&]() {
    // Create PWM signal
    auto pwm_result = gpio.CreatePwm(kPinNum[gpio.GetBoardType()], 50, 50);
    auto pwm = pwm_result.second;
    double value = 7.5;
    double increment = 0.1;
    pwm->Start();
#ifdef DEBUG
    std::cout << "PWM started." << std::endl;
#endif
    while (!stop) {
      // std::this_thread::sleep_for(std::chrono::milliseconds(6));
      // if (value >= 9.5) {
      //   std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      //   increment = -increment;
      // }
      // if (value <= 5.5) {
      //   std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      //   increment = -increment;
      // }
      // value += increment;
      pwm->ResetDutyCycle(value + delta);
    }
    pwm->ResetDutyCycle(7.5f);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    pwm->Stop();
#ifdef DEBUG
    std::cout << "PWM stopped." << std::endl;
#endif
  });

  auto result2 = std::async([&]() {
    int fd = open("/dev/input/js0", O_RDONLY);

    if (fd < 0) {
      printf("Can not open device\n");
      return -1;
    } else {
      printf("Opened success\n");
    }

    struct js_event e;

    while (!stop) {
      read(fd, &e, sizeof(e));
      if (e.type == JS_EVENT_BUTTON) {
        printf("button: number %u / value %d\n", e.number, e.value);
      }

      if (e.type == JS_EVENT_AXIS) {
        if (e.number == 2) delta = map.Map(-e.value);
        printf("AXIS: number %u / value %d\n", e.number, e.value);
      }
    }

    close(fd);
  });

  std::cout << "Press any key to stop: ";
  auto c = std::getchar();
  std::cout << c << std::endl;
  stop = true;
  result.get();
  result2.get();

  return 0;
}
