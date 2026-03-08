#ifndef PINS_H
#define PINS_H

// =============================================================================
// pins.h — Single source of truth for ALL GPIO assignments
// Mochi Desk Robot • Waveshare ESP32-S3-Touch-AMOLED-1.32
// =============================================================================

// -----------------------------------------------------------------------------
// Shared I2C Bus (Touch CST820 + IMU QMI8658 + Audio Codec ES8311)
// -----------------------------------------------------------------------------
#define PIN_I2C_SDA         6
#define PIN_I2C_SCL         7

// -----------------------------------------------------------------------------
// Display — CO5300 QSPI AMOLED (466x466)
// Managed by Arduino_GFX / LovyanGFX driver — pins set in display init
// -----------------------------------------------------------------------------
#define PIN_DISPLAY_CS      11
#define PIN_DISPLAY_SCK     9
#define PIN_DISPLAY_D0      14
#define PIN_DISPLAY_D1      13
#define PIN_DISPLAY_D2      10
#define PIN_DISPLAY_D3      12
#define PIN_DISPLAY_RST     17
#define PIN_DISPLAY_TE      8

// -----------------------------------------------------------------------------
// Touch Controller — CST820
// -----------------------------------------------------------------------------
#define PIN_TOUCH_INT       16   // Interrupt pin (from community code)
#define TOUCH_I2C_ADDR      0x15

// -----------------------------------------------------------------------------
// IMU — QMI8658 (6-axis accelerometer + gyroscope)
// Address auto-detected by SensorLib
// -----------------------------------------------------------------------------
// Uses shared I2C bus (PIN_I2C_SDA / PIN_I2C_SCL)

// -----------------------------------------------------------------------------
// Audio Codec — ES8311 (I2C control + I2S data)
// I2C control uses shared bus above
// -----------------------------------------------------------------------------
// TODO: I2S pin numbers MUST be confirmed from the official Waveshare schematic PDF
// Download schematic from: waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.32 → Resources tab
#define PIN_I2S_BCLK        -1   // TODO: Confirm from schematic
#define PIN_I2S_LRCK        -1   // TODO: Confirm from schematic (also called WS)
#define PIN_I2S_DOUT        -1   // TODO: Confirm from schematic (speaker output)
#define PIN_I2S_DIN         -1   // TODO: Confirm from schematic (mic input)
#define PIN_I2S_MCLK        -1   // TODO: Confirm from schematic (may not be needed)

#define ES8311_I2C_ADDR     0x18

// -----------------------------------------------------------------------------
// Buttons
// -----------------------------------------------------------------------------
// PWR button — programmable, active LOW (onboard)
// BOOT button — hold on power-up for flash mode, also programmable

// -----------------------------------------------------------------------------
// Expansion — SH1.0 12-PIN connector
// Multiple configurable GPIOs, I2C, UART exposed
// -----------------------------------------------------------------------------

#endif // PINS_H
