// =============================================================================
// main.cpp — Mochi Desk Robot entry point
// Waveshare ESP32-S3-Touch-AMOLED-1.32
// =============================================================================

#include <Arduino.h>
#include <Wire.h>
#include "../config/pins.h"
#include "emotion_state.h"
#include "display.h"
#include "touch.h"
#include "imu.h"
#include "audio.h"
#include "phrase_detect.h"

// -----------------------------------------------------------------------------
// Touch → Emotion + Display wiring
// Called from the touch FreeRTOS task (Core 1)
// -----------------------------------------------------------------------------
static void on_touch_event(const TouchEvent *ev) {
    emotion_state_reset_idle_timer();

    switch (ev->gesture) {
        case GESTURE_TAP:
            // Head tap or face tap → HAPPY
            if (ev->zone == ZONE_HEAD_TOP || ev->zone == ZONE_FACE_CENTER) {
                emotion_state_set(EMOTION_HAPPY);
                display_show_emotion(EMOTION_HAPPY);
            }
            break;

        case GESTURE_LONG_PRESS:
            // Long press on face → LOVED (held)
            if (ev->zone == ZONE_FACE_CENTER) {
                emotion_state_set(EMOTION_LOVED);
                display_show_emotion(EMOTION_LOVED);
            }
            break;

        case GESTURE_LONG_PRESS_RELEASE:
            // Release from loved → HAPPY
            emotion_state_force_set(EMOTION_HAPPY);
            display_show_emotion(EMOTION_HAPPY);
            break;

        case GESTURE_DOUBLE_TAP:
            emotion_state_set(EMOTION_EXCITED);
            display_show_emotion(EMOTION_EXCITED);
            break;

        case GESTURE_SWIPE_UP:
            emotion_state_set(EMOTION_EXCITED);
            display_show_emotion(EMOTION_EXCITED);
            break;

        case GESTURE_SWIPE_DOWN:
            emotion_state_set(EMOTION_SLEEPY);
            display_show_emotion(EMOTION_SLEEPY);
            break;

        case GESTURE_TWO_FINGER_TAP:
            emotion_state_set(EMOTION_CONFUSED);
            display_show_emotion(EMOTION_CONFUSED);
            break;

        default:
            break;
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("=== Mochi Desk Robot ===");

    // Init shared I2C bus (touch + IMU + audio codec)
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

    // Initialize subsystems
    display_init();
    touch_init();
    touch_register_callback(on_touch_event);
    imu_init();
    audio_init();
    phrase_detect_init();
    emotion_state_init();

    Serial.println("Mochi is awake!");
}

void loop() {
    // Main loop — subsystems are driven by FreeRTOS tasks
    // This loop handles LVGL tick and any lightweight coordination
    display_update();
    delay(5);
}
