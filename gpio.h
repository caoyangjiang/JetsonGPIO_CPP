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
  /**
   * @brief Detect board type and gather board information
   *
   * @return The result of board detection
   */
  JResult Detect();

  /**
   * @brief Set the board mode. Once the board mode is set, it can't be changed
   * anymore unless GPIO object is destroyed.
   *
   * @param mode The board mode
   * @return The result of setting board mode
   */
  JResult SetMode(BoardMode mode);

  /**
   * @brief Get the channel information. Use tools in  utils.h to simplify
   * printing.
   *
   * @param mode The mode of the board
   * @param channel The channel where the information will be retrieved.
   * @return channel information
   */
  ChannelInfo GetChannelInfo(BoardMode mode, std::string channel) const;

  /**
   * @brief Get the detected board type
   *
   * @return Type of the board
   */
  BoardType GetBoardType() const;

  /**
   * @brief Get the board mode
   *
   * @return Mode of the board
   */
  BoardMode GetBoardMode() const;

  /**
   * @brief Get the human readable board name
   *
   * @return name of the detected board
   */
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
   * @return Creation result.
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
   */
  void DestroyBinary();

  /**
   * @brief On success, create a pwm controller. Note this uses RAII and does
   * not require explicit destroy for memory safety.
   *
   * @param channel The channel where pwm will be created.
   * @param frequency_hz Frequency of the pwm signal.
   * @param duty_cycle Duty cycle of the pmw signal. The valid range is (0,100)
   * @return The result of pwm controller creation.
   */
  PwmResult CreatePwm(std::string channel, float frequency_hz,
                      float duty_cycle);

  /**
   * @brief Destroy a pwm controller explicitly. No effect if given channel do
   * not exist or was not created. This might be useful if a channel was used
   * for PWM and later wanted for gpio.
   *
   * @param channel the name of the channel.
   */
  void DestroyPwm(std::string channel);

  /**
   * @brief Destroy all pwm controller explicitly.
   */
  void DestroyPwm();

 private:
  BoardType type_ = BoardType::UNKNOWN;
  ChannelData data_;
  BoardMode curr_board_mode_ = BoardMode::UNKNONW;

  std::list<std::unique_ptr<BinaryController>> binaries_;
  std::list<std::unique_ptr<PWMController>> pwms_;
};

}  // namespace jetson
