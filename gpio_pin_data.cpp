

#include "gpio_pin_data.h"
#include <algorithm>
#include <experimental/filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

// debug
#include <iostream>

namespace fs = std::experimental::filesystem;

namespace jetson {

bool GpioUtility::DetectBoardType(BoardType* board_type,
                                  std::string* error_message) {
  *board_type = jetson::BoardType::UNKNOWN;

  const std::string kCompatsPath = "/proc/device-tree/compatible";
  const std::string kIdsPath = "/proc/device-tree/chosen/plugin-manager/ids";

  std::ifstream compats_file(kCompatsPath, std::ios::in);
  if (!compats_file.is_open()) {
    *error_message = "failed to open compatible file at " + kCompatsPath;
    return false;
  }

  std::vector<std::string> detected_compatibles;

  // 1. Retrieve all compatibles
  std::stringstream ss;
  ss << compats_file.rdbuf();
  std::string compatible;
  while (std::getline(ss, compatible, static_cast<char>(0x00))) {
    detected_compatibles.push_back(compatible);
  }

  // 2. Find a matching compatible
  for (const auto& bc : jetson::kBoardCompats) {
    for (const auto& c : bc.second) {
      auto result = std::find(detected_compatibles.begin(),
                              detected_compatibles.end(), c);

      if (result != detected_compatibles.end()) {
        *board_type = bc.first;
        goto COMPATIBLE_DETECT_DONE;
      }
    }
  }

COMPATIBLE_DETECT_DONE:
  if (*board_type == jetson::BoardType::UNKNOWN) {
    *error_message = "failed to detect board type from compatibles.";
    return false;
  }

  // board type determined.
  const auto& kPinDefs = kBoardPins.at(*board_type);

  // 3. Find a matching carrier board
  if (!fs::exists(kIdsPath)) {
    *error_message = "ids path " + kIdsPath + " do not exist.";
    return false;
  }

  auto carried_board_id =
      std::to_string(jetson::kBoardInfos.at(*board_type).carrier_board);
  std::optional<std::string> matching_id_file;
  for (auto& p : fs::directory_iterator(kIdsPath)) {
    auto path = p.path().filename().string();
    if (path.substr(0, carried_board_id.size()) == carried_board_id) {
      matching_id_file = path;
      break;
    }
  }

  if (matching_id_file == std::nullopt) {
    std::cout << "WARNING: Carrier board is not from a Jetson Developer "
                 "Kit.\nWARNNIG: Jetson.GPIO library has not been verified with"
                 "this carrier board.\nWARNING: and in fact is unlikely to "
                 "work correctly.\n"
              << std::endl;
  }

#ifdef DEBUG
  std::cout << "[info]: Id file " << *matching_id_file << '\n';
#endif

  // 4. Get GPIO chip offsets
  const std::vector<std::string> kSysfsPrefixes = {"/sys/devices/",
                                                   "/sys/devices/platform/"};

  std::map<std::string, std::string> gpio_chip_dirs;
  std::map<std::string, int> gpio_chip_base;
  std::map<std::string, int> gpio_chip_ngpio;

  for (const auto& pin_def : kPinDefs) {
    std::string gpio_chip_dir = "";

    if (pin_def.chip_gpio_sysfs_dir != "") {
      for (const auto& prefix : kSysfsPrefixes) {
        auto dir = prefix + pin_def.chip_gpio_sysfs_dir;
        if (fs::exists(dir)) {
          gpio_chip_dir = dir;
          break;
        }
      }

      if (gpio_chip_dir == "") {
        *error_message = "Cannot find GPIO chip " + pin_def.chip_gpio_sysfs_dir;
        return false;
      }

      gpio_chip_dirs[pin_def.chip_gpio_sysfs_dir] = gpio_chip_dir;
      auto gpio_chip_gpio_dir = gpio_chip_dir + "/gpio";
      for (auto& p : fs::directory_iterator(gpio_chip_gpio_dir)) {
        auto path = p.path().filename().string();

        const std::string kGpioChipPrefix = "gpiochip";

        if (path.substr(0, kGpioChipPrefix.size()) == kGpioChipPrefix) {
          // finds /sys/devices/**/gpiochip** directory
          // it must contain "base" and "ngpio" file
          std::ifstream base_file(gpio_chip_gpio_dir + "/" + path + "/base",
                                  std::ios::in | std::ios::binary);
          base_file >> gpio_chip_base[pin_def.chip_gpio_sysfs_dir];
          std::ifstream ngpio_file(gpio_chip_gpio_dir + "/" + path + "/ngpio",
                                   std::ios::in | std::ios::binary);
          ngpio_file >> gpio_chip_ngpio[pin_def.chip_gpio_sysfs_dir];
        }
      }
    }
  }

  // 5. Find PWM Chips
  std::map<std::string, std::string> pwm_dirs;

  for (const auto& pin_def : kPinDefs) {
    if (pin_def.chip_pwm_sysfs_dir != kNONE) {
      std::string pwm_chip_dir = "";

      for (const auto& prefix : kSysfsPrefixes) {
        auto dir = prefix + *(pin_def.chip_pwm_sysfs_dir);
        if (fs::exists(dir)) {
          pwm_chip_dir = dir;
          break;
        }
      }

      // check if /sys/device/**/{pwm_chip_sysfs_dir} has a "pwm" directory or
      // not.
      if (pwm_chip_dir == "" || !fs::exists(pwm_chip_dir + "/pwm")) {
        continue;
      }

      // futher check if "pmwchip*" exists or not
      for (auto& p : fs::directory_iterator(pwm_chip_dir + "/pwm")) {
        auto path = p.path().filename().string();
        const std::string kPwmChipPrefix = "pwmchip";
        if (path.substr(0, kPwmChipPrefix.size()) == kPwmChipPrefix) {
          pwm_dirs[*(pin_def.chip_pwm_sysfs_dir)] = pwm_chip_dir + "/" + path;

#ifdef DEBUG
          std::cout << "[info]: pwm detected at "
                    << pwm_dirs[*(pin_def.chip_pwm_sysfs_dir)] << std::endl;
#endif
        }
      }
    }
  }

  // Final Step: gather all data

  return true;
}

}  // namespace jetson
