// =============================================================================
// Touch implementation — CST820 on I2C
// Zone mapping for 466x466 circular display
// =============================================================================

#include "touch.h"
#include "../config/pins.h"
#include <Arduino.h>
#include <Wire.h>

void touch_init() {
    // TODO: M2 — Configure CST820 interrupt on PIN_TOUCH_INT
    // TODO: M2 — Verify CST820 at TOUCH_I2C_ADDR on I2C bus
    Serial.println("[Touch] Init placeholder");
}

bool touch_read(uint16_t *x, uint16_t *y) {
    // TODO: M2 — Read CST820 touch registers via I2C
    return false;
}

TouchZone touch_get_zone(uint16_t x, uint16_t y) {
    // Zone map: 466x466 circle, center = (233, 233)
    // HEAD_TOP:      Y < 140
    // CHIN_BOTTOM:   Y > 326
    // SIDE_LEFT:     X < 140
    // SIDE_RIGHT:    X > 326
    // FACE_CENTER:   133 < X < 333, 133 < Y < 333
    if (y < 140) return ZONE_HEAD_TOP;
    if (y > 326) return ZONE_CHIN_BOTTOM;
    if (x < 140) return ZONE_SIDE_LEFT;
    if (x > 326) return ZONE_SIDE_RIGHT;
    if (x > 133 && x < 333 && y > 133 && y < 333) return ZONE_FACE_CENTER;
    return ZONE_NONE;
}
