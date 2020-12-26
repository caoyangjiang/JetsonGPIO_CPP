/**
 * @file gpio_pin_data.h
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
#include <string>
#include <vector>

#include "types.h"

namespace jetson {

static const auto kNONE = std::nullopt;

// ------ Definitions for board "Jetson Xavier" ------ //
static const std::vector<PinDef> kJETSON_XAVIER_PIN_DEFS = {
    {106, "2200000.gpio", 7, 4, "MCLK05", "SOC_GPIO42", kNONE, kNONE},
    {140, "2200000.gpio", 11, 17, "UART1_RTS", "UART1_RTS", kNONE, kNONE},
    {63, "2200000.gpio", 12, 18, "I2S2_CLK", "DAP2_SCLK", kNONE, kNONE},
    {136, "2200000.gpio", 13, 27, "PWM01", "SOC_GPIO44", "32f0000.pwm", 0},
    {105, "2200000.gpio", 15, 22, "GPIO27", "SOC_GPIO54", "3280000.pwm", 0},
    {8, "2200000.gpio", 16, 23, "GPIO8", "CAN1_STB", kNONE, kNONE},
    {56, "2200000.gpio", 18, 24, "GPIO35", "SOC_GPIO12", "32c0000.pwm", 0},
    {205, "2200000.gpio", 19, 10, "SPI1_MOSI", "SPI1_MOSI", kNONE, kNONE},
    {204, "2200000.gpio", 21, 9, "SPI1_MISO", "SPI1_MISO", kNONE, kNONE},
    {129, "2200000.gpio", 22, 25, "GPIO17", "SOC_GPIO21", kNONE, kNONE},
    {203, "2200000.gpio", 23, 11, "SPI1_CLK", "SPI1_CLK", kNONE, kNONE},
    {206, "2200000.gpio", 24, 8, "SPI_CS0_N", "SPI_CS0_N", kNONE, kNONE},
    {207, "2200000.gpio", 26, 7, "SPI_CS1_N", "SPI_CS1_N", kNONE, kNONE},
    {3, "2200000.gpio", 29, 5, "CAN0_DIN", "CAN0_DIN", kNONE, kNONE},
    {2, "2200000.gpio", 31, 6, "CAN0_DOUT", "CAN0_DOUT", kNONE, kNONE},
    {9, "2200000.gpio", 32, 12, "GPIO9", "CAN1_EN", kNONE, kNONE},
    {0, "2200000.gpio", 33, 13, "CAN1_DOUT", "CAN1_DOUT", kNONE, kNONE},
    {66, "2200000.gpio", 35, 19, "I2S2_FS", "DAP2_FS", kNONE, kNONE},
    {141, "2200000.gpio", 36, 16, "UART1_CTS", "UART1_CTS", kNONE, kNONE},
    {1, "2200000.gpio", 37, 26, "CAN1_DIN", "CAN1_DIN", kNONE, kNONE},
    {65, "2200000.gpio", 38, 20, "I2S2_DIN", "DAP2_DIN", kNONE, kNONE},
    {64, "2200000.gpio", 40, 21, "I2S2_DOUT", "DAP2_DOUT", kNONE, kNONE}};

static const std::vector<std::string> kJETSON_XAVIER_COMPATIBLES = {
    "nvida,p2972-0000", "nvidia,p2972-006", "nvidia,jetson-xavier"};

static const BoardInformation kJETSON_XAVIER_BOARD_INFORMATION{
    1, 16384, -1, "Jetson Xavier", "NVIDIA", "ARM Carmel", 2822};

// ------ Definitions for board "Jetson Agx Xavier" ------ //
static const auto kJETSON_AGX_XAVIER_PIN_DEFS = kJETSON_XAVIER_PIN_DEFS;

static const std::vector<std::string> kJETSON_AGX_XAVIER_COMPATIBLES = {
    "nvida,p2972-0000", "nvidia,p2972-006", "nvidia,jetson-xavier"};

static const BoardInformation kJETSON_AGX_XAVIER_BOARD_INFORMATION{
    1, 32768, -1, "Jetson Agx Xavier", "NVIDIA", "ARM Carmel", 2822};

// For easy lookup
static const std::map<BoardType, std::vector<PinDef>> kBoardPins = {
    {BoardType::JETSON_XAVIER, kJETSON_XAVIER_PIN_DEFS},
    {BoardType::JETSON_AGX_XAVIER, kJETSON_AGX_XAVIER_PIN_DEFS}};

static const std::map<BoardType, std::vector<std::string>> kBoardCompats = {
    {BoardType::JETSON_XAVIER, kJETSON_XAVIER_COMPATIBLES},
    {BoardType::JETSON_AGX_XAVIER, kJETSON_AGX_XAVIER_COMPATIBLES}};

static const std::map<BoardType, BoardInformation> kBoardInfos = {
    {BoardType::JETSON_XAVIER, kJETSON_XAVIER_BOARD_INFORMATION},
    {BoardType::JETSON_AGX_XAVIER, kJETSON_AGX_XAVIER_BOARD_INFORMATION}};

}  // namespace jetson
