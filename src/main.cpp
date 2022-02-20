#include <Arduino.h>

/* Just a list of supported boards by LovyanGFX. Use LGFX_AUTODETECT */
// #define LGFX_M5STACK               // M5Stack (Basic / Gray / Go / Fire)
// #define LGFX_M5STACK_CORE2         // M5Stack Core2
// #define LGFX_M5STACK_COREINK       // M5Stack CoreInk
// #define LGFX_M5STICK_C             // M5Stick C / CPlus
// #define LGFX_M5PAPER               // M5Paper
// #define LGFX_M5TOUGH               // M5Tough
// #define LGFX_ODROID_GO             // ODROID-GO
// #define LGFX_TTGO_TS               // TTGO TS
// #define LGFX_TTGO_TWATCH           // TTGO T-Watch
// #define LGFX_TTGO_TWRISTBAND       // TTGO T-Wristband
// #define LGFX_TTGO_TDISPLAY         // TTGO T-Display
// #define LGFX_DDUINO32_XS           // DSTIKE D-duino-32 XS
// #define LGFX_LOLIN_D32_PRO         // LoLin D32 Pro
// #define LGFX_ESP_WROVER_KIT        // ESP-WROVER-KIT
// #define LGFX_WIFIBOY_PRO           // WiFiBoy Pro
// #define LGFX_WIFIBOY_MINI          // WiFiBoy mini
// #define LGFX_MAKERFABS_TOUCHCAMERA // Makerfabs Touch with Camera
// #define LGFX_MAKERFABS_MAKEPYTHON  // Makerfabs MakePython
// #define LGFX_WT32_SC01             // Seeed WT32-SC01
// #define LGFX_WIO_TERMINAL          // Seeed Wio Terminal
// #define LGFX_PYBADGE               // Adafruit PyBadge
// #define LGFX_ESPBOY                // ESPboy

  #define LGFX_AUTODETECT // Above boards are supported so it will autodetect

// Enable v1.0.0, older version is missing some of the predefined boards
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#include <LGFX_AUTODETECT.hpp>  
// Check this file if you want to define custom boards/pins
// #include <lgfx_user/LGFX_ESP32_sample.hpp> 
#include <vector>

#define LINE_COUNT 6

// Wireless Tag => LGFX_WT32_SC01 (also Seeed WT32-SC01)
// 3.5" 480x320 ST7796S TFT Display
// Capacitive touchscreen FT6336U

static LGFX lcd;

static std::vector<int> points[LINE_COUNT];
static int colors[] = { TFT_RED, TFT_GREEN, TFT_BLUE, TFT_CYAN, TFT_MAGENTA, TFT_YELLOW };
static int xoffset, yoffset, point_count;

int getBaseColor(int x, int y)
{
  return ((x^y)&3 || ((x-xoffset)&31 && y&31) ? TFT_BLACK : ((!y || x == xoffset) ? TFT_WHITE : TFT_DARKGREEN));
}

void setup(void)
{
  lcd.init();
    
  if (lcd.width() < lcd.height()) lcd.setRotation(lcd.getRotation() ^ 1);

  yoffset = lcd.height() >> 1;
  xoffset = lcd.width()  >> 1;
  point_count = lcd.width() + 1;

  for (int i = 0; i < LINE_COUNT; i++)
  {
    points[i].resize(point_count);
  }

  lcd.startWrite();
  lcd.setAddrWindow(0, 0, lcd.width(), lcd.height());
  for (int y = 0; y < lcd.height(); y++)
  {
    for (int x = 0; x < lcd.width(); x++)
    {
      lcd.writeColor(getBaseColor(x, y - yoffset), 1);
    }
  }
  lcd.endWrite();
}

static int32_t x, y;
void loop(void)
{
  /* FPS calculation */
  static int prev_sec;
  static int fps;
  ++fps;
  int sec = millis() / 1000;
  if (prev_sec != sec)
  {
    prev_sec = sec;
    lcd.setCursor(0,0);
    lcd.printf("fps:%03d", fps);
    fps = 0;
  }

  static int count;

  // Setting up random data for graph
  for (int i = 0; i < LINE_COUNT; i++)
  {
    points[i][count % point_count] = (sinf((float)count / (10 + 30 * i))+sinf((float)count / (13 + 37 * i))) * (lcd.height() >> 2);
  }

  ++count;

  // startWrite/endWrite is used to draw buffer at once=smooth graphics
  lcd.startWrite();
  int index1 = count % point_count;
  for (int x = 0; x < point_count-1; x++)
  {
    int index0 = index1;
    index1 = (index0 +1) % point_count;
    for (int i = 0; i < LINE_COUNT; i++)
    {
      int y = points[i][index0];
      if (y != points[i][index1])
      {
        lcd.writePixel(x, y + yoffset, getBaseColor(x, y));
      }
    }

    for (int i = 0; i < LINE_COUNT; i++)
    {
      int y = points[i][index1];
      lcd.writePixel(x, y + yoffset, colors[i]);
    }
  }
  lcd.endWrite();
  
  /* Touch draw with rectangles 
   * Have only this if you just want to only draw on screen
  */
  if (lcd.getTouch(&x, &y)) {
    lcd.fillRect(x-2, y-2, 5, 5, count*7);
    lcd.setCursor(380,0);
    lcd.printf("Touch:(%03d,%03d)", x,y);
  }
}