
#include "gpio.h"

int main() {
  jetson::Gpio gpio;
  gpio.Detect();
  return 0;
}
