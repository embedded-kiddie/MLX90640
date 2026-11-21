/*================================================================================
 * Pixel interpolation
 *================================================================================*/
inline float get_point(float *p, const int rows, const int cols, int x, int y) __attribute__((always_inline));
inline float get_point(float *p, const int rows, const int cols, int x, int y) {
//if (x < 0) { x = 0; } else
  if (x >= cols) { x = cols - 1; }
//if (y < 0) { y = 0; } else
  if (y >= rows) { y = rows - 1; }
  return p[y * cols + x];
}

/*--------------------------------------------------------------------------------------------
 * Bilinear interpolation
 * https://algorithm.joho.info/image-processing/bi-linear-interpolation/
 *--------------------------------------------------------------------------------------------*/
static float table_ratio[INTERPOLATE_SCALE][2];

void interpolate_setup(const int scale) {
  for (int i = 0; i < scale; ++i) {
    table_ratio[i][0] = (float)i / (float)scale;
    table_ratio[i][1] = 1.0f - table_ratio[i][0];
  }
}

/*--------------------------------------------------------------------------------------------
 * A macro that simplifies the `map()` function
 *
 * The original `map()` is defined as follows:
 * https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/WMath.cpp#L72
 *  long map(long x, long in_min, long in_max, long out_min, long out_max) {
 *    const long run = in_max - in_min;
 *    if (run == 0) {
 *      log_e("map(): Invalid input range, min == max");
 *      return -1;  // AVR returns -1, SAM returns 0
 *    }
 *    const long rise = out_max - out_min;
 *    const long delta = x - in_min;
 *    return (delta * rise) / run + out_min;
 *  }
 *--------------------------------------------------------------------------------------------*/
#define MAP(x, in_min, in_max, out_min, out_max)  \
  (((long)(x - in_min)) * (long)(out_max /* - out_min */) / (long)(in_max - in_min) /* + (long)out_min */)

/*--------------------------------------------------------------------------------------------
 * Improve performance by separating the loops for each rendering function
 *--------------------------------------------------------------------------------------------*/
#define LOOP(render) \
  for (int y = 0; y < src_rows; ++y) {\
    Y = y * scale;\
\
    for (int x = 0; x < src_cols; ++x) {\
      X = x * scale;\
\
      X0Y0 = get_point(src, src_rows, src_cols, x,     y    );\
      X1Y0 = get_point(src, src_rows, src_cols, x + 1, y    );\
      X0Y1 = get_point(src, src_rows, src_cols, x,     y + 1);\
      X1Y1 = get_point(src, src_rows, src_cols, x + 1, y + 1);\
\
      for (int j = 0; j < scale; ++j) {\
        y_ratio_lo = table_ratio[j][0];\
        y_ratio_hi = table_ratio[j][1];\
\
        v0 = y_ratio_hi * X0Y0;\
        v1 = y_ratio_hi * X1Y0;\
        w0 = y_ratio_lo * X0Y1;\
        w1 = y_ratio_lo * X1Y1;\
\
        for (int i = 0; i < scale; ++i) {\
          x_ratio_lo = table_ratio[i][0];\
          x_ratio_hi = table_ratio[i][1];\
\
          t = v0 * x_ratio_hi + v1 * x_ratio_lo +\
              w0 * x_ratio_hi + w1 * x_ratio_lo;\
\
          colorIndex = MAP(t, r_min, r_max, 0, N_HEATMAP - 1);\
          colorIndex = constrain(colorIndex, 0, N_HEATMAP - 1);\
\
          render;\
        }\
      }\
    }\
    yield();\
  }

void interpolate_image(float *src, const int src_rows, const int src_cols, const int dst_rows, const int dst_cols) {
  int X, Y, colorIndex;
  float X0Y0, X1Y0, X0Y1, X1Y1;
  float x_ratio_lo, x_ratio_hi;
  float y_ratio_lo, y_ratio_hi;
  float t, v0, v1, w0, w1;
  const int r_min = mlx_cnf.range_min;
  const int r_max = mlx_cnf.range_max;
  const int scale = dst_rows / src_rows;
  const int box_size = mlx_cnf.box_size;
  const uint16_t *hm = heatmap[mlx_cnf.color_scheme];

  // Bilinear interpolation
  if (IS_SELFIE(mlx_cap)) {
    if (box_size == 1) {
      LOOP(GFX_FAST(drawPixel(X + i, Y + j, hm[colorIndex])));
    } else {
      LOOP(GFX_FAST(fillRect((X + i) * box_size, (Y + j) * box_size, box_size, box_size, hm[colorIndex])));
    }
  } else {
    if (box_size == 1) {
      LOOP(GFX_FAST(drawPixel(dst_cols - 1 - (X + i), (Y + j), hm[colorIndex])));
    } else {
      LOOP(GFX_FAST(fillRect((dst_cols - 1 - (X + i)) * box_size, (Y + j) * box_size, box_size, box_size, hm[colorIndex])));
    }
  }
}
