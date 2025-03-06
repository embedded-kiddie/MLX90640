/*----------------------------------------------------------------------------
 * A wrapper class for the touchscreen libraries that supports 
 * calibration of the touch panel and rotation of the screen
 *
 * This program is based on the following article:
 * https://bytesnbits.co.uk/arduino-touchscreen-calibration-coding/
 *----------------------------------------------------------------------------*/
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

#if defined (_XPT2046_Touchscreen_h_)
/*************************************************/
/**** Inherits from XPT2046_Touchscreen class ****/
/*************************************************/
#define PARENT_CLASS  XPT2046_Touchscreen
#define SCREEN_POINT  TS_Point
#define SP_RAW_X(p)   p.x
#define SP_RAW_Y(p)   p.y
#define SP_RAW_Z(p)   p.z

#define Z_THRESHOLD 600
#endif // _XPT2046_Touchscreen_h_

#if defined (XPT2046_Bitbang_h)
/*************************************************/
/****** Inherits from XPT2046_Bitbang class ******/
/*************************************************/
#define PARENT_CLASS  XPT2046_Bitbang
#define SCREEN_POINT  TouchPoint
#define SP_RAW_X(p)   p.xRaw
#define SP_RAW_Y(p)   p.yRaw
#define SP_RAW_Z(p)   p.zRaw

#define N_SAMPLES   4
#define N_THRESHOLD 50
#define Z_THRESHOLD 600
#endif // XPT2046_Bitbang_h

/*----------------------------------------------------------------------
 * Defining the class that inherits from XPT2046 libraries
 *----------------------------------------------------------------------*/
class XPT2046_ScreenPoint : public PARENT_CLASS {
  // Inheriting constructor from the parent class
  using PARENT_CLASS::PARENT_CLASS;

private:
  bool calibrated = false;
  uint8_t rotation = 0;
  uint16_t width = 0, height = 0;
  float xCalM = 0.0, yCalM = 0.0;  // gradients
  float xCalC = 0.0, yCalC = 0.0;  // y axis crossing points

#if defined (_XPT2046_Touchscreen_h_)
  /*************************************************/
  /**** Inherits from XPT2046_Touchscreen class ****/
  /*************************************************/
public:
  void setRotation(uint8_t r) {
    PARENT_CLASS::setRotation(rotation = r % 4);
  }

  void begin(SPIClass &spi, uint16_t w, uint16_t h, uint8_t r = 0) {
    PARENT_CLASS::begin(spi);
    setRotation(r);
    width  = w;
    height = h;
  }

  void begin(uint16_t w, uint16_t h, uint8_t r = 0) {
    begin(SPI, w, h, r);
  }
#endif // _XPT2046_Touchscreen_h_

#if defined (XPT2046_Bitbang_h)
  /*************************************************/
  /****** Inherits from XPT2046_Bitbang class ******/
  /*************************************************/
  #include <vector>
  #include <algorithm>

  // Smirnov-Grubbs criterion
  static SCREEN_POINT filter(SCREEN_POINT *p) {
    std::vector<uint16_t> xRaw, yRaw;
    for (int i = 0; i < N_SAMPLES; ++i) {
      xRaw.push_back(SP_RAW_X(p[i]));
      yRaw.push_back(SP_RAW_Y(p[i]));
    }
    std::sort(xRaw.begin(), xRaw.end());
    std::sort(yRaw.begin(), yRaw.end());

    uint16_t xMed, yMed;
    for (int i = 1; i < N_SAMPLES - 1; ++i) {
      xMed += xRaw[i];
      yMed += yRaw[i];
    }
    xMed /= (N_SAMPLES - 2);
    yMed /= (N_SAMPLES - 2);

    uint16_t n, x, y, z;
    n = x = y = z = 0;
    for (int i = 0; i < N_SAMPLES; i++) {
      if (abs(xMed - SP_RAW_X(p[i])) <= N_THRESHOLD && abs(yMed - SP_RAW_Y(p[i])) <= N_THRESHOLD) {
        x += SP_RAW_X(p[i]);
        y += SP_RAW_Y(p[i]);
        z += SP_RAW_Z(p[i]);
        n++;
      }
    }

    if (n > 0) {
      x /= n;
      y /= n;
      z /= n;
    }

    return SCREEN_POINT {0, 0, x, y, z};
  }

public:
  void setRotation(uint8_t r) {
    rotation = r % 4;
  }

  void begin(uint16_t w, uint16_t h, uint8_t r = 0) {
    PARENT_CLASS::begin();
    setRotation(r);
    width  = w;
    height = h;
  }

  void begin(void) {
    begin();
  }

  SCREEN_POINT getPoint(void) {
    SCREEN_POINT p[N_SAMPLES], q, r;

    for (int i = 0; i < N_SAMPLES; i++) {
      p[i] = PARENT_CLASS::getTouch();
    }

    // Filter outliers
    q = filter(p);

		switch (rotation) {
      case 0:
        SP_RAW_X(r) = 4095 - SP_RAW_Y(q);
        SP_RAW_Y(r) = SP_RAW_X(q);
        break;
      case 1:
        SP_RAW_X(r) = SP_RAW_X(q);
        SP_RAW_Y(r) = SP_RAW_Y(q);
        break;
      case 2:
        SP_RAW_X(r) = SP_RAW_Y(q);
        SP_RAW_Y(r) = 4095 - SP_RAW_X(q);
        break;
      default: // 3
        SP_RAW_X(r) = 4095 - SP_RAW_X(q);
        SP_RAW_Y(r) = 4095 - SP_RAW_Y(q);
        break;
		}
    SP_RAW_Z(r) = SP_RAW_Z(q);
    r.x = r.y = 0;
    return r;
  }

  bool touched(void) {
    auto p = getPoint(); // PARENT_CLASS::getTouch();
    return (SP_RAW_Z(p) >= Z_THRESHOLD);
  }
#endif // XPT2046_Bitbang_h

  bool getTouch(uint16_t *x, uint16_t *y, uint16_t threshold = Z_THRESHOLD) {
    auto gp = getPoint();
    if (SP_RAW_Z(gp) >= threshold) {
      if (calibrated) {
        int16_t xCoord = round((SP_RAW_X(gp) * xCalM) + xCalC);
        int16_t yCoord = round((SP_RAW_Y(gp) * yCalM) + yCalC);
  
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
        *x = map(SP_RAW_X(gp), cal[rotation].xmin, cal[rotation].xmax, 0, width );
        *y = map(SP_RAW_Y(gp), cal[rotation].ymin, cal[rotation].ymax, 0, height);
      }
      return true;
    }
    return false;
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

  void calibrateTouch(uint16_t *cal, GFX_TYPE *tft, uint32_t color_fg, uint32_t color_bg) {
    tft->fillScreen(color_bg);
    tft->setTextColor(color_fg, color_bg);
    tft->setTextSize(1);

#if defined (_TFT_eSPIH_) || defined (LOVYANGFX_HPP_)
    tft->setTextDatum(CC_DATUM);
    tft->drawString("Touch the center of the cross", width / 2, height / 2, 2);
#else // _ADAFRUIT_GFX_H or _ARDUINO_TFT_H_
    tft->setCursor((width - 30 * 12) / 2, (height - 16) / 2); // font size: 12x16
    tft->print("Touch the center of the cross");
#endif

    while (touched()); // wait for no touch

    tft->drawFastHLine(10, 20, 20, color_fg);
    tft->drawFastVLine(20, 10, 20, color_fg);

    while (!touched());
    delay(50); // wait for touch being stable

    SCREEN_POINT p;
    do {
      p = getPoint();
    } while (SP_RAW_Z(p) < Z_THRESHOLD);

    cal[0] = SP_RAW_X(p);
    cal[1] = SP_RAW_Y(p);
    tft->drawFastHLine(10, 20, 20, color_bg);
    tft->drawFastVLine(20, 10, 20, color_bg);

    delay(500);
    while (touched()); // wait for no touch

    tft->drawFastHLine(width - 30, height - 20, 20, color_fg);
    tft->drawFastVLine(width - 20, height - 30, 20, color_fg);

    while (!touched());
    delay(50); // wait for touch being stable

    do {
      p = getPoint();
    } while (SP_RAW_Z(p) < Z_THRESHOLD);

    cal[2] = SP_RAW_X(p);
    cal[3] = SP_RAW_Y(p);
    tft->drawFastHLine(width - 30, height - 20, 20, color_bg);
    tft->drawFastVLine(width - 20, height - 30, 20, color_bg);

    cal[4] = rotation;
    calibrated = true;
  }
};

#endif // _XPT2046_SCREENPOINT_H_
