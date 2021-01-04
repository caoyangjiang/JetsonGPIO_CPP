/**
 * @file gpio.h
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

#include <list>
#include <memory>
#include <string>
#include "pwm.h"
#include "types.h"

namespace jetson {

class Gpio {
 public:
  bool Detect(std::string* error_message = nullptr);

  JResult SetMode(BoardMode mode);
  // JResult Setup(std::string channel, Direction direction,
  //               Signal initial_value = Signal::LOW);
  JResult Setup(std::string channel, Direction direction,
                Signal initial_value = Signal::LOW, Pull pull = Pull::OFF);

  void TearDown(std::string channel);
  void TearDown();

  ChannelInfo GetChannelInfo(BoardMode mode, std::string channel) const;
  BoardMode GetBoardMode() const;
  BoardType GetBoardType() const;
  std::string GetBoardName() const;

  PWMController* CreatePWMController(std::string channel, float frequency_hz,
                                     float duty_cycle);

 private:
  JResult ExportPwm(std::string channel);
  void UnexportPwm(std::string channel);
  JResult ExportGpio(std::string channel, Direction direction);
  void UnexportGpio(std::string channel);

 private:
  BoardType type_ = BoardType::UNKNOWN;
  ChannelData data_;
  BoardMode curr_board_mode_ = BoardMode::UNKNONW;
  ChannelConfigurations config_;
  std::list<std::unique_ptr<PWMController>> pwms_;
};

}  // namespace jetson
