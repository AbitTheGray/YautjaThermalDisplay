
//----------------------------------------------------------------
// Included headers

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>

#include "DataStructs.h"
#include "DrawUtils.h"

#if CAMERA_TYPE == 0
#include "Camera_00_Virtual.h"
#elif CAMERA_TYPE == 1
#include "Camera_01_Adafruit.h"
#elif CAMERA_TYPE == 2
#include "Camera_02_Melexis.h"
#else
# warning "Unimplemented camera type"
# define CAMERA_TYPE 0
#endif

//----------------------------------------------------------------
// Configurations

// Screen dimensions
const int Screen_Width = 128; // (do not change)
const int Screen_Height = 128; // Change this to 96 for 1.27" OLED, won't display status line

// Screen pins
#if SCREEN_PINOUT_VARIANT == 0
  // Hardware, Teensy 3.2
  const int Screen_Pin_MOSI = 11; // Data Output
  const int Screen_Pin_CLK  = 13; // sck = Clock
  const int Screen_Pin_CS   = 10; // Device Select
  const int Screen_Pin_DC   = 15; // Display/Command
  const int Screen_Pin_RST  = ??; // Reset (optional = -1)
#elif SCREEN_PINOUT_VARIANT == 1
  // Alternate Hardware, Teensy 3.2
  const int Screen_Pin_MOSI =  7; // Data Output
  const int Screen_Pin_CLK  = 14; // sck = Clock
  const int Screen_Pin_CS   = 10; // Device Select
  const int Screen_Pin_DC   = 15; // Display/Command
  const int Screen_Pin_RST  = ??; // Reset (optional = -1)
#elif SCREEN_PINOUT_VARIANT == 99
  // Custom
  const int Screen_Pin_MOSI = 11; // Data Output
  const int Screen_Pin_CLK  = 13; // sck = Clock
  const int Screen_Pin_CS   = 14; // Device Select
  const int Screen_Pin_DC   = 15; // Display/Command
  const int Screen_Pin_RST  = 16; // Reset (optional = -1)
#else
# error "No pinout selected"
#endif

//----------------------------------------------------------------
// Global variables

// Color definitions
#define OLED_COLOR_BLACK           0x0000
#define OLED_COLOR_BLUE            0x001F
#define OLED_COLOR_RED             0xF800
#define OLED_COLOR_GREEN           0x07E0
#define OLED_COLOR_CYAN            0x07FF
#define OLED_COLOR_MAGENTA         0xF81F
#define OLED_COLOR_YELLOW          0xFFE0  
#define OLED_COLOR_WHITE           0xFFFF

Adafruit_SSD1351 oled = Adafruit_SSD1351(Screen_Width, Screen_Height, Screen_Pin_CS, Screen_Pin_DC, Screen_Pin_MOSI, Screen_Pin_CLK, Screen_Pin_RST);

//----------------------------------------------------------------
// Functions

void setup(void)
{
#if SERIAL_DEBUG > 0
  while(!Serial)
    delay(10);
  Serial.begin(115200);
#endif
    
  oled.begin();
  oled.fillScreen(OLED_COLOR_BLACK);

  setupMlx90640();
}

void loop()
{
  Temp currentTemp;

  //oled.fillScreen(BLACK);

  loadTemps(currentTemp);
#if SERIAL_DEBUG == 2
  //TODO Print CurrentTemp to Serial
#endif
  
  if(Config_ManualTemp)
  {
    currentTemp.Min = TempConfig_ManualMin;
    currentTemp.Max = TempConfig_ManualMax;
  }

  drawMap(currentTemp);

  //delay(500);
}
        
void drawMap(Temp& temp)
{
  // Temp map
  // height: 96
  drawTempMap(oled, temp, 0);

  // Info
  if(Screen_Height > 96)
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
            oled, 
            temp.Min + (temp.Max - temp.Min) * (x / (float)sizeX),
            temp.Min,
            temp.Max
          )
        );
      }

      // Minimum temperature
      {
        oled.drawChar(
          offsetX - 32, 
          offsetY,
          '0' + ((int)(temp.Min / 10) % 10),
          OLED_COLOR_WHITE,
          OLED_COLOR_BLACK,
          1
        );
        oled.drawChar(
          offsetX - 24, 
          offsetY,
          '0' + ((int)temp.Min % 10),
          OLED_COLOR_WHITE,
          OLED_COLOR_BLACK,
          1
        );
        oled.drawChar(
          offsetX - 18, 
          offsetY,
          '.',
          OLED_COLOR_WHITE,
          OLED_COLOR_BLACK,
          1
        );
        oled.drawChar(
          offsetX - 12, 
          offsetY,
          '0' + ((int)(temp.Min * 10) % 10),
          OLED_COLOR_WHITE,
          OLED_COLOR_BLACK,
          1
        );
      }

      // Maximum temperature
      {
        oled.drawChar(
          offsetX + sizeX + 4, 
          offsetY,
          '0' + ((int)(temp.Max / 10) % 10),
          OLED_COLOR_WHITE,
          OLED_COLOR_BLACK,
          1
        );
        oled.drawChar(
          offsetX + sizeX + 12, 
          offsetY,
          '0' + ((int)temp.Max % 10),
          OLED_COLOR_WHITE,
          OLED_COLOR_BLACK,
          1
        );
        oled.drawChar(
          offsetX + sizeX + 18, 
          offsetY,
          '.',
          OLED_COLOR_WHITE,
          OLED_COLOR_BLACK,
          1
        );
        oled.drawChar(
          offsetX + sizeX + 24, 
          offsetY,
          '0' + ((int)(temp.Max * 10) % 10),
          OLED_COLOR_WHITE,
          OLED_COLOR_BLACK,
          1
        );
      }
    }
  }
}
