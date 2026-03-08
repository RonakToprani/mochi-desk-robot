// =============================================================================
// IMU implementation — QMI8658 via SensorLib
// Polled at ~50 Hz for gesture detection
// =============================================================================

#include "imu.h"
#include "../config/pins.h"
#include <Arduino.h>

void imu_init() {
    // TODO: M3 — Initialize QMI8658 via SensorLib on shared I2C bus
    // TODO: M3 — Configure accelerometer + gyroscope data rates
    Serial.println("[IMU] Init placeholder");
}

void imu_update() {
    // TODO: M3 — Read accel/gyro, run 5-sample rolling average
    // TODO: M3 — Check thresholds for pick-up, shake, face-down, tilt
}

bool imu_is_picked_up() {
    // |delta_accZ| > 1.5g in < 200ms
    return false;
}

bool imu_is_shaking() {
    // RMS(accXYZ) > 2.5g for > 300ms
    return false;
}

bool imu_is_face_down() {
    // accZ < -4.9 m/s^2 sustained for > 2s
    return false;
}

bool imu_is_tilted() {
    // accX > +/- 0.5g
    return false;
}
