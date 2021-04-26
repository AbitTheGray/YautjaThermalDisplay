# Yautja Thermal Display

My attempt at Thermal Camera + Display setup for future cosplay.

I am closing this project for a near future as my `MLX90640` is faulty (I2C problems) and buying new one costs far more than few years ago, if you manage to find the 50 degree one at all.

## Components

- SSD1351 1.5" 128x128 display
- MLX90640 32x24 thermal camera
- Teensy 3.2

Thsoe are the components I used but it should work with other, similar setup.

## Setup

- Install [Arduino](https://www.arduino.cc/en/software) (1.8.12 in my case)
- Install [Teensyduino](https://www.pjrc.com/teensy/teensyduino.html) into Arduino IDE
- Open `YautjaThermalDisplay.ino`
- Change configuration inside `DataStructs` (2nd driver, `Camera_02_Melexis` does not work)
