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

#include "binary_gpio.h"
#include <sys/inotify.h>
#include <sys/types.h>
#include <unistd.h>
#include <chrono>
#include <experimental/filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <utility>
#include "limits.h"
#include "types.h"

namespace fs = std::experimental::filesystem;

namespace jetson {

BinaryController::BinaryController(ChannelInfo info, Direction direction,
                                   Signal signal, Pull pull)
    : info_(info), direction_(direction) {
  Export();
  SetDirection();
  monitor_ = std::async(std::bind(&BinaryController::EdgeMonitor, this));
  if (direction_ == Direction::OUT) Write2(signal);
}

BinaryController::~BinaryController() {
  if (direction_ == Direction::OUT) Write2(Signal::LOW);
  Unexport();
  monitor_.get();
}

void BinaryController::Write(int s) {
  if (s >= 0) Write2(s == 0 ? Signal::LOW : Signal::HIGH);
}

void BinaryController::Write2(Signal s) {
  if (direction_ == Direction::OUT) {
    f_value_.seekp(0, std::ios::beg);
    if (s == Signal::HIGH)
      f_value_.write("1", 1);
    else
      f_value_.write("0", 1);
    f_value_.flush();
  }
}

int BinaryController::Read() { return Read2() == Signal::LOW ? 0 : 1; }

Signal BinaryController::Read2() {
  if (direction_ == Direction::IN) {
    char value = 0;
    f_value_.seekg(0, std::ios::beg);
    f_value_.read(&value, 1);
    return (value - '0') == 0 ? Signal::LOW : Signal::HIGH;
  }

  return Signal::UNKNOWN;
}

void BinaryController::Export() {
  const std::string kEXPORT_FILE = kSysfs_Root + "/export";
  const std::string kGPIO_DIR = kSysfs_Root + "/" + info_.gpio_name;
  const std::string kGPIO_DIRECTION_FILE =
      kSysfs_Root + "/" + info_.gpio_name + "/direction";
  const std::string kGPIO_VALUE_FILE =
      kSysfs_Root + "/" + info_.gpio_name + "/value";
  const std::string kGPIO_EDGE_FILE =
      kSysfs_Root + "/" + info_.gpio_name + "/edge";

  // Export channel by writing into export file
  if (!fs::exists(kGPIO_DIR)) {
    std::ofstream file(kEXPORT_FILE);
    if (!file) throw std::runtime_error("open \"Export\" file failed. ");
    std::string gpio_num_str = std::to_string(info_.gpio);
    file.write(gpio_num_str.c_str(), gpio_num_str.size());
    file.close();

    // It takes time for gpioxxx to show up
    int safe_counter = 100;  // try up to one second
    bool gpio_created = false;
    do {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      std::fstream check_fs(kGPIO_DIRECTION_FILE);
      if (check_fs) {
        gpio_created = true;
        break;
      } else {
        safe_counter--;
      }
    } while (safe_counter > 0);

    if (!gpio_created) {
      Unexport();
      throw std::runtime_error(
          "Gpio exported but gpio directory was not created.");
    }

    f_direction_.open(kGPIO_DIRECTION_FILE);
    f_value_.open(kGPIO_VALUE_FILE);

    // always trigger on both edge
    std::ofstream f_edge(kGPIO_EDGE_FILE, std::ios::out | std::ios::binary);
    std::string edge_name("both");
    f_edge.write(edge_name.c_str(), edge_name.size());
    f_edge.close();
  }
}

void BinaryController::Unexport() {
  f_value_.close();
  f_direction_.close();

  std::ofstream file(kSysfs_Root + "/unexport");
  std::string gpio_num_str = std::to_string(info_.gpio);
  file.write(gpio_num_str.c_str(), gpio_num_str.size());
  file.close();
}

void BinaryController::SetDirection() {
  f_direction_.seekp(0, std::ios::beg);
  if (direction_ == Direction::OUT)
    f_direction_.write("out", 3);
  else
    f_direction_.write("in", 2);
  f_direction_.flush();
}

Direction BinaryController::GetDirection() const { return direction_; }

std::string BinaryController::GetChannel() const { return info_.channel; }

void BinaryController::RegisterCallback(TriggerEdge edge,
                                        TriggerCallBack callback) {
  callbacks_[edge].emplace_back(std::move(callback));
}

void BinaryController::EdgeMonitor() {
  int inotify = inotify_init();

  const std::string kGPIO_VALUE_FILE =
      kSysfs_Root + "/" + info_.gpio_name + "/value";
  int watch = inotify_add_watch(inotify, kGPIO_VALUE_FILE.c_str(),
                                IN_MODIFY | IN_CLOSE_WRITE);

  bool delete_detected = false;
  while (!delete_detected) {
    constexpr int kEvent_Size = sizeof(struct inotify_event);
    constexpr int kEvent_Buffer_Len = (kEvent_Size + NAME_MAX + 1) * 8;
    char buffer[kEvent_Buffer_Len];
    auto length = read(inotify, buffer, kEvent_Buffer_Len);

    int i = 0;
    while (i < length) {
      struct inotify_event *event =
          reinterpret_cast<struct inotify_event *>(&buffer + i);

      if (event->mask & IN_CLOSE_WRITE) {
        delete_detected = true;
        break;
      }

      if (event->mask & IN_MODIFY) {
        auto value = Read();
        for (auto &cb : callbacks_[TriggerEdge::BOTH]) {
          std::invoke(cb, value);
        }

        if (value == 1) {
          for (auto &cb : callbacks_[TriggerEdge::RISING]) {
            std::invoke(cb, value);
          }
        } else {
          for (auto &cb : callbacks_[TriggerEdge::FALLING]) {
            std::invoke(cb, value);
          }
        }
      }

      i += kEvent_Size + event->len;
    }
  }

  inotify_rm_watch(inotify, watch);
  close(inotify);
}

}  // namespace jetson
