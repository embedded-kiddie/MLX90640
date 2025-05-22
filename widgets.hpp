/*================================================================================
 * Widget definitions for each screen
 *================================================================================*/
#include "widgets.h"

#if     false
#define DBG_FUNC(x) DBG_EXEC(x)
#else
#define DBG_FUNC(x)
#endif

/*--------------------------------------------------------------------------------
 * MLX90640 configuration and Widget control da
 *--------------------------------------------------------------------------------*/
extern MLXConfig_t    mlx_cnf;
extern MLXCapture_t   mlx_cap;
extern TouchConfig_t  tch_cnf;

static MLXConfig_t    mlx_copy;
static MLXConfig_t    cnf_copy;
static TouchConfig_t  tch_copy;
static TouchConfig_t  tch_ajst;

/*--------------------------------------------------------------------------------
 * Image data of widget
 *--------------------------------------------------------------------------------*/
static constexpr Image_t image_main[]               = { { screen_main,          sizeof(screen_main         ) }, }; // 320 x 240
static constexpr Image_t image_configuration[]      = { { screen_configuration, sizeof(screen_configuration) }, }; // 320 x 240
static constexpr Image_t image_resolution[]         = { { screen_resolution,    sizeof(screen_resolution   ) }, }; // 320 x 240
static constexpr Image_t image_thermograph[]        = { { screen_thermograph,   sizeof(screen_thermograph  ) }, }; // 320 x 240
static constexpr Image_t image_file_manager[]       = { { screen_file_manager,  sizeof(screen_file_manager ) }, }; // 320 x 240
static constexpr Image_t image_file_confirm[]       = { { screen_file_confirm,  sizeof(screen_file_confirm ) }, }; // 316 x  82
static constexpr Image_t image_capture_mode[]       = { { screen_capture_mode,  sizeof(screen_capture_mode ) }, }; // 320 x 240
static constexpr Image_t image_calibration[]        = { { screen_calibration,   sizeof(screen_calibration  ) }, }; // 320 x 240
static constexpr Image_t image_adjust_offset[]      = { { screen_adjust_offset, sizeof(screen_adjust_offset) }, }; // 270 x  60
static constexpr Image_t image_information[]        = { { screen_information,   sizeof(screen_information  ) }, }; // 320 x 240
static constexpr Image_t image_icon_configuration[] = { { icon_configuration,   sizeof(icon_configuration  ) }, }; //  50 x  50

static constexpr Image_t image_icon_close[] = {
  { icon_close_on,  sizeof(icon_close_on ) }, // 30 x 30
};
static constexpr Image_t image_icon_apply[] = {
  { icon_apply_off, sizeof(icon_apply_off) }, // 30 x 30
  { icon_apply_on,  sizeof(icon_apply_on ) }, // 30 x 30
};
static constexpr Image_t image_icon_reset[] = {
  { icon_reset_off, sizeof(icon_reset_off) }, // 30 x 30
  { icon_reset_on,  sizeof(icon_reset_on ) }, // 30 x 30
};
static constexpr Image_t image_icon_camera[] = {
  { icon_camera1, sizeof(icon_camera1) }, // 50 x 50
  { icon_camera2, sizeof(icon_camera2) }, // 50 x 50
  { icon_video,   sizeof(icon_video  ) }, // 50 x 50
  { icon_stop,    sizeof(icon_stop   ) }, // 50 x 50
};
static constexpr Image_t image_radio[] = {
  { radio_off, sizeof(radio_off) }, // 26 x 26
  { radio_on,  sizeof(radio_on ) }, // 26 x 26
};
static constexpr Image_t image_toggle[] = {
  { toggle_off, sizeof(toggle_off) }, // 42 x 26
  { toggle_on,  sizeof(toggle_on ) }, // 42 x 26
};
static constexpr Image_t image_arrowL[] = {
  { icon_arrow_Loff, sizeof(icon_arrow_Loff) }, // 22 x 26
  { icon_arrow_Lon,  sizeof(icon_arrow_Lon ) }, // 22 x 26
};
static constexpr Image_t image_arrowR[] = {
  { icon_arrow_Roff, sizeof(icon_arrow_Roff) }, // 22 x 26
  { icon_arrow_Ron,  sizeof(icon_arrow_Ron ) }, // 22 x 26
};
static constexpr Image_t image_arrowU[] = {
  { icon_arrow_Uoff, sizeof(icon_arrow_Uoff) }, // 26 x 26
  { icon_arrow_Uon,  sizeof(icon_arrow_Uon ) }, // 26 x 26
};
static constexpr Image_t image_arrowD[] = {
  { icon_arrow_Doff, sizeof(icon_arrow_Doff) }, // 26 x 26
  { icon_arrow_Don,  sizeof(icon_arrow_Don ) }, // 26 x 26
};
static constexpr Image_t image_checkbox[] = {
  { icon_checkbox_off, sizeof(icon_checkbox_off) }, // 26 x 26
  { icon_checkbox_on,  sizeof(icon_checkbox_on ) }, // 26 x 26
};
static constexpr Image_t image_rewind[] = {
  { icon_player_rewind_off, sizeof(icon_player_rewind_off) }, // 24 x 24
  { icon_player_rewind_on,  sizeof(icon_player_rewind_on ) }, // 24 x 24
};
static constexpr Image_t image_play[] = {
  { icon_player_play_off,  sizeof(icon_player_play_off ) }, // 24 x 24
  { icon_player_play_on,   sizeof(icon_player_play_on  ) }, // 24 x 24
  { icon_player_play_stop, sizeof(icon_player_play_stop) }, // 24 x 24
};
static constexpr Image_t image_prev[] = {
  { icon_player_prev_off, sizeof(icon_player_prev_off) }, // 24 x 24
  { icon_player_prev_on,  sizeof(icon_player_prev_on ) }, // 24 x 24
};
static constexpr Image_t image_next[] = {
  { icon_player_next_off, sizeof(icon_player_next_off) }, // 24 x 24
  { icon_player_next_on,  sizeof(icon_player_next_on ) }, // 24 x 24
};
static constexpr Image_t image_trashbox[] = {
  { icon_trashbox_off, sizeof(icon_trashbox_off) }, // 30 x 30
  { icon_trashbox_on,  sizeof(icon_trashbox_on ) }, // 30 x 30
};
static constexpr Image_t image_save_flash[] = {
  { save_flash_off, sizeof(save_flash_off) }, // 70 x 70
  { save_flash_on,  sizeof(save_flash_on ) }, // 70 x 70
  { save_flash_red, sizeof(save_flash_red) }, // 70 x 70
};
static constexpr Image_t image_target[] = {
  { target_off, sizeof(target_off) }, // 32 x 32
  { target_on,  sizeof(target_on ) }, // 32 x 32
};

/*--------------------------------------------------------------------------------
 * Slider model
 * <------------------ Bar width -------------------->
 *      <- width ->                   <- width ->
 * +---+----------+------------------+----------+---+ ^
 * |   |    ||    |                  |    ||    |   | |
 * |   |   Knob   |                  |   Knob   |   | Bar height = Knob height
 * |   | (pos_min)|                  | (pos_max)|   | | (including background)
 * +---+----------+------------------+----------+---+ v
 * <--->                                        <--->
 * SLIDER_KNOB_OFFSET               SLIDER_KNOB_OFFSET
 * "||" ... The center of the knob (touching point)
 *--------------------------------------------------------------------------------*/
#define SLIDER_KNOB_OFFSET1 6 // Offset from both ends of the bar1
#define SLIDER_KNOB_OFFSET2 4 // Offset from both ends of the bar2

static constexpr Image_t image_slider1[] = {
  { slider_bar1, sizeof(slider_bar1) }, // 160 x 26
  { slider_knob, sizeof(slider_knob) }, //  26 x 26
};
static constexpr Image_t image_slider2[] = {
  { slider_bar2,     sizeof(slider_bar2    ) }, // 200 x 26
  { slider_knob,     sizeof(slider_knob    ) }, //  26 x 26
  { slider_knob_off, sizeof(slider_knob_off) }, //  26 x 26
};

/*--------------------------------------------------------------------------------
 * List of widget data based on the screen
 * The callback function may be something this:
 * static void onCallbackFunction(const Widget_t *widget, const Touch_t &touch) {
 *   if (touch.event == EVENT_INIT) {
 *     // Setup Process
 *   } else {
 *     // Event Process
 *   }
 * }
 *--------------------------------------------------------------------------------*/
// Screen - Main
static void onMainScreen        (const Widget_t *widget, const Touch_t &touch);
static void onMainInside        (const Widget_t *widget, const Touch_t &touch);
static void onMainOutside       (const Widget_t *widget, const Touch_t &touch);
static void onMainThermograph   (const Widget_t *widget, const Touch_t &touch);
static void onMainCapture       (const Widget_t *widget, const Touch_t &touch);
static void onMainConfiguration (const Widget_t *widget, const Touch_t &touch);

static constexpr Widget_t widget_main[] = {
  {   0,   0, 320, 240, image_main,        EVENT_NONE, onMainScreen        },
  {   0,   0, 256, 192, NULL,              EVENT_ALL,  onMainInside        },
  { 258,   0,  62, 134, NULL,              EVENT_ALL,  onMainOutside       },
  {   0, 195, 256,  45, NULL,              EVENT_ALL,  onMainThermograph   },
  { 265, 135,  50,  50, image_icon_camera, EVENT_UP,   onMainCapture       },
  { 265, 185,  50,  50, NULL,              EVENT_ALL,  onMainConfiguration },
};

// Screen - Configuration
static void onConfigurationScreen       (const Widget_t *widget, const Touch_t &touch);
static void onConfigurationResolution   (const Widget_t *widget, const Touch_t &touch);
static void onConfigurationThermograph  (const Widget_t *widget, const Touch_t &touch);
static void onConfigurationCaptureMode  (const Widget_t *widget, const Touch_t &touch);
static void onConfigurationFileManager  (const Widget_t *widget, const Touch_t &touch);
static void onConfigurationCalibration  (const Widget_t *widget, const Touch_t &touch);
static void onConfigurationInformation  (const Widget_t *widget, const Touch_t &touch);
static void onConfigurationReturn       (const Widget_t *widget, const Touch_t &touch);

static constexpr Widget_t widget_configuration[] = {
  {   0,   0, 320, 240, image_configuration, EVENT_NONE, onConfigurationScreen      },
  {  24,  12,  70,  70, NULL,                EVENT_ALL,  onConfigurationResolution  },
  { 125,  12,  70,  70, NULL,                EVENT_ALL,  onConfigurationThermograph },
  { 226,  12,  70,  70, NULL,                EVENT_ALL,  onConfigurationCaptureMode },
  {  24, 104,  70,  70, NULL,                EVENT_ALL,  onConfigurationFileManager },
  { 125, 104,  70,  70, NULL,                EVENT_ALL,  onConfigurationCalibration },
  { 226, 104,  70,  70, NULL,                EVENT_ALL,  onConfigurationInformation },
  { 145, 206,  30,  30, NULL,                EVENT_ALL,  onConfigurationReturn      },
};

// Screen - Resolution
static void onResolutionScreen  (const Widget_t *widget, const Touch_t &touch);
static void onResolutionSlider1 (const Widget_t *widget, const Touch_t &touch);
static void onResolutionSlider2 (const Widget_t *widget, const Touch_t &touch);
static void onResolutionSlider3 (const Widget_t *widget, const Touch_t &touch);
static void onResolutionClose   (const Widget_t *widget, const Touch_t &touch);
static void onResolutionApply   (const Widget_t *widget, const Touch_t &touch);

static constexpr Widget_t widget_resolution[] = {
  {   0,   0, 320, 240, image_resolution, EVENT_NONE,  onResolutionScreen  },
  { 138,  30, 160,  26, image_slider1,    EVENT_DRAG,  onResolutionSlider1 },
  { 138,  80, 160,  26, image_slider1,    EVENT_DRAG,  onResolutionSlider2 },
  { 138, 166, 160,  26, image_slider1,    EVENT_DRAG,  onResolutionSlider3 },
  {  60, 206,  30,  30, NULL,             EVENT_ALL,   onResolutionClose   },
  { 230, 206,  30,  30, image_icon_apply, EVENT_CLICK, onResolutionApply   },
};

// Screen - Thermograph
static void onThermographScreen   (const Widget_t *widget, const Touch_t &touch);
static void onThermographRadio1   (const Widget_t *widget, const Touch_t &touch);
static void onThermographRadio2   (const Widget_t *widget, const Touch_t &touch);
static void onThermographToggle1  (const Widget_t *widget, const Touch_t &touch);
static void onThermographToggle2  (const Widget_t *widget, const Touch_t &touch);
static void onThermographSlider1  (const Widget_t *widget, const Touch_t &touch);
static void onThermographSlider2  (const Widget_t *widget, const Touch_t &touch);
static void onThermographMinUp    (const Widget_t *widget, const Touch_t &touch);
static void onThermographMinDown  (const Widget_t *widget, const Touch_t &touch);
static void onThermographMaxUp    (const Widget_t *widget, const Touch_t &touch);
static void onThermographMaxDown  (const Widget_t *widget, const Touch_t &touch);
static void onThermographClose    (const Widget_t *widget, const Touch_t &touch);
static void onThermographReset    (const Widget_t *widget, const Touch_t &touch);
static void onThermographApply    (const Widget_t *widget, const Touch_t &touch);

static constexpr Widget_t widget_thermograph[] = {
  {   0,   0, 320, 240, image_thermograph, EVENT_NONE,  onThermographScreen  },
  { 137,   5, 110,  26, image_radio,       EVENT_DOWN,  onThermographRadio1  },
  { 137,  38, 110,  26, image_radio,       EVENT_DOWN,  onThermographRadio2  },
  { 137,  71, 170,  26, image_toggle,      EVENT_DOWN,  onThermographToggle1 },
  { 137, 103, 150,  26, image_toggle,      EVENT_DOWN,  onThermographToggle2 },
  {  34, 134, 200,  26, image_slider2,     EVENT_DRAG,  onThermographSlider1 },
  {  34, 173, 200,  26, image_slider2,     EVENT_DRAG,  onThermographSlider2 },
  { 297, 134,  22,  26, image_arrowR,      EVENT_CLICK, onThermographMinUp   },
  { 235, 134,  22,  26, image_arrowL,      EVENT_CLICK, onThermographMinDown },
  { 297, 173,  22,  26, image_arrowR,      EVENT_CLICK, onThermographMaxUp   },
  { 235, 173,  22,  26, image_arrowL,      EVENT_CLICK, onThermographMaxDown },
  {  60, 206,  30,  30, NULL,              EVENT_ALL,   onThermographClose   },
  { 147, 206,  30,  30, image_icon_reset,  EVENT_CLICK, onThermographReset   },
  { 230, 206,  30,  30, image_icon_apply,  EVENT_CLICK, onThermographApply   },
};

// Screen - Capture mode
static void onCaptureModeScreen (const Widget_t *widget, const Touch_t &touch);
static void onCaptureModeCamera (const Widget_t *widget, const Touch_t &touch);
static void onCaptureModeVideo  (const Widget_t *widget, const Touch_t &touch);
static void onCaptureModeApply  (const Widget_t *widget, const Touch_t &touch);

static constexpr Widget_t widget_capture_mode[] = {
  {   0,   0, 320, 240, image_capture_mode, EVENT_NONE, onCaptureModeScreen   },
  {  48,  50, 240,  26, image_radio,        EVENT_DOWN, onCaptureModeCamera   },
  {  48, 126, 240,  26, image_radio,        EVENT_DOWN, onCaptureModeVideo    },
  { 145, 206,  30,  30, NULL,               EVENT_ALL,  onCaptureModeApply    },
};

// Configuration for the scroll box
#define FONT_WIDTH    12  // [px] (for setTextSize(2))
#define FONT_HEIGHT   16  // [px] (for setTextSize(2))
#define FONT_MARGIN   3   // [px] (margin for each top, right, bottom, left)
#define ITEM_WIDTH    11  // text length of an item (mlx0000.bmp)
#define VIEW_ITEMS    10  // number of items in a view
#define ITEM_HEIGHT   (FONT_HEIGHT + FONT_MARGIN * 2)
#define VIEW_WIDTH    (FONT_WIDTH  * ITEM_WIDTH + FONT_MARGIN * 2) // 138
#define VIEW_HEIGHT   (ITEM_HEIGHT * VIEW_ITEMS) // 220
#define SCROLL_COLOR  RGB565(0x01, 0xA1, 0xFF)

// Screen - File manager
static void onFileManagerScreen   (const Widget_t *widget, const Touch_t &touch);
static void onFileManagerCheckAll (const Widget_t *widget, const Touch_t &touch);
static void onFileManagerScrollBox(const Widget_t *widget, const Touch_t &touch);
static void onFileManagerScrollBar(const Widget_t *widget, const Touch_t &touch);
static void onFileManagerRewind   (const Widget_t *widget, const Touch_t &touch);
static void onFileManagerPlay     (const Widget_t *widget, const Touch_t &touch);
static void onFileManagerPrev     (const Widget_t *widget, const Touch_t &touch);
static void onFileManagerNext     (const Widget_t *widget, const Touch_t &touch);
static void onFileManagerClose    (const Widget_t *widget, const Touch_t &touch);
static void onFileManagerApply    (const Widget_t *widget, const Touch_t &touch);
static void onFileManagerWatch    (const Widget_t *widget, const Touch_t &touch);

static constexpr Widget_t widget_file_manager[] = {
  {   0,   0, 320, 240, image_file_manager, EVENT_NONE,   onFileManagerScreen    },
  {   0,   9,  26,  26, image_checkbox,     EVENT_DOWN,   onFileManagerCheckAll  },
  {  29,  10, 138, 220, NULL,               EVENT_SELECT, onFileManagerScrollBox }, // VIEW_WIDTH x VIEW_HEIGHT
  { 170,   9,  15, 220, NULL,               EVENT_DRAG,   onFileManagerScrollBar }, // scroll bar x VIEW_HEIGHT
  { 190,  62, 128,  96, NULL,               EVENT_NONE,   nullptr                }, // thumbnail area
  { 194, 165,  24,  26, image_rewind,       EVENT_CLICK,  onFileManagerRewind    }, // 24 x 24 --> 24 x 26 for DrawPress()
  { 228, 165,  24,  26, image_play,         EVENT_CLICK,  onFileManagerPlay      }, // 24 x 24 --> 24 x 26 for DrawPress()
  { 260, 165,  24,  26, image_prev,         EVENT_CLICK,  onFileManagerPrev      }, // 24 x 24 --> 24 x 26 for DrawPress()
  { 292, 165,  24,  26, image_next,         EVENT_CLICK,  onFileManagerNext      }, // 24 x 24 --> 24 x 26 for DrawPress()
  { 273, 205,  30,  32, image_trashbox,     EVENT_CLICK,  onFileManagerApply     }, // 30 x 30 --> 30 x 32 for DrawPress()
  { 207, 205,  30,  32, NULL,               EVENT_ALL,    onFileManagerClose     }, // 30 x 30 --> 30 x 32 for DrawPress()
  {   0,   0,   0,   0, NULL,               EVENT_WATCH,  onFileManagerWatch     }, // special callback executed every cycle
};

// Screen - Confirmation by File Manager
static void onFileConfirmScreen(const Widget_t *widget, const Touch_t &touch);
static void onFileConfirmClose (const Widget_t *widget, const Touch_t &touch);
static void onFileConfirmApply (const Widget_t *widget, const Touch_t &touch);

static constexpr Widget_t widget_file_confirm[] = {
  {   1, 157, 316, 82, image_file_confirm, EVENT_NONE,   onFileConfirmScreen },
  {  42, 202,  82, 26, NULL,               EVENT_CLICK,  onFileConfirmClose  },
  { 190, 202,  82, 26, NULL,               EVENT_CLICK,  onFileConfirmApply  },
};

// Screen - Calibration
static void onCalibrationScreen (const Widget_t *widget, const Touch_t &touch);
static void onCalibrationExec   (const Widget_t *widget, const Touch_t &touch);
static void onCalibrationAdjust (const Widget_t *widget, const Touch_t &touch);
static void onCalibrationSave   (const Widget_t *widget, const Touch_t &touch);
static void onCalibrationXup    (const Widget_t *widget, const Touch_t &touch);
static void onCalibrationXdown  (const Widget_t *widget, const Touch_t &touch);
static void onCalibrationYup    (const Widget_t *widget, const Touch_t &touch);
static void onCalibrationYdown  (const Widget_t *widget, const Touch_t &touch);
static void onCalibrationClose  (const Widget_t *widget, const Touch_t &touch);
static void onCalibrationApply  (const Widget_t *widget, const Touch_t &touch);

static constexpr Widget_t widget_calibration[] = {
  {   0,   0, 320, 240, image_calibration, EVENT_NONE,  onCalibrationScreen   },
  {  22,  12,  70,  70, NULL,              EVENT_UP,    onCalibrationExec     },
  { 125,  12,  70,  70, NULL,              EVENT_UP,    onCalibrationAdjust   },
  { 228,  12,  70,  70, image_save_flash,  EVENT_UP,    onCalibrationSave     },
  { 164, 154,  26,  26, image_arrowU,      EVENT_CLICK, onCalibrationXup      },
  { 164, 180,  26,  26, image_arrowD,      EVENT_CLICK, onCalibrationXdown    },
  { 263, 154,  26,  26, image_arrowU,      EVENT_CLICK, onCalibrationYup      },
  { 263, 180,  26,  26, image_arrowD,      EVENT_CLICK, onCalibrationYdown    },
  {  60, 206,  30,  30, NULL,              EVENT_ALL,   onCalibrationClose    },
  { 230, 206,  30,  30, image_icon_apply,  EVENT_CLICK, onCalibrationApply    },
};

// Screen - Adjust Offset
static void onAdjustOffsetScreen(const Widget_t *widget, const Touch_t &touch);
static void onAdjustOffsetTarget(const Widget_t *widget, const Touch_t &touch);
static void onAdjustOffsetClose (const Widget_t *widget, const Touch_t &touch);
static void onAdjustOffsetApply (const Widget_t *widget, const Touch_t &touch);

static constexpr Widget_t widget_adjust_offset[] = {
  {  25,  90, 270,  60, image_adjust_offset, EVENT_NONE,  onAdjustOffsetScreen },
  { 144, 104,  32,  32, image_target,        EVENT_DOWN,  onAdjustOffsetTarget },
  {  60, 105,  30,  30, NULL,                EVENT_ALL,   onAdjustOffsetClose  },
  { 230, 105,  30,  30, image_icon_apply,    EVENT_CLICK, onAdjustOffsetApply  },
};

// Screen - Information
static void onInformationScreen(const Widget_t *widget, const Touch_t &touch);
static void onInformationClose (const Widget_t *widget, const Touch_t &touch);

static constexpr Widget_t widget_information[] = {
  {   0,   0, 320, 240, image_information, EVENT_NONE, onInformationScreen   },
  { 145, 206,  30,  30, NULL,              EVENT_ALL,  onInformationClose    },
};

/*--------------------------------------------------------------------------------
 * Common method - Apply
 *--------------------------------------------------------------------------------*/
static bool Apply(const Widget_t *widget, const Touch_t &touch, bool enable) {
  if (touch.event == EVENT_INIT) {
    DrawButton(widget, enable);
  }

  else if (enable) {
    DrawPress(widget, touch.event);

    if (touch.event & EVENT_RISING) {
      DrawButton(widget, 0);
      return true; // ready to apply
    }
  }

  return false;
}

/*--------------------------------------------------------------------------------
 * Callback functions - Main
 *--------------------------------------------------------------------------------*/
static void onMainScreen(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  DrawScreen(widget);

  // Draw color bar and range
  DrawTemperatureRange(3);

  // Draw resolution
  GFX_EXEC(setTextSize(2));
  GFX_EXEC(setTextDatum(TL_DATUM));
  gfx_printf(260 + FONT_WIDTH, LINE_HEIGHT * 0.5, "%2d:%d", mlx_cnf.interpolation, mlx_cnf.box_size);
}

static void onMainInside(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    const int box = mlx_cnf.box_size * mlx_cnf.interpolation;
#if ENA_OUTWARD_CAMERA
    te_pick.x = MLX90640_COLS - 1 - touch.x / box;
#else
    te_pick.x = touch.x / box;
#endif
    te_pick.y = touch.y / box;
    mlx_cnf.marker_mode |= 2;
  }
}

static void onMainOutside(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    te_pick.x = te_pick.y = 0;
    lp_pick.y = (MINTEMP + MAXTEMP) / 2;
    mlx_cnf.marker_mode &= 1;
  }
}

static void onMainThermograph(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    widget_state(STATE_THERMOGRAPH);
  }
}

static void onMainCapture(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event == EVENT_INIT) {
    DrawButton(widget, mlx_cap.capture_mode == 0 ? 0 : 2); // 0: Camera, 2: Video
  }

  else if (mlx_cap.capture_mode == 0) {
    DrawButton(widget, 1); // draw icon_camera2
    sdcard_save();
    DrawButton(widget, 0); // draw icon_camera1
  }

  else if (mlx_cap.recording == false) {
    if (mlx_cap.recording = sdcard_record_begin(mlx_cap.filename, sizeof(mlx_cap.filename))) {
      // adjust scale to mitigate the impact on input cycles
      if (mlx_cnf.interpolation * mlx_cnf.box_size >= 6) {
        mlx_copy = mlx_cnf;
        mlx_cnf.marker_mode = 0;
        if (mlx_cnf.interpolation == 6) {
          mlx_cnf.interpolation = 4;
        } else {
          mlx_cnf.interpolation = 1;
          mlx_cnf.box_size = INTERPOLATE_SCALE;
        }
        mlx_cnf.init();
      }
      DrawButton(widget, 3); // draw icon_stop
    }
  }

  else {
    DrawButton(widget, 2); // draw icon_video
    sdcard_record_end();
    mlx_cap.recording = false;
    mlx_cnf = mlx_copy;
    mlx_cnf.init();
  }
}

static void onMainConfiguration(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    widget_state(STATE_CONFIGURATION);
  }
}

/*--------------------------------------------------------------------------------
 * Callback functions - Configuration
 *--------------------------------------------------------------------------------*/
static void onConfigurationScreen(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  // copy MLX90640 configuration data and stop recording video
  cnf_copy = mlx_cnf;
  tch_copy = tch_cnf;
  mlx_cap.recording = false;

  DrawScreen(widget);
}

static void onConfigurationResolution(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    widget_state(STATE_RESOLUTION);
  }
}

static void onConfigurationThermograph(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    widget_state(STATE_THERMOGRAPH);
  }
}

static void onConfigurationCaptureMode(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    widget_state(STATE_CAPTURE_MODE);
  }
}

static void onConfigurationFileManager(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    widget_state(STATE_FILE_MANAGER);
  }
}

static void onConfigurationCalibration(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    widget_state(STATE_CALIBRATION);
  }
}

static void onConfigurationInformation(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    widget_state(STATE_INFORMATION);
  }
}

static void onConfigurationReturn(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    widget_state(STATE_MAIN);
  }
}

/*--------------------------------------------------------------------------------
 * Callback functions - Resolution
 *--------------------------------------------------------------------------------*/
static void MakeSliderPos(const Widget_t *widget, const int16_t *scale, const int16_t n_scale, int16_t *pos) {
  // Here it's assumed that the knob width is equal to its height.
  // maximum  knob position on the bar
  int16_t pos_max = widget->w - widget->h - SLIDER_KNOB_OFFSET1;
  const float step = (float)(pos_max - SLIDER_KNOB_OFFSET1) / (float)(scale[n_scale-1] - scale[0]);

  for (int i = 0; i < n_scale; ++i) {
    pos[i] = (int)((float)(scale[i] - scale[0]) * step + 0.5f) + SLIDER_KNOB_OFFSET1;

    CHECK_WIDGET_AREA(drawFastVLine(widget->x + pos[i] + widget->h / 2, widget->y - 10, 10, TFT_RED));
  }
}

static int UpdateSliderPos(const Widget_t *widget, const Touch_t &touch, int16_t *pos, const int16_t n_pos) {
  // Here it's assumed that the knob width is equal to its height.
  // relative knob position on the bar
  int16_t X = touch.x - widget->x - widget->h / 2;
  X = constrain(X, pos[0], pos[n_pos - 1]);

  for (int i = 0; i < n_pos - 1 ; ++i) {
    if (pos[i] <= X && X <= pos[i + 1]) {
      if (X < (pos[i] + pos[i + 1]) / 2) {
        DrawSlider(widget, pos[i  ]);
        return i;
      } else {
        DrawSlider(widget, pos[i+1]);
        return i + 1;
      }
    }
  }

  // never reach
  return 0;
}

static void onResolutionScreen(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  DrawScreen(widget);
}

static void onResolutionSlider1(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  const int16_t scale[] = {1, 2, 4, 6, 8};
  const int n = sizeof(scale) / sizeof(scale[0]);
  Touch_t t = touch;
  int16_t pos[n];

  // Calculate the position inside the widget (scale --> pos)
  MakeSliderPos(widget, scale, n, pos);

  if (touch.event == EVENT_INIT) {
    // Here it's assumed that the knob width is equal to its height.
    t.x = widget->x + widget->h / 2;
    for (int i = 0; i < n; ++i) {
      if (scale[i] == cnf_copy.interpolation) {
        t.x += pos[i];
        break;
      }
    }
  }

  // update the knob position and the configuration
  int i = UpdateSliderPos(widget, t, pos, n);
  cnf_copy.interpolation = scale[i];

  // restrict pixel interpolation and block size
  if (cnf_copy.interpolation * cnf_copy.box_size > INTERPOLATE_SCALE) {
    cnf_copy.box_size = INTERPOLATE_SCALE / cnf_copy.interpolation;
    onResolutionSlider2(widget + 1, doInit);
  }

  // Enable apply if somethig is changed
  onResolutionApply(widget + 4, doInit);
}

static void onResolutionSlider2(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  const int16_t scale[] = {1, 2, 4, 8};
  const int n = sizeof(scale) / sizeof(scale[0]);
  Touch_t t = touch;
  int16_t pos[n];

  // Calculate the position inside the widget (scale --> pos)
  MakeSliderPos(widget, scale, n, pos);

  if (touch.event == EVENT_INIT) {
    // Here it's assumed that the knob width is equal to its height.
    t.x = widget->x + widget->h / 2;
    for (int i = 0; i < n; ++i) {
      if (scale[i] == cnf_copy.box_size) {
        t.x += pos[i];
        break;
      }
    }
  }

  // update the knob position and the configuration
  int i = UpdateSliderPos(widget, t, pos, n);
  cnf_copy.box_size = scale[i];

  // restrict pixel interpolation and block size
  if (cnf_copy.interpolation * cnf_copy.box_size > INTERPOLATE_SCALE) {
    cnf_copy.interpolation = INTERPOLATE_SCALE / cnf_copy.box_size;
    onResolutionSlider1(widget - 1, doInit);
  }

  // Enable apply if somethig is changed
  onResolutionApply(widget + 3, doInit);
}

static void onResolutionSlider3 (const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  const int16_t scale[] = {2, 3, 4, 5, 6}; // MLX90640_2_HZ - MLX90640_32_HZ
  const int n = sizeof(scale) / sizeof(scale[0]);
  Touch_t t = touch;
  int16_t pos[n];

  // Calculate the position inside the widget (scale --> pos)
  MakeSliderPos(widget, scale, n, pos);

  if (touch.event == EVENT_INIT) {
    // Here it's assumed that the knob width is equal to its height.
    t.x = widget->x + widget->h / 2;
    for (int i = 0; i < n; ++i) {
      if (scale[i] == cnf_copy.refresh_rate) {
        t.x += pos[i];
        break;
      }
    }
  }

  // update the knob position and the configuration
  int i = UpdateSliderPos(widget, t, pos, n);
  cnf_copy.refresh_rate = scale[i];

  // Enable apply if somethig is changed
  onResolutionApply(widget + 2, doInit);
}

static void onResolutionClose(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    widget_state(STATE_CONFIGURATION);
  }
}

static void onResolutionApply(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (Apply(widget, touch, (mlx_cnf >= cnf_copy))) {
    mlx_cnf = cnf_copy;
    mlx_cnf.init();
  }
}

/*--------------------------------------------------------------------------------
 * Callback functions - Thermograph
 *--------------------------------------------------------------------------------*/
#define TERMOGRAPH_MIN      (-20)
#define TERMOGRAPH_MAX      (140)
#define TERMOGRAPH_STEP     (1)   // --> 'RANGE_STEP' in filter.hpp
#define TERMOGRAPH_DIFF     (5)
#define TERMOGRAPH_MIN_X    260
#define TERMOGRAPH_MIN_Y    140
#define TERMOGRAPH_MAX_X    260
#define TERMOGRAPH_MAX_Y    179

static __attribute__((optimize("O0"))) int16_t GetThermoSlider(const Widget_t *widget, const Touch_t &touch) {
  // Here it's assumed that the knob width is equal to its height.
  int16_t X = touch.x - widget->x - widget->h / 2;         // Relative x coordinate of top left of knob
  int16_t Y = SLIDER_KNOB_OFFSET2;                         // Minimum value of knob top left coordinate
  int16_t Z = widget->w - widget->h - SLIDER_KNOB_OFFSET2; // Maximum value of knob top left coordinate

  DrawSlider(widget, X = constrain(X, Y, Z));

  // Set the X coordinate in 5 degree increments
  X -= Y;
  X = (X / TERMOGRAPH_STEP) * TERMOGRAPH_STEP;
  X += Y;

  return TERMOGRAPH_MIN + ((X - Y) * (TERMOGRAPH_MAX - TERMOGRAPH_MIN)) / (Z - Y); // value
}

static void PutThermoSlider(const Widget_t *widget, int16_t V, bool enable) {
  // Here it's assumed that the knob width is equal to its height.
  int16_t Y = SLIDER_KNOB_OFFSET2;                         // Minimum value of knob top left coordinate
  int16_t Z = widget->w - widget->h - SLIDER_KNOB_OFFSET2; // Maximum value of knob top left coordinate

  int16_t X = (V - TERMOGRAPH_MIN) * (Z - Y) / (TERMOGRAPH_MAX - TERMOGRAPH_MIN) + Y;
  DrawSlider(widget, X = constrain(X, Y, Z), enable);
}

static void DrawRangeMin(const Widget_t* widget, const Touch_t &touch) {
  // draw button when touch.event == EVENT_INIT or EVENT_UP
  if (touch.event != EVENT_DOWN) {
    DrawButton(widget,     (!cnf_copy.range_auto && cnf_copy.range_min < TERMOGRAPH_MAX) ? 1 : 0);
    DrawButton(widget + 1, (!cnf_copy.range_auto && cnf_copy.range_min > TERMOGRAPH_MIN) ? 1 : 0);
  }
}

static void DrawRangeMax(const Widget_t* widget, const Touch_t &touch) {
  // draw button when touch.event == EVENT_INIT or EVENT_UP
  if (touch.event != EVENT_DOWN) {
    DrawButton(widget,     (!cnf_copy.range_auto && cnf_copy.range_max < TERMOGRAPH_MAX) ? 1 : 0);
    DrawButton(widget + 1, (!cnf_copy.range_auto && cnf_copy.range_max > TERMOGRAPH_MIN) ? 1 : 0);
  }
}

static void onThermographScreen(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  DrawScreen(widget);

  // copy MLX90640 configuration data and stop recording video
  cnf_copy = mlx_copy = mlx_cnf;
  mlx_cap.recording = false;
  mlx_cnf.box_size = 1;
  mlx_cnf.interpolation = 4;
  mlx_cnf.init();
}

static void onThermographRadio1(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    mlx_cnf.color_scheme = cnf_copy.color_scheme = 0;
  }

  DrawRadio(widget, 2, cnf_copy.color_scheme);

  // Enable apply if somethig is changed
  DrawTemperatureRange(1);
  onThermographApply(widget + 12, doInit);
}

static void onThermographRadio2(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    mlx_cnf.color_scheme = cnf_copy.color_scheme = 1;
  }

  DrawRadio(widget - 1, 2, cnf_copy.color_scheme);

  // Enable apply if somethig is changed
  DrawTemperatureRange(1);
  onThermographApply(widget + 11, doInit);
}

static void onThermographToggle1(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    mlx_cnf.marker_mode = cnf_copy.marker_mode ^= 1;
  }

  DrawToggle(widget, cnf_copy.marker_mode & 1);

  // Enable apply if somethig is changed
  onThermographApply(widget + 10, doInit);
}

static void onThermographToggle2(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    mlx_cnf.range_auto = cnf_copy.range_auto = !cnf_copy.range_auto;

    // Update min/max to measured values
    if (!cnf_copy.range_auto) {
      cnf_copy.range_min = mlx_cnf.range_min;
      cnf_copy.range_max = mlx_cnf.range_max;
    }
  }

  DrawToggle(widget, cnf_copy.range_auto);

  // Disable sliders and up/down buttons
  onThermographSlider1(widget + 1, doInit);
  onThermographSlider2(widget + 2, doInit);

  // Enable apply if somethig is changed
  onThermographApply(widget + 9, doInit);
}

static void onThermographSlider1(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event == EVENT_INIT || touch.event == EVENT_SHOW) {
    // Temperature minimum and maximum restrictions
    if (cnf_copy.range_max - cnf_copy.range_min < TERMOGRAPH_DIFF) {
      mlx_cnf.range_min = cnf_copy.range_min = cnf_copy.range_max - TERMOGRAPH_DIFF;
    }

    // when range_auto == true then make slider disable
    PutThermoSlider(widget, cnf_copy.range_min, !cnf_copy.range_auto);
    gfx_printf(TERMOGRAPH_MIN_X, TERMOGRAPH_MIN_Y, "%3d", cnf_copy.range_min);
  }

  else if (!cnf_copy.range_auto) {
    // Convert to temperature and display without flickering
    int16_t v = GetThermoSlider(widget, touch);
    static int16_t V = 0xFFFF;
    if (V != v) {
      mlx_cnf.range_min = cnf_copy.range_min = V = v;
      gfx_printf(TERMOGRAPH_MIN_X, TERMOGRAPH_MIN_Y, "%3d", cnf_copy.range_min);
      onThermographSlider2(widget + 1, doInit);
    }
  }

  // Update up/down buttons
  if (touch.event != EVENT_SHOW) {
    onThermographMinUp  (widget + 2, doInit);
    onThermographMinDown(widget + 3, doInit);
  }

  // Enable apply if somethig is changed
  DrawTemperatureRange(2);
  onThermographApply(widget + 8, doInit);
}

static void onThermographSlider2(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event == EVENT_INIT || touch.event == EVENT_SHOW) {
    // Temperature minimum and maximum restrictions
    if (cnf_copy.range_max - cnf_copy.range_min < TERMOGRAPH_DIFF) {
      mlx_cnf.range_max = cnf_copy.range_max = cnf_copy.range_min + TERMOGRAPH_DIFF;
    }

    // when range_auto == true then make slider disable
    PutThermoSlider(widget, cnf_copy.range_max, !cnf_copy.range_auto);
    gfx_printf(TERMOGRAPH_MAX_X, TERMOGRAPH_MAX_Y, "%3d", cnf_copy.range_max);
  }

  else if (!cnf_copy.range_auto) {
    // Convert to temperature and display without flickering
    int16_t v = GetThermoSlider(widget, touch);
    static int16_t V = 0xFFFF;
    if (V != v) {
      mlx_cnf.range_max = cnf_copy.range_max = V = v;
      gfx_printf(TERMOGRAPH_MAX_X, TERMOGRAPH_MAX_Y, "%3d", cnf_copy.range_max);
      onThermographSlider1(widget - 1, doInit);
    }
  }

  // Update up/down buttons
  if (touch.event != EVENT_SHOW) {
    onThermographMaxUp  (widget + 3, doInit);
    onThermographMaxDown(widget + 4, doInit);
  }

  // Enable apply if somethig is changed
  DrawTemperatureRange(2);
  onThermographApply(widget + 7, doInit);
}

static void onThermographMinUp(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT && !cnf_copy.range_auto && cnf_copy.range_min < TERMOGRAPH_MAX) {
    DrawPress(widget, touch.event);
    if (touch.event & EVENT_DOWN) {
      mlx_cnf.range_min = ++cnf_copy.range_min;
      onThermographSlider1(widget - 2, showRange);
    }
  }

  DrawRangeMin(widget, touch);
}

static void onThermographMinDown(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT && !cnf_copy.range_auto && cnf_copy.range_min > TERMOGRAPH_MIN) {
    DrawPress(widget, touch.event);
    if (touch.event & EVENT_DOWN) {
      mlx_cnf.range_min = --cnf_copy.range_min;
      onThermographSlider1(widget - 3, showRange);
    }
  }

  DrawRangeMin(widget - 1, touch);
}

static void onThermographMaxUp(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT && !cnf_copy.range_auto && cnf_copy.range_max < TERMOGRAPH_MAX) {
    DrawPress(widget, touch.event);
    if (touch.event & EVENT_DOWN) {
      mlx_cnf.range_max = ++cnf_copy.range_max;
      onThermographSlider2(widget - 3, showRange);
    }
  }

  DrawRangeMax(widget, touch);
}

static void onThermographMaxDown(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event & EVENT_DOWN && !cnf_copy.range_auto && cnf_copy.range_max > TERMOGRAPH_MIN) {
    DrawPress(widget, touch.event);
    if (touch.event & EVENT_DOWN) {
      mlx_cnf.range_max = --cnf_copy.range_max;
      onThermographSlider2(widget - 4, showRange);
    }
  }

  DrawRangeMax(widget - 1, touch);
}

static void onThermographClose(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    mlx_cnf = mlx_copy;
    mlx_cnf.init();
    widget_state(STATE_CONFIGURATION);
  }
}

static void onThermographReset(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event == EVENT_INIT) {
    DrawWidget(widget, (mlx_cnf != mlx_ini));
  }

  else if (Apply(widget, touch, (mlx_cnf != mlx_ini))) {
    // Copy some members
    cnf_copy <<= mlx_ini;
    mlx_cnf = mlx_copy = cnf_copy;
    mlx_cnf.box_size = 1;
    mlx_cnf.interpolation = 4;
    mlx_cnf.init();

    onThermographRadio1 (widget - 11, doInit);
    onThermographRadio2 (widget - 10, doInit);
    onThermographToggle1(widget -  9, doInit);
    onThermographToggle2(widget -  8, doInit);
  }
}

static void onThermographApply(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (Apply(widget, touch, (mlx_copy != cnf_copy))) {
    mlx_cnf = mlx_copy = cnf_copy;
    mlx_cnf.box_size = 1;
    mlx_cnf.interpolation = 4;
    mlx_cnf.init();

    onThermographReset(widget - 1, doInit);
  }
}

/*--------------------------------------------------------------------------------
 * Callback functions - Capture Mode
 *--------------------------------------------------------------------------------*/
static void onCaptureModeScreen(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  DrawScreen(widget);
}

static void onCaptureModeCamera(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    mlx_cap.capture_mode = 0;
  }

  DrawRadio(widget, 2, mlx_cap.capture_mode);
}

static void onCaptureModeVideo(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    mlx_cap.capture_mode = 1;
  }

  DrawRadio(widget - 1, 2, mlx_cap.capture_mode);
}

static void onCaptureModeApply(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    widget_state(STATE_CONFIGURATION);
  }
}

/*--------------------------------------------------------------------------------
 * Callback functions - File Manager
 *--------------------------------------------------------------------------------*/
#include <vector>
#include <string>
#include <algorithm>

static std::vector<FileInfo_t> files;
static int n_files;
static int scroll_pos, scroll_max, bar_height;
static bool file_selected, keep_selected;
static bool mlx_status; // false: stop, true: playing
static MLXViewer mlx_viewer;

static void ScrollView(const Widget_t *widget, int scroll_pos) {
#if   defined (LOVYANGFX_HPP_)

  static LGFX_Sprite sprite_view(&lcd);
  sprite_view.setPsram(true);

#elif defined (_TFT_eSPIH_)

  static TFT_eSprite sprite_view(&tft);

#endif

  GFX_EXEC(startWrite());
  sprite_view.setTextSize(2);
  sprite_view.setTextWrap(false);
  sprite_view.setTextColor(TFT_WHITE, TFT_BLACK);
  sprite_view.createSprite(VIEW_WIDTH, VIEW_HEIGHT);

  int scaled_pos = scroll_pos * widget->h / bar_height;
  int item_head  = scaled_pos / ITEM_HEIGHT;
  int item_tail  = item_head + (VIEW_ITEMS + 1);
  item_tail = min(item_tail, n_files);
  DBG_FUNC(printf("item_head: %d, item_tail: %d\n", item_head, item_tail));

  bool invert = false;
  int base_pos = item_head * ITEM_HEIGHT;
  int delta_pos = base_pos - scaled_pos + FONT_MARGIN - ITEM_HEIGHT;

  for (int i = item_head; i < item_tail; ++i) {
    delta_pos += ITEM_HEIGHT;

    if (invert == false && files[i].isSelected == true) {
      invert = true;
      sprite_view.setTextColor(TFT_BLACK, TFT_WHITE);
    } else

    if (invert == true && files[i].isSelected == false) {
      invert = false;
      sprite_view.setTextColor(TFT_WHITE, TFT_BLACK);
    }

    if (invert == true) {
      sprite_view.fillRect(0, delta_pos - FONT_MARGIN, VIEW_WIDTH, ITEM_HEIGHT, TFT_WHITE);
    }

    // eliminate directory and extract file name only
    sprite_view.setCursor(FONT_MARGIN, delta_pos);
    const char *p = strrchr(files[i].path.c_str(), '/');
    sprite_view.print(p ? p + 1 : p);
  }

  sprite_view.pushSprite(widget->x, widget->y);
  sprite_view.deleteSprite();
  GFX_EXEC(endWrite());
}

static void UpdateViewer(const Widget_t *widget) {
  onFileManagerRewind(widget + 0, doInit);
  onFileManagerPlay  (widget + 1, doInit);
  onFileManagerPrev  (widget + 2, doInit);
  onFileManagerNext  (widget + 3, doInit);
}

static void onFileManagerScreen(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  DrawScreen(widget);

  file_selected = false;

  if (sdcard_mount()) {
    uint32_t total, free;
    sdcard_size(&total, &free);
    GFX_EXEC(setTextSize(2));
    if (total < 10000UL) {
      gfx_printf(245, 13, "%4luMB", total);
      gfx_printf(245, 37, "%4luMB", free);
    } else {
      gfx_printf(245, 13, "%4.1fGB", (float)total / 1000.0);
      gfx_printf(245, 37, "%4.1fGB", (float)free  / 1000.0);
    }

    if (!keep_selected) {
      files.clear();
      GetFileList(SD, MLX90640_DIR.c_str(), 0, files);
      n_files = files.size();
      std::sort(files.begin() + 1 /* skip @number.txt */, files.end(), [](FileInfo_t &a, FileInfo_t &b) {
        return a.path.compare(b.path) > 0 ? true : false;
      });
    }
/*
    DBG_EXEC({
      for (const auto& file : files) {
        printf("%s, %lu\n", file.path.c_str(), file.size);
      }
    });
/*
    DBG_EXEC({
      files[0].isSelected = true;
      files[9].isSelected = true;
      files[n_files-1].isSelected = true;
    });
//*/
  }

  else {
    gfx_printf(245, 13, "failed");
  }
}

static void onFileManagerCheckAll(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    file_selected = !file_selected;
    for (auto& file : files) {
      file.isSelected = file_selected;
    }
    ScrollView(widget + 1, scroll_pos);
    onFileManagerApply(widget + 8, doInit);
  }

  DrawCheck(widget, file_selected ? 1 : 0);
}

static void onFileManagerScrollBox(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    int scaled_pos = scroll_pos * widget->h / bar_height;
    int selected =  (scaled_pos + touch.y - widget->y - FONT_MARGIN) / ITEM_HEIGHT;
    files[selected].isSelected = !files[selected].isSelected;

    ScrollView(widget, scroll_pos);
    onFileManagerApply(widget + 7, doInit);

    const Widget_t *thumbnail = widget + 2; 
    const char *path = files[selected].path.c_str();
    const char *ext  = strrchr(path, '.');

    if (ext != NULL && !files[selected].isDirectory && files[selected].isSelected) {
      if (strcmp(ext, ".bmp") == 0) {
        mlx_viewer.close();
        DrawThumb(thumbnail, path);
      }
      else if (strcmp(ext, ".raw") == 0) {
        mlx_viewer.open(thumbnail, path);
      }
    }
    else {
      mlx_viewer.close();
      GFX_EXEC(fillRect(thumbnail->x, thumbnail->y, thumbnail->w, thumbnail->h, TFT_BLACK));
    }

    UpdateViewer(widget + 3);
  }
}

static void onFileManagerScrollBar(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  static int drag_pos;

  if (touch.event == EVENT_INIT) {
    drag_pos = 0;
    if (!keep_selected) {
      scroll_pos = 0;
    }

    if (n_files > VIEW_ITEMS) {
      scroll_max = widget->h * (n_files - VIEW_ITEMS) / n_files + 1; // '1' for round up
      bar_height = widget->h - scroll_max;
    } else {
      bar_height = widget->h;
      scroll_max = 0;
    }
  }

  else if (touch.event & EVENT_DOWN) {
    drag_pos = touch.y;
  } // else drag

  // Relative movement from the previous position
  scroll_pos += touch.y - drag_pos;
  scroll_pos = constrain(scroll_pos, 0, scroll_max);
//DBG_FUNC(printf("scroll_pos: %d, scroll_max: %d\n", scroll_pos, scroll_max));

#if   defined (LOVYANGFX_HPP_)

  static LGFX_Sprite sprite_scroll(&lcd);
  sprite_scroll.setPsram(true);

#elif defined (_TFT_eSPIH_)

  static TFT_eSprite sprite_scroll(&tft);

#endif

  sprite_scroll.createSprite(widget->w, widget->h);
  sprite_scroll.fillRect(0, scroll_pos, widget->w, bar_height, SCROLL_COLOR);
  sprite_scroll.pushSprite(widget->x, widget->y);
  sprite_scroll.deleteSprite();

  ScrollView(widget - 1, scroll_pos);

  // Update the previous position
  drag_pos = touch.y;
}

static void onFileManagerRewind(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event == EVENT_INIT) {
    DrawButton(widget, mlx_viewer.isOpened() ? 1 : 0);
  } else {
    DrawPress(widget, touch.event);
    if (touch.event == EVENT_UP) {
      mlx_status = false;
      mlx_viewer.rewind();
      UpdateViewer(widget - 0);
    }
  }
}

static void onFileManagerPlay(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event == EVENT_INIT) {
    DrawButton(widget, mlx_viewer.isOpened() ? (mlx_viewer.isEnd() ? 0 : (mlx_status ? 2 : 1)) : 0);
  } else {
    DrawPress(widget, touch.event);
    if (touch.event == EVENT_UP) {
      mlx_status = !mlx_status;
      UpdateViewer(widget - 1);
    }
  }
}

static void onFileManagerPrev(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event == EVENT_INIT) {
    DrawButton(widget, mlx_viewer.isOpened() ? (mlx_viewer.isTop() ? 0 : 1) : 0);
  } else {
    DrawPress(widget, touch.event);
    if (touch.event == EVENT_UP) {
      mlx_status = false;
      mlx_viewer.prev();
      UpdateViewer(widget - 2);
    }
  }
}

static void onFileManagerNext(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event == EVENT_INIT) {
    DrawButton(widget, mlx_viewer.isOpened() ? (mlx_viewer.isEnd() ? 0 : 1) : 0);
  } else {
    DrawPress(widget, touch.event);
    if (touch.event == EVENT_UP) {
      mlx_status = false;
      mlx_viewer.next();
      UpdateViewer(widget - 3);
    }
  }
}

static void onFileManagerClose(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    widget_state(STATE_CONFIGURATION);
  }
}

static void onFileManagerApply(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  bool selected = false;
  for (auto& file : files) {
    if (file.isSelected) {
      selected = true;
      break;
    }
  }

  if (touch.event == EVENT_INIT) {
    // Reset selection
    keep_selected = false;

    DrawButton(widget, selected ? 1 : 0);
  } else {
    DrawPress(widget, touch.event);

    // Show dialog to confirm to delete files
    if (touch.event == EVENT_UP && selected) {
      widget_state(STATE_FILE_CONFIRM);
    }
  }
}

static void onFileManagerWatch(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (mlx_status) {
    uint32_t currentTime = millis();
    static uint32_t prevTime;
    if (currentTime - prevTime > (uint32_t)(mlx_cnf.sampling_period * 1000.0f)) {
      prevTime = currentTime;
      if (!mlx_viewer.next()) {
        mlx_status = false;
      }
      UpdateViewer(widget - 6);
    }
  }
}

/*--------------------------------------------------------------------------------
 * Callback functions - Confirmation by File Manager 
 *--------------------------------------------------------------------------------*/
static void onFileConfirmScreen(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event == EVENT_INIT) {
    DrawScreen(widget);
  }
}

static void onFileConfirmClose(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    // Back to the file manager screen
    keep_selected = true;
    widget_state(STATE_FILE_MANAGER);
  }
}

static void onFileConfirmApply(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    for (auto& file : files) {
      if (file.isSelected) {
        // DBG_EXEC(printf("%s\n", file.path.c_str()));
        DeleteFile(SD, file.path.c_str());
      }
    }

    // Back to the file manager screen
    widget_state(STATE_FILE_MANAGER);
  }
}

/*--------------------------------------------------------------------------------
 * Callback functions - Calibration
 *--------------------------------------------------------------------------------*/
static bool enable_save;
static bool SaveCalibration(TouchConfig_t &config) {
  if (touch_save(&tch_cnf)) {
    enable_save = false;
    return true;
  } else {
    return false;
  }
}

#define TOUCH_OFFSET_MIN    (-10)
#define TOUCH_OFFSET_MAX    ( 10)
#define TOUCH_OFFSET_X_COL  123
#define TOUCH_OFFSET_X_ROW  172
#define TOUCH_OFFSET_Y_COL  223
#define TOUCH_OFFSET_Y_ROW  172

static void DrawOffsetX(const Widget_t* widget, const Touch_t &touch, const Widget_t *apply) {
  // draw button when touch.event == EVENT_INIT or EVENT_UP
  if (touch.event != EVENT_DOWN) {
    DrawButton(widget,     (tch_copy.offset[0] < TOUCH_OFFSET_MAX) ? 1 : 0);
    DrawButton(widget + 1, (tch_copy.offset[0] > TOUCH_OFFSET_MIN) ? 1 : 0);
  }

  gfx_printf(TOUCH_OFFSET_X_COL, TOUCH_OFFSET_X_ROW, "%3d", (int)tch_copy.offset[0]);

  // Enable apply if somethig is changed
  onCalibrationApply(apply, doInit);
}

static void DrawOffsetY(const Widget_t* widget, const Touch_t &touch, const Widget_t *apply) {
  // draw button when touch.event == EVENT_INIT or EVENT_UP
  if (touch.event != EVENT_DOWN) {
    DrawButton(widget,     (tch_copy.offset[1] < TOUCH_OFFSET_MAX) ? 1 : 0);
    DrawButton(widget + 1, (tch_copy.offset[1] > TOUCH_OFFSET_MIN) ? 1 : 0);
  }

  gfx_printf(TOUCH_OFFSET_Y_COL, TOUCH_OFFSET_Y_ROW, "%3d", (int)tch_copy.offset[1]);

  // Enable apply if somethig is changed
  onCalibrationApply(apply, doInit);
}

static void onCalibrationScreen(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  tch_ajst = tch_copy;

  DrawScreen(widget);

  GFX_EXEC(setTextSize(1));

  uint16_t *c = tch_cnf.cal;
  gfx_printf(115,  94, "%4d, %4d, %4d, %4d", c[0], c[1], c[2], c[3]);
  gfx_printf(115, 106, "%4d, %4d, %4d, %4d", c[4], c[5], c[6], c[7]);

  if (!(tch_cnf >= tch_copy)) {
    c = tch_copy.cal;
    gfx_printf(115, 126, "%4d, %4d, %4d, %4d", c[0], c[1], c[2], c[3]);
    gfx_printf(115, 138, "%4d, %4d, %4d, %4d", c[4], c[5], c[6], c[7]);
  }

  GFX_EXEC(setTextSize(2));
}

static void onCalibrationExec(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    touch_calibrate(&tch_copy);
    widget_state(STATE_CALIBRATION);
  }
}

static void onCalibrationAdjust(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    widget_state(STATE_ADJUST_OFFSET);
  }
}

static void onCalibrationSave(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  // Only when using flash is true
  enable_save &= USE_PREFERENCES;

  if (touch.event == EVENT_INIT) {
    DrawButton(widget, enable_save);
  }

  // now ready to save touch configuration data into flash
  else if (enable_save) {
    DrawButton(widget, 2);
    if (SaveCalibration(tch_cnf)) {
      widget_state(STATE_CALIBRATION);
    }
  }
}

static void onCalibrationXup(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT && tch_copy.offset[0] < TOUCH_OFFSET_MAX) {
    DrawPress(widget, touch.event);
    if (touch.event & EVENT_DOWN ) {
      ++tch_copy.offset[0];
    }
  }

  DrawOffsetX(widget, touch, widget + 5);
}

static void onCalibrationXdown(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT && tch_copy.offset[0] > TOUCH_OFFSET_MIN) {
    DrawPress(widget, touch.event);
    if (touch.event & EVENT_DOWN) {
      --tch_copy.offset[0];
    }
  }

  DrawOffsetX(widget - 1, touch, widget + 4);
}

static void onCalibrationYup(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT && tch_copy.offset[1] < TOUCH_OFFSET_MAX) {
    DrawPress(widget, touch.event);
    if (touch.event & EVENT_DOWN) {
      ++tch_copy.offset[1];
    }
  }

  DrawOffsetY(widget, touch, widget + 3);
}

static void onCalibrationYdown(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT && tch_copy.offset[1] > TOUCH_OFFSET_MIN) {
    DrawPress(widget, touch.event);
    if (touch.event & EVENT_DOWN) {
      --tch_copy.offset[1];
    }
  }

  DrawOffsetY(widget - 1, touch, widget + 2);
}

static void onCalibrationClose(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event & EVENT_DOWN) {
    widget_state(STATE_CONFIGURATION);
  }
}

static void onCalibrationApply(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (Apply(widget, touch, (tch_cnf != tch_copy))) {
    tch_cnf = tch_copy;

    // Enable to save if somethig configuration data
    enable_save = true;
    onCalibrationSave(widget - 6, doInit);
  }
}

/*--------------------------------------------------------------------------------
 * Callback functions - Adjust Offset
 *--------------------------------------------------------------------------------*/
static void onAdjustOffsetScreen(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

#if   defined (LOVYANGFX_HPP_)

  GFX_EXEC(beginTransaction());
  GFX_EXEC(fillRectAlpha(0, 0, lcd_width, lcd_height, 160, TFT_BLACK));
  GFX_EXEC(endTransaction());

#elif defined (_TFT_eSPIH_)

  GFX_EXEC(startWrite());
  DrawPNG(screen_calibration_opacity70, sizeof(screen_calibration_opacity70), 0, 0, pngDraw);
  GFX_EXEC(endWrite());

#endif

  DrawScreen(widget);
}

static void onAdjustOffsetTarget(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event == EVENT_INIT) {
    DrawButton(widget);
  }

  else {
    DrawButton(widget, 1);

    tch_ajst.offset[0] = lcd_width  / 2 - 1 - touch.x;
    tch_ajst.offset[1] = lcd_height / 2 - 1 - touch.y;

    tch_ajst.offset[0] = constrain(tch_ajst.offset[0], TOUCH_OFFSET_MIN, TOUCH_OFFSET_MAX);
    tch_ajst.offset[1] = constrain(tch_ajst.offset[1], TOUCH_OFFSET_MIN, TOUCH_OFFSET_MAX);

    gfx_printf(TOUCH_OFFSET_X_COL, TOUCH_OFFSET_X_ROW, "%3d", (int)tch_ajst.offset[0]);
    gfx_printf(TOUCH_OFFSET_Y_COL, TOUCH_OFFSET_Y_ROW, "%3d", (int)tch_ajst.offset[1]);

    // Enable apply if somethig is changed
    onAdjustOffsetApply(widget + 2, doInit);
  }
}

static void onAdjustOffsetClose(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    widget_state(STATE_CALIBRATION);
  }
}

static void onAdjustOffsetApply(const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (Apply(widget, touch, (tch_copy <= tch_ajst))) {
    tch_copy = tch_ajst;
    widget_state(STATE_CALIBRATION);
  }
}

/*--------------------------------------------------------------------------------
 * Callback functions - Information
 *--------------------------------------------------------------------------------*/
static void onInformationScreen (const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  DrawScreen(widget);
  /*
    https://en.cppreference.com/w/cpp/compiler_support
    https://forum.arduino.cc/t/which-version-of-c-is-currently-supported/1285868/13
    https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/api-guides/cplusplus.html
    https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html#:~:text=__cplusplus,gnu%2B%2B26.
    199711 ➜ C++98 or C++03 standards.
    201103 ➜ C++11 standard.
    201402 ➜ C++14 standard.
    201703 ➜ C++17 standard.
    202002 ➜ C++20 standard.
    202302 ➜ C++23 standard.
  */
  const struct {
    const uint32_t ver;
    const char* std; 
  } cpp[] = {
    {199711, "C++03"},
    {201103, "C++11"},
    {201402, "C++14"},
    {201703, "C++17"},
    {202002, "C++20"},
    {202302, "C++23"},
    {203000, "C++xx"},
  };
  const char *cpp_ver = "";
  for (int i = 0; i < sizeof(cpp) / sizeof(cpp[0]) - 1; ++i) {
    if (cpp[i].ver <= __cplusplus && __cplusplus < cpp[i+1].ver) {
      cpp_ver = cpp[i].std;
      break;
    }
  }

  const uint16_t x = 5, y = 5;
  const uint16_t h = FONT_HEIGHT + 2;

  // https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/Esp.h
  // https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/misc_system_api.html
  gfx_printf(x, y + h *  0, "MLX90640 S/N:%04X%04X%04X", mlx.serialNumber[0], mlx.serialNumber[1], mlx.serialNumber[2]);
  gfx_printf(x, y + h *  1, "MCU model   :%s R%d", ESP.getChipModel(), ESP.getChipRevision());
  gfx_printf(x, y + h *  2, "ESP-IDF ver :%d.%d.%d %s", ESP_IDF_VERSION_MAJOR, ESP_IDF_VERSION_MINOR, ESP_IDF_VERSION_PATCH, cpp_ver);
  gfx_printf(x, y + h *  3, "Sketch space:%7d", ESP.getFreeSketchSpace());
  gfx_printf(x, y + h *  4, "Sketch size :%7d", ESP.getSketchSize());
  gfx_printf(x, y + h *  5, "Task 1 stack:%7d", uxTaskGetStackHighWaterMark(taskHandle[0]));
  gfx_printf(x, y + h *  6, "Task 2 stack:%7d", uxTaskGetStackHighWaterMark(taskHandle[1]));
  gfx_printf(x, y + h *  7, "Heap total  :%7d", ESP.getHeapSize());
  gfx_printf(x, y + h *  8, "Heap free   :%7d", esp_get_free_internal_heap_size());
  gfx_printf(x, y + h *  9, "Heap remain :%7d", ESP.getMinFreeHeap());
  gfx_printf(x, y + h * 10, "Heap lowest :%7d", esp_get_minimum_free_heap_size());

  DBG_EXEC({
    printf("MLX90640 S/N:%04X%04X%04X\n", mlx.serialNumber[0], mlx.serialNumber[1], mlx.serialNumber[2]);
    printf("MCU model   :%s R%d\n", ESP.getChipModel(), ESP.getChipRevision());
    printf("ESP-IDF ver :%d.%d.%d %s\n", ESP_IDF_VERSION_MAJOR, ESP_IDF_VERSION_MINOR, ESP_IDF_VERSION_PATCH, cpp_ver);
    printf("Sketch space:%7d\n", ESP.getFreeSketchSpace());
    printf("Sketch size :%7d\n", ESP.getSketchSize());
    printf("Task 1 stack:%7d\n", uxTaskGetStackHighWaterMark(taskHandle[0]));
    printf("Task 2 stack:%7d\n", uxTaskGetStackHighWaterMark(taskHandle[1]));
    printf("Heap total  :%7d\n", ESP.getHeapSize());
    printf("Heap free   :%7d\n", esp_get_free_internal_heap_size());
    printf("Heap remain :%7d\n", ESP.getMinFreeHeap());
    printf("Heap lowest :%7d\n", esp_get_minimum_free_heap_size());

    // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/heap_debug.html
    // https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/mem_alloc.html
    printf("PSRAM total :%7d\n", ESP.getPsramSize());
    printf("PSRAM lowest:%7d\n", ESP.getMinFreePsram());
    printf("Min MALLOC_CAP_INTERNAL:%7d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL));
    printf("Min MALLOC_CAP_DMA     :%7d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_DMA));
    printf("Max MALLOC_CAP_INTERNAL:%7d\n", heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));
    printf("Max MALLOC_CAP_DMA     :%7d\n", heap_caps_get_largest_free_block(MALLOC_CAP_DMA));
#if defined (LOVYANGFX_HPP_)
    GFX_FAST(createSprite(256, 192));
    printf("Sprite buffer adrs :0x%X\n", lcd_sprite.getBuffer());
    printf("Sprite buffer len  :%d\n",   lcd_sprite.bufferLength());
    GFX_FAST(deleteSprite());
#endif
    printf("__cplusplus:%d\n", __cplusplus); // 201703
  });
}

static void onInformationClose  (const Widget_t *widget, const Touch_t &touch) {
  DBG_FUNC(printf("%s\n", __func__));

  if (touch.event != EVENT_INIT) {
    widget_state(STATE_CONFIGURATION);
  }
}
