# JetsonGPIO_CPP
A C++ implementation of the python based JetsonGPIO

The project is mainly a C++ replication of the python NVIDIA/jetson-gpio repository (). I am trying to follow as closely to the python project as possible. This project do not have any external dependency except STL. The code involves a great deal of file/directory manipulation and it is accomplished through the "fileystem" module in STL. However, with the default gcc (7.5), the filesystem module is still in experimental mode and also requies linking to "stdc++fs". If you have higher version compiler, you should remove those experimental related stuff. 


## Prerequiste

*. Compile on the actual Jetson board with the JetPack 4.4.1 or above. Cross compilation is not tested.

*. The best way to verify PWM feature is hooking it up to an oscilloscope. However, a simple LED + Resistor will do.


## Compilation

~~~
sh compile.sh
~~~

## Comments

The PWM functionality is only tested on Jetson Agx Xavier and Jetson Nano (2G) edition. You are more than welcomed to test on other boards and submit pull requests.

I can add "conan" or "cmake" upon request.