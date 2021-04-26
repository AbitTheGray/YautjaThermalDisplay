// Color definitions
#define	OLED_COLOR_BLACK           0x0000
#define	OLED_COLOR_BLUE            0x001F
#define	OLED_COLOR_RED             0xF800
#define	OLED_COLOR_GREEN           0x07E0
#define OLED_COLOR_CYAN            0x07FF
#define OLED_COLOR_MAGENTA         0xF81F
#define OLED_COLOR_YELLOW          0xFFE0  
#define OLED_COLOR_WHITE           0xFFFF

//----------------------------------------------------------------
// Pragma configuration

//  0 = Hardware, Teensy 3.2
//  1 = Alternate Hardware, Teensy 3.2
// 99 = custom (back side)
#define SCREEN_PINOUT_VARIANT 99

//  0 = Virtual (random data)
//  1 = MLX90640 ( https://github.com/adafruit/Adafruit_MLX90640 )
//  2 = MLX90640 ( https://github.com/melexis/mlx90640-library )
#define CAMERA_TYPE 1

//  0 = Disabled, no debug
//  1 = Enabled, only main info
//  2 = Enabled, output sensor data
#define SERIAL_DEBUG 1

//TODO
#define CAMERA_ROTATION 0

//----------------------------------------------------------------
// Included headers

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>

#if CAMERA_TYPE == 1
# include <Wire.h>
# include <Adafruit_MLX90640.h>
#elif CAMERA_TYPE == 2
# include <Wire.h>
# include <MLX90640_API.h>
# include <MLX90640_I2C_Driver.h>
#elif CAMERA_TYPE == 0
# define CAMERA_TYPE 0
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

// Input temperature
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

// Display configuration
const bool Config_ManualTemp = false;

// Current temperature values
struct Temp
{
  float Values[Temps_Size];
  float Min = TempConfig_ManualMin;
  float Max = TempConfig_ManualMax;
};

#if CAMERA_TYPE == 1
  Adafruit_MLX90640 mlx;
#endif

#if CAMERA_TYPE == 2
  const byte MLX90640_address = 0x33; //Default 7-bit unshifted address of the MLX90640
  #define TA_SHIFT 8 //Default shift for MLX90640 in open air
  
  paramsMLX90640 mlx90640;
#endif

//----------------------------------------------------------------
// Global variables

Adafruit_SSD1351 oled = Adafruit_SSD1351(Screen_Width, Screen_Height, Screen_Pin_CS, Screen_Pin_DC, Screen_Pin_MOSI, Screen_Pin_CLK, Screen_Pin_RST);

Temp CurrentTemp;

void setup(void)
{
#if SERIAL_DEBUG > 0
  while(!Serial)
    delay(10);
  Serial.begin(115200);
#endif
    
  oled.begin();
  oled.fillScreen(OLED_COLOR_BLACK);
      
#if CAMERA_TYPE == 1
  Wire.begin();
  Wire.setClock(400000); //Increase I2C clock speed to 400kHz
#endif
}

void loadTemps(Temp& temp)
{
#if CAMERA_TYPE == 0
  // Random data
  for(int y = 0; y < Temps_Height; y++)
  {
    int yi = y * Temps_Width;
    for(int x = 0; x < Temps_Width; x++)
    {
      int i = yi + x;

      temp.Values[i] = random(temp.Min * 100, temp.Max * 100) / 100.0f;
    }
  }
#elif CAMERA_TYPE == 1
  if(mlx.getFrame(temp.Values) != 0)
  {
#if SERIAL_DEBUG > 0
    Serial.println("Failed");
    // Clear previous values
    for(int vi = 0; vi < Temps_Size; vi++)
      temp.Values[vi] = temp.Min + (temp.Max - temp.Min) * (vi / (float)Temps_Size);
#endif
    return;
  }
#elif CAMERA_TYPE == 2
  // MLX90640
#else
# warning "Unimplemented camera type"
#endif
}

void loop()
{
  //oled.fillScreen(BLACK);

  loadTemps(CurrentTemp);
#if SERIAL_DEBUG == 2
  //TODO Print CurrentTemp to Serial
#endif
  
  if(Config_ManualTemp)
  {
    CurrentTemp.Min = TempConfig_ManualMin;
    CurrentTemp.Max = TempConfig_ManualMax;
  }

  drawMap(CurrentTemp);

  //delay(500);
}

#if CAMERA_TYPE == 1
void setupMlx90640()
{  
#if SERIAL_DEBUG > 0
  Serial.println("Adafruit MLX90640 Simple Test");
#endif
  if (!mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire))
  {    
#if SERIAL_DEBUG > 0
    Serial.println("MLX90640 not found!");
#endif
    while(1)
      delay(10);
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

  mlx.setRefreshRate(MLX90640_2_HZ);
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
#endif

#if CAMERA_TYPE == 2
/// Returns true if the MLX90640 is detected on the I2C bus
boolean isMlx90640Connected()
{
  Wire.beginTransmission((uint8_t)MLX90640_address);
  
  if (Wire.endTransmission() != 0)
     return false; //Sensor did not ACK
  
  return true;
}  

void setupMlx90640()
{
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
#endif

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
        
void drawMap(Temp& temp)
{
  // Temp map
  // height: 96
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
        uint16_t c = getColorFromTemp((t00 + t01 + t10 + t11) / 4.0f, temp.Min, temp.Max);
        colors[cyi_0 + cx + 0] = c;
        colors[cyi_0 + cx + 1] = c;
        colors[cyi_1 + cx + 0] = c;
        colors[cyi_1 + cx + 1] = c;

        // Left
        c = getColorFromTemp((t01 + t11) / 2.0f, temp.Min, temp.Max);
        colors[cyi_2 + cx + 0] = c;
        colors[cyi_2 + cx + 1] = c;
        colors[cyi_3 + cx + 0] = c;
        colors[cyi_3 + cx + 1] = c;

        // Top
        c = getColorFromTemp((t10 + t11) / 2.0f, temp.Min, temp.Max);
        colors[cyi_0 + cx + 2] = c;
        colors[cyi_0 + cx + 3] = c;
        colors[cyi_1 + cx + 2] = c;
        colors[cyi_1 + cx + 3] = c;

        // Original
        c = getColorFromTemp(t11, temp.Min, temp.Max);
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
            temp.Min + (temp.Max - temp.Min) * (x / (float)sizeX),
            temp.Min,
            temp.Max
          )
        );
      }

      // t_min
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

      // t_max
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
