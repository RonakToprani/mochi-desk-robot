# Mochi Desk Robot

Autonomous desk pet running on the **Waveshare ESP32-S3-Touch-AMOLED-1.32** board.
Mochi reacts to touch, physical gestures, voice commands, and ambient sound — no WiFi required for core personality.

## Hardware Spec

### Core SoC

| Parameter | Value |
|---|---|
| Chip | ESP32-S3-PICO-1-N8R8 |
| Architecture | Xtensa 32-bit LX7 Dual-Core |
| Clock | Up to 240 MHz |
| Internal SRAM | 512 KB |
| Internal ROM | 384 KB |
| PSRAM (external) | 8 MB (octal SPI) |
| Flash (external) | 8 MB |
| Wi-Fi | 2.4 GHz 802.11 b/g/n |
| Bluetooth | BLE 5.0, onboard ceramic antenna |
| USB | Type-C (programming + log via CH343P) |
| Buttons | PWR (customizable) + BOOT (download mode) |

### Display

| Parameter | Value |
|---|---|
| Type | AMOLED |
| Size | 1.32 inch round |
| Resolution | 466 x 466 pixels |
| Color Depth | 16.7M colors (24-bit RGB) |
| Driver IC | CO5300 |
| Interface | QSPI (Quad SPI) |
| Shape | Round (full circle) |

### Touch Controller

| Parameter | Value |
|---|---|
| Chip | CST820 |
| Interface | I2C |
| Address | 0x15 |
| Interrupt Pin | GPIO 16 |
| Capabilities | Single-touch + gesture detection (tap, swipe, long-press) |
| Zone Mapping | Full 466x466 circle (top/side/chin/center regions) |

### Audio Codec — ES8311

| Parameter | Value |
|---|---|
| Chip | ES8311 (low-power, high-quality) |
| Interface | I2S (data) + I2C (control) |
| Resolution | 16-bit audio |
| Sample Rate | 16 kHz (speech) / 44.1 kHz (music) |
| Microphone | Onboard SMD MIC (PDM/I2S input) |
| Speaker Out | MX1.25 2-PIN header (3-8 ohm passive speaker) |

### IMU — QMI8658

| Parameter | Value |
|---|---|
| Chip | QMI8658 6-axis |
| Axes | 3-axis accelerometer + 3-axis gyroscope |
| Interface | I2C |
| Data Rate | Polled at ~50 Hz |
| Use Cases | Pick-up, tilt, shake, face-down sleep detection |

### Power & Battery

| Parameter | Value |
|---|---|
| Battery Header | MX1.25 2-PIN, 3.7V LiPo |
| Charging | Onboard charge management via USB-C |
| Power Button | PWR button (programmable) |

### Expansion

| Parameter | Value |
|---|---|
| Connector | SH1.0 12-PIN expansion port |
| Exposed I/O | Multiple configurable GPIOs, I2C, UART |
| SD Card | Not onboard (use PSRAM for buffers) |

### GPIO Pin Map

| Signal | GPIO | Peripheral | Notes |
|---|---|---|---|
| I2C SDA | 6 | CST820 + QMI8658 + ES8311 | Shared I2C bus |
| I2C SCL | 7 | CST820 + QMI8658 + ES8311 | Shared I2C bus |
| QSPI CS | 11 | CO5300 display | |
| QSPI SCK | 9 | CO5300 display | |
| QSPI D0 | 14 | CO5300 display | |
| QSPI D1 | 13 | CO5300 display | |
| QSPI D2 | 10 | CO5300 display | |
| QSPI D3 | 12 | CO5300 display | |
| Display RST | 17 | CO5300 display | |
| Display TE | 8 | CO5300 display | |
| Touch INT | 16 | CST820 interrupt | |
| I2S BCLK | **TBD** | ES8311 audio | Confirm from schematic |
| I2S LRCK | **TBD** | ES8311 audio | Confirm from schematic |
| I2S DOUT | **TBD** | ES8311 speaker | Confirm from schematic |
| I2S DIN | **TBD** | ES8311 mic | Confirm from schematic |

> I2S pin numbers must be verified from the [official schematic PDF](https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.32) before writing audio init code.

## Architecture

### Two-Core Task Split (FreeRTOS)

| Core | Responsibility | Priority |
|---|---|---|
| Core 0 | Display rendering (LVGL + GIF decode + animations) | High |
| Core 1 | Sensors: IMU, touch, mic RMS, boredom timer, phrases, state machine | High |

### Memory Budget (8 MB PSRAM)

| Allocation | Size | Contents |
|---|---|---|
| GIF frame buffers | ~3 MB | Pre-decoded RGB565 frames for 5-6 animations |
| LVGL canvas buffer | ~0.9 MB | Double-buffered 466x466 RGB565 |
| Audio record buffer | ~128 KB | 16kHz 16-bit PCM, 4s window |
| ESP-SR model | ~1.5 MB | WakeNet9 + MultiNet6 in PSRAM |
| Sound assets | ~1 MB | Pre-decoded PCM clips |
| System + stack | ~512 KB | FreeRTOS stacks, WiFi, misc |

## Library Stack

| Library | Role |
|---|---|
| [Arduino_GFX](https://github.com/moononournation/Arduino_GFX) | CO5300 QSPI AMOLED display driver |
| [LovyanGFX](https://github.com/lovyan03/LovyanGFX) | Alternative display driver (smoother DMA) |
| [LVGL](https://lvgl.io) | GUI framework (v8/v9) |
| [AnimatedGIF](https://github.com/bitbank2/AnimatedGIF) | Runtime GIF decoder |
| [SensorLib](https://github.com/lewisxhe/SensorsLib) | QMI8658 IMU driver |
| ESP-IDF I2S driver | Built-in I2S peripheral config |
| [ESP-SR](https://github.com/espressif/esp-sr) | WakeNet9 + MultiNet6 speech recognition |

## Project Structure

```
mochi-desk-robot/
├── config/
│   └── pins.h              # Single source of truth for all GPIO defines
├── src/
│   ├── main.cpp             # Entry point
│   ├── emotion_state.h/cpp  # Personality & emotion state machine
│   ├── display.h/cpp        # AMOLED display + LVGL + GIF playback
│   ├── touch.h/cpp          # CST820 touch zones & gestures
│   ├── imu.h/cpp            # QMI8658 accelerometer/gyroscope
│   ├── audio.h/cpp          # ES8311 codec (speaker + mic)
│   └── phrase_detect.h/cpp  # ESP-SR wake word + commands
├── assets/
│   ├── sounds/              # PCM sound clips
│   └── gifs/                # Mochi expression GIFs
├── platformio.ini           # Build configuration
├── MILESTONES.md            # M0-M9 build checklist
└── README.md                # This file
```

## Building

Requires [PlatformIO](https://platformio.org/).

```bash
# Build (do not flash yet — verify I2S pins from schematic first)
pio run

# Flash
pio run --target upload

# Serial monitor
pio device monitor
```

## Milestones

See [MILESTONES.md](MILESTONES.md) for the full M0-M9 build checklist.

## License

MIT
