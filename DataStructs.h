#pragma once

//----------------------------------------------------------------
// Pragma configuration

// Selection from pre-defined layouts
//  0 = Hardware, Teensy 3.2
//  1 = Alternate Hardware, Teensy 3.2
// 99 = custom (rear edge of Teensy 3.2)
#define SCREEN_PINOUT_VARIANT 99

// Defines which driver for MLX90640 to use
// May support different sensors later as long as their resolution is 32x24
//  0 = Virtual (random data)
//  1 = MLX90640 ( https://github.com/adafruit/Adafruit_MLX90640 )
//  2 = MLX90640 ( https://github.com/melexis/mlx90640-library )
#define CAMERA_TYPE 1

// Debug level to Serial output
// Uses `Serial`
//  0 = Disabled, no debug = no Serial
//  1 = Enabled, only main info
//  2 = Enabled, output sensor data
#define SERIAL_DEBUG 1

//TODO
// Rotation of camera
//  0 = Standard Landscape
//  1 = Standard Portrait
//  2 = Upside-down Landscape
//  3 = Upside-down Portrait
#define CAMERA_ROTATION 0

//----------------------------------------------------------------
// Configuration

// Display configuration
bool Config_ManualTemp = false;

//----------------------------------------------------------------
// Structs

// Input temperature (do not change)
const int Temps_Width  = 32;
const int Temps_Height = 24;
const int Temps_Size   = Temps_Width * Temps_Height;

// Displayed temperature (do not alter)
const int Temps_Width4  = Temps_Width * 4;
const int Temps_Height4 = Temps_Height * 4;
const int Temps_Size4   = Temps_Width4 * Temps_Height4;

// Min/Max temperature in fixed-temp mode
const int TempConfig_ManualMin = 15;
const int TempConfig_ManualMax = 30;

// Current temperature values
struct Temp
{
  float Values[Temps_Size];
  float Min = TempConfig_ManualMin;
  float Max = TempConfig_ManualMax;
};
