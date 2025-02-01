/*================================================================================
 * Thermograpy camera
 * Copyright (c) 2024 - 2025 embedded-kiddie
 * Released under the MIT license
 * https://opensource.org/license/mit
 *================================================================================*/
#include <Arduino.h>
#include "pin_assign.h"

/*--------------------------------------------------------------------------------
 * Step 1: Configure serial output for debugging
 *--------------------------------------------------------------------------------*/
#define DEBUG false
#if     DEBUG
#define DBG_EXEC(x) x
#else
#define DBG_EXEC(x)
#endif

/*--------------------------------------------------------------------------------
 * Step 2: Configure Operational settings
 *--------------------------------------------------------------------------------*/
#define ENA_INTERPOLATION   true  // Enable interpolation
#define ENA_MULTITASKING    true  // Enable multi-task by FreeRTOS
#define ENA_OUTWARD_CAMERA  false // Camera orientation (true: Outward, false: Selfie)

/*--------------------------------------------------------------------------------
 * Step 3: Select GFX Library
 *--------------------------------------------------------------------------------*/
#if   1
/*--------------------------------------------------------------------------------
 * LovyanGFX
 * Note: Currently 'AUTODETECT' only works for 'ESP32 2432S028R'.
 * For other boards you need to configure appropriately to fit your device.
 * See https://github.com/lovyan03/LovyanGFX/blob/master/src/lgfx/boards.hpp
 *--------------------------------------------------------------------------------*/
#define USE_LOVYAN_GFX
#define USE_AUTODETECT  true

#else
/*--------------------------------------------------------------------------------
 * TFT_eSPI
 * It does not allow the display and touch screen to be on different SPI buses.
 *--------------------------------------------------------------------------------*/
#define USE_TFT_ESPI
#ifdef  CYD_TFT_CS
#error Not yet supported  // CYD needs XPT2046_Touchscreen library
#endif
#endif

/*--------------------------------------------------------------------------------
 * Step 4: Configure flash memory setting to save touch calibration data
 *--------------------------------------------------------------------------------*/
#define USE_PREFERENCES false

/*--------------------------------------------------------------------------------
 * Configure MLX90640 settings
 *--------------------------------------------------------------------------------*/
#if ENA_INTERPOLATION
#define INTERPOLATE_SCALE 8
#define BOX_SIZE          1
#define REFRESH_RATE      (ENA_MULTITASKING ? MLX90640_32_HZ : MLX90640_16_HZ)
#else
#define INTERPOLATE_SCALE 1
#define BOX_SIZE          8
#define REFRESH_RATE      (ENA_MULTITASKING ? MLX90640_32_HZ : MLX90640_16_HZ)
#endif

/*--------------------------------------------------------------------------------
 * Heat map
 *--------------------------------------------------------------------------------*/
#define N_HEATMAP   512 // 256 or 512 --> heatmap.h
constexpr uint16_t heatmap[2][N_HEATMAP] = {
#include "heatmap.h"    // 0; Rainbow, 1: Inferno
};

/*--------------------------------------------------------------------------------
 * Load modules
 *--------------------------------------------------------------------------------*/
#include "mlx.hpp"
#include "gfx.hpp"
#include "task.hpp"
#include "touch.hpp"
#include "sdcard.hpp"
#include "filter.hpp"
#include "interpolation.hpp"
#include "widget.hpp"

/*--------------------------------------------------------------------------------
 * Input process - Get thermal image from MLX90640
 *--------------------------------------------------------------------------------*/
void ProcessInput(uint8_t bank) {
  // Refresh MLX90640 when refresh rate changes
  bool refresh = mlx_refresh();

  // Get new image
  if (mlx.getFrame(src[bank]) != 0) {
    gfx_printf(TFT_WIDTH / 2 - FONT_WIDTH * 2, TFT_HEIGHT / 2 - FONT_HEIGHT * 4, "Failed");
    DBG_EXEC(printf("Failed\n"));
    delay(1000); // false = no new frame capture
  }

  if (refresh) {
    // Avoid cluttered image
    for (int i = 0; i < MLX90640_COLS * MLX90640_ROWS; ++i) {
      src[bank][i] = (float)mlx_cnf.range_min;
    }
  } else {
    // Measure temperature of min/max/pickup
    filter_temperature(src[bank]);
  }
}

/*--------------------------------------------------------------------------------
 * Output process - Interpolate thermal image and display on LCD.
 *--------------------------------------------------------------------------------*/
void ProcessOutput(uint8_t bank, uint32_t inputStart, uint32_t inputFinish) {
  // Widget controller
  State_t state = widget_control();
  if (bank != NOT_UPDATED && (state == STATE_MAIN || state == STATE_THERMOGRAPH)) {
    static uint32_t outputFinish, outputPeriod, prevStart;
    uint32_t outputStart = millis();

    const int dst_rows = mlx_cnf.interpolation * MLX90640_ROWS;
    const int dst_cols = mlx_cnf.interpolation * MLX90640_COLS;
    const int box_size = mlx_cnf.box_size;

    GFX_EXEC(startWrite());
    GFX_FAST(createSprite(dst_cols * box_size, dst_rows * box_size));

    interpolate_image(src[bank], MLX90640_ROWS, MLX90640_COLS, dst_rows, dst_cols);

    if (mlx_cnf.marker_mode) {
      static uint32_t prevUpdate;
      if (outputStart - prevUpdate > 1000) {
        prevUpdate = outputStart;
        filter_update();
      }
      DrawTemperatureMarker();
    }

    if (mlx_cnf.range_auto) {
      DrawTemperatureRange(2);
    }

    if (state == STATE_MAIN) {
      gfx_printf(260 + FONT_WIDTH, LINE_HEIGHT * 3.5, "%4d", inputFinish - inputStart); // input processing time
      gfx_printf(260 + FONT_WIDTH, LINE_HEIGHT * 5.0, "%4d", outputPeriod); // output processing time

      float v = 1000.0f / (float)(outputStart - prevStart);
      gfx_printf(260 + FONT_WIDTH, LINE_HEIGHT * 2.0, "%4.1f", v); // FPS

      v = mlx.getTa(false);
      if (0.0f < v && v < 100.0f) {
        gfx_printf(260 + FONT_WIDTH, LINE_HEIGHT * 6.5, "%4.1f", v); // Sensor temperature
      }
    }

    GFX_FAST(pushSprite(0, 0));
    GFX_FAST(deleteSprite());
    GFX_EXEC(endWrite());

    // Save video
    if (mlx_cap.recording) {
      sdcard_record((uint8_t*)src[bank], sizeof(src[bank]), mlx_cap.filename);
    }

#if DEBUG
  // Capture screen
  else if (Serial.available()) {
    Serial.readStringUntil('\n');
    sdcard_save();
  }
#endif

    // Update processing time
    prevStart = outputStart;
    outputFinish = millis();
    outputPeriod = outputFinish - outputStart;
  }
}

void setup() {
  // Prevent blocking when the USB cable is not connected to PC
  DBG_EXEC(Serial.begin(115200));
  DBG_EXEC(delay(500));

  // Initialize peripherals
  mlx_setup();
  gfx_setup();
  sdcard_setup();
  touch_setup();
  widget_setup();

  // Initialize interpolation
  interpolate_setup(mlx_cnf.interpolation);

  // Start tasks
#if ENA_MULTITASKING
  task_setup(ProcessInput, ProcessOutput);
#endif
}

void loop() {
#if ENA_MULTITASKING
  delay(1000);
#else
  uint32_t inputStart = millis();
  ProcessInput(0); // always use bank 0
  ProcessOutput(0, inputStart, millis());
#endif
}
