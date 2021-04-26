#pragma once
#include "DataStructs.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>

uint16_t getColorFromTemp(Adafruit_SSD1351& oled, float temp, float t_min, float t_max)
{
  float range = 180.0 * (temp - t_min) / (t_max - t_min);

  if(range < 0)
    return oled.color565(0, 0, 0);
  
  if(range >= 0 && range < 30)
    return oled.color565(
      0,
      0,
      20 + (120.0/30.0) * range
    );
  
  if(range >= 30 && range < 60)
    return oled.color565(
      (120.0 / 30) * (range - 30.0),
      0,
      140 - (60.0/30.0) * (range - 30.0)
    );
  
  if(range >= 60 && range < 90)
    return oled.color565(
      120 + (135.0/30.0) * (range - 60.0),
      0,
      80 - (70.0/30.0) * (range - 60.0)
    );
  
  if (range >= 90 && range < 120)
    return oled.color565(
      255,
      0 + (60.0/30.0) * (range - 90.0),
      10 - (10.0/30.0) * (range - 90.0)
    );
  
  if (range >= 120 && range < 150)
    return oled.color565(
      255,
      60 + (175.0/30.0) * (range - 120.0),
      0
    );
  
  if (range >= 150 && range <= 180)
    return oled.color565(
      255,
      235 + (20.0/30.0) * (range - 150.0),
      0 + 255.0/30.0 * (range - 150.0)
    );

  return oled.color565(255, 255, 255);
}

void drawTempMap(Adafruit_SSD1351& oled, Temp& temp, int offsetX = 0)
{
  uint16_t colors[Temps_Size4];
  
  for(int y = 0; y < Temps_Height; y++)
  {
    int yi = y * Temps_Width;
    int yi_prev = (y > 0 ? y-1 : 0) * Temps_Width;

    int cy = y * 4;

    int cyi_0 = (cy + 0) * Temps_Width4;
    int cyi_1 = (cy + 1) * Temps_Width4;
    int cyi_2 = (cy + 2) * Temps_Width4;
    int cyi_3 = (cy + 3) * Temps_Width4;
    
    for(int x = 0; x < Temps_Width; x++)
    {
      int i = yi + x;
      int i_prev = yi_prev + x;

      int cx = x * 4;
      
      float t00 = temp.Values[i_prev - (x > 0 ? 1 : 0)];
      float t01 = temp.Values[i - (x > 0 ? 1 : 0)];
      float t10 = temp.Values[i_prev];
      float t11 = temp.Values[i];

      // Top Left
      uint16_t c = getColorFromTemp(oled, (t00 + t01 + t10 + t11) / 4.0f, temp.Min, temp.Max);
      colors[cyi_0 + cx + 0] = c;
      colors[cyi_0 + cx + 1] = c;
      colors[cyi_1 + cx + 0] = c;
      colors[cyi_1 + cx + 1] = c;

      // Left
      c = getColorFromTemp(oled, (t01 + t11) / 2.0f, temp.Min, temp.Max);
      colors[cyi_2 + cx + 0] = c;
      colors[cyi_2 + cx + 1] = c;
      colors[cyi_3 + cx + 0] = c;
      colors[cyi_3 + cx + 1] = c;

      // Top
      c = getColorFromTemp(oled, (t10 + t11) / 2.0f, temp.Min, temp.Max);
      colors[cyi_0 + cx + 2] = c;
      colors[cyi_0 + cx + 3] = c;
      colors[cyi_1 + cx + 2] = c;
      colors[cyi_1 + cx + 3] = c;

      // Original
      c = getColorFromTemp(oled, t11, temp.Min, temp.Max);
      colors[cyi_2 + cx + 2] = c;
      colors[cyi_2 + cx + 3] = c;
      colors[cyi_3 + cx + 2] = c;
      colors[cyi_3 + cx + 3] = c;
    }
  }
  
  oled.drawRGBBitmap(0, 0, colors, Temps_Width4, Temps_Height4);
}
