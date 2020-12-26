

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

enum class ChannelType {
  UNKNONW = 0,
  BOARD,
  BCM,
  CVM,
  TEGRA_SOC,
};

struct ChannelContent {
  std::map<std::string, std::string> gpio_chip_dir;
};

using ChannelData = std::map<ChannelType, std::vector<ChannelContent>>;

constexpr const char* BoardType2String(BoardType type) {
  switch (type) {
    case BoardType::UNKNOWN:
      return "Unknow board type";
    case BoardType::JETSON_XAVIER:
      return "Jetson Xavier";
    case BoardType::JETSON_AGX_XAVIER:
      return "Jetson Agx Xavier";
  }
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
