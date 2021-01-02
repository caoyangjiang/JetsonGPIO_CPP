/**
 * @file utils.h
 * @author xxx xxx (xxx@hypevr.com)
 * @brief
 * @version 0.1
 * @date 2021-01-01
 *
 * @copyright Copyright (c) 2021, 3NFINITE
 *
 */

#pragma once

#include <iostream>
#include "types.h"

namespace jetson {

void PrintChannelInfo(const ChannelInfo& info) {
  std::cout << "Channel Info for "
            << "\"" << info.channel << "\"" << std::endl;
  std::cout << "    "
            << "GPIO chip Dir: " << info.gpio_chip_dir << std::endl;
  std::cout << "    "
            << "Chip GPIO: " << info.chip_gpio << std::endl;
  std::cout << "    "
            << "GPIO: " << info.gpio << std::endl;
  std::cout << "    "
            << "GPIO Name: " << info.gpio_name << std::endl;
  std::cout << "    "
            << "PWM Sys Dir: "
            << (info.pwm_chip_dir == std::nullopt ? "N/A" : *info.pwm_chip_dir)
            << std::endl;
  std::cout << "    "
            << "Chip PWM Id: "
            << (info.chip_pwm_id == std::nullopt
                    ? "N/A"
                    : std::to_string(*info.chip_pwm_id))
            << std::endl;
}
}  // namespace jetson
