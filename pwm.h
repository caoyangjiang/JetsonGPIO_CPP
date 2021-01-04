/**
 * @file pwm.h
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
class PWMController {
 public:
  PWMController(ChannelInfo info, float frequency, float duty_cycle);
  ~PWMController();

  /**
   * @brief output PWM signal on the designated channel.
   *
   */
  void Start();

  /**
   * @brief stop outputing PWM signal on the designated channel.
   *
   */
  void Stop();

  /**
   * @brief Reset the PWM frequency. Frequency must be greater than 0.
   *
   * @param frequency Frequency in unit of Hz.
   */
  void ResetFrequency(double frequency);

  /**
   * @brief Reset the PWM duty cycle.
   *
   * @param duty_cycle A value in the range of (0,100).
   */
  void ResetDutyCycle(double duty_cycle);

  /**
   * @brief Get the channel of the PWM.
   *
   * @return channel id or name.
   */
  std::string GetChannel() const;

  /**
   * @brief Get the frequency.
   *
   * @return Frequency in unit of Hz.
   */
  double GetFrequency() const;

  /**
   * @brief Get the duty cycle.
   *
   * @return A value in the range of (0,100).
   */
  double GetDutyCycle() const;

 private:
  PWMController(const PWMController&) = delete;
  PWMController(PWMController&&) = delete;

 private:
  void Export();
  void Unexport();

 private:
  ChannelInfo info_;

  std::ofstream f_duty_cycle_;
  std::ofstream f_period_;
  std::ofstream f_enable_;

  double frequency_ = 50;   // 50 hz
  double duty_cycle_ = 50;  // 50%
};
}  // namespace jetson
