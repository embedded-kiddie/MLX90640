/*--------------------------------------------------------------------------------
 * MLX90640 instance
 *--------------------------------------------------------------------------------*/
#include <Wire.h>
#include <Adafruit_MLX90640.h>
Adafruit_MLX90640 mlx;

/*--------------------------------------------------------------------------------
 * The size of thermal image (max INTERPOLATE_SCALE = 8)
 *--------------------------------------------------------------------------------*/
#define MLX90640_COLS 32
#define MLX90640_ROWS 24
#define INTERPOLATED_COLS (MLX90640_COLS * INTERPOLATE_SCALE)
#define INTERPOLATED_ROWS (MLX90640_ROWS * INTERPOLATE_SCALE)

/*--------------------------------------------------------------------------------
 * Thermography image
 *--------------------------------------------------------------------------------*/
#if ENA_MULTITASKING
float src[2][MLX90640_ROWS  * MLX90640_COLS];
#else
float src[1][MLX90640_ROWS  * MLX90640_COLS];
#endif

/*--------------------------------------------------------------------------------
 * Initial values for range of temperature
 *--------------------------------------------------------------------------------*/
#define MINTEMP 20  // Low  temperature range
#define MAXTEMP 35  // High temperature range

/*--------------------------------------------------------------------------------
 * MLX90640 control configuration
 *--------------------------------------------------------------------------------*/
typedef struct MLXConfig {
  // Member Variables
  uint8_t   interpolation;    // 1, 2, 4, 6, 8
  uint8_t   box_size;         // 1, 2, 4, 8
  uint8_t   refresh_rate;     // sampline frequency (mlx90640_refreshrate_t)
  uint8_t   color_scheme;     // 0: rainbow, 1: inferno
  uint8_t   marker_mode;      // 0: off, 1: min/max, 2: picked up by user
  bool      range_auto;       // automatic measurement of temperature min/max
  int16_t   range_min;        // minimum temperature
  int16_t   range_max;        // maximum temperature
  float     sampling_period;  // sampling period [sec]

  // Comparison Operators
  bool operator >= (const MLXConfig &RHS) {
    return (
      (interpolation != RHS.interpolation) ||
      (box_size      != RHS.box_size     ) ||
      (refresh_rate  != RHS.refresh_rate )
    );
  }
  bool operator != (const MLXConfig &RHS) {
    return (
      (color_scheme  != RHS.color_scheme ) ||
      (marker_mode   != RHS.marker_mode  ) ||
      (range_auto    != RHS.range_auto   ) ||
      (range_min     != RHS.range_min    ) ||
      (range_max     != RHS.range_max    )
    );
  }
  // Copy some members
  MLXConfig operator <<= (const MLXConfig &RHS) {
    color_scheme = RHS.color_scheme;
    marker_mode  = RHS.marker_mode;
    range_auto   = RHS.range_auto;
    range_min    = RHS.range_min;
    range_max    = RHS.range_max;
    return RHS;
  };

  // Setup refresh_rate according to INTERPOLATE_SCALE and BOX_SIZE
  void init(void) {
    sampling_period = 2.0f / pow(2.0f, (float)(refresh_rate - 1));

    void filter_reset(void);
    filter_reset();
  }
} MLXConfig_t;

constexpr MLXConfig_t mlx_ini = {
  .interpolation  = INTERPOLATE_SCALE,  // 1, 2, 4, 6, 8
  .box_size       = BOX_SIZE,           // 1, 2, 4, 8
  .refresh_rate   = REFRESH_RATE,       // sampline frequency (32Hz, 16Hz, 8Hz, 4Hz, 2Hz)
  .color_scheme   = 0,                  // 0: rainbow, 1: inferno
  .marker_mode    = 0,                  // 0: off, 1: min/max, 2: picked up by user
  .range_auto     = false,              // automatic measurement of temperature min/max
  .range_min      = MINTEMP,            // minimum temperature (20 deg)
  .range_max      = MAXTEMP,            // maximum temperature (35 deg)
};

MLXConfig_t mlx_cnf = mlx_ini;

/*--------------------------------------------------------------------------------
 * MLX90640 capture control
 *--------------------------------------------------------------------------------*/
typedef struct MLXCapture {
  uint8_t       capture_mode; // 0: camera, 1: video
  bool          recording;    // false: stop, true: recording
  char          filename[30]; // "/MLX90640/mlx%04d.raw"
} MLXCapture_t;

MLXCapture_t mlx_cap = {
  .capture_mode   = 0,        // 0: camera, 1: video
  .recording      = false,    // false: stop, true: recording
};

/*--------------------------------------------------------------------------------
 * Set MLX90640 refresh rate
 *--------------------------------------------------------------------------------*/
bool mlx_refresh(void) {
  // The initial value 64Hz is out of range
  static uint8_t refreshRate = MLX90640_64_HZ;

  // Refresh MLX90640 when refresh rate changes
  if (refreshRate != mlx_cnf.refresh_rate) {
    mlx.setRefreshRate((mlx90640_refreshrate_t)(refreshRate = mlx_cnf.refresh_rate));
    mlx_cnf.init();
    delay(100); // Wait a bit for the change to take effect
    return true;
  } else {
    return false;
  }
}

/*--------------------------------------------------------------------------------
 * MLX90640 initializing
 *--------------------------------------------------------------------------------*/
void mlx_setup(void) {
#ifdef  MLX_I2C_SDA // defined in pin_assign.h
  Wire.begin(MLX_I2C_SDA, MLX_I2C_SCL);
#else
  Wire.begin();
#endif

  // Initialize MLX90640 with I2C
  if (!mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
    DBG_EXEC(printf("MLX90640 not found!\n"));
  } else {
    DBG_EXEC(printf("MLX90640 found at I2C address 0x%X.\n", MLX90640_I2CADDR_DEFAULT));
    DBG_EXEC(printf("Serial number: %04X%04X%04X\n", mlx.serialNumber[0], mlx.serialNumber[1], mlx.serialNumber[2]));
  }

  // I2C bus clock for MLX90640 (Officially, the ESP32S3 supports up to 800KHz)
  Wire.setClock(1000000); // 400 KHz (Sm) or 1 MHz (Fm+)

  // Set MLX90640 operating mode
  mlx.setMode(MLX90640_CHESS); // or MLX90640_INTERLEAVED
  mlx.setResolution(MLX90640_ADC_18BIT);  // 16BIT, 17BIT, 18BIT (default) or 19BIT
  mlx_refresh();
}
