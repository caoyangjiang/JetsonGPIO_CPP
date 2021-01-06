/**
 * @file binary_gpio.cpp
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

#include <fstream>
#include <string>
#include "types.h"

namespace jetson {
class BinaryController {
 public:
  BinaryController(ChannelInfo info, Direction direction,
                   Signal initial_value = Signal::LOW, Pull pull = Pull::OFF);
  ~BinaryController();

  /**
   * @brief Any value >= 1 is considered as high. Value euqal to 0 is considered
   * as low. All other values are ignored.
   *
   * @param signal 0 or 1.
   */
  void Write(int signal);

  /**
   * @brief Output a signal.
   *
   * @param signal high or low
   */
  void Write2(Signal signal);

  /**
   * @brief Read a input signal.
   *
   * @return 0 or 1.
   */
  int Read();

  /**
   * @brief Read from the input signal.
   *
   * @return high or low.
   */
  Signal Read2();

  /**
   * @brief Get the direction
   *
   * @return direction
   */
  Direction GetDirection() const;

  /**
   * @brief Get the channel name
   *
   * @return channel name
   */
  std::string GetChannel() const;

 private:
  BinaryController(const BinaryController&) = delete;
  BinaryController(BinaryController&&) = delete;

 private:
  void Export();
  void Unexport();
  void SetDirection();

 private:
  const ChannelInfo info_;
  const Direction direction_;
  std::ofstream f_direction_;
  std::fstream f_value_;
};
}  // namespace jetson