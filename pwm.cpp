/**
 * @file pwm.cpp
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

#include "pwm.h"
#include <chrono>
#include <experimental/filesystem>
#include <iostream>
#include <string>
#include <thread>
#include "types.h"

namespace fs = std::experimental::filesystem;

namespace jetson {

PWMController::PWMController(ChannelInfo info, float frequency,
                             float duty_cycle)
    : info_(info) {
  Export();

  // On boot, both period and duty cycle are both 0. In this state, the period
  // must be set first; any configuration change made while period==0 is
  // rejected. This is fine if we actually want a duty cycle of 0. Later, once
  // any period has been set, we will always be able to set a duty cycle of 0.
  // The code could be written to always read the current value, and only
  // write the value if the desired value is different.
  ResetFrequency(frequency);
  ResetDutyCycle(duty_cycle);
}

PWMController::~PWMController() { Unexport(); }

void PWMController::Start() {
  f_enable_.seekp(0, std::ios::beg);
  f_enable_ << "1";
  f_enable_.flush();  // "must flush otherwise buffered."
}

void PWMController::Stop() {
  f_enable_.seekp(0, std::ios::beg);
  f_enable_ << "0";
  f_enable_.flush();  // "must flush otherwise buffered."
}

void PWMController::ResetFrequency(double frequency) {
  if (frequency > 0 && frequency <= 1e9) {
    int64_t period = 1e9 / frequency;  // period in nano second
    f_period_.seekp(0, std::ios::beg);
    f_period_ << std::to_string(period);
    f_period_.flush();  // "must flush otherwise buffered."

    frequency_ = frequency;

    // the duty cycle was set according to frequency
    ResetDutyCycle(duty_cycle_);
  }
}

void PWMController::ResetDutyCycle(double duty_cycle) {
  if (duty_cycle >= 0 && duty_cycle <= 100) {
    int64_t high = (1e7 / frequency_) * duty_cycle;  // "high" in nano second
    std::cout << (1e7 / frequency_) << " " << high << std::endl;
    f_duty_cycle_.seekp(0, std::ios::beg);
    f_duty_cycle_ << std::to_string(high);
    f_duty_cycle_.flush();  // "must flush otherwise buffered."

    duty_cycle_ = duty_cycle;
  }
}

std::string PWMController::GetChannel() const { info_.channel; }

double PWMController::GetFrequency() const { return frequency_; }

double PWMController::GetDutyCycle() const { return duty_cycle_; }

void PWMController::Export() {
  const std::string kExport_File = *(info_.pwm_chip_dir) + "/export";
  const std::string kPwm_Root_Dir =
      *(info_.pwm_chip_dir) + "/pwm" + std::to_string(*(info_.chip_pwm_id));
  const std::string kPwm_Duty_Cycle_File = kPwm_Root_Dir + "/duty_cycle";
  const std::string kPwm_Period_File = kPwm_Root_Dir + "/period";
  const std::string kPwm_Enable_File = kPwm_Root_Dir + "/enable";

  // write pwm chip id into export to create pwm root dir
  if (!fs::exists(kPwm_Root_Dir)) {
    std::ofstream file(kExport_File);
    file << *(info_.chip_pwm_id);
    file.flush();
    file.close();

    // It takes time for pwmxxx to show up
    int safe_counter = 100;  // try up to one second
    bool pwm_created = false;
    do {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      std::fstream check_enable(kPwm_Enable_File);
      std::fstream check_period(kPwm_Period_File);
      std::fstream check_duty_cycle(kPwm_Duty_Cycle_File);
      if (check_enable && check_period && check_duty_cycle) {
        pwm_created = true;
        break;
      } else {
        safe_counter--;
      }
    } while (safe_counter > 0);

    if (!pwm_created)
      throw std::runtime_error(
          "creating pwm failed after waiting for a second.");
  }

  // open duty cycle file
  f_duty_cycle_ =
      std::ofstream(kPwm_Duty_Cycle_File, std::ios::binary | std::ios::out);

  // open period file
  f_period_ = std::ofstream(kPwm_Period_File, std::ios::binary | std::ios::out);

  // open enable file
  f_enable_ = std::ofstream(kPwm_Enable_File, std::ios::binary | std::ios::out);
}

void PWMController::Unexport() {
  f_enable_.close();
  f_period_.close();
  f_duty_cycle_.close();

  const std::string kUnexport_File = *(info_.pwm_chip_dir) + "/unexport";
  std::ofstream unexport_fs(kUnexport_File, std::ios::out | std::ios::binary);
  unexport_fs << *(info_.chip_pwm_id);
  unexport_fs.flush();
  unexport_fs.close();
}
}  // namespace jetson
