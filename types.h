/**
 * @file types.h
 * @author Caoyang Jiang (caoyangjiang@gmail.com)
 * @brief
 * @version 0.1
 * @date 2020-12-26
 *
 * @copyright Copyright (c) 2020, Caoyang Jiang
 *
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

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace jetson {

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
};

enum class BoardMode {
  UNKNONW = 0,
  BOARD,
  BCM,
  CVM,
  TEGRA_SOC,
};

enum class Direction { OUT = 0, IN = 1 };

enum class Signal { LOW = 0, HIGH = 1 };

struct ChannelInfo {
  std::string channel;
  std::string gpio_chip_dir;
  int chip_gpio;
  std::string gpio;
  std::string gpio_name;
  std::optional<std::string> pwm_chip_dir;
  std::optional<int> chip_pwm_id;
};

using ChannelData = std::map<BoardMode, std::map<std::string, ChannelInfo>>;

constexpr inline const char* BoardType2String(BoardType type) {
  switch (type) {
    case BoardType::UNKNOWN:
      return "Unknow board type";
    case BoardType::JETSON_XAVIER:
      return "Jetson Xavier";
    case BoardType::JETSON_AGX_XAVIER:
      return "Jetson Agx Xavier";
  }
}

inline std::string PinNumber2String(int pin_num) {
  return std::to_string(pin_num);
}

class BoardPinDef {
 public:
  explicit BoardPinDef(BoardType type);

  int GetChipGpioPinNumber() const;
  std::string GetChipGpioSysfsDir() const;
  int GetBoardPinNumber() const;
  int GetBcmPinNumber() const;
  const std::string& GetCvmPinName() const;
  const std::string& GetTegraSocPinName() const;

  bool HaveChipPwmSysfsDir() const;
  bool HaveChipPwmId() const;

  const std::string& GetChipPwmSysfsDir() const;
  const std::string& GetChipPwmId() const;

 private:
  BoardType board_type_;
};

}  // namespace jetson
