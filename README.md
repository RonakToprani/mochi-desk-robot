# Mochi Desk Robot

Autonomous desk pet running on the **Waveshare ESP32-S3-Touch-AMOLED-1.32** board.
Mochi reacts to touch, physical gestures, voice commands, and ambient sound — no WiFi required for core personality.

The firmware base is [xiaozhi-esp32](https://github.com/78/xiaozhi-esp32) (MIT license). Mochi-specific files in `main/mochi/` are additive hooks that integrate into the xiaozhi framework.

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
| Buttons | PWR (GPIO 17, long-press = shutdown) + BOOT (GPIO 0, download mode) |

### Display

| Parameter | Value |
|---|---|
| Type | AMOLED |
| Size | 1.32 inch round |
| Resolution | 466 x 466 pixels |
| Color Depth | 16.7M colors (24-bit RGB) |
| Driver IC | CO5300 (SH8601 compatible) |
| Interface | QSPI (Quad SPI) |
| Shape | Round (full circle) |

### Touch Controller

| Parameter | Value |
|---|---|
| Chip | CST820 |
| Interface | I2C (shared bus) |
| Address | 0x15 |
| Interrupt Pin | GPIO 16 |
| Capabilities | Single-touch + gesture detection (tap, swipe, long-press) |
| Zone Mapping | Full 466x466 circle (top/side/chin/center regions) |

### Audio Codec — ES8311

| Parameter | Value |
|---|---|
| Chip | ES8311 (low-power, high-quality) |
| Interface | I2S (data) + I2C (control, shared bus on GPIO 47/48) |
| Resolution | 16-bit audio |
| Sample Rate | 24 kHz (input and output) |
| Microphone | Onboard SMD MIC (I2S input on GPIO 40) |
| Speaker Out | MX1.25 2-PIN header (3-8 ohm passive speaker) |
| PA Enable | GPIO 46 (active HIGH) |

### IMU — QMI8658

| Parameter | Value |
|---|---|
| Chip | QMI8658 6-axis |
| Axes | 3-axis accelerometer + 3-axis gyroscope |
| Interface | I2C (shared bus) |
| Data Rate | Polled at ~50 Hz |
| Use Cases | Pick-up, tilt, shake, face-down sleep detection |

### Power & Battery

| Parameter | Value |
|---|---|
| Battery Header | MX1.25 2-PIN, 3.7V LiPo |
| Charging | Onboard charge management via USB-C |
| Power Button | GPIO 17 (long-press = shutdown) |
| Power Latch | GPIO 18 (PWR_EN, active HIGH) |

### Expansion

| Parameter | Value |
|---|---|
| Connector | SH1.0 12-PIN expansion port |
| Exposed I/O | Multiple configurable GPIOs, I2C, UART |
| SD Card | Not onboard (use PSRAM for buffers) |

### GPIO Pin Map (verified from config.h)

| Signal | GPIO | Peripheral | Notes |
|---|---|---|---|
| I2C SDA | 47 | ES8311 + CST820 + QMI8658 | Shared I2C bus |
| I2C SCL | 48 | ES8311 + CST820 + QMI8658 | Shared I2C bus |
| I2S MCLK | 38 | ES8311 audio | Master clock |
| I2S BCLK | 39 | ES8311 audio | Bit clock |
| I2S WS/LRCK | 41 | ES8311 audio | Word select |
| I2S DIN | 40 | ES8311 mic input | Microphone data |
| I2S DOUT | 42 | ES8311 speaker | Speaker data |
| PA Enable | 46 | ES8311 amplifier | Active HIGH |
| QSPI CS | 10 | CO5300 display | Chip select |
| QSPI CLK | 11 | CO5300 display | SPI clock |
| QSPI D0 | 12 | CO5300 display | Data line 0 |
| QSPI D1 | 13 | CO5300 display | Data line 1 |
| QSPI D2 | 14 | CO5300 display | Data line 2 |
| QSPI D3 | 15 | CO5300 display | Data line 3 |
| LCD RST | 8 | CO5300 display | Reset |
| Touch INT | 16 | CST820 | Interrupt |
| BOOT Button | 0 | — | Download mode / chat toggle |
| PWR Button | 17 | — | Long-press = shutdown |
| PWR Enable | 18 | — | Power latch (active HIGH) |

## Architecture

### Firmware Base

The firmware runs on **ESP-IDF v5.5.2** (NOT Arduino, NOT PlatformIO). The base is [xiaozhi-esp32](https://github.com/78/xiaozhi-esp32), a Chinese open-source AI assistant framework. Mochi hooks are additive — they don't replace xiaozhi functionality.

### Emotion Sync Flow (Session 2)

The emotion routing system ensures GIF animations switch BEFORE TTS audio begins.
Xiaozhi WebSocket protocol: the LLM emotion packet always precedes the TTS start packet.
Both are processed through Schedule() (FIFO deque), so ordering is preserved.

```
Server: {"type":"llm","text":"...","emotion":"smile"}
  → application.cc OnIncomingJson → EmotionFromString("smile") → kHappy
  → Schedule() → mochi_display_set_emotion(kHappy) → GIF_LOAD: happy.gif
  → (later) TTS start → STATE: speaking (GIF preserved)
```

### Device State to GIF Mapping

| Device State | Mochi GIF | Behavior |
|---|---|---|
| kDeviceStateIdle | idle.gif | Default resting face |
| kDeviceStateListening | listening.gif | Always override to listening |
| kDeviceStateConnecting | thinking.gif | Show thinking while connecting |
| kDeviceStateSpeaking | (no change) | Preserves LLM emotion GIF |

### FreeRTOS Task Layout

| Core | Task | Priority | Stack | Purpose |
|------|------|----------|-------|---------|
| Core 0 | mochi_display | — | — | GIF decode + LVGL rendering |
| Core 1 | mochi_audio | 5 | 6 KB | PCM playback, 16→24kHz resample |
| Core 1 | Audio service | — | — | Xiaozhi TTS output (shared I2S) |

### Memory Budget (8 MB PSRAM)

| Allocation | Size | Contents |
|---|---|---|
| GIF frame buffers | ~3 MB | Pre-decoded RGB565 frames for 5-6 animations |
| LVGL canvas buffer | ~0.9 MB | Double-buffered 466x466 RGB565 |
| Audio record buffer | ~128 KB | 16kHz 16-bit PCM, 4s window |
| ESP-SR model | ~1.5 MB | WakeNet9 + MultiNet6 in PSRAM |
| Sound assets | ~1 MB | Pre-decoded PCM clips |
| System + stack | ~512 KB | FreeRTOS stacks, WiFi, misc |

## Project Structure

```
xiaozhi-esp32/main/mochi/          ← Mochi-specific source (in xiaozhi tree)
├── mochi_emotion.h                 # 15-state MochiEmotion enum + string/emoji mappers
├── mochi_display.h/cc              # GIF display engine (FreeRTOS task, Core 0)
├── mochi_audio.h/cc                # PCM sound engine (FreeRTOS task, Core 1, 6KB stack)
└── mochi_personality.h/cc          # Emotion→sound mapping + purr_loop logic

xiaozhi-esp32/main/boards/waveshare/esp32-s3-touch-amoled-1.32/
├── config.h                        # All GPIO pin defines
└── esp32-s3-touch-amoled-1.32.cc   # Board init (calls mochi_*_init())

xiaozhi-esp32/main/assets/
├── lang_config.h                   # AUTO-GENERATED from locale JSON (do not edit!)
└── locales/en-US/language.json     # English UI strings (Listening..., Speaking..., etc.)

xiaozhi-esp32/scripts/
└── gen_lang.py                     # Generates lang_config.h from locale JSON

xiaozhi-esp32/tools/
├── generate_pcm_sounds.py          # Generates 14 synthetic PCM test tones
└── repack_assets.py                # Injects PCM files into mmap_assets binary

mochi-desk-robot/                   ← This repo (docs + standalone dev)
├── src/                            # Session 1 PlatformIO code (touch driver)
├── MILESTONES.md                   # Session build checklist
└── README.md                       # This file
```

## Building

### Prerequisites
- ESP-IDF v5.5.2 installed at `$HOME/esp/esp-idf`
- macOS with zsh (must use `bash -c` wrapper for IDF env)

### Build
```bash
bash -c 'cd $HOME/esp/esp-idf && . ./export.sh > /dev/null 2>&1 && \
  cd "<path-to>/xiaozhi-esp32" && idf.py build'
```

### Flash (app only — faster)
```bash
bash -c 'cd $HOME/esp/esp-idf && . ./export.sh > /dev/null 2>&1 && \
  python -m esptool --chip esp32s3 -p /dev/cu.usbmodem1101 -b 460800 \
  --before default_reset --after hard_reset \
  write_flash 0x20000 "<path-to>/xiaozhi-esp32/build/xiaozhi.bin"'
```

### Flash (full — app + assets)
```bash
bash -c 'cd $HOME/esp/esp-idf && . ./export.sh > /dev/null 2>&1 && \
  python -m esptool --chip esp32s3 -p /dev/cu.usbmodem1101 -b 460800 \
  --before default_reset --after hard_reset \
  write_flash 0x20000 "<path-to>/xiaozhi-esp32/build/xiaozhi.bin" \
  0x600000 "<path-to>/xiaozhi-esp32/build/generated_assets.bin"'
```

> **Note**: USB port name (`/dev/cu.usbmodem*`) can change after device reset. Always verify with `ls /dev/cu.usb*`.

### Critical sdkconfig Settings
These MUST be set correctly (defaults are wrong for our board):
```
CONFIG_BOARD_TYPE_WAVESHARE_ESP32_S3_TOUCH_AMOLED_1_32=y
CONFIG_LANGUAGE_EN_US=y
```

### Standalone PlatformIO Build (Session 1 touch dev only)
```bash
cd mochi-desk-robot && pio run && pio run --target upload
```

## Milestones

See [MILESTONES.md](MILESTONES.md) for the full M0-M9 build checklist.

## License

MIT
