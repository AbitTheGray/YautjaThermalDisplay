/*************************************************** 
  This is a example sketch demonstrating graphic drawing
  capabilities of the SSD1351 library for the 1.5" 
  and 1.27" 16-bit Color OLEDs with SSD1351 driver chip

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/1431
  ------> http://www.adafruit.com/products/1673
 
  If you're using a 1.27" OLED, change SCREEN_HEIGHT to 96 instead of 128.

  These displays use SPI to communicate, 4 or 5 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution

  The Adafruit GFX Graphics core library is also required
  https://github.com/adafruit/Adafruit-GFX-Library
  Be sure to install it!
 ****************************************************/

// Screen dimensions
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128 // Change this to 96 for 1.27" OLED.

// You can use any (4 or) 5 pins 
#define OLED_DIN 11
#define OLED_CLK 13
#define OLED_CS 14
#define OLED_DC 15
#define OLED_RST 16

// Color definitions
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>

// Option 1: use any pins but a little slower
Adafruit_SSD1351 oled = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_CS, OLED_DC, OLED_DIN, OLED_CLK, OLED_RST);  

// Option 2: must use the hardware SPI pins 
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be 
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
//Adafruit_SSD1351 oled = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

const int Temps_Width  = 32;
const int Temps_Height = 24;
const int Temps_Size   = Temps_Width * Temps_Height;
const int Temps_Width4  = Temps_Width * 4;
const int Temps_Height4 = Temps_Height * 4;
const int Temps_Size4   = Temps_Width4 * Temps_Height4;

void setup(void)
{
  oled.begin();
}

void loop()
{
  //oled.fillScreen(BLACK);

  float temps[Temps_Size];
  float t_min = 20;
  float t_max = 30;
  
  for(int y = 0; y < Temps_Height; y++)
  {
    int yi = y * Temps_Width;
    for(int x = 0; x < Temps_Width; x++)
    {
      int i = yi + x;

      temps[i] = random(t_min * 100, t_max * 100) / 100.0f;
    }
  }

  drawMap(temps, t_min, t_max);

  //delay(500);
}

uint16_t getColorFromTemp(float temp, float t_min, float t_max)
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

/*
const float weights[9 * 4] = {
  0.5f, 0.2f, 0.2f, 0.1f, // x-2 y-2
  0.2f, 0.5f, 0.1f, 0.2f, // x-1 y-2
  0.0f, 0.6f, 0.0f, 0.4f, // x   y-2
  
  0.2f, 0.1f, 0.5f, 0.2f, // x-2 y-1
  0.1f, 0.2f, 0.2f, 0.5f, // x-1 y-1
  0.0f, 0.4f, 0.0f, 0.6f, // x   y-1
  
  0.0f, 0.0f, 0.6f, 0.4f, // x-2 y
  0.0f, 0.0f, 0.4f, 0.6f, // x-1 y
  0.0f, 0.0f, 0.0f, 1.0f, // x   y   = Result pixel (raw)
};
*/
        
void drawMap(float* temps, float t_min, float t_max)
{
  // Temp map
  // height: 96
  /*
  for(int y = 0; y < Temps_Height; y++)
  {
    int yi = y * Temps_Width;
    int yi_prev = (y > 0 ? y-1 : 0) * Temps_Width;
    for(int x = 0; x < Temps_Width; x++)
    {
      int i = yi + x;
      int i_prev = yi_prev + x;

      // x-
      oled.fillRect(
        x * 4 - 1,
        y * 4 + 1,
        2,
        2,
        getColorFromTemp(
          (
            temps[i]
            + (x > 0 ? temps[i - 1] : temps[i])
          ) / 2, 
          t_min, 
          t_max
        )
      );

      // y-
      oled.fillRect(
        x * 4 + 1,
        y * 4 - 1,
        2,
        2,
        getColorFromTemp(
          (
            temps[i]
            + temps[i_prev]
          ) / 2, 
          t_min, 
          t_max
        )
      );

      // x- y-
      oled.fillRect(
        x * 4 - 1,
        y * 4 - 1,
        2,
        2,
        getColorFromTemp(
          (
            temps[i]
            + (x > 0 ? temps[i - 1] : temps[i])
            + temps[i_prev]
            + (x > 0 ? temps[i_prev - 1] : temps[i_prev])
          ) / 4, 
          t_min, 
          t_max
        )
      );
      
      oled.fillRect(
        x * 4 + 1,
        y * 4 + 1,
        2,
        2,
        getColorFromTemp(temps[i], t_min, t_max)
      );
    }
  }
  */
  /*
  {
    uint16_t colors[Temps_Size];
    
    for(int y = 0; y < Temps_Height; y++)
    {
      int yi = y * Temps_Width;
      for(int x = 0; x < Temps_Width; x++)
      {
        int i = yi + x;

        colors[i] = getColorFromTemp(temps[i], t_min, t_max);
      }
    }
    
    oled.drawRGBBitmap(0, 0, colors, Temps_Width, Temps_Height);
  }
  */
  /*
  {
    uint16_t colors[Temps_Size4];
    
    for(int y = 0; y < Temps_Height; y++)
    {
      int yi = y * Temps_Width;
      int yi_prev = (y > 0 ? y-1 : 0) * Temps_Width;

      int cyi = y * Temps_Width4;
      for(int x = 0; x < Temps_Width; x++)
      {
        int i = yi + x;
        int i_prev = yi_prev + x;
        
        float t00 = temps[i_prev - (x > 0 ? 1 : 0)];
        float t01 = temps[i - (x > 0 ? 1 : 0)];
        float t10 = temps[i_prev];
        float t11 = temps[i];

        if(y > 0)
        {
          if(x > 0)
          {
            // x-2 y-2
            colors[cyi - 2 - (Temps_Width4 * 2)] = getColorFromTemp(
              t00 * weights[0 + 0] + t10 * weights[0 + 1] + t01 * weights[0 + 2] + t11 * weights[0 + 3], 
              t_min, 
              t_max
            );
            
            // x-1 y-2
            colors[cyi - 1 - (Temps_Width4 * 2)] = getColorFromTemp(
              t00 * weights[4 + 0] + t10 * weights[4 + 1] + t01 * weights[4 + 2] + t11 * weights[4 + 3], 
              t_min, 
              t_max
            );
          }
            
          // x y-2
          colors[cyi - 0 - (Temps_Width4 * 2)] = getColorFromTemp(
            t00 * weights[8 + 0] + t10 * weights[8 + 1] + t01 * weights[8 + 2] + t11 * weights[8 + 3], 
            t_min, 
            t_max
          );
  
          if(x > 0)
          {
            // x-2 y-1
            colors[cyi - 2 - (Temps_Width4 * 1)] = getColorFromTemp(
              t00 * weights[12 + 0] + t10 * weights[12 + 1] + t01 * weights[12 + 2] + t11 * weights[12 + 3], 
              t_min, 
              t_max
            );
            
            // x-1 y-1
            colors[cyi - 1 - (Temps_Width4 * 1)] = getColorFromTemp(
              t00 * weights[16 + 0] + t10 * weights[16 + 1] + t01 * weights[16 + 2] + t11 * weights[16 + 3], 
              t_min, 
              t_max
            );
          }
            
          // x y-1
          colors[cyi - 0 - (Temps_Width4 * 1)] = getColorFromTemp(
            t00 * weights[20 + 0] + t10 * weights[20 + 1] + t01 * weights[20 + 2] + t11 * weights[20 + 3], 
            t_min, 
            t_max
          );
        }
  
        if(x > 0)
        {
          // x-2 y
          colors[cyi - 2 - (Temps_Width4 * 0)] = getColorFromTemp(
            t00 * weights[24 + 0] + t10 * weights[24 + 1] + t01 * weights[24 + 2] + t11 * weights[24 + 3], 
            t_min, 
            t_max
          );
          
          // x-1 y
          colors[cyi - 1 - (Temps_Width4 * 0)] = getColorFromTemp(
            t00 * weights[28 + 0] + t10 * weights[28 + 1] + t01 * weights[28 + 2] + t11 * weights[28 + 3], 
            t_min, 
            t_max
          );
        }
          
        // x y
        colors[cyi - 0 - (Temps_Width4 * 0)] = getColorFromTemp(
          t00 * weights[32 + 0] + t10 * weights[32 + 1] + t01 * weights[32 + 2] + t11 * weights[32 + 3], 
          t_min, 
          t_max
        );
      }
    }
    
    //oled.drawRGBBitmap(0, 0, colors, Temps_Width4, Temps_Height4);
  }
  */
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
        
        float t00 = temps[i_prev - (x > 0 ? 1 : 0)];
        float t01 = temps[i - (x > 0 ? 1 : 0)];
        float t10 = temps[i_prev];
        float t11 = temps[i];

        // Top Left
        uint16_t c = getColorFromTemp((t00 + t01 + t10 + t11) / 4.0f, t_min, t_max);
        colors[cyi_0 + cx + 0] = c;
        colors[cyi_0 + cx + 1] = c;
        colors[cyi_1 + cx + 0] = c;
        colors[cyi_1 + cx + 1] = c;

        // Left
        c = getColorFromTemp((t01 + t11) / 2.0f, t_min, t_max);
        colors[cyi_2 + cx + 0] = c;
        colors[cyi_2 + cx + 1] = c;
        colors[cyi_3 + cx + 0] = c;
        colors[cyi_3 + cx + 1] = c;

        // Top
        c = getColorFromTemp((t10 + t11) / 2.0f, t_min, t_max);
        colors[cyi_0 + cx + 2] = c;
        colors[cyi_0 + cx + 3] = c;
        colors[cyi_1 + cx + 2] = c;
        colors[cyi_1 + cx + 3] = c;

        // Original
        c = getColorFromTemp(t11, t_min, t_max);
        colors[cyi_2 + cx + 2] = c;
        colors[cyi_2 + cx + 3] = c;
        colors[cyi_3 + cx + 2] = c;
        colors[cyi_3 + cx + 3] = c;
      }
    }

    //for(int i = 0; i < Temps_Size4; i++)
    //  colors[i] = WHITE;
    
    oled.drawRGBBitmap(0, 0, colors, Temps_Width4, Temps_Height4);
  }

  // Info
  {
    // Scale
    {
      int offsetX = 32;
      int sizeX = 64;
      
      int sizeY = 8;
      int offsetY = 128 - sizeY - 1;

      for(int x = 0; x < sizeX; x++)
      {
        oled.fillRect(
          offsetX + x,
          offsetY,
          1,
          sizeY,
          getColorFromTemp(
            t_min + (t_max - t_min) * (x / (float)sizeX),
            t_min,
            t_max
          )
        );
      }

      // t_min
      {
        oled.drawChar(
          offsetX - 32, 
          offsetY,
          '0' + ((int)(t_min / 10) % 10),
          WHITE,
          BLACK,
          1
        );
        oled.drawChar(
          offsetX - 24, 
          offsetY,
          '0' + ((int)t_min % 10),
          WHITE,
          BLACK,
          1
        );
        oled.drawChar(
          offsetX - 18, 
          offsetY,
          '.',
          WHITE,
          BLACK,
          1
        );
        oled.drawChar(
          offsetX - 12, 
          offsetY,
          '0' + ((int)(t_min * 10) % 10),
          WHITE,
          BLACK,
          1
        );
      }

      // t_max
      {
        oled.drawChar(
          offsetX + sizeX + 4, 
          offsetY,
          '0' + ((int)(t_max / 10) % 10),
          WHITE,
          BLACK,
          1
        );
        oled.drawChar(
          offsetX + sizeX + 12, 
          offsetY,
          '0' + ((int)t_max % 10),
          WHITE,
          BLACK,
          1
        );
        oled.drawChar(
          offsetX + sizeX + 18, 
          offsetY,
          '.',
          WHITE,
          BLACK,
          1
        );
        oled.drawChar(
          offsetX + sizeX + 24, 
          offsetY,
          '0' + ((int)(t_max * 10) % 10),
          WHITE,
          BLACK,
          1
        );
      }
    }
  }
}
