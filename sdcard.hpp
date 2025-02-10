/*================================================================================
 * SD card utility
 *================================================================================*/
#include "FS.h"
#include "SD.h"
#include "SPI.h"

/*--------------------------------------------------------------------------------
 * SPI bus and Graphics library
 *--------------------------------------------------------------------------------*/
#if defined (_TFT_eSPIH_)
#define SD_CONFIG SD_CS, GFX_EXEC(getSPIinstance()), SPI_SD_FREQUENCY
#else
#define SD_CONFIG SD_CS, SPI, SPI_SD_FREQUENCY
#endif

// Temporary buffer size for string manipulation
#ifndef BUF_SIZE
#define BUF_SIZE  64
#endif

// Default directory and sequence number management file
#define MLX90640_DIR  String("/MLX90640")
#define MLX90640_NUM  String("/@number.txt")

/*--------------------------------------------------------------------------------
 * File name and size for GetFileList()
 *--------------------------------------------------------------------------------*/
#include <string>
#include <vector>
#include <exception>

typedef struct {
  std::string path;
  size_t      size;
  bool        isDirectory;
  bool        isSelected;
} FileInfo_t;

/*--------------------------------------------------------------------------------
 * Function prototyping
 *--------------------------------------------------------------------------------*/
bool sdcard_setup (void);
bool sdcard_mount (void);
bool sdcard_save  (void);
int  sdcard_fileno(void);
void sdcard_size  (uint32_t *total, uint32_t *free);

bool sdcard_record_begin(char *filename, size_t size);
bool sdcard_record_end(void);
bool sdcard_record(uint8_t *adrs, size_t size, char *filename);

void GetFileList  (fs::FS &fs, const char *dirname, uint8_t levels, std::vector<FileInfo_t> &files);
bool DeleteDir    (fs::FS &fs, const char *path);
void DeleteFile   (fs::FS &fs, const char *path);
bool SaveBMP24    (fs::FS &fs, const char *path, GFX_TYPE &tft);

/*--------------------------------------------------------------------------------
 * Sequence number management file
 *--------------------------------------------------------------------------------*/
static int GetFileNo(fs::FS &fs) {

  if (!fs.exists(MLX90640_DIR)) {
    DBG_EXEC(printf("Creating Dir: %s\n", MLX90640_DIR));
    if (fs.mkdir(MLX90640_DIR)) {
      DBG_EXEC(printf("done.\n"));
    } else {
      DBG_EXEC(printf("failed.\n"));
      return 0;
    }
  }

  int number = 0;
  String path = MLX90640_DIR + MLX90640_NUM;

  File file = fs.open(path, FILE_READ);
  if (file.available()) {
    String n = file.readString();
    number = n.toInt();
  }

  file.close();

  file = fs.open(path, FILE_WRITE);
  if (file.print(++number)) {
    DBG_EXEC(printf("Sequential number: %d\n", number));
  } else {
    DBG_EXEC(printf("Failed: %d\n", number));
  }

  file.close();
  return number;
}

/*--------------------------------------------------------------------------------
 * A function to get a list of files in a specified directory.
 *--------------------------------------------------------------------------------*/
void GetFileList(fs::FS &fs, const char *dirname, uint8_t levels, std::vector<FileInfo_t> &files) {
  File root = fs.open(dirname);
  if (!root) {
    DBG_EXEC(printf("Failed to open directory.\n"));
    return;
  }
  if (!root.isDirectory()) {
    DBG_EXEC(printf("Not a directory.\n"));
    return;
  }

  File file = root.openNextFile();
  while (file) {
    // DBG_EXEC(printf("%s\n", file.path()));
    bool isDir = file.isDirectory();

    // skip dot file
#ifdef USE_SDFAT
    char name[BUF_SIZE];
    file.getName(name, sizeof(name));
    if (file.isHidden()) {
      // DBG_EXEC(printf("%s is skipped.\n", name));
    }
#else
    const char *p = strrchr(file.path(), '/');
    if (p[p ? 1 : 0] == '.') {
      // DBG_EXEC(printf("%s is skipped.\n", file.path()));
    }
#endif

    else {
      // Add full path to vector
      // file.path(), file.name(), file.size()
      // https://cpprefjp.github.io/reference/exception/exception.html
      // https://stackoverflow.com/questions/27609839/about-c-vectorpush-back-exceptions-ellipsis-catch-useful
      try {
#ifdef USE_SDFAT
        files.push_back({"/" + std::string(dirname) + "/" + std::string(name), (size_t)file.fileSize(), isDir, false});
#else
        files.push_back({file.path(), file.size(), isDir, false});
#endif
      } catch (const std::exception &e) {
        DBG_EXEC(printf("Exception: %s\n", e.what()));
        return;
      }

      if (isDir && levels) {
#ifdef USE_SDFAT
        GetFileList(fs, name, levels - 1, files);
#else
        GetFileList(fs, file.path(), levels - 1, files);
#endif
      }
    }

    file = root.openNextFile();
  }
}

/*--------------------------------------------------------------------------------
 * Basic functions to delete a directory and a file.
 *--------------------------------------------------------------------------------*/
bool DeleteDir(fs::FS &fs, const char *path) {
  // 'path' must be empty
  if (fs.rmdir(path)) {
    DBG_EXEC(printf("Delete %s: done.\n", path));
    return true;
  } else {
    DBG_EXEC(printf("Delete %s: failed.\n", path));
    return false;
  }
}

void DeleteFile(fs::FS &fs, const char *path) {
  if (fs.remove(path)) {
    DBG_EXEC(printf("Delete %s: done.\n", path));
  } else {
    DBG_EXEC(printf("Delete %s: failed.\n", path));
  }
}

/*--------------------------------------------------------------------------------
 * Convert between RGB565 and RGB888
 *--------------------------------------------------------------------------------*/
#define RGB_CORRECTION    (0) // 0: normal, 1: for TFT_graphicstest_PDQ
#define RGB_SWAP(t, a, b) {t tmp = a; a = b; b = tmp;}
#define RGB565(r, g, b)   ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))

inline void color565toRGB(uint16_t color, uint8_t &r, uint8_t &g, uint8_t &b) __attribute__((always_inline));
inline void color565toRGB(uint16_t color, uint8_t &r, uint8_t &g, uint8_t &b) {
  r = (color>>8)&0x00F8;
  g = (color>>3)&0x00FC;
  b = (color<<3)&0x00F8;
}

/*--------------------------------------------------------------------------------
 * LCD screen capture to save image to SD card
 *--------------------------------------------------------------------------------*/
bool SaveBMP24(fs::FS &fs, const char *path, GFX_TYPE &tft) {
  uint32_t start = millis();

  uint32_t w = tft.width();
  uint32_t h = tft.height();

#if   defined (LOVYANGFX_HPP_)

  lgfx::rgb888_t rgb[w];

#elif defined (_TFT_eSPIH_)

  uint8_t rgb[w * 3];

#endif // LovyanGFX or TFT_eSPI

  File file = fs.open(path, FILE_WRITE);

  if (!file) {
    DBG_EXEC(printf("SD: open %s failed.\n", path));
    return false;
  } else {
    DBG_EXEC(printf("saving %s\n", path));
  }

  unsigned char bmFlHdr[14] = {
    'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0
  };

  // set color depth to 24 as we're storing 8 bits for r-g-b
  unsigned char bmInHdr[40] = {
    40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0,
  };

  unsigned long fileSize = 3ul * h * w + 54;

  bmFlHdr[ 2] = (unsigned char)(fileSize);
  bmFlHdr[ 3] = (unsigned char)(fileSize >> 8);
  bmFlHdr[ 4] = (unsigned char)(fileSize >> 16);
  bmFlHdr[ 5] = (unsigned char)(fileSize >> 24);

  bmInHdr[ 4] = (unsigned char)(w);
  bmInHdr[ 5] = (unsigned char)(w >> 8);
  bmInHdr[ 6] = (unsigned char)(w >> 16);
  bmInHdr[ 7] = (unsigned char)(w >> 24);
  bmInHdr[ 8] = (unsigned char)(h);
  bmInHdr[ 9] = (unsigned char)(h >> 8);
  bmInHdr[10] = (unsigned char)(h >> 16);
  bmInHdr[11] = (unsigned char)(h >> 24);

  file.write(bmFlHdr, sizeof(bmFlHdr));
  file.write(bmInHdr, sizeof(bmInHdr));

  for (int y = h - 1; y >= 0; --y) {
    if (y % 10 == 0) {
      DBG_EXEC(printf("."));
      yield(); // Prevent the watchdog from firing in core 0
    }

#if defined (LOVYANGFX_HPP_)

    tft.readRect(0, y, w, 1, rgb);

  #if RGB_CORRECTION
    for (int i = 0; i < w; ++i) {
      rgb[i].r <<= RGB_CORRECTION;
      rgb[i].g <<= RGB_CORRECTION;
      rgb[i].b <<= RGB_CORRECTION;
    }
  #endif

#else // TFT_eSPI

    tft.readRectRGB(0, y, w, 1, (uint8_t*)rgb);

    for (int i = 0; i < sizeof(rgb); i += 3) {
#if defined (TFT_RGB_ORDER) && (TFT_RGB_ORDER == TFT_BGR)
      RGB_SWAP(uint8_t, rgb[i+1], rgb[i+2]);
      rgb[i  ] <<= (RGB_CORRECTION + 1);
      rgb[i+1] <<= (RGB_CORRECTION + 1);
      rgb[i+2] <<= (RGB_CORRECTION + 1);
#else
      RGB_SWAP(uint8_t, rgb[i+0], rgb[i+2]);
  #if RGB_CORRECTION
      rgb[i  ] <<= RGB_CORRECTION;
      rgb[i+1] <<= RGB_CORRECTION;
      rgb[i+2] <<= RGB_CORRECTION;
  #endif
#endif
    }

#endif // LovyanGFX or TFT_eSPI

    int len = file.write((uint8_t*)rgb, sizeof(rgb));

    if (file.getWriteError() != 0) {
      DBG_EXEC(printf("SD write error: len: %d\n", len)); // getWriteError() returns 1
      file.close();
      return false;
    };
  }

  file.close();
  start = millis() - start;
  DBG_EXEC(printf("done (%d msec).\n", start));
  return true;
}

/*--------------------------------------------------------------------------------
 * Setup SD card (Mounting fails if card is not inserted)
 *--------------------------------------------------------------------------------*/
bool sdcard_setup(void) {
  static int8_t isMounted = false;

  if (isMounted == true) {
    return true;
  }

#if defined (ARDUINO_ESP32_2432S028R)
  // It works with LovyanGFX but not with TFT_eSPI.
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
#endif

  if (SD.begin(SD_CONFIG)) {
    DBG_EXEC(printf("SD card is successfully mounted.\n"));
  } else {
    DBG_EXEC(printf("Failed to mount SD card.\n"));
    return false;
  }

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    DBG_EXEC(printf("No SD card attached.\n"));
    return false;
  }

  DBG_EXEC(printf("SD card type: "));
  if (cardType == CARD_MMC) {
    DBG_EXEC(printf("MMC\n"));
  } else if (cardType == CARD_SD) {
    DBG_EXEC(printf("SDSC\n"));
  } else if (cardType == CARD_SDHC) {
    DBG_EXEC(printf("SDHC\n"));
  } else {
    DBG_EXEC(printf("UNKNOWN\n"));
  }

  return (isMounted = true);
}

bool sdcard_mount(void) {
  return sdcard_setup();
}

int sdcard_fileno(void) {
  if (!sdcard_mount()) {
    return 0;
  }

  return GetFileNo(SD);
}

void sdcard_size(uint32_t *total, uint32_t *free) {
#ifdef USE_SDFAT
  *total = (uint32_t)(0.000512 * (uint32_t)SD.card()->sectorCount() + 0.5);
  *free  = (uint32_t)((SD.vol()->bytesPerCluster() * SD.vol()->freeClusterCount()) / (1024 * 1024));
#else
  *total = (uint32_t)(SD.totalBytes() / (1024 * 1024));
  *free  = (uint32_t)(*total - SD.usedBytes()  / (1024 * 1024));
#endif
}

bool sdcard_save(void) {
  DBG_EXEC(uint32_t start = millis());

  if (!sdcard_mount()) {
    return false;
  }

  int no = GetFileNo(SD);
  char path[BUF_SIZE];
  sprintf(path, "%s/mlx%04d.bmp", MLX90640_DIR, no);

  if (!SaveBMP24(SD, path, GFX_INSTANCE)) {
    return false;
  }

  DBG_EXEC(printf("Elapsed time: %d msec\n", millis() - start));
  DBG_EXEC({
    std::vector<FileInfo_t> files;
    GetFileList(SD, "/", 1, files);
    for (const auto& file : files) {
      printf("%s, %lu\n", file.path.c_str(), file.size);
    }
  });

  // SD.end(); // Activating this line will cause some GFX libraries to stop working.

  DBG_EXEC({
    uint32_t total; uint32_t free;
    sdcard_size(&total, &free);
    printf("Card size: %luMB\nFree size: %luMB\n", total, free);
  });

  return true;
}

/*--------------------------------------------------------------------------------
 * Video recording
 *--------------------------------------------------------------------------------*/
bool sdcard_record_begin(char *filename, size_t size) {
  int no = sdcard_fileno();
  if (no) {
    snprintf(filename, size, "%s/mlx%04d.raw", MLX90640_DIR, no);
    return true;
  }
  return false;
}

bool sdcard_record_end(void) {
  // SD.end(); // Activating this line will cause some GFX libraries to stop working.
  return true;
}

bool sdcard_record(uint8_t *adrs, size_t size, char *filename) {
//DBG_EXEC(uint32_t start = millis());

  File file = SD.open(filename, FILE_APPEND);
  int len = file.write(adrs, sizeof(float) * MLX90640_ROWS * MLX90640_COLS);
  file.close();

//DBG_EXEC(printf("Saved %d bytes, Elapsed time: %d msec\n", len, millis() - start));
  return true;
}
