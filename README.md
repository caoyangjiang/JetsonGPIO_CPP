# JetsonGPIO_CPP
A C++ implementation of the python based JetsonGPIO

The project is mainly a C++ replication of the python NVIDIA/jetson-gpio repository (). I am trying to follow as closely to the python project as possible. This project do not have any external dependency except STL. The code involves a great deal of file/directory manipulation and it is accomplished through the "fileystem" module from the STL. However, with the default gcc (7.5), the filesystem module is still in experimental mode and thus requires linking to "stdc++fs". If you have higher version compiler, you can remove those experimental related stuff. 


## Prerequiste

- Compile on the actual Jetson board with the JetPack 4.4.1 or above. Cross compilation is not tested.

- Configure PWM functionality by running the python script located at /opt/nvidia/jetsion-io/jetsion-io.py. 

- The easiest way to verify PWM signals is hooking it up to an oscilloscope. However, a simple LED + Resistor circuit will do.

- The easiest way to verify binary GPIOs is to create a simple LED + Resistor circuit.

## Attention

The PWM functionality is only tested on few selected channels using Jetson Agx Xavier and Jetson Nano (2G) boards. You are more than welcomed to test on other channels/boards and give me feedbacks.

## Compilation examples

~~~
sh compile_pwm.sh
sh compile_output.sh
~~~

## Binary GPIO Usage
```cpp
#include "gpio.h"

jetson::GPIO gpio;
gpio.Detect();
gpio.SetBoardMode(jetson::BoardMode::BOARD);

auto s1 = gpio.CreateBinary("18", jetson::Direction::OUT);
s1->Write(0);
s1->Write(1);
s1->Write2(jetson::Signal::HIGH);
s1->Write2(jetson::Signal::LOW);

auto s2 = gpio.CreateBinary("19", jetson::Direction::IN);
auto value = s2->Read();
auto signal = s2->Read2()
```

## PWM GPIO Usage
```cpp

#include "gpio.h"

jetson::GPIO gpio;
gpio.Detect();
gpio.SetBoardMode(jetson::BoardMode::BOARD);

auto pwm1 = gpio.CreatePwm("18", 50, 50);
pwm1->Start();
pwm1->ResetFrequency(100);
pwm1->ResetDutyCycle(20);
pwm1->Stop();
```

## Utility Tools Usage
```cpp
#include "gpio.h"
#include "utils.h"

jetson::GPIO gpio;
gpio.Detect();
gpio.SetBoardMode(jetson::BoardMode::BOARD);
jetson::PrintChannelInfo(gpio.GetChannelInfo(jetson::BoardMode::BOARD, "18"));

```

## Comments

I can upgrade this repository for example adding "conan" and/or "cmake" support if this gets attention. Any constructive advices are welcomed.
