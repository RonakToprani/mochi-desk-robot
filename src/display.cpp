// =============================================================================
// Display implementation — CO5300 QSPI AMOLED (466x466)
// Uses Arduino_GFX + LVGL + AnimatedGIF
// =============================================================================

#include "display.h"
#include "../config/pins.h"
#include <Arduino.h>

void display_init() {
    // TODO: M1 — Initialize Arduino_GFX with CO5300 QSPI driver
    // TODO: M1 — Initialize LVGL, register flush callback
    // TODO: M1 — Initialize AnimatedGIF decoder
    // TODO: M1 — Load default IDLE animation from SPIFFS/PSRAM
    Serial.println("[Display] Init placeholder");
}

void display_update() {
    // TODO: M1 — Call lv_timer_handler() for LVGL tick
}

void display_set_brightness(uint8_t level) {
    // TODO: M1 — Set AMOLED brightness (0-255)
}
