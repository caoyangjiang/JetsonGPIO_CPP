/**
 * @file gpio.cpp
 * @author Caoyang Jiang (caoyangjiang@gmail.com)
 * @brief
 * @version 0.1
 * @date 2020-12-26
 *
 * @copyright 
 * 
 * Copyright (c) 2020, Caoyang Jiang
 * Copyright (c) 2012-2017 Ben Croston <ben@croston.org>.
 * Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */


#include "gpio.h"
#include <algorithm>
#include <experimental/filesystem>
#include <fstream>
#include "gpio_pin_data.h"

#ifdef DEBUG
#include <iostream>
#endif

namespace fs = std::experimental::filesystem;

namespace jetson {

bool Gpio::Detect(std::string* error_message) {
  type_ = jetson::BoardType::UNKNOWN;

  const std::string kCompatsPath = "/proc/device-tree/compatible";
  const std::string kIdsPath = "/proc/device-tree/chosen/plugin-manager/ids";

  std::ifstream compats_file(kCompatsPath, std::ios::in);
  if (error_message != nullptr && !compats_file.is_open()) {
    *error_message = "failed to open compatible file at " + kCompatsPath;
    return false;
  }

  std::vector<std::string> detected_compatibles;

  // 1. Retrieve all compatibles
  std::stringstream ss;
  ss << compats_file.rdbuf();
  std::string compatible;
  while (std::getline(ss, compatible, static_cast<char>(0x00))) {
    detected_compatibles.push_back(compatible);
  }

  // 2. Find a matching compatible
  for (const auto& bc : jetson::kBoardCompats) {
    for (const auto& c : bc.second) {
      auto result = std::find(detected_compatibles.begin(),
                              detected_compatibles.end(), c);

      if (result != detected_compatibles.end()) {
        type_ = bc.first;
        goto COMPATIBLE_DETECT_DONE;
      }
    }
  }

COMPATIBLE_DETECT_DONE:
  if (error_message != nullptr && type_ == jetson::BoardType::UNKNOWN) {
    *error_message = "failed to deterime board type from compatibles.";
    return false;
  }

  // board type determined.
  const auto& kPinDefs = kBoardPins.at(type_);

  // 3. Find a matching carrier board
  if (error_message != nullptr && !fs::exists(kIdsPath)) {
    *error_message = "ids path " + kIdsPath + " do not exist.";
    return false;
  }

  auto carried_board_id =
      std::to_string(jetson::kBoardInfos.at(type_).carrier_board);
  std::optional<std::string> matching_id_file;
  for (auto& p : fs::directory_iterator(kIdsPath)) {
    auto path = p.path().filename().string();
    if (path.substr(0, carried_board_id.size()) == carried_board_id) {
      matching_id_file = path;
      break;
    }
  }

  if (matching_id_file == std::nullopt) {
    std::cout << "WARNING: Carrier board is not from a Jetson Developer "
                 "Kit.\nWARNNIG: Jetson.GPIO library has not been verified with"
                 "this carrier board.\nWARNING: and in fact is unlikely to "
                 "work correctly.\n"
              << std::endl;
  }

#ifdef DEBUG
  std::cout << "[info]: Id file " << *matching_id_file << '\n';
#endif

  // 4. Get GPIO chip offsets
  const std::vector<std::string> kSysfsPrefixes = {"/sys/devices/",
                                                   "/sys/devices/platform/"};

  std::map<std::string, std::string> gpio_chip_dirs;
  std::map<std::string, int> gpio_chip_base;
  std::map<std::string, int> gpio_chip_ngpio;

  for (const auto& pin_def : kPinDefs) {
    std::string gpio_chip_dir = "";

    if (pin_def.chip_gpio_sysfs_dir != "") {
      for (const auto& prefix : kSysfsPrefixes) {
        auto dir = prefix + pin_def.chip_gpio_sysfs_dir;
        if (fs::exists(dir)) {
          gpio_chip_dir = dir;
          break;
        }
      }

      if (error_message != nullptr && gpio_chip_dir == "") {
        *error_message = "cannot find GPIO chip " + pin_def.chip_gpio_sysfs_dir;
        return false;
      }

      gpio_chip_dirs[pin_def.chip_gpio_sysfs_dir] = gpio_chip_dir;
      auto gpio_chip_gpio_dir = gpio_chip_dir + "/gpio";
      for (auto& p : fs::directory_iterator(gpio_chip_gpio_dir)) {
        auto path = p.path().filename().string();

        const std::string kGpioChipPrefix = "gpiochip";

        if (path.substr(0, kGpioChipPrefix.size()) == kGpioChipPrefix) {
          // finds /sys/devices/**/gpiochip** directory
          // it must contain "base" and "ngpio" file
          std::ifstream base_file(gpio_chip_gpio_dir + "/" + path + "/base",
                                  std::ios::in | std::ios::binary);
          base_file >> gpio_chip_base[pin_def.chip_gpio_sysfs_dir];
          std::ifstream ngpio_file(gpio_chip_gpio_dir + "/" + path + "/ngpio",
                                   std::ios::in | std::ios::binary);
          ngpio_file >> gpio_chip_ngpio[pin_def.chip_gpio_sysfs_dir];
        }
      }
    }
  }

  // 5. Find PWM Chips
  std::map<std::string, std::string> pwm_dirs;

  for (const auto& pin_def : kPinDefs) {
    if (pin_def.chip_pwm_sysfs_dir != kNONE) {
      std::string pwm_chip_dir = "";

      for (const auto& prefix : kSysfsPrefixes) {
        auto dir = prefix + *(pin_def.chip_pwm_sysfs_dir);
        if (fs::exists(dir)) {
          pwm_chip_dir = dir;
          break;
        }
      }

      // check if /sys/device/**/{pwm_chip_sysfs_dir} has a "pwm" directory or
      // not.
      if (pwm_chip_dir == "" || !fs::exists(pwm_chip_dir + "/pwm")) {
        continue;
      }

      // futher check if "pmwchip*" exists or not
      for (auto& p : fs::directory_iterator(pwm_chip_dir + "/pwm")) {
        auto path = p.path().filename().string();
        const std::string kPwmChipPrefix = "pwmchip";
        if (path.substr(0, kPwmChipPrefix.size()) == kPwmChipPrefix) {
          pwm_dirs[*(pin_def.chip_pwm_sysfs_dir)] = pwm_chip_dir + "/" + path;

#ifdef DEBUG
          std::cout << "[info]: pwm detected at "
                    << pwm_dirs[*(pin_def.chip_pwm_sysfs_dir)] << std::endl;
#endif
        }
      }
    }
  }

  auto create_gpio_id_name =
      [&](int chip_relative_id, std::string gpio_chip_name) -> std::pair<int, std::string> {
    std::pair<int, std::string> id_name;
    id_name.first = gpio_chip_base[gpio_chip_name] + chip_relative_id;
    id_name.second = "gpio" + std::to_string(id_name.first);
    return id_name;
  };

  
  // Final Step: gather all channel data
  auto& board_mode_data = data_[BoardMode::BOARD];
  for (const auto& x : kPinDefs) {
    auto pin_number_str = PinNumber2String(x.board_pin_num);
    auto id_name = create_gpio_id_name(x.chip_gpio_pin_num,x.chip_gpio_sysfs_dir);
    board_mode_data[pin_number_str] = ChannelInfo{pin_number_str,
                                       gpio_chip_dirs[x.chip_gpio_sysfs_dir],
                                       x.chip_gpio_pin_num,
                                       id_name.first,
                                       id_name.second,
                                       x.chip_pwm_sysfs_dir,
                                       x.chip_pwm_id};
  }

  auto& bcm_mode_data = data_[BoardMode::BCM];
  for (const auto& x : kPinDefs) {
    auto pin_number_str = PinNumber2String(x.bcm_pin_num);
    auto id_name = create_gpio_id_name(x.chip_gpio_pin_num,x.chip_gpio_sysfs_dir);
    bcm_mode_data[pin_number_str] = {pin_number_str,
                                     gpio_chip_dirs[x.chip_gpio_sysfs_dir],
                                     x.chip_gpio_pin_num,
                                     id_name.first,
                                     id_name.second,
                                     x.chip_pwm_sysfs_dir,
                                     x.chip_pwm_id};
  }

  auto& cvm_mode_data = data_[BoardMode::CVM];
  for (const auto& x : kPinDefs) {
    auto id_name = create_gpio_id_name(x.chip_gpio_pin_num,x.chip_gpio_sysfs_dir);
    cvm_mode_data[x.cvm_pin_name] = {x.cvm_pin_name,
                                     gpio_chip_dirs[x.chip_gpio_sysfs_dir],
                                     x.chip_gpio_pin_num,
                                     id_name.first,
                                     id_name.second,
                                     x.chip_pwm_sysfs_dir,
                                     x.chip_pwm_id};
  }

  auto& tegra_soc_mode_data = data_[BoardMode::TEGRA_SOC];
  for (const auto& x : kPinDefs) {
    auto id_name = create_gpio_id_name(x.chip_gpio_pin_num,x.chip_gpio_sysfs_dir);
    tegra_soc_mode_data[x.tegra_soc_pin_name] = {
        x.tegra_soc_pin_name,
        gpio_chip_dirs[x.chip_gpio_sysfs_dir],
        x.chip_gpio_pin_num,
        id_name.first,
        id_name.second,
        x.chip_pwm_sysfs_dir,
        x.chip_pwm_id};
  }

  return true;
}

}  // namespace jetson
