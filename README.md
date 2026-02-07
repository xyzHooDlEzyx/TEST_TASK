# Weather Station

This project is a weather station with an LTE module for displaying current weather in Kyiv and Lviv.

## Overview

The firmware runs on an STM32F411 and uses a SIM800C LTE module to fetch weather data. An SSD1306 OLED is used for on-device display.

## Hardware

- STM32F411
- SIM800C (LTE/2G module)
- SSD1306 OLED display
- UART connection between MCU and SIM800C

## Build

This project uses CMake presets.

```bash
./build.sh
```

To clean and rebuild, use:

```bash
./rebuild.sh
```

## Configuration

- APN and provider settings are configured in the source code.
- UART instance and baud rate are configured in STM32CubeMX and generated files.
- City list is configured in the application logic.

## Usage

After flashing the firmware, the display shows the current weather for Kyiv and Lviv. The data is updated periodically via the LTE module.

## Project Structure

- Core/Src: application code
- Core/Inc: headers
- Drivers: STM32 HAL and CMSIS
- cmake: toolchain and CubeMX integration
