/*================================================================================
 * GFX Library
 * For the 'drawBmpFile()' function to work correctly, LovyanGFX requires 
 * that the SD library header file be included before '<LovyanGFX.hpp>`.
 *================================================================================*/
#include <SD.h>
#include <SPI.h>

#if defined (USE_LOVYAN_GFX)
/*--------------------------------------------------------------------------------
 * LovyanGFX
 * https://github.com/lovyan03/LovyanGFX
 *--------------------------------------------------------------------------------*/
  /**************************************************/
  /***** variants/jczn_2432s028r/pins_arduino.h *****/
  /**************************************************/
  #if defined (ARDUINO_ESP32_2432S028R)
    #if USE_AUTODETECT
    #define LGFX_AUTODETECT
    #include <LovyanGFX.h>
    #else
    // false: Panel driver: ILI9341 (micro-USB x 1 type)
    // true : Panel driver: ST7789  (micro-USB x 1 + USB-C x 1 type)
    #define DISPLAY_CYD_2USB  true
    #include "./boards/LGFX_ESP32_2432S028R_CYD.hpp"
    #endif

  /************************************************/
  /***** variants/XIAO_ESP32S3/pins_arduino.h *****/
  /************************************************/
  #elif defined (ARDUINO_XIAO_ESP32S3)
    #include "./boards/LGFX_XIAO_ESP32S3_ST7789.hpp"

  /***************************************************/
  /***** variants/your_board_type/pins_arduino.h *****/
  /***************************************************/
  #else
  #endif

#elif defined (USE_TFT_ESPI)
/*--------------------------------------------------------------------------------
 * TFT_eSPI
 * https://github.com/Bodmer/TFT_eSPI
 *--------------------------------------------------------------------------------*/
  /**************************************************/
  /************* Configure User_Setup.h *************/
  /**************************************************/
  #include <TFT_eSPI.h>
#endif

/*--------------------------------------------------------------------------------
 * Definitions of graphics helpers
 *--------------------------------------------------------------------------------*/
// Font size for setTextSize(2)
#define FONT_WIDTH    12 // [px] (Device coordinate system)
#define FONT_HEIGHT   16 // [px] (Device coordinate system)
#define LINE_HEIGHT   18 // [px] (FONT_HEIGHT + margin)

uint16_t lcd_width;
uint16_t lcd_height;

/*--------------------------------------------------------------------------------
 * LovyanGFX
 *--------------------------------------------------------------------------------*/
#if defined (LOVYANGFX_HPP_)

LGFX lcd;
LGFX_Sprite lcd_sprite(&lcd);

#define SCREEN_ROTATION 3
#define GFX_TYPE        LGFX
#define GFX_INSTANCE    lcd
#define GFX_EXEC(x)     lcd.x
#define GFX_FAST(x)     lcd_sprite.x

void gfx_setup(void) {
  GFX_EXEC(init());
  GFX_EXEC(clear(TFT_BLACK));
  GFX_EXEC(setBrightness(128)); // 0 ~ 255
  GFX_EXEC(setRotation(SCREEN_ROTATION));
  GFX_EXEC(setTextColor(TFT_WHITE, TFT_BLACK));
  GFX_EXEC(initDMA());
  lcd_width  = GFX_EXEC(width());
  lcd_height = GFX_EXEC(height());
}

/*--------------------------------------------------------------------------------
 * TFT_eSPI
 *--------------------------------------------------------------------------------*/
#elif defined (_TFT_eSPIH_)

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite tft_sprite(&tft);

#define SCREEN_ROTATION 3
#define GFX_TYPE        TFT_eSPI
#define GFX_INSTANCE    tft
#define GFX_EXEC(x)     tft.x
#define GFX_FAST(x)     tft_sprite.x
#define setClipRect     setViewport
#define clearClipRect   resetViewport

void gfx_setup(void) {
  GFX_EXEC(init());
  GFX_EXEC(fillScreen(0));
  GFX_EXEC(setRotation(SCREEN_ROTATION));
  GFX_EXEC(setTextColor(TFT_WHITE, TFT_BLACK));
  GFX_EXEC(initDMA(false)); // 'true' puts garbage on PNG images.
  lcd_width  = GFX_EXEC(width());
  lcd_height = GFX_EXEC(height());
}
#endif // LovyanGFX or TFT_eSPI

/*--------------------------------------------------------------------------------
 * Common function
 *--------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdarg.h>

#ifndef BUF_SIZE
#define BUF_SIZE  64
#endif

void gfx_printf(uint16_t x, uint16_t y, const char* fmt, ...) {
  int len = 0;
  char buf[BUF_SIZE];

  va_list arg_ptr;
  va_start(arg_ptr, fmt);
  len = vsnprintf(buf, sizeof(buf), fmt, arg_ptr);
  va_end(arg_ptr);

  // use the followings:
  // setTextColor(foreground_color, background_color);
  // setTextDatum(textdatum_t::...);
  GFX_EXEC(drawString(buf, x, y));
}
