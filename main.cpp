
#include "gpio_pin_data.h"

int main() {
  std::string error_message;
  jetson::BoardType type;
  auto result = jetson::GpioUtility::DetectBoardType(&type, &error_message);
  return 0;
}