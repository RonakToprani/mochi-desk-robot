#ifndef TOUCH_H
#define TOUCH_H

// =============================================================================
// Touch — CST820 capacitive touch with zone mapping
// =============================================================================

#include <stdint.h>

typedef enum {
    ZONE_NONE = 0,
    ZONE_HEAD_TOP,
    ZONE_CHIN_BOTTOM,
    ZONE_SIDE_LEFT,
    ZONE_SIDE_RIGHT,
    ZONE_FACE_CENTER
} TouchZone;

typedef enum {
    GESTURE_NONE = 0,
    GESTURE_TAP,
    GESTURE_DOUBLE_TAP,
    GESTURE_LONG_PRESS,
    GESTURE_SWIPE_UP,
    GESTURE_SWIPE_DOWN,
    GESTURE_SWIPE_LEFT,
    GESTURE_SWIPE_RIGHT
} TouchGesture;

void touch_init();
bool touch_read(uint16_t *x, uint16_t *y);
TouchZone touch_get_zone(uint16_t x, uint16_t y);

#endif // TOUCH_H
