#pragma once
#include "DataStructs.h"
// https://github.com/adafruit/Adafruit_MLX90640

#include <Wire.h>
#include <Adafruit_MLX90640.h>

//----------------------------------------------------------------
// Configurations

Adafruit_MLX90640 mlx;

//----------------------------------------------------------------
// Main functions 

void setupMlx90640()
{  
#if SERIAL_DEBUG > 0
  Serial.println("Adafruit MLX90640 Simple Test");
#endif
  while(!mlx.begin(0x33, &Wire))
  {    
#if SERIAL_DEBUG > 0
    Serial.println("MLX90640 not found!");
#endif
    delay(100);
  }
#if SERIAL_DEBUG > 0
  Serial.println("Found Adafruit MLX90640");

  Serial.print("Serial number: ");
  Serial.print(mlx.serialNumber[0], HEX);
  Serial.print(mlx.serialNumber[1], HEX);
  Serial.println(mlx.serialNumber[2], HEX);
#endif
  
  //mlx.setMode(MLX90640_INTERLEAVED);
  mlx.setMode(MLX90640_CHESS);
#if SERIAL_DEBUG > 0
  Serial.print("Current mode: ");
  if (mlx.getMode() == MLX90640_CHESS) {
    Serial.println("Chess");
  } else {
    Serial.println("Interleave");    
  }
#endif

  mlx.setResolution(MLX90640_ADC_18BIT);
#if SERIAL_DEBUG > 0
  Serial.print("Current resolution: ");
  mlx90640_resolution_t res = mlx.getResolution();
  switch (res) {
    case MLX90640_ADC_16BIT: Serial.println("16 bit"); break;
    case MLX90640_ADC_17BIT: Serial.println("17 bit"); break;
    case MLX90640_ADC_18BIT: Serial.println("18 bit"); break;
    case MLX90640_ADC_19BIT: Serial.println("19 bit"); break;
  }
#endif

  mlx.setRefreshRate(MLX90640_0_5_HZ);
#if SERIAL_DEBUG > 0
  Serial.print("Current frame rate: ");
  mlx90640_refreshrate_t rate = mlx.getRefreshRate();
  switch (rate) {
    case MLX90640_0_5_HZ: Serial.println("0.5 Hz"); break;
    case MLX90640_1_HZ: Serial.println("1 Hz"); break; 
    case MLX90640_2_HZ: Serial.println("2 Hz"); break;
    case MLX90640_4_HZ: Serial.println("4 Hz"); break;
    case MLX90640_8_HZ: Serial.println("8 Hz"); break;
    case MLX90640_16_HZ: Serial.println("16 Hz"); break;
    case MLX90640_32_HZ: Serial.println("32 Hz"); break;
    case MLX90640_64_HZ: Serial.println("64 Hz"); break;
  }
#endif
}

void loadTemps(Temp& temp)
{
  int frameSuccess = mlx.getFrame(temp.Values);
  if(frameSuccess != 0)
  {
#if SERIAL_DEBUG > 0
    Serial.print("Failed with error code ");
    Serial.println(frameSuccess);
    
    // Clear previous values
    temp.Min = TempConfig_ManualMin;
    temp.Max = TempConfig_ManualMax;
    /*
    for(int vi = 0; vi < Temps_Size; vi++)
      temp.Values[vi] = temp.Min + (temp.Max - temp.Min) * (vi / (float)Temps_Size);
    */
#endif
    //return;
  }

  // Calculate min/max temperature
  {
    float minTemp = 100;
    float maxTemp = -100;
    {
      static const int tempMeasurePoints_length = 4 * 5 + 8;
      static const int tempMeasurePoints[tempMeasurePoints_length] = {
          0,  31, 736, 767, // Corners
        367, 368, 399, 400, // Center
        268, 275, 492, 499, // Inner Circle
        169, 182, 585, 598, // Mid Circle
         99, 124, 643, 668, // Outer Circle
        // Cross
        175, 176, // Upper
        361, 393, // Left
        374, 406, // Right
        591, 592  // Lower
      };
      for(int tmpi = 0; tmpi < tempMeasurePoints_length; tmpi++)
      {
        float t = temp.Values[tempMeasurePoints[tmpi]];
        if(minTemp > t)
          minTemp = t;
        if(maxTemp < t)
          maxTemp = t;
      }
    }
#if SERIAL_DEBUG > 0
    Serial.print("Original Temp: min=");
    Serial.print(minTemp);
    Serial.print("; max=");
    Serial.print(maxTemp);
    Serial.println();
#endif

    if(maxTemp < minTemp)
    {
      float t = maxTemp;
      maxTemp = minTemp;
      minTemp = t;
    }
    if(maxTemp - minTemp < 2)
    {
      minTemp -= 0;
      maxTemp += 1;
    }

#if SERIAL_DEBUG > 0
    Serial.print("Result Temp: min=");
    Serial.print(minTemp);
    Serial.print("; max=");
    Serial.print(maxTemp);
    Serial.println();
#endif
    temp.Min = minTemp;
    temp.Max = maxTemp;
  }
}
