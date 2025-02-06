# MLX90640 Thermography Camera for ESP32

## Description

Program for thermal imaging camera with MLX90640 far-infrared (IR) thermal sensor srray for XIAO ESP32S3 and ESP32-2432S028R

## Features

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

## Demo

### File Explorer
[![File Explorer](images/Demo-FileExplorer.gif)](https://youtu.be/9El-2NYCDNU)

### MLX90640Viewer
[![MLX90640Viewer](images/Demo-MLX90640Viewer.jpg)](https://youtu.be/FFbE1BNb92w)

## Hardware Setup

<details>
<summary>XIAO ESP32S3</summary>

### Block Diagram
![Block Diagram](images/XIAO-BlockDiagram.jpg)

### Wiring
![Wiring](images/XIAO-MLX90640.jpg)
</details>

<details>
<summary>ESP32-2432S028R</summary>

### Connecting MLX90640 to I2C
![Connecting MLX90640 to I2C with ESP32-2432S028R-MLX90640](images/CYD-CN1.jpg)

### Li-Po Battery
![Schematics](images/CYD-P5.jpg)
![Li-Po and Booster](images/CYD-LiPo.jpg)
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

### Dual-Core Timing Diagram
![Timing Diagram](images/TimingDiagram.png)

## Related Project
- [MLX90640Viewer](https://github.com/embedded-kiddie/MLX90640Viewer)  
    The MLX90640Viewer is a tool that applies heatmaps and visualizes the output of the Melexis MLX90640 32x24 IR array, which is stored as continuous raw data in a file.
