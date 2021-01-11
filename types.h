/**
 * @file types.h
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

#pragma once

#include <fstream>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace jetson {

template <typename T>
using JOutcome = std::pair<std::string, T>;
using JResult = JOutcome<bool>;
static const JResult JOK = JResult{"No Error", true};

static const int kPull_Offset = 20;
static const int kEdge_Offset = 30;
static const std::string kSysfs_Root = "/sys/class/gpio";

struct PinDef {
  int chip_gpio_pin_num;
  std::string chip_gpio_sysfs_dir;
  int board_pin_num;
  int bcm_pin_num;
  std::string cvm_pin_name;
  std::string tegra_soc_pin_name;
  std::optional<std::string> chip_pwm_sysfs_dir;
  std::optional<int> chip_pwm_id;
};

struct BoardInformation {
  int p1_revision;
  int ram_size;
  int revision;
  std::string type;
  std::string manufacturer;
  std::string processor;
  int carrier_board;
};

enum class BoardType {
  UNKNOWN = 0,
  JETSON_XAVIER,
  JETSON_AGX_XAVIER,
  JETSON_NANO,
  JETSON_TX1,
  JETSON_TX2,
  JETSON_NX_XAVIER,
  CLARA_AGX_XAVIER,
};

enum class BoardMode {
  UNKNONW = 0,
  BOARD = 10,
  BCM = 11,
  CVM = 1000,
  TEGRA_SOC = 1001,
};

enum class Direction { OUT = 0, IN = 1, UNKNOWN };

enum class Signal { LOW = 0, HIGH = 1, UNKNOWN };

enum class Pull {
  OFF = 0 + kPull_Offset,
  DOWN = 1 + kPull_Offset,
  UP = 2 + kPull_Offset,
};

enum class TriggerEdge {
  NONE = 0,
  RISING = 1 + kEdge_Offset,
  FALLING = 2 + kEdge_Offset,
  BOTH = 3 + kEdge_Offset,
};

static constexpr const char* TriggerEdge2String(TriggerEdge edge) {
  switch (edge) {
    case TriggerEdge::NONE:
      return "none";
    case TriggerEdge::RISING:
      return "rising";
    case TriggerEdge::FALLING:
      return "failing";
    case TriggerEdge::BOTH:
      return "both";
  }
}

struct ChannelInfo {
  std::string channel;
  std::string gpio_chip_dir;
  int chip_gpio;
  int gpio;
  std::string gpio_name;
  std::optional<std::string> pwm_chip_dir;
  std::optional<int> chip_pwm_id;
};

using ChannelData = std::map<BoardMode, std::map<std::string, ChannelInfo>>;

struct ChannelConfiguration {
  ChannelInfo channel_info;
  Direction direction;
  std::ofstream file_d;
  std::fstream file_v;
};

using ChannelConfigurations = std::map<std::string, ChannelConfiguration>;

constexpr inline const char* BoardType2String(BoardType type) {
  switch (type) {
    case BoardType::UNKNOWN:
      return "Unknow board type";
    case BoardType::JETSON_XAVIER:
      return "Jetson Xavier";
    case BoardType::JETSON_AGX_XAVIER:
      return "Jetson Agx Xavier";
    case BoardType::JETSON_NANO:
      return "Jetson Nano";
  }
}

inline std::string PinNumber2String(int pin_num) {
  return std::to_string(pin_num);
}

}  // namespace jetson
