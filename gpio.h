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
#include "binary_gpio.h"
#include "pwm.h"
#include "types.h"

namespace jetson {

class Gpio {
 public:
  using BinaryResult = JOutcome<BinaryController*>;
  using PwmResult = JOutcome<PWMController*>;

 public:
  bool Detect(std::string* error_message = nullptr);
  // JResult Detect();
  JResult SetMode(BoardMode mode);

  ChannelInfo GetChannelInfo(BoardMode mode, std::string channel) const;
  BoardMode GetBoardMode() const;
  BoardType GetBoardType() const;
  std::string GetBoardName() const;

  /**
   * @brief Create a binary GPIO using RAII. The binary GPIO will be destroy
   * automatically.
   *
   * @param channel The channel where the binary GPIO will be created. The name
   * of the channel depends on the board mode.
   * @param direction Set as input or output gpio.
   * @param initial_value Set as high ro low.
   * @param pull Only applicable when direction is input.
   * @return BinaryResult Creation result.
   */
  BinaryResult CreateBinary(std::string channel, Direction direction,
                            Signal initial_value = Signal::LOW,
                            Pull pull = Pull::OFF);

  /**
   * @brief Destroy a binary gpio explicitly. No effect if given channel do not
   * exist or was not created. This might be useful if a channel was used for
   * binary GPIO and later wanted for PWM.
   *
   * @param channel the name of the channel.
   */
  void DestroyBinary(std::string channel);

  /**
   * @brief Destroy all binary gpio explicitly.
   *
   */
  void DestroyBinary();

  PwmResult CreatePwm(std::string channel, float frequency_hz,
                      float duty_cycle);
  void DestroyPwm(std::string channel);
  void DestroyPwm();

 private:
  BoardType type_ = BoardType::UNKNOWN;
  ChannelData data_;

  // BoardType detected_board_type_ = BoardType::UNKNOWN;
  // ChannelData detected_channel_data_;

  BoardMode curr_board_mode_ = BoardMode::UNKNONW;
  ChannelConfigurations config_;

  std::list<std::unique_ptr<BinaryController>> binaries_;
  std::list<std::unique_ptr<PWMController>> pwms_;
};

}  // namespace jetson
