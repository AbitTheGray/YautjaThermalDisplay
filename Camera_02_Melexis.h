#pragma once
#include "DataStructs.h"
// https://github.com/melexis/mlx90640-library

#include <Wire.h>
#include "Camera_02_Melexis/MLX90640_API.h"
#include "Camera_02_Melexis/MLX90640_I2C_Driver.h"

//----------------------------------------------------------------
// Configurations

const byte MLX90640_address = 0x33; //Default 7-bit unshifted address of the MLX90640
#define TA_SHIFT 8 //Default shift for MLX90640 in open air

paramsMLX90640 mlx90640;

// 0x00 - 0.25Hz effective - Works
// 0x01 - 0.5Hz effective - Works
// 0x02 - 1Hz effective - Works
// 0x03 - 2Hz effective - Works
// 0x04 - 4Hz effective - Works
// 0x05 - 8Hz effective - Works at 800kHz
// 0x06 - 16Hz effective - Works at 800kHz
// 0x07 - 32Hz effective - fails
const uint8_t Mlx90640_FrequencyIndex = 0x04;

//----------------------------------------------------------------
// Utility Functions

/// Returns true if the MLX90640 is detected on the I2C bus
boolean isMlx90640Connected()
{
  Wire.beginTransmission((uint8_t)MLX90640_address);
  
  if (Wire.endTransmission() != 0)
     return false; //Sensor did not ACK
  
  return true;
}

//----------------------------------------------------------------
// Main functions 

void setupMlx90640()
{
#if CAMERA_TYPE == 2
  Wire.begin();
  if(Mlx90640_FrequencyIndex >= 0x05) // 8Hz
    Wire.setClock(800'000); //Increase I2C clock speed to 800kHz
  else
    Wire.setClock(400'000); //Increase I2C clock speed to 400kHz
#endif

  if (isMlx90640Connected() == false)
  {
#if SERIAL_DEBUG > 0
    Serial.println("MLX90640 not detected at default I2C address. Please check wiring. Freezing.");
#endif
    while (1);
  }
     
#if SERIAL_DEBUG > 0
  Serial.println("MLX90640 online!");
#endif

  //Get device parameters - We only have to do this once
  int status;
  uint16_t eeMLX90640[832];
  
  status = MLX90640_DumpEE(MLX90640_address, eeMLX90640);

#if SERIAL_DEBUG > 0
  if(status != 0)
    Serial.println("Failed to load system parameters");
#endif

  status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);

#if SERIAL_DEBUG > 0
  if(status != 0)
  {
    Serial.println("Parameter extraction failed");
    Serial.print(" status = ");
    Serial.println(status);
  }
#endif

  //Once params are extracted, we can release eeMLX90640 array

  MLX90640_I2CWrite(0x33, 0x800D, 6401);    // writes the value 1901 (HEX) = 6401 (DEC) in the register at position 0x800D to enable reading out the temperatures!!!
  // ===============================================================================================================================================================

  //MLX90640_SetRefreshRate(MLX90640_address, 0x00); //Set rate to 0.25Hz effective - Works
  //MLX90640_SetRefreshRate(MLX90640_address, 0x01); //Set rate to 0.5Hz effective - Works
  //MLX90640_SetRefreshRate(MLX90640_address, 0x02); //Set rate to 1Hz effective - Works
  //MLX90640_SetRefreshRate(MLX90640_address, 0x03); //Set rate to 2Hz effective - Works
  MLX90640_SetRefreshRate(MLX90640_address, 0x04); //Set rate to 4Hz effective - Works
  //MLX90640_SetRefreshRate(MLX90640_address, 0x05); //Set rate to 8Hz effective - Works at 800kHz
  //MLX90640_SetRefreshRate(MLX90640_address, 0x06); //Set rate to 16Hz effective - Works at 800kHz
  //MLX90640_SetRefreshRate(MLX90640_address, 0x07); //Set rate to 32Hz effective - fails
}

void loadTemps(Temp& temp)
{
  for (byte x = 0; x < 2; x++) //Read both subpages
  {
    uint16_t mlx90640Frame[834];
    int status = MLX90640_GetFrameData(MLX90640_address, mlx90640Frame);
  
#if SERIAL_DEBUG > 0
    if (status < 0)
    {
      Serial.print("GetFrame Error: ");
      Serial.println(status);
    }
#endif

    float vdd = MLX90640_GetVdd(mlx90640Frame, &mlx90640);
    float Ta = MLX90640_GetTa(mlx90640Frame, &mlx90640);

    float tr = Ta - TA_SHIFT; //Reflected temperature based on the sensor ambient temperature
    float emissivity = 0.95;

    MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, temp.Values);
  }
   
  // determine T_min and T_max and eliminate error pixels
  // ====================================================

  temp.Values[1*32 + 21] = 0.5 * (temp.Values[1*32 + 20] + temp.Values[1*32 + 22]);    // eliminate the error-pixels
  temp.Values[4*32 + 30] = 0.5 * (temp.Values[4*32 + 29] + temp.Values[4*32 + 31]);    // eliminate the error-pixels
  
  float minTemp = temp.Values[0];
  float maxTemp = temp.Values[0];
  {
    for (int i = 1; i < 768; i++)
    {
      if((temp.Values[i] > -41) && (temp.Values[i] < 301))
      {
        if(temp.Values[i] < minTemp)
          minTemp = temp.Values[i];
  
        if(temp.Values[i] > maxTemp)
          maxTemp = temp.Values[i];
      }
      else if(i > 0)   // temperature out of range
        temp.Values[i] = temp.Values[i-1];
      else
        temp.Values[i] = temp.Values[i+1];
    }
  }
  temp.Min = minTemp;
  temp.Max = maxTemp;
}
