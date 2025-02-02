/*================================================================================
 * I2C / SPI pin assign
 * https://github.com/espressif/arduino-esp32/tree/master/variants
 *================================================================================*/
#ifndef _PIN_ASSIGN_H_
#define _PIN_ASSIGN_H_

/*--------------------------------------------------------------------------------
 * I2C pin assignment
 *--------------------------------------------------------------------------------*/
// variants/jczn_2432s028r/pins_arduino.h
#if defined (ARDUINO_ESP32_2432S028R)
#define MLX_I2C_SDA D27 // GPIO27 on CN1
#define MLX_I2C_SCL SCL // GPIO22 on CN1
#endif

/*--------------------------------------------------------------------------------
 * Display panel resolution (defalult orientation: portrait)
 *--------------------------------------------------------------------------------*/
#define TFT_WIDTH     240
#define TFT_HEIGHT    320

/*--------------------------------------------------------------------------------
 * SPI pin assignment
 *--------------------------------------------------------------------------------*/
/**************************************************/
/***** variants/jczn_2432s028r/pins_arduino.h *****/
/**************************************************/
#if defined (ARDUINO_ESP32_2432S028R)
#define TFT_DC        CYD_TFT_DC      //  2
#define TFT_MISO      CYD_TFT_MISO    // 12
#define TFT_MOSI      CYD_TFT_MOSI    // 13
#define TFT_SCLK      CYD_TFT_SCK     // 14
#define TFT_CS        CYD_TFT_CS      // 15
#define TFT_RST       -1              //  2 (CYD_TFT_RS = CYD_TFT_DC)
#define TFT_BL        CYD_TFT_BL      // 21
#define TFT_SPI_BUS   CYD_TFT_SPI_BUS // HSPI

#define TOUCH_IRQ     CYD_TP_IRQ      // 36
#define TOUCH_MOSI    CYD_TP_MOSI     // 32
#define TOUCH_MISO    CYD_TP_MISO     // 39
#define TOUCH_CLK     CYD_TP_CLK      // 25
#define TOUCH_CS      CYD_TP_CS       // 33
#define TOUCH_SPI_BUS -1              // CYD_TP_SPI_BUS (VSPI)

#define SD_CS         CYD_SD_SS       //  5
#define SD_MOSI       CYD_SD_MOSI     // 23
#define SD_MISO       CYD_SD_MISO     // 19
#define SD_SCK        CYD_SD_SCK      // 18
#define SD_SPI_BUS    CYD_SD_SPI_BUS  // VSPI
#define SPI_MODE      SPI_MODE0       // SPI_MODE0, SPI_MODE2 or SPI_MODE3

// false: Panel driver: ILI9341 (micro-USB x 1 type)
// true : Panel driver: ST7789  (micro-USB x 1 + USB-C x 1 type)
#if DISPLAY_CYD_2USB
#define SPI_FREQUENCY       80000000  // 80 MHz (ST7789)
#else
#define SPI_FREQUENCY       40000000  // 40 MHz (ILI9341)
#endif
#define SPI_SD_FREQUENCY    50000000  // 50 MHz
#define SPI_READ_FREQUENCY  16000000  // 16 MHz
#define SPI_TOUCH_FREQUENCY  1000000  //  1 MHz

/************************************************/
/***** variants/XIAO_ESP32S3/pins_arduino.h *****/
/************************************************/
#elif defined (ARDUINO_XIAO_ESP32S3)
#define TFT_SCLK      SCK       // D8  = 7
#define TFT_MISO      MISO      // D9  = 8
#define TFT_MOSI      MOSI      // D10 = 9
#define TFT_CS        D2        //  3
#define TFT_RST       D1        //  2
#define TFT_DC        D0        //  1
#define TOUCH_CS      D3        //  4
#define TOUCH_IRQ     D7        // 44
#define SD_CS         D6        // 43
#define SPI_MODE      SPI_MODE3 // SPI_MODE0, SPI_MODE2 or SPI_MODE3

#define SPI_FREQUENCY       80000000  // 80 MHz
#define SPI_SD_FREQUENCY    50000000  // 50 MHz
#define SPI_READ_FREQUENCY   4000000  //  4 MHz
#define SPI_TOUCH_FREQUENCY   250000  // 250KHz

#else
#error Pin assignment required depending on your board
#endif

#endif // _PIN_ASSIGN_H_
