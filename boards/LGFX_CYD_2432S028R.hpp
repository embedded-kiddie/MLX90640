#pragma once

#define LGFX_USE_V1

#include <LovyanGFX.hpp>

// Example of settings for LovyanGFX on ESP32
// https://github.com/espressif/arduino-esp32/blob/master/variants/jczn_2432s028r/pins_arduino.h

/*
Copy this file, give it a new name, and change the settings to suit your environment.
The created file can be used by including it in your user program.

The new file can also be placed in the "lgfx_user" folder of the library,
but please note that in this case it may be deleted when the library is updated.

To operate safely, make a backup or place it in the user project folder.
//*/

// false: Panel driver: ILI9341 (micro-USB x 1 type)
// true : Panel driver: ST7789  (micro-USB x 1 + USB-C x 1 type)
#ifndef DISPLAY_CYD_2USB
#error DISPLAY_CYD_2USB should be defined.
#endif

/// Create a class for your own settings by deriving from LGFX_Device.
class LGFX : public lgfx::LGFX_Device
{
/*
You can change the class name from "LGFX" to a different name.
When using with AUTODETECT, "LGFX" is used, so change it to a name other than LGFX.
Also, if you are using multiple panels at the same time, give each panel a different name.
If you change the class name, you must also change the constructor name to the same name.

You can name it whatever you like, but in case the number of settings increases,
for example, if you are setting up an SPI-connected ILI9341 with ESP32 DevKit-C, 
you can name it like LGFX_DevKitC_SPI_ILI9341 to match the file name and class name, 
which will make it less confusing when using it.
//*/

// Select an instance that matches the type of panel.
//lgfx::Panel_GC9A01      _panel_instance;
//lgfx::Panel_GDEW0154M09 _panel_instance;
//lgfx::Panel_HX8357B     _panel_instance;
//lgfx::Panel_HX8357D     _panel_instance;
//lgfx::Panel_ILI9163     _panel_instance;
//lgfx::Panel_ILI9341     _panel_instance;
//lgfx::Panel_ILI9342     _panel_instance;
//lgfx::Panel_ILI9481     _panel_instance;
//lgfx::Panel_ILI9486     _panel_instance;
//lgfx::Panel_ILI9488     _panel_instance;
//lgfx::Panel_IT8951      _panel_instance;
//lgfx::Panel_RA8875      _panel_instance;
//lgfx::Panel_SH110x      _panel_instance; // SH1106, SH1107
//lgfx::Panel_SSD1306     _panel_instance;
//lgfx::Panel_SSD1327     _panel_instance;
//lgfx::Panel_SSD1331     _panel_instance;
//lgfx::Panel_SSD1351     _panel_instance; // SSD1351, SSD1357
//lgfx::Panel_SSD1963     _panel_instance;
//lgfx::Panel_ST7735      _panel_instance;
//lgfx::Panel_ST7735S     _panel_instance;
//lgfx::Panel_ST7789      _panel_instance;
//lgfx::Panel_ST7796      _panel_instance;

#if DISPLAY_CYD_2USB
  lgfx::Panel_ST7789      _panel_instance;
#else
  lgfx::Panel_ILI9341     _panel_instance;
#endif

// Select an instance that matches the type of bus for your panel.
  lgfx::Bus_SPI       _bus_instance;   // SPI bus instance
//lgfx::Bus_I2C       _bus_instance;   // I2C bus instance (ESP32 only)
//lgfx::Bus_Parallel8 _bus_instance;   // 8-bit parallel bus instance (ESP32 only)

// If backlight control is possible, set an instance. (Delete if not needed)
  lgfx::Light_PWM     _light_instance;

// Select an instance that matches the touch screen type. (Delete if not needed)
//lgfx::Touch_CST816S          _touch_instance;
//lgfx::Touch_FT5x06           _touch_instance; // FT5206, FT5306, FT5406, FT6206, FT6236, FT6336, FT6436
//lgfx::Touch_GSL1680E_800x480 _touch_instance; // GSL_1680E, 1688E, 2681B, 2682B
//lgfx::Touch_GSL1680F_800x480 _touch_instance;
//lgfx::Touch_GSL1680F_480x272 _touch_instance;
//lgfx::Touch_GSLx680_320x320  _touch_instance;
//lgfx::Touch_GT911            _touch_instance;
//lgfx::Touch_STMPE610         _touch_instance;
//lgfx::Touch_TT21xxx          _touch_instance; // TT21100
  lgfx::Touch_XPT2046          _touch_instance;

public:

  // Create a constructor and set various settings here.
  // When you change the class name, specify the same name for the constructor.
  LGFX(void)
  {
    { //Sets the bus control.
      auto cfg = _bus_instance.config();    // Get the bus configuration structure.

// SPI bus settings
      cfg.spi_host = HSPI_HOST;     // Select the SPI (ESP32-S2,C3: SPI2_HOST or SPI3_HOST / ESP32: VSPI_HOST or HSPI_HOST)
      // Due to the ESP-IDF version upgrade, the VSPI_HOST and HSPI_HOST are deprecated, so if an error occurs, use SPI2_HOST and SPI3_HOST instead.
      cfg.spi_mode = 0;             // SPI communication mode (0 to 3)
#if DISPLAY_CYD_2USB
      cfg.freq_write = 80000000;    // SPI clock for transmit (Maximum 80MHz, rounded to an integer value of 80MHz)
#else
      cfg.freq_write = 40000000;    // SPI clock for transmit (Maximum 80MHz, rounded to an integer value of 80MHz)
#endif
      cfg.freq_read  = 16000000;    // SPI clock for receive
      cfg.spi_3wire  = false;       // Set to true if receive on the MOSI pin
      cfg.use_lock   = true;        // Set to true if transaction lock is used
      cfg.dma_channel = SPI_DMA_CH_AUTO; // Set the DMA channel (0=DMA not used / 1=1ch / 2=2ch / SPI_DMA_CH_AUTO=automatic)
      // Due to the ESP-IDF version upgrade, SPI_DMA_CH_AUTO is recommended. Specifying 1ch or 2ch is no longer recommended.
      cfg.pin_sclk = CYD_TFT_SCK;   // Set the SPI SCLK pin
      cfg.pin_mosi = CYD_TFT_MOSI;  // Set the SPI MOSI pin
      cfg.pin_miso = CYD_TFT_MISO;  // Set the SPI MISO pin (-1 = disable)
      cfg.pin_dc   = CYD_TFT_DC;    // Set the SPI D/C  pin (-1 = disable)
     // When you use a common SPI bus with the SD card, be sure to set MISO and do not omit it.
//*/
/*
// I2C bus settings
      cfg.i2c_port    = 0;          // Select the I2C port to use (0 or 1)
      cfg.freq_write  = 400000;     // Clock for transmit
      cfg.freq_read   = 400000;     // Clock for receive
      cfg.pin_sda     = 21;         // SDA pin number
      cfg.pin_scl     = 22;         // SCL pin number
      cfg.i2c_addr    = 0x3C;       // I2C device address
//*/
/*
// 8-bit parallel bus settings
      cfg.i2s_port = I2S_NUM_0;     // Select the I2S port to use (I2S_NUM_0 or I2S_NUM_1) (Use the I2S LCD mode of ESP32)
      cfg.freq_write = 20000000;    // Clock for transmit (Maximum 20MHz, rounded to an integer division of 80MHz)
      cfg.pin_wr =  4;              // WR pin number
      cfg.pin_rd =  2;              // RD pin number
      cfg.pin_rs = 15;              // RS(D/C) pin number
      cfg.pin_d0 = 12;              // D0 pin number
      cfg.pin_d1 = 13;              // D1 pin number
      cfg.pin_d2 = 26;              // D2 pin number
      cfg.pin_d3 = 25;              // D3 pin number
      cfg.pin_d4 = 17;              // D4 pin number
      cfg.pin_d5 = 16;              // D5 pin number
      cfg.pin_d6 = 27;              // D6 pin number
      cfg.pin_d7 = 14;              // D7 pin number
//*/

      _bus_instance.config(cfg);    // Configure setting values in the bus.
      _panel_instance.setBus(&_bus_instance); // Set the bus on the panel.
    }

    { // Configure the display panel control settings.
      auto cfg = _panel_instance.config();    // Get the structure for display panel settings.

      cfg.pin_cs           = CYD_TFT_CS;  // CS   pin number (-1 = disable)
      cfg.pin_rst          = -1;          // RST  pin number (-1 = disable) CYD_TFT_RS = CYD_TFT_CS, RESET is connected to board RST
      cfg.pin_busy         = -1;          // BUSY pin number (-1 = disable)

      // The following are set to general values, so try commenting out if you are unsure of.

      cfg.panel_width      =   240;  // Panel width
      cfg.panel_height     =   320;  // Panel height
      cfg.offset_x         =     0;  // Panel offset in X direction
      cfg.offset_y         =     0;  // Panel offset in Y direction
#if DISPLAY_CYD_2USB
      cfg.offset_rotation  =     0;  // Rotation direction offset 0~7 (4~7 are upside down)
      cfg.dummy_read_pixel =    16;  // Number of dummy read bits before pixel read
#else
      cfg.offset_rotation  =     2;  // Rotation direction offset 0~7 (4~7 are upside down)
      cfg.dummy_read_pixel =     8;  // Number of dummy read bits before pixel read
#endif
      cfg.dummy_read_bits  =     1;  // Number of dummy read bits before reading non-pixel data
      cfg.readable         =  true;  // Set to true if data can be read
      cfg.invert           = false;  // Set to true if the panel is inverted
      cfg.rgb_order        = false;  // Set to true if the red and blue of the panel are swapped
      cfg.dlen_16bit       = false;  // Set to true if the panel transmit data in 16-bit via 16-bit parallel or SPI
      cfg.bus_shared       = false;  // Set to true if the bus is shared with the SD card (The bus is controlled for drawJpg etc.)

// Set the following only if your display is misaligned, such as ST7735 or ILI9163, which have variable pixel counts.
      cfg.memory_width     =   240;  // Maximum width  supported by driver IC
      cfg.memory_height    =   320;  // Maximum height supported by driver IC

      _panel_instance.config(cfg);
    }

//*
    { // Set the backlight control. (Delete if not required)
      auto cfg = _light_instance.config();    // Get the backlight setting structure

      cfg.pin_bl = CYD_TFT_BL;      // Backlight pin number
      cfg.invert = false;           // Set to true if the backlight brightness is inverted
      cfg.freq   = 12000;           // Backlight PWM frequency
      cfg.pwm_channel = 7;          // The PWM channel number

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);  // Set the backlight on the panel.
    }
//*/

//
    { // Configure touch screen control (delete if not needed)
      auto cfg = _touch_instance.config();

      cfg.x_min =  240;         // Minimum X value (raw value) from touch screen
      cfg.x_max = 3800;         // Maximum X value (raw value) from touch screen
      cfg.y_min = 3700;         // Minimum Y value (raw value) from touch screen
      cfg.y_max =  200;         // Maximum Y value (raw value) from touch screen
      cfg.pin_int = CYD_TP_IRQ; // Interrupt pin number
      cfg.bus_shared = false;   // Set to true if the bus shared with the screen
#if DISPLAY_CYD_2USB
      cfg.offset_rotation = 2;  // Adjust when display and touch orientation do not match (0~7)
#else
      cfg.offset_rotation = 0;  // Adjust when display and touch orientation do not match (0~7)
#endif

// For SPI connection
      cfg.spi_host = -1;            // Select the SPI (HSPI_HOST or VSPI_HOST) or only XPT2046 can be set to -1.
      cfg.freq = 1000000;           // Set the SPI clock
      cfg.pin_sclk = CYD_TP_CLK;    // SCLK pin number
      cfg.pin_mosi = CYD_TP_MOSI;   // MOSI pin number
      cfg.pin_miso = CYD_TP_MISO;   // MISO pin number
      cfg.pin_cs   = CYD_TP_CS;     // CS   pin number

// For I2C connection
      //cfg.i2c_port = 1;      // Select the I2C (0 or 1)
      //cfg.i2c_addr = 0x38;   // I2C device addres
      //cfg.pin_sda  = 23;     // SDA pin number
      //cfg.pin_scl  = 32;     // SCL pin number
      //cfg.freq = 400000;     // Set the I2C clock

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);  // Set the touch screen on the panel.
    }
//*/

    setPanel(&_panel_instance); // Set the panel to be used.
  }
};
