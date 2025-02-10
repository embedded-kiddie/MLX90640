/*----------------------------------------------------------------------
 * Touch screen calibration for XPT2046_Touchscreen
 *
 * This program is based on the following article:
 * https://bytesnbits.co.uk/arduino-touchscreen-calibration-coding/
 *----------------------------------------------------------------------*/
#ifndef _XPT2046_SCREENPOINT_H_
#define _XPT2046_SCREENPOINT_H_

#include <math.h>

/*----------------------------------------------------------------------
 * Graphics library
 *----------------------------------------------------------------------*/
#if     defined (LOVYANGFX_HPP_)
#define GFX_TYPE  LGFX

#elif   defined (_TFT_eSPIH_)
#define GFX_TYPE  TFT_eSPI

#elif defined (_ADAFRUIT_GFX_H)
#define GFX_TYPE  Adafruit_GFX

#elif defined (_ARDUINO_TFT_H_)
#define GFX_TYPE  Arduino_TFT

#else
#error use LovyanGFX, TFT_eSPI, Adafruit_GFX or GFX_Library_for_Arduino
#endif

/*----------------------------------------------------------------------
 * Defining the class that inherits from XPT2046_Touchscreen class
 *----------------------------------------------------------------------*/
class XPT2046_ScreenPoint : public XPT2046_Touchscreen {
  // Inheriting constructor from the parent class
  using XPT2046_Touchscreen::XPT2046_Touchscreen;

private:
  bool calibrated = false;
  uint8_t rotation = 0;
  uint16_t width = 0, height = 0;
  float xCalM = 0.0, yCalM = 0.0;  // gradients
  float xCalC = 0.0, yCalC = 0.0;  // y axis crossing points

public:
  void setRotation(uint8_t r) {
    XPT2046_Touchscreen::setRotation(rotation = r % 4);
  }

  void begin(SPIClass &spi, uint16_t w, uint16_t h, uint8_t r = 0) {
    XPT2046_Touchscreen::begin(spi);
    setRotation(r);
    width  = w;
    height = h;
  }

  void begin(uint16_t w, uint16_t h, uint8_t r = 0) {
    begin(SPI, w, h, r);
  }

  bool setTouch(const uint16_t *cal) {
    if (cal[4] == rotation) {
      int16_t x1, y1, x2, y2;
      int16_t xDist = width - 40;
      int16_t yDist = height - 40;

      x1 = cal[0];
      y1 = cal[1];
      x2 = cal[2];
      y2 = cal[3];

      // translate in form pos = m x val + c
      xCalM = (float)xDist / (float)(x2 - x1);
      xCalC = 20.0 - ((float)x1 * xCalM);

      yCalM = (float)yDist / (float)(y2 - y1);
      yCalC = 20.0 - ((float)y1 * yCalM);

      calibrated = true;
    }

    return calibrated;
  }

  bool getTouch(uint16_t *x, uint16_t *y, uint16_t threshold = 600) {
    if (touched()) {
      TS_Point p = getPoint();

      if (p.z >= threshold) {
        if (calibrated) {
          int16_t xCoord = round((p.x * xCalM) + xCalC);
          int16_t yCoord = round((p.y * yCalM) + yCalC);
    
          if (xCoord < 0) xCoord = 0; else
          if (xCoord >= width ) xCoord = width  - 1;
          if (yCoord < 0) yCoord = 0; else
          if (yCoord >= height) yCoord = height - 1;
    
          *x = xCoord;
          *y = yCoord;
        } else {
          // https://randomnerdtutorials.com/lvgl-cheap-yellow-display-esp32-2432s028r/
          static const struct {
            uint16_t xmin, xmax, ymin, ymax;
          } cal[4] = {
            {240, 3800, 200, 3700},
            {200, 3700, 240, 3800},
            {260, 3850, 300, 3950},
            {300, 3950, 260, 3850}
          };
          *x = map(p.x, cal[rotation].xmin, cal[rotation].xmax, 0, width );
          *y = map(p.y, cal[rotation].ymin, cal[rotation].ymax, 0, height);
        }
        return true;
      }
    }
    return false;
  }

  void calibrateTouch(uint16_t *cal, GFX_TYPE *tft, uint32_t color_fg, uint32_t color_bg) {
    tft->fillScreen(color_bg);
    tft->setTextColor(color_fg, color_bg);

#if defined (_TFT_eSPIH_) || defined (LOVYANGFX_HPP_)
    tft->setTextDatum(CC_DATUM);
    tft->drawString("Touch the center of the cross", width / 2, height / 2, 2);
#else // _ADAFRUIT_GFX_H or _ARDUINO_TFT_H_
    tft->setTextSize(2);
    tft->setCursor((width - 30 * 12) / 2, (height - 16) / 2); // font size: 12x16
    tft->print("Touch the center of the cross");
#endif

    while (touched()); // wait for no touch

    tft->drawFastHLine(10, 20, 20, color_fg);
    tft->drawFastVLine(20, 10, 20, color_fg);

    while (!touched());
    delay(50); // wait for touch being stable

    TS_Point p = getPoint();
    cal[0] = p.x;
    cal[1] = p.y;
    tft->drawFastHLine(10, 20, 20, color_bg);
    tft->drawFastVLine(20, 10, 20, color_bg);

    delay(500);
    while (touched()); // wait for no touch

    tft->drawFastHLine(width - 30, height - 20, 20, color_fg);
    tft->drawFastVLine(width - 20, height - 30, 20, color_fg);

    while (!touched());
    delay(50); // wait for touch being stable

    p = getPoint();
    cal[2] = p.x;
    cal[3] = p.y;
    tft->drawFastHLine(width - 30, height - 20, 20, color_bg);
    tft->drawFastVLine(width - 20, height - 30, 20, color_bg);

    cal[4] = rotation;
    calibrated = true;
  }
};

#endif // _XPT2046_SCREENPOINT_H_
