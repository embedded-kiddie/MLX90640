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

#define FURTHER_OPTIMIZATION  true // Is the compiler smart enough?

void interpolate_image(float *src, const int src_rows, const int src_cols, const int dst_rows, const int dst_cols) {
  int X, Y;
  float X0Y0, X1Y0, X0Y1, X1Y1;
  float x_ratio_lo, x_ratio_hi;
  float y_ratio_lo, y_ratio_hi;
  const int scale = dst_rows / src_rows;
#if FURTHER_OPTIMIZATION
  float v0, v1, w0, w1;
#endif
  const int box_size = mlx_cnf.box_size;
  const uint16_t *hm = heatmap[mlx_cnf.color_scheme];

  // Bilinear interpolation
  for (int y = 0; y < src_rows; ++y) {
    Y = y * scale;

    for (int x = 0; x < src_cols; ++x) {
      X = x * scale;

      X0Y0 = get_point(src, src_rows, src_cols, x,     y    );
      X1Y0 = get_point(src, src_rows, src_cols, x + 1, y    );
      X0Y1 = get_point(src, src_rows, src_cols, x,     y + 1);
      X1Y1 = get_point(src, src_rows, src_cols, x + 1, y + 1);

      for (int j = 0; j < scale; ++j) {
        y_ratio_lo = table_ratio[j][0];
        y_ratio_hi = table_ratio[j][1];

#if FURTHER_OPTIMIZATION
        v0 = y_ratio_hi * X0Y0;
        v1 = y_ratio_hi * X1Y0;
        w0 = y_ratio_lo * X0Y1;
        w1 = y_ratio_lo * X1Y1;
#endif
        for (int i = 0; i < scale; ++i) {
          x_ratio_lo = table_ratio[i][0];
          x_ratio_hi = table_ratio[i][1];

#if FURTHER_OPTIMIZATION
          float t = v0 * x_ratio_hi + v1 * x_ratio_lo +
                    w0 * x_ratio_hi + w1 * x_ratio_lo;
#else
          float t = y_ratio_hi * (x_ratio_hi * X0Y0 + x_ratio_lo * X1Y0) +
                    y_ratio_lo * (x_ratio_hi * X0Y1 + x_ratio_lo * X1Y1);
#endif
          t = min((int)t, (int)mlx_cnf.range_max);
          t = max((int)t, (int)mlx_cnf.range_min); 

          int colorIndex = map(t, mlx_cnf.range_min, mlx_cnf.range_max, 0, N_HEATMAP - 1);

#if ENA_OUTWARD_CAMERA
          if (box_size == 1) {
            GFX_FAST(drawPixel(dst_cols - 1 - (X + i), (Y + j), hm[colorIndex]));
          } else {
            GFX_FAST(fillRect((dst_cols - 1 - (X + i)) * box_size, (Y + j) * box_size, box_size, box_size, hm[colorIndex]));
          }
#else // Selfie Camera
          if (box_size == 1) {
            GFX_FAST(drawPixel(X + i, Y + j, hm[colorIndex]));
          } else {
            GFX_FAST(fillRect((X + i) * box_size, (Y + j) * box_size, box_size, box_size, hm[colorIndex]));
          }
#endif
        }
      }
    }
    yield(); // Prevent the watchdog from firing
  }
}
