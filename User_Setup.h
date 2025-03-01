/*================================================================================
 * TFT_eSPI configuration
 * https://github.com/espressif/arduino-esp32/tree/master/variants
 *================================================================================*/
#include "./pin_assign.h"

#if defined (ARDUINO_ESP32_2432S028R)
/**************************************************/
/***** variants/jczn_2432s028r/pins_arduino.h *****/
/**************************************************/
// false: Panel driver: ILI9341 (micro-USB x 1 type)
// true : Panel driver: ST7789  (micro-USB x 1 + USB-C x 1 type)
#define DISPLAY_CYD_2USB  true
#include "./boards/Setup_ESP32_2432S028R_CYD.h"

#elif defined (ARDUINO_XIAO_ESP32S3)
/************************************************/
/***** variants/XIAO_ESP32S3/pins_arduino.h *****/
/************************************************/
#include "./boards/Setup_XIAO_ESP32S3_ST7789.h"
#endif
