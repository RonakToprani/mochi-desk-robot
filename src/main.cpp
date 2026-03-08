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

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("=== Mochi Desk Robot ===");

    // Init shared I2C bus (touch + IMU + audio codec)
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

    // Initialize subsystems
    display_init();
    touch_init();
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
