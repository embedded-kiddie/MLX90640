/*================================================================================
 * Functions for drawing widget
 *================================================================================*/

/*--------------------------------------------------------------------------------
 * Functions prototyping
 *--------------------------------------------------------------------------------*/
static void DrawTemperatureMarker(void);
static void DrawTemperatureRange(uint8_t type);
static void DrawScreen(const Widget_t *widget);
static void DrawWidget(const Widget_t *widget, uint8_t offset = 0);
static void DrawButton(const Widget_t *widget, uint8_t offset = 0);
static void DrawToggle(const Widget_t *widget, bool check = false);
static void DrawCheck (const Widget_t *widget, bool check = false);
static void DrawPress (const Widget_t *widget, Event_t event = EVENT_INIT);
static void DrawSlider(const Widget_t *widget, int16_t pos, bool enable = true);
static void DrawRadio (const Widget_t *widget, uint8_t n_widget, uint8_t selected = 0);
static void DrawThumb (const Widget_t *widget, const char *path);
static void DrawVideo (const Widget_t *widget, const char *path);

/*--------------------------------------------------------------------------------
 * Sprite object
 *--------------------------------------------------------------------------------*/
#if defined (LOVYANGFX_HPP_)

static LGFX_Sprite sprite_draw(&lcd);

#elif defined (_TFT_eSPIH_)

static TFT_eSprite sprite_draw(&tft);

#endif

#ifdef _TFT_eSPIH_
/*--------------------------------------------------------------------------------
 * Helper function for TFT_eSPI
 * Include the PNG decoder library, available via the IDE library manager
 * https://github.com/Bodmer/TFT_eSPI/tree/master/examples/PNG%20Images
 * pngDraw: Callback function to draw pixels to the display
 *--------------------------------------------------------------------------------*/
#include <PNGdec.h>

static PNG png; // PNG decoder instance

// Position variables must be global (PNGdec does not handle position coordinates)
static uint16_t xpos = 0;
static uint16_t ypos = 0;

/*--------------------------------------------------------------------------------
 * This next function will be called during decoding of the png file to
 * render each image line to the TFT. If you use a different TFT library
 * you will need to adapt this function to suit.
 *--------------------------------------------------------------------------------*/
static void pngDraw(PNGDRAW *pDraw) {
  uint16_t lineBuffer[TFT_WIDTH > TFT_HEIGHT ? TFT_WIDTH : TFT_HEIGHT];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  GFX_EXEC(pushImage(xpos, ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer));
}

static void pngSprite(PNGDRAW *pDraw) {
  uint16_t lineBuffer[TFT_WIDTH > TFT_HEIGHT ? TFT_WIDTH : TFT_HEIGHT];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  sprite_draw.pushImage(xpos, ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}

static void DrawPNG(const uint8_t *img, size_t size, uint16_t x, uint16_t y, PNG_DRAW_CALLBACK *draw) {
  xpos = x;
  ypos = y;

  if (png.openFLASH((uint8_t*)img, size, draw) == PNG_SUCCESS) {
    GFX_EXEC(startWrite());
    png.decode(NULL, 0);
    GFX_EXEC(endWrite());
//  png.close(); // Required for files, not needed for FLASH arrays
  }
}
#endif // _TFT_eSPIH_

/*--------------------------------------------------------------------------------
 * Converting byte order according to MCU architecture
 *--------------------------------------------------------------------------------*/
#if defined (__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

#define SWAP_ENDIAN(type, a, b)  { type tmp = a; a = b; b = tmp; }

// Alternatives to std::byteswap (C++23)
// big-endian (PNG) --> little-endian (ESP32)
uint32_t swap_endian(uint32_t v) {
  uint8_t r[4];
  *(uint32_t*)r = v;
  SWAP_ENDIAN(uint8_t, r[0], r[3]);
  SWAP_ENDIAN(uint8_t, r[1], r[2]);
  return *(uint32_t*)r;
}

#else // __ORDER_BIG_ENDIAN__ or __ORDER_PDP_ENDIAN__ or not defined

#define swap_endian(v)  (v)

#endif // __BYTE_ORDER__

/*--------------------------------------------------------------------------------
 * Get width and height of PNG image
 *--------------------------------------------------------------------------------*/
#define PNG_HEADER_WIDTH    16 // PNG file signature + offset from chunk data
#define PNG_HEADER_HEIGHT   20 // PNG file signature + offset from chunk data

static inline uint32_t get_width (const uint8_t *data) __attribute__((always_inline));
static inline uint32_t get_height(const uint8_t *data) __attribute__((always_inline));
static inline uint32_t get_width (const uint8_t *data) { return swap_endian(*(uint32_t*)(data + PNG_HEADER_WIDTH )); }
static inline uint32_t get_height(const uint8_t *data) { return swap_endian(*(uint32_t*)(data + PNG_HEADER_HEIGHT)); }

/*--------------------------------------------------------------------------------
 * Marker icons
 *--------------------------------------------------------------------------------*/
#include "marker.h"

static constexpr Image_t icon_marker[] = {
  { icon_marker0, sizeof(icon_marker0) }, // 14 x 14
  { icon_marker1, sizeof(icon_marker1) }, // 16 x 16
};

/*--------------------------------------------------------------------------------
 * Draw marker with value
 *--------------------------------------------------------------------------------*/
static void DrawMarker(const Image_t *image, Temperature_t *temp) {
#define TEST_MARKER  false
#if TEST_MARKER
  temp->x = MLX90640_COLS / 2;
  temp->y = MLX90640_ROWS / 2;
#endif

  // check onMainInside() in widgets.hpp
  const int box = mlx_cnf.box_size * mlx_cnf.interpolation;
  const int W = box * (MLX90640_COLS - 1);
  const int Y = box * temp->y;
#if ENA_OUTWARD_CAMERA
  const int X = box * (MLX90640_COLS - 1 - temp->x);
#else
  const int X = box * temp->x;
#endif

  const int w = get_width (image->data);
  const int h = get_height(image->data);

#if   defined (LOVYANGFX_HPP_)

  sprite_draw.setPsram(true);
  sprite_draw.createSprite(w, h);
  sprite_draw.drawPng(image->data, image->size, 0, 0);
  sprite_draw.pushSprite(&lcd_sprite, X - (w >> 1), Y - (h >> 1), TFT_BLACK);
  sprite_draw.deleteSprite();

#elif defined (_TFT_eSPIH_)

  sprite_draw.createSprite(w, h);
  DrawPNG(image->data, image->size, 0, 0, pngSprite);
  sprite_draw.pushToSprite(&tft_sprite, X - (w >> 1), Y - (h >> 1), TFT_BLACK);
  sprite_draw.deleteSprite();

#endif

#if TEST_MARKER
  GFX_FAST(drawLine(0, 0, MLX90640_COLS * box - 1, MLX90640_ROWS * box - 1, TFT_RED));
  GFX_FAST(drawLine(MLX90640_COLS * box - 1, 0, 0, MLX90640_ROWS * box - 1, TFT_RED));
#endif

  char buf[BUF_SIZE];
  sprintf(buf, "%4.1f", temp->v);
  GFX_FAST(drawString(buf, constrain(X, w, W - w), Y + (temp->y < MLX90640_ROWS / 2 ? h : -h)));
}

/*--------------------------------------------------------------------------------
 * Draw temperature with markers and values
 *--------------------------------------------------------------------------------*/
void DrawTemperatureMarker(void) {
  const int box = mlx_cnf.box_size * mlx_cnf.interpolation;

  GFX_FAST(setTextSize(1));
  GFX_FAST(setTextDatum(CC_DATUM));
  GFX_EXEC(setClipRect(0, 0, MLX90640_COLS * box, MLX90640_ROWS * box));

  if (mlx_cnf.marker_mode & 1) {
    DrawMarker(&icon_marker[0], &_te_min);
    DrawMarker(&icon_marker[0], &_te_max);
  }

  if (mlx_cnf.marker_mode & 2) {
    DrawMarker(&icon_marker[1], &te_pick);
  }

  GFX_EXEC(clearClipRect());
}

/*--------------------------------------------------------------------------------
 * Draw temperature color bar and range
 *--------------------------------------------------------------------------------*/
void DrawTemperatureRange(uint8_t type) {
  const int box = mlx_cnf.box_size * mlx_cnf.interpolation;
  const int w = box * MLX90640_COLS;
  int       y = box * MLX90640_ROWS + 3;

  GFX_EXEC(startWrite());

  // Draw color bar
  if (type & 1) {
    const uint16_t *hm = heatmap[mlx_cnf.color_scheme];
 
    for (int i = 0; i < N_HEATMAP; ++i) {
      int x = map(i, 0, N_HEATMAP, 0, w);
#if defined (LOVYANGFX_HPP_)
      GFX_EXEC(writeFastVLine(x, y, FONT_HEIGHT, hm[i]));
#else
      GFX_EXEC(drawFastVLine(x, y, FONT_HEIGHT, hm[i]));
#endif
    }
  }

  // Draw thermal range
  if (type & 2) {
    y += FONT_HEIGHT + 4;
    const uint8_t size = (box > 4 ? 2 : 1);
    const int font_w = (size == 2 ? FONT_WIDTH  : FONT_WIDTH  >> 1);
    const int font_h = (size == 2 ? FONT_HEIGHT : FONT_HEIGHT >> 1);
    GFX_EXEC(setTextSize(size));

    GFX_EXEC(setTextDatum(TL_DATUM));
    gfx_printf(0, y, "%d  ", mlx_cnf.range_min);

    GFX_EXEC(setTextDatum(TR_DATUM));
    gfx_printf(w, y, " %d", mlx_cnf.range_max);

    if (box > 1) {
      GFX_EXEC(setTextDatum(TC_DATUM));
      gfx_printf(w / 2, y, " %3.1f ", (float)(mlx_cnf.range_min + mlx_cnf.range_max) / 2.0f);
    }
  }

  GFX_EXEC(endWrite());
  GFX_EXEC(setTextSize(2));
  GFX_EXEC(setTextDatum(TL_DATUM));
}

/*--------------------------------------------------------------------------------
 * Draw widget
 *--------------------------------------------------------------------------------*/
static void DrawWidget(const Widget_t *widget, uint8_t offset /* = 0 */) {
  const Image_t *image = &widget->image[offset];

  if (image) {
    GFX_EXEC(startWrite());

#if   defined (LOVYANGFX_HPP_)

    GFX_EXEC(drawPng(image->data, image->size, widget->x, widget->y));

#elif defined (_TFT_eSPIH_)

    DrawPNG(image->data, image->size, widget->x, widget->y, pngDraw);

#endif

    CHECK_WIDGET_AREA(drawRect(widget->x, widget->y, widget->w, widget->h, TFT_RED));
    GFX_EXEC(endWrite());
  }
}

/*--------------------------------------------------------------------------------
 * Draw screen
 *--------------------------------------------------------------------------------*/
static void DrawScreen(const Widget_t *widget) {
  DrawWidget(widget, 0);
}

/*--------------------------------------------------------------------------------
 * Draw a button-like icon
 *--------------------------------------------------------------------------------*/
static void DrawButton(const Widget_t *widget, uint8_t offset /* = 0 */) {
  DrawWidget(widget, offset);
}

/*--------------------------------------------------------------------------------
 * Draw slider
 *--------------------------------------------------------------------------------*/
static void DrawSlider(const Widget_t *widget, int16_t pos, bool enable /* = true */) {
  const Image_t *bar  = &widget->image[0];
  const Image_t *knob = &widget->image[enable ? 1 : 2]; // [1]: enable, [2]: disable

  if (bar && knob) {
    GFX_EXEC(startWrite());

#if   defined (LOVYANGFX_HPP_)

    sprite_draw.setPsram(true);
    sprite_draw.createSprite(get_width(bar->data), get_height(bar->data));
    sprite_draw.drawPng(bar->data,  bar->size,  0,   0);
    sprite_draw.drawPng(knob->data, knob->size, pos, 0);
    sprite_draw.pushSprite(widget->x, widget->y);
    sprite_draw.deleteSprite();

#elif defined (_TFT_eSPIH_)

    sprite_draw.createSprite(get_width(bar->data), get_height(bar->data));
    DrawPNG(bar->data,  bar->size,  0,   0, pngSprite);
    DrawPNG(knob->data, knob->size, pos, 0, pngSprite);
    sprite_draw.pushSprite(widget->x, widget->y);
    sprite_draw.deleteSprite();

#endif

    CHECK_WIDGET_AREA(drawRect(widget->x, widget->y, widget->w, widget->h, TFT_RED));
    GFX_EXEC(endWrite());
  }
}

static void DrawToggle(const Widget_t *widget, bool check /* = false */) {
  DrawButton(widget, check ? 1 : 0);
}

static void DrawCheck(const Widget_t *widget, bool check /* = false */) {
  DrawButton(widget, check ? 1 : 0);
}

static void DrawRadio(const Widget_t *widget, uint8_t n_widget, uint8_t selected /* = 0 */) {
  for (int i = 0; i < n_widget; ++i) {
    DrawButton(&widget[i], (int8_t)(i == selected));
  }
}

/*--------------------------------------------------------------------------------
 * Rendering the pressed effects
 *--------------------------------------------------------------------------------*/
#define PRESSED_BUFFER_LEN  64  // [px]
#define PRESSED_OFFSET      1   // 1 or 2 (need a wider background image)

static void DrawPress(const Widget_t *widget, Event_t event /* = EVENT_INIT */) {
  int16_t offset;
  const uint16_t x = widget->x;
  const uint16_t y = widget->y;
  const uint16_t w = widget->w;
  const uint16_t h = widget->h;
  const uint16_t d = PRESSED_OFFSET * 2;

#if   defined (LOVYANGFX_HPP_)

  lgfx::rgb888_t rgb[PRESSED_BUFFER_LEN];

#elif defined (_TFT_eSPIH_)

  uint16_t rgb[PRESSED_BUFFER_LEN];

#endif

  if (event & EVENT_FALLING) {
    offset = +PRESSED_OFFSET;
  } else if (event & EVENT_RISING) {
    offset = -PRESSED_OFFSET;
  } else {
    offset = 0;
  }

  if (w + d <= PRESSED_BUFFER_LEN) {
    GFX_EXEC(startWrite());

    if (offset >= 0) {
      for (int i = y + h + offset - 1; i >= y - offset; --i) {
        GFX_EXEC(readRect (x - offset, i,          w + d, 1, rgb));
        GFX_EXEC(pushImage(x + offset, i + offset, w + d, 1, rgb));
      }
    } else {
      for (int i = y - offset; i <= y + h + offset - 1; ++i) {
        GFX_EXEC(readRect (x - offset, i,          w + d, 1, rgb));
        GFX_EXEC(pushImage(x + offset, i + offset, w + d, 1, rgb));
      }
    }

    CHECK_WIDGET_AREA(drawRect(widget->x, widget->y, widget->w, widget->h, TFT_RED));
    GFX_EXEC(endWrite());
  }

  if (event & EVENT_RISING) {
    touch_clear();
  }
}

/*--------------------------------------------------------------------------------
 * Draw an image from a bitmap file
 *--------------------------------------------------------------------------------*/
static void DrawThumb(const Widget_t *widget, const char *path) {
  GFX_EXEC(fillRect(widget->x, widget->y, widget->w, widget->h, TFT_BLACK));

#if defined (LOVYANGFX_HPP_) && (defined (_SD_H_) || SD_FAT_VERSION <= 20203)

  GFX_EXEC(drawBmpFile(
    SD,
    path,
    widget->x, widget->y, widget->w, widget->h,
    0, 0,
    0.4, 0.4  // 320 x 240 --> 128 x 96
  ));

#else
#warning Requires implementation in DrawThumb()
#endif
}

/*--------------------------------------------------------------------------------
 * Render images from a raw file
 *--------------------------------------------------------------------------------*/
class MLXViewer {
 private:
  String path;
  const Widget_t *widget;
  uint32_t frameNo;
  uint32_t frameCount;
  const uint32_t frameSize = (MLX90640_COLS * MLX90640_ROWS * sizeof(float));

 public:
  MLXViewer() { path = ""; }

 private:
  bool read(int n) {
    File file = SD.open(path, FILE_READ);
    if (file) {
      file.seek(n * frameSize);
      uint32_t len = file.read((uint8_t*)src[0], frameSize);
      file.close();
      if (len == frameSize) {
        return true;
      }
    }
    return false;
  }

  void render(void) {
    if (read(frameNo)) {
      const int dst_rows = widget->h;
      const int dst_cols = widget->w;
      const uint16_t *hm = heatmap[mlx_cnf.color_scheme];

      // measure min/max temperature
      filter_temperature(src[0]);

      GFX_EXEC(startWrite());
      GFX_FAST(createSprite(dst_cols, dst_rows));

      interpolate_image(src[0], MLX90640_ROWS, MLX90640_COLS, dst_rows, dst_cols);

      GFX_FAST(pushSprite(widget->x, widget->y));
      GFX_FAST(deleteSprite());
      GFX_EXEC(endWrite());
    }
  }

 public:
  bool open(const Widget_t *_widget, const char *_path) {
    if (sdcard_mount()) { // already mounted in onFileManagerScreen()
      widget = _widget;
      path = String(_path);

      File file = SD.open(path, FILE_READ);
      if (file) {
        frameNo = 0;
        frameCount = file.size() / frameSize;
        file.close();
        DBG_EXEC(printf("%s: %d frames\n", _path, frameCount));
        render();
        return true;
      } else {
        DBG_EXEC(printf("%s: open error\n", _path));
      }
    }
    return false;
  }

  void close(void) {
    path = "";
    frameNo = 0;
    frameCount = 0;
  }

  bool isOpened(void) {
    return (path.length() != 0);
  }

  bool isTop(void) {
    return frameNo == 0;
  }

  bool isEnd(void) {
    return frameNo == frameCount - 1;
  }

  bool rewind(void) {
    if (isOpened()) {
      frameNo = 0;
      render();
      return true;
    } else {
      return false;
    }
  }

  bool next(void) {
    if (isOpened()) {
      if (frameNo < frameCount - 1) {
        ++frameNo;
        render();
      }
      return (frameNo < frameCount -1);
    } else {
      return false;
    }
  }

  bool prev(void) {
    if (isOpened()) {
      if (frameNo > 0) {
        frameNo--;
        render();
      }
      return (frameNo > 0);
    } else {
      return false;
    }
  }
};
