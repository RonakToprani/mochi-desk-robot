#ifndef TOUCH_H
#define TOUCH_H

// =============================================================================
// Touch — CST820 capacitive touch with zone mapping & gesture detection
// Interrupt-driven on GPIO 16, I2C address 0x15
// =============================================================================

#include <stdint.h>

// --- Touch zones on 466x466 circular display ---
typedef enum {
    ZONE_NONE = 0,
    ZONE_HEAD_TOP,       // Y < 140
    ZONE_CHIN_BOTTOM,    // Y > 326
    ZONE_SIDE_LEFT,      // X < 140
    ZONE_SIDE_RIGHT,     // X > 326
    ZONE_FACE_CENTER     // 133 < X < 333, 133 < Y < 333
} TouchZone;

// --- Gesture types ---
typedef enum {
    GESTURE_NONE = 0,
    GESTURE_TAP,
    GESTURE_DOUBLE_TAP,
    GESTURE_LONG_PRESS,
    GESTURE_LONG_PRESS_RELEASE,
    GESTURE_SWIPE_UP,
    GESTURE_SWIPE_DOWN,
    GESTURE_TWO_FINGER_TAP
} TouchGesture;

// --- Touch event passed to callbacks ---
typedef struct {
    TouchZone zone;
    TouchGesture gesture;
    uint16_t x;
    uint16_t y;
} TouchEvent;

// Callback signature for touch events
typedef void (*touch_event_cb_t)(const TouchEvent *event);

void touch_init();
void touch_register_callback(touch_event_cb_t cb);
TouchZone touch_get_zone(uint16_t x, uint16_t y);
const char* touch_zone_name(TouchZone zone);
const char* touch_gesture_name(TouchGesture gesture);

#endif // TOUCH_H
