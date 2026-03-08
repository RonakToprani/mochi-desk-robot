// =============================================================================
// Audio implementation — ES8311 codec over I2C (control) + I2S (data)
// =============================================================================

#include "audio.h"
#include "../config/pins.h"
#include <Arduino.h>

void audio_init() {
    // TODO: M4 — Initialize ES8311 codec via I2C (Wire)
    // TODO: M4 — Configure I2S driver with pins from pins.h
    //   i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    //   i2s_set_pin(I2S_NUM_0, &pin_config);
    //   NOTE: I2S pins are currently TODO in pins.h — must confirm from schematic first
    // TODO: M4 — Set default volume (75) and mic gain (20)
    Serial.println("[Audio] Init placeholder");
}

void audio_play_sound(const char *name) {
    // TODO: M4 — Load PCM from SPIFFS, play via i2s_write() in FreeRTOS task
}

void audio_set_volume(uint8_t vol) {
    // TODO: M4 — ES8311 speaker volume (0-100)
}

void audio_set_mic_gain(uint8_t gain) {
    // TODO: M5 — ES8311 mic gain (0-100)
}

float audio_get_mic_rms() {
    // TODO: M5 — Read I2S mic buffer, compute RMS for fright detection
    // Threshold: ~8000 on 16-bit scale triggers SCARED state
    return 0.0f;
}
