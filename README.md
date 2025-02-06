# MLX90640 Thermography Camera by XIAO ESP32S3

## Description

Program for thermal imaging camera with MLX90640 far-infrared (IR) thermal sensor srray for ESP32-2432S028R and XIAO ESP32S3

## Features

### Menu GUI

It comes with a graphical user interface to properly configure the MLX90640 for different scenarios.

![Configuration Menu](images/MLX90640-menu.jpg)

- Display frame rate: 1 FPS (min) / 16 FPS (max)
- Resolution: 32 x 24 (min) / 256 x 192 (max)
- Heat map mode: Rainbow / Inferno
- Measurement range: Auto scaling / Manual adjustment.
- Measurement features: Automatic min/max / a specified point.
- Screen capture: Save 320 x 240 bitmap (24-bit) to SD card.
- Video recording: Record 32 x 24 raw data continuously onto SD card.
- File explorer: Display thumbnails / Play videos on SD card.
- Offline video viewer: Offline video playback on PC
- Touch screen: Calibration on the first launch / in the configuration menu.

## Hardware Setup

<details>
<summary>ESP32-2432S028R</summary>

### Connecting MLX90640 to I2C
![Connecting MLX90640 to I2C with ESP32-2432S028R-MLX90640](images/CYD-CN1.jpg)

### Li-Po Battery
![Schematics](images/CYD-P5.jpg)
![Li-Po and Booster](images/CYD-LiPo.jpg)
</details>

<details>
<summary>XIAO ESP32S3</summary>

### Block Diagram
![Block Diagram](images/XIAO-BlockDiagram.jpg)

### Wiring
![Wiring](images/XIAO-MLX90640.jpg)
</details>

<details>
<summary>I2C pull-up registers</summary>

![with and without pull-up registers](images/MLX90640-SCL.jpg)
</details>

## Software Setup

### File Structure and Dependencies

```
MLX90640.ino
├─ pin_assign.h
├─ heatmap.h
│
├─ mlx.hpp
├─ gfx.hpp
│   ├─ LovyanGFX.h
│   │   └─ boards
│   │        ├─ LGFX_CYD_2432S028R.hpp
│   │        └─ LGFX_XIAO_ESP32S3_ST7789.hpp
│   └─ TFT_eSPI.h
│        ├─ User_Setup.h
│        └─ boards
│             ├─ Setup_CYD_2432S028R.h
│             └─ Setup_XIAO_ESP32S3_ST7789.h
├─ task.hpp
├─ touch.hpp
├─ sdcard.hpp
├─ filter.hpp
├─ interpolation.hpp
└─ widget.hpp
     ├─ draw.hpp
     │   └─ marker.h
     └─ widgets.hpp
         └─ widgets.h
```

### Configuration

1. Depending on the board type, define pinouts of the I2C for MLX90640 and SPI for LCD / touch screen / SD card in `pin_assign.h`.

2. Configure each step in `MLX90640.ino`
  - Step1: Debugging mode
  - Step2: Operational specifications
    - `ENA_INTERPOLATION`
    - `ENA_MULTITASKING`
    - `ENA_OUTWARD_CAMERA`
  - Step3: GFX library
    - `USE_LOVYAN_GFX`
        - `USE_AUTODETECT` or manual settings
    - `USE_TFT_ESPI`
        - define `User_Setup.h`
  - Step4: Flash memory
    - `USE_PREFERENCES`
  - Step5: Resolution
