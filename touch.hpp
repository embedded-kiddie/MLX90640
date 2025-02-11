/*================================================================================
 * Touch event manager
 *================================================================================*/

/*--------------------------------------------------------------------------------
 * Configuration data for calibration of touch panel
 *--------------------------------------------------------------------------------*/
typedef struct TouchConfig {
  // Member variables
  uint16_t  cal[8];
  int8_t    offset[2];

  // Comparison operator
  bool operator >= (TouchConfig &RHS) {
    return !bcmp(cal, RHS.cal, sizeof(cal));
  }
  bool operator <= (TouchConfig &RHS) {
    return (offset[0] != RHS.offset[0]) || (offset[1] != RHS.offset[1]);
  }
  bool operator != (TouchConfig &RHS) {
    return bcmp(cal, RHS.cal, sizeof(cal)) || (offset[0] != RHS.offset[0]) || (offset[1] != RHS.offset[1]);
  }
} TouchConfig_t;

/*--------------------------------------------------------------------------------
 * Touch panel calibration parameters
 * When 'USE_PREFERENCES' is 'false', the calibration result must be embedded.
 *--------------------------------------------------------------------------------*/
TouchConfig_t tch_cnf = {
#if   defined (LOVYANGFX_HPP_)

  // LovyanGFX
  .cal = { 0, 0, 0, 0, 0, 0, 0, 0 },

#elif defined (_XPT2046_Touchscreen_h_)

  // XPT2046_Touchscreen
  .cal = { 0, 0, 0, 0, 0, },

#else

  // TFT_eSPI
  .cal = { 0, 0, 0, 0, 0, },

#endif

  .offset = { 0, },
};

/*--------------------------------------------------------------------------------
 * Event definition
 *--------------------------------------------------------------------------------*/
#define PERIOD_DEBOUNCE      25 // [msec]
#define PERIOD_TOUCHED       50 // [msec]
#define PERIOD_TAP2         200 // [msec]
#define PERIOD_CLEAR_EVENT  100 // [msec]

typedef enum {
  EVENT_NONE    = (0x00), // considered as 'HIGH'
  EVENT_RISING  = (0x01), //   touch --> untouch
  EVENT_FALLING = (0x02), // untouch -->   touch
  EVENT_TOUCHED = (0x04), //   touch -->   touch
  EVENT_TAP2    = (0x08), // double tap
  EVENT_EXPAND  = (0x10), // not a touch event, but define for convenience

  // alias
  EVENT_INIT    = (EVENT_NONE),
  EVENT_UP      = (EVENT_RISING),
  EVENT_DOWN    = (EVENT_FALLING),
  EVENT_DRAG    = (EVENT_FALLING | EVENT_TOUCHED),
  EVENT_TAP     = (EVENT_FALLING | EVENT_RISING),
  EVENT_CLICK   = (EVENT_FALLING | EVENT_RISING),
  EVENT_CHANGE  = (EVENT_FALLING | EVENT_RISING),
  EVENT_SELECT  = (EVENT_FALLING | EVENT_TAP2),
  EVENT_ALL     = (EVENT_FALLING | EVENT_RISING | EVENT_TOUCHED),
  EVENT_WATCH   = (EVENT_EXPAND), // watch events on a specific screen
  EVENT_SHOW    = (EVENT_EXPAND), // show something on a specific screen
} Event_t;

typedef struct Touch {
  Event_t     event;  // Detected event
  uint16_t    x, y;   // The coordinates where the event fired
} Touch_t;

/*--------------------------------------------------------------------------------
 * Function prototyping
 *--------------------------------------------------------------------------------*/
bool touch_setup(void);
bool touch_event(Touch_t &touch);
void touch_clear(void);
void touch_calibrate(TouchConfig_t *config);
bool touch_save(TouchConfig_t *config);
bool touch_load(TouchConfig_t *config);

/*--------------------------------------------------------------------------------
 * Simple touch point correction
 *--------------------------------------------------------------------------------*/
extern uint16_t lcd_width;
extern uint16_t lcd_height;

/*--------------------------------------------------------------------------------
 * Instance of XPT2046_Touchscreen library
 *--------------------------------------------------------------------------------*/
#if defined (_XPT2046_Touchscreen_h_)
#include "boards/XPT2046_ScreenPoint.h"
static SPIClass sp_spi = SPIClass(TOUCH_SPI_BUS);
static XPT2046_ScreenPoint sp(TOUCH_CS, TOUCH_IRQ);
#endif

/*--------------------------------------------------------------------------------
 * Setup touch manager
 *--------------------------------------------------------------------------------*/
bool touch_setup(void) {

#if defined (_XPT2046_Touchscreen_h_)
  sp_spi.begin(TOUCH_CLK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS);
  sp.begin(sp_spi, lcd_width, lcd_height, SCREEN_ROTATION);
#endif

#if USE_PREFERENCES
  // Load calibration parameters from FLASH
  if (touch_load(&tch_cnf) == false) {
    touch_calibrate(&tch_cnf);
    touch_save(&tch_cnf);
  }
#else
  if (tch_cnf.cal[0] == 0) {
    touch_calibrate(&tch_cnf);
  }
#endif

#if   defined (LOVYANGFX_HPP_)

  // https://github.com/lovyan03/LovyanGFX/discussions/539
  GFX_EXEC(setTouchCalibrate(tch_cnf.cal));
  return true;

#elif defined (_XPT2046_Touchscreen_h_)

  sp.setTouch(static_cast<const uint16_t*>(tch_cnf.cal));
  return true;

#else // defined (_TFT_eSPIH_)

  // https://github.com/Bodmer/TFT_eSPI/tree/master/examples/Generic/Touch_calibrate
  GFX_EXEC(setTouch(tch_cnf.cal));
  return true;

#endif
}

/*--------------------------------------------------------------------------------
 * Touch event manager
 *--------------------------------------------------------------------------------*/
bool touch_event(Touch_t &touch) {
  uint32_t time = millis();
  static uint32_t prev_time;
  static uint16_t x, y;
  static bool prev_stat;
  static uint8_t count;
  Event_t event = EVENT_NONE;

#if defined (_XPT2046_Touchscreen_h_)

  bool stat = sp.getTouch(&x, &y);

#else // LovyanGFX || TFT_eSPI

  bool stat = GFX_EXEC(getTouch(&x, &y));

#endif // _XPT2046_Touchscreen_h_

  // when state changes, check again after a certain period of time
  uint32_t dt = time - prev_time;
  if (stat != prev_stat) {
    if (dt < PERIOD_DEBOUNCE) {
      return false;
    } else {
      // update the time when state changes
      prev_time = time;

      // reset double tap counter
      if (dt > PERIOD_TAP2 || count >= 4) {
        count = 0;
      }
    }
  }

  // untouch --> touch
  if (prev_stat == false && stat == true) {
    event = EVENT_FALLING;
    count = (count == 0 ? count + 1 : (dt <= PERIOD_TAP2 ? count + 1 : 0));
  } else

  // touch --> untouch
  if (prev_stat == true && stat == false) {
    event = EVENT_RISING;
    count = dt <= PERIOD_TAP2 ? count + 1 : 0;
  } else

  // touch --> touch
  if (prev_stat == true && stat == true) {
    event = EVENT_TOUCHED;
  }

  // update state
  prev_stat = stat;

  if (event != EVENT_NONE) {
    if (stat) {
        x += tch_cnf.offset[0];
        y += tch_cnf.offset[1];
        x = constrain(x, 0, lcd_width  - 1);
        y = constrain(y, 0, lcd_height - 1);
    }

    // TAP2 = FALLING --> RISING --> FALLING --> RISING
    touch.event = (Event_t)(event | (count >= 4 ? EVENT_TAP2 : EVENT_NONE));
    touch.x = x;
    touch.y = y;

//  DBG_EXEC(printf("event: %d, x: %d, y: %d, dt: %d, count: %d\n", touch.event, x, y, dt, count));
    return true;
  }

  return false;
}

/*--------------------------------------------------------------------------------
 * Wait until there are no more touch events
 *--------------------------------------------------------------------------------*/
void touch_clear(void) {
  Touch_t touch;
  delay(PERIOD_CLEAR_EVENT);
  while(touch_event(touch));
}

/*--------------------------------------------------------------------------------
 * Calibrating the touch panel
 *--------------------------------------------------------------------------------*/
void touch_calibrate(TouchConfig_t *config) {
  if (!Serial) {
    Serial.begin(115200);
  }

#if   defined (_XPT2046_Touchscreen_h_)

  extern TFT_eSPI tft;
  sp.calibrateTouch(config->cal, &tft, TFT_WHITE, TFT_BLACK);

  printf("\n// XPT2046\n");
  printf(".cal = { ");
  for (int i = 0; i < 5; ++i) {
    printf("%d", config->cal[i]);
    printf(i < 4 ? ", " : ", 0, },\n");
  }

#elif defined (LOVYANGFX_HPP_)

  // https://github.com/lovyan03/LovyanGFX/tree/master/examples/HowToUse/2_user_setting
  GFX_EXEC(clear(0));
  GFX_EXEC(setTextSize(2));
  GFX_EXEC(setTextDatum(textdatum_t::middle_center));
  GFX_EXEC(drawString("touch the arrow marker.", lcd_width >> 1, lcd_height >> 1));
  GFX_EXEC(setTextDatum(textdatum_t::top_left));
  GFX_EXEC(calibrateTouch(config->cal, TFT_WHITE, TFT_BLACK, std::max(lcd_width, lcd_height) >> 3));

  printf("\n// LovyanGFX\n");
  printf(".cal = { ");
  for (int i = 0; i < 8; ++i) {
    printf("%d", config->cal[i]);
    printf(i < 7 ? ", " : " },\n");
  }

#elif defined (_TFT_eSPIH_)

  // https://github.com/Bodmer/TFT_eSPI/tree/master/examples/Generic/Touch_calibrate
  GFX_EXEC(fillScreen(TFT_BLACK));
  GFX_EXEC(setCursor(20, 20));
  GFX_EXEC(setTextSize(2));
  GFX_EXEC(setTextColor(TFT_WHITE, TFT_BLACK));
  GFX_EXEC(println("Touch corners in order"));
  GFX_EXEC(calibrateTouch(config->cal, TFT_MAGENTA, TFT_BLACK, 15));

  printf("\n// TFT_eSPI\n");
  printf(".cal = { ");
  for (int i = 0; i < 5; ++i) {
    printf("%d", config->cal[i]);
    printf(i < 4 ? ", " : ", 0, },\n");
  }

#endif

  // clear offset
  config->offset[0] = config->offset[1] = 0;
}

/*--------------------------------------------------------------------------------
 * Saving and loading calibration configurations using preferences.h
 * https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/preferences.html
 * https://github.com/espressif/arduino-esp32/tree/master/libraries/Preferences
 *--------------------------------------------------------------------------------*/
#include <Preferences.h>

#define RW_MODE   false
#define RO_MODE   true
#define INIT_KEY  "initialized"

#if     defined   (_XPT2046_Touchscreen_h_)
#define PREF_KEY  "XPT2046"
#elif   defined   (LOVYANGFX_HPP_)
#define PREF_KEY  "LovyanGFX"
#elif   defined   (_TFT_eSPIH_)
#define PREF_KEY  "TFT_eSPI"
#endif

bool touch_save(TouchConfig_t *config) {
#if USE_PREFERENCES
  Preferences touchPref;

  if (touchPref.begin(PREF_KEY, RW_MODE) == false) {
    DBG_EXEC(printf("Preferences: begin(%s) failed.\n", PREF_KEY));
    return false;
  } else {
    DBG_EXEC(printf("Preferences: found %s.\n", PREF_KEY));
  }

  if (touchPref.putBytes("cal", static_cast<const void*>(config->cal), sizeof(config->cal)) != sizeof(config->cal)) {
    DBG_EXEC(printf("Preferences: putBytes(cal) failed.\n"));
    touchPref.end();
    return false;
  }

  if (touchPref.putBytes("offset", static_cast<const void*>(config->offset), sizeof(config->offset)) != sizeof(config->offset)) {
    DBG_EXEC(printf("Preferences: putBytes(offset) failed.\n"));
    touchPref.end();
    return false;
  }

  if (touchPref.putBool(INIT_KEY, true) != sizeof(true)) {
    DBG_EXEC(printf("Preferences: putBool(%s) failed.\n", INIT_KEY));
    touchPref.end();
    return false;
  }

  touchPref.end();
#endif
  return true;
}

bool touch_load(TouchConfig_t *config) {
#if USE_PREFERENCES
  Preferences touchPref;
  TouchConfig_t c;

  if (touchPref.begin(PREF_KEY, RO_MODE) == false) {
    DBG_EXEC(printf("Preferences: %s does not exist.\n", PREF_KEY));
    return false;
  }

  // Check if it already exists
  if (touchPref.isKey(INIT_KEY) == false) {
    DBG_EXEC(printf("Preferences: %s does not exist.\n", INIT_KEY));
    touchPref.end();
    return false;
  }

  if (touchPref.getBytes("cal", static_cast<void*>(&c.cal), sizeof(c.cal)) != sizeof(c.cal)) {
    DBG_EXEC(printf("Preferences: getBytes(cal) failed.\n"));
    touchPref.end();
    return false;
  }

  if (touchPref.getBytes("offset", static_cast<void*>(&c.offset), sizeof(c.offset)) != sizeof(c.offset)) {
    DBG_EXEC(printf("Preferences: getBytes(offset) failed.\n"));
    touchPref.end();
    return false;
  }

  *config = c;
  touchPref.end();
#endif
  return true;
}
