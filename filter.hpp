/*================================================================================
 * Multitasking helper functions
 *================================================================================*/
#define TIME_CONSTANT   3.0f // [sec]

/*--------------------------------------------------------------------------------
 * Low pass filter (x: input, T: sampling time [sec])
 *--------------------------------------------------------------------------------*/
typedef struct {
  float y;
  float filter(float x, const float T) {
    return (y += (T / (T + TIME_CONSTANT) * (x - y)));
  }
} LowPass_t;

static LowPass_t lp_min, lp_max, lp_pick;

/*--------------------------------------------------------------------------------
 * Set the initial value for the low-pass filter
 *--------------------------------------------------------------------------------*/
void filter_reset(void) {
  lp_min.y  = MINTEMP;
  lp_max.y  = MAXTEMP;
  lp_pick.y = (MINTEMP + MAXTEMP) / 2;
}

/*--------------------------------------------------------------------------------
 * Measure temperature and update it's range & min/max points
 *--------------------------------------------------------------------------------*/
#if false
#define CHECK_VALUE
#include <math.h>
#endif

typedef struct {
  uint16_t  x, y;
  float     v;
} Temperature_t;

static Temperature_t te_min, te_max, _te_min, _te_max, te_pick;

void filter_update(void) {
  _te_min = te_min;
  _te_max = te_max;
}

void filter_temperature(float *src) {
  // Measure the temperature at the picked up point
  if (te_pick.x != 0 || te_pick.y != 0) {
    te_pick.v = src[te_pick.x + (te_pick.y * MLX90640_COLS)];
    te_pick.v = lp_pick.filter(te_pick.v, mlx_cnf.sampling_period);
  }

  // Measure min/max temperature
  te_min.v =  999.0f;
  te_max.v = -999.0f;

  for (uint16_t y = 0; y < MLX90640_ROWS; ++y) {
    for (uint16_t x = 0; x < MLX90640_COLS; ++x, ++src) {
      float t = *src;
#ifdef  CHECK_VALUE
      if (isinf(t) || isnan(t) || t < -30.0f || 300.0f < t) {
        continue;
      }
#endif
      if (t < te_min.v) { te_min.x = x; te_min.y = y; te_min.v = t; } else
      if (t > te_max.v) { te_max.x = x; te_max.y = y; te_max.v = t; }
    }
  }

  if (mlx_cnf.range_auto) {
    #define RANGE_STEP  (1) // --> 'TERMOGRAPH_STEP' in widgets.hpp
    mlx_cnf.range_min = ((int)((float)lp_min.filter(te_min.v, mlx_cnf.sampling_period) / (float)RANGE_STEP) + 0.5f) * RANGE_STEP;
    mlx_cnf.range_max = ((int)((float)lp_max.filter(te_max.v, mlx_cnf.sampling_period) / (float)RANGE_STEP) + 0.5f) * RANGE_STEP;

    // debug for serial ploter
    // DBG_EXEC(printf("%4.1f, %4.1f, %4.1f, %4.1f\n", te_min.v, te_max.v, lp_min.y, lp_max.y));
  }
}
