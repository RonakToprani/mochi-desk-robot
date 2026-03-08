// =============================================================================
// Touch implementation — CST820 on I2C (interrupt-driven)
// Zone mapping for 466x466 circular display
// =============================================================================

#include "touch.h"
#include "../config/pins.h"
#include <Arduino.h>
#include <Wire.h>

// -----------------------------------------------------------------------------
// CST820 Register Addresses
// -----------------------------------------------------------------------------
#define REG_GESTURE_ID    0x01
#define REG_FINGER_NUM    0x02
#define REG_XPOS_H        0x03
#define REG_CHIP_ID       0xA7
#define REG_FW_VERSION    0xA9
#define REG_MOTION_MASK   0xEC
#define REG_IRQ_CTL       0xFA
#define REG_DIS_AUTO_SLP  0xFE

// -----------------------------------------------------------------------------
// CST820 Built-in Gesture Codes
// -----------------------------------------------------------------------------
#define CST_GESTURE_NONE         0x00
#define CST_GESTURE_SLIDE_DOWN   0x01
#define CST_GESTURE_SLIDE_UP     0x02
#define CST_GESTURE_SINGLE_CLICK 0x05
#define CST_GESTURE_DOUBLE_CLICK 0x0B
#define CST_GESTURE_LONG_PRESS   0x0C

// Touch event types (bits 7:6 of register 0x03)
#define TOUCH_EVENT_DOWN    0
#define TOUCH_EVENT_UP      1
#define TOUCH_EVENT_CONTACT 2

// -----------------------------------------------------------------------------
// Timing Constants
// -----------------------------------------------------------------------------
#define DEBOUNCE_MS       100
#define LONG_PRESS_MS     1500
#define TOUCH_POLL_MS     50    // Poll interval while finger is held (for long-press)

// -----------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------
static volatile bool touch_irq_flag = false;
static TaskHandle_t  touch_task_handle = nullptr;
static touch_event_cb_t event_callback = nullptr;

// Touch tracking
static bool     touch_active      = false;
static uint32_t touch_start_ms    = 0;
static uint16_t touch_start_x     = 0;
static uint16_t touch_start_y     = 0;
static bool     long_press_fired  = false;
static uint32_t last_event_ms     = 0;

// -----------------------------------------------------------------------------
// ISR — sets flag + wakes task, no I2C here
// -----------------------------------------------------------------------------
static void IRAM_ATTR touch_isr() {
    touch_irq_flag = true;
    if (touch_task_handle) {
        BaseType_t woken = pdFALSE;
        vTaskNotifyGiveFromISR(touch_task_handle, &woken);
        portYIELD_FROM_ISR(woken);
    }
}

// -----------------------------------------------------------------------------
// I2C helpers
// -----------------------------------------------------------------------------
static bool cst820_read_regs(uint8_t reg, uint8_t *buf, uint8_t len) {
    Wire.beginTransmission(TOUCH_I2C_ADDR);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) return false;
    Wire.requestFrom((uint8_t)TOUCH_I2C_ADDR, len);
    for (uint8_t i = 0; i < len && Wire.available(); i++) {
        buf[i] = Wire.read();
    }
    return true;
}

static void cst820_write_reg(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(TOUCH_I2C_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

// -----------------------------------------------------------------------------
// Dispatch a touch event (with debounce)
// -----------------------------------------------------------------------------
static void dispatch_event(TouchGesture gesture, uint16_t x, uint16_t y) {
    uint32_t now = millis();
    // Skip debounce for long-press release so it always fires
    if (gesture != GESTURE_LONG_PRESS_RELEASE) {
        if (now - last_event_ms < DEBOUNCE_MS) return;
    }
    last_event_ms = now;

    TouchZone zone = touch_get_zone(x, y);
    TouchEvent ev = { zone, gesture, x, y };

    Serial.printf("[Touch] Zone: %-12s  Gesture: %-20s  (%d, %d)\n",
                  touch_zone_name(zone), touch_gesture_name(gesture), x, y);

    if (event_callback) {
        event_callback(&ev);
    }
}

// -----------------------------------------------------------------------------
// FreeRTOS task — processes touch interrupts on Core 1
// -----------------------------------------------------------------------------
static void touch_task(void *) {
    for (;;) {
        // While finger is held, poll every TOUCH_POLL_MS for long-press timing.
        // Otherwise block indefinitely waiting for the next interrupt.
        TickType_t wait = touch_active ? pdMS_TO_TICKS(TOUCH_POLL_MS)
                                       : portMAX_DELAY;
        ulTaskNotifyTake(pdTRUE, wait);
        touch_irq_flag = false;

        uint8_t data[6];
        if (!cst820_read_regs(REG_GESTURE_ID, data, 6)) continue;

        uint8_t gesture_id = data[0];
        uint8_t fingers    = data[1];
        uint8_t event_type = (data[2] >> 6) & 0x03;
        uint16_t x = ((data[2] & 0x0F) << 8) | data[3];
        uint16_t y = ((data[4] & 0x0F) << 8) | data[5];

        // ----- Finger just touched down -----
        if (fingers > 0 && !touch_active) {
            touch_active     = true;
            touch_start_ms   = millis();
            touch_start_x    = x;
            touch_start_y    = y;
            long_press_fired = false;

            Serial.printf("[Touch] Down at (%d, %d) — Zone: %s\n",
                          x, y, touch_zone_name(touch_get_zone(x, y)));

            // Two-finger tap (CST820 is single-touch but reports finger count)
            if (fingers >= 2) {
                dispatch_event(GESTURE_TWO_FINGER_TAP, x, y);
                touch_active = false;
                continue;
            }
            continue;
        }

        // ----- Finger still held — check long press -----
        if (touch_active && fingers > 0) {
            if (!long_press_fired &&
                (millis() - touch_start_ms >= LONG_PRESS_MS)) {
                long_press_fired = true;
                dispatch_event(GESTURE_LONG_PRESS,
                               touch_start_x, touch_start_y);
            }
            continue;
        }

        // ----- Finger released -----
        if (touch_active && (fingers == 0 || event_type == TOUCH_EVENT_UP)) {
            touch_active = false;

            if (long_press_fired) {
                // Release after long press
                dispatch_event(GESTURE_LONG_PRESS_RELEASE,
                               touch_start_x, touch_start_y);
            } else {
                // Map CST820 built-in gesture to our enum
                switch (gesture_id) {
                    case CST_GESTURE_DOUBLE_CLICK:
                        dispatch_event(GESTURE_DOUBLE_TAP, x, y);
                        break;
                    case CST_GESTURE_SLIDE_UP:
                        dispatch_event(GESTURE_SWIPE_UP, x, y);
                        break;
                    case CST_GESTURE_SLIDE_DOWN:
                        dispatch_event(GESTURE_SWIPE_DOWN, x, y);
                        break;
                    default:
                        // Single tap (built-in 0x05 or short touch)
                        dispatch_event(GESTURE_TAP, x, y);
                        break;
                }
            }
        }
    }
}

// =============================================================================
// Public API
// =============================================================================

void touch_init() {
    // Verify CST820 presence on I2C
    uint8_t chip_id = 0;
    if (!cst820_read_regs(REG_CHIP_ID, &chip_id, 1)) {
        Serial.println("[Touch] ERROR — CST820 not found on I2C bus!");
        return;
    }
    Serial.printf("[Touch] CST820 detected — Chip ID: 0x%02X\n", chip_id);

    uint8_t fw_ver = 0;
    cst820_read_regs(REG_FW_VERSION, &fw_ver, 1);
    Serial.printf("[Touch] FW Version: 0x%02X\n", fw_ver);

    // Configure CST820
    cst820_write_reg(REG_IRQ_CTL, 0x70);       // Enable motion IRQ
    cst820_write_reg(REG_DIS_AUTO_SLP, 0xFE);  // Disable auto-sleep
    cst820_write_reg(REG_MOTION_MASK, 0x01);    // Enable double-click detection

    // Attach interrupt — falling edge on GPIO 16
    pinMode(PIN_TOUCH_INT, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_TOUCH_INT), touch_isr, FALLING);

    // Spawn processing task on Core 1
    xTaskCreatePinnedToCore(touch_task, "touch", 4096, NULL, 5,
                            &touch_task_handle, 1);

    Serial.println("[Touch] Initialized — interrupt-driven on GPIO 16");
}

void touch_register_callback(touch_event_cb_t cb) {
    event_callback = cb;
}

TouchZone touch_get_zone(uint16_t x, uint16_t y) {
    // Circle boundary check: center (233,233), radius 233
    int16_t dx = (int16_t)x - 233;
    int16_t dy = (int16_t)y - 233;
    if ((int32_t)dx * dx + (int32_t)dy * dy > 233L * 233L) return ZONE_NONE;

    // Priority: directional edges first, then center
    if (y < 140) return ZONE_HEAD_TOP;
    if (y > 326) return ZONE_CHIN_BOTTOM;
    if (x < 140) return ZONE_SIDE_LEFT;
    if (x > 326) return ZONE_SIDE_RIGHT;
    if (x > 133 && x < 333 && y > 133 && y < 333) return ZONE_FACE_CENTER;

    return ZONE_FACE_CENTER;  // Remaining interior → face
}

const char* touch_zone_name(TouchZone zone) {
    switch (zone) {
        case ZONE_HEAD_TOP:    return "HEAD_TOP";
        case ZONE_CHIN_BOTTOM: return "CHIN_BOTTOM";
        case ZONE_SIDE_LEFT:   return "SIDE_LEFT";
        case ZONE_SIDE_RIGHT:  return "SIDE_RIGHT";
        case ZONE_FACE_CENTER: return "FACE_CENTER";
        default:               return "NONE";
    }
}

const char* touch_gesture_name(TouchGesture gesture) {
    switch (gesture) {
        case GESTURE_TAP:                return "TAP";
        case GESTURE_DOUBLE_TAP:         return "DOUBLE_TAP";
        case GESTURE_LONG_PRESS:         return "LONG_PRESS";
        case GESTURE_LONG_PRESS_RELEASE: return "LONG_PRESS_RELEASE";
        case GESTURE_SWIPE_UP:           return "SWIPE_UP";
        case GESTURE_SWIPE_DOWN:         return "SWIPE_DOWN";
        case GESTURE_TWO_FINGER_TAP:     return "TWO_FINGER_TAP";
        default:                         return "NONE";
    }
}
