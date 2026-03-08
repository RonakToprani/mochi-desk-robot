// =============================================================================
// Emotion State Machine implementation
// =============================================================================

#include "emotion_state.h"
#include <Arduino.h>

static EmotionState current_state = EMOTION_IDLE;

// Priority table — higher number wins when competing
static const uint8_t state_priority[EMOTION_COUNT] = {
    0,  // IDLE
    3,  // HAPPY
    4,  // EXCITED
    4,  // LOVED
    1,  // BORED
    2,  // ATTENTION_SEEK
    2,  // LONELY
    1,  // SLEEPY
    8,  // SCARED
    7,  // STARTLED
    3,  // RELIEVED
    5,  // THINKING
    6,  // TALKING
    7,  // ANGRY
    3,  // CONFUSED
    1,  // SAD
};

void emotion_state_init() {
    current_state = EMOTION_IDLE;
    Serial.println("[EmotionState] Initialized — IDLE");
}

void emotion_state_set(EmotionState new_state) {
    if (new_state >= EMOTION_COUNT) return;
    if (new_state == current_state) return;  // debounce re-entry

    // Higher priority wins
    if (state_priority[new_state] >= state_priority[current_state]) {
        Serial.printf("[EmotionState] %d -> %d\n", current_state, new_state);
        current_state = new_state;
    }
}

void emotion_state_force_set(EmotionState new_state) {
    if (new_state >= EMOTION_COUNT) return;
    if (new_state == current_state) return;
    Serial.printf("[EmotionState] %d -> %d (forced)\n", current_state, new_state);
    current_state = new_state;
}

EmotionState emotion_state_get() {
    return current_state;
}

static const char* state_names[EMOTION_COUNT] = {
    "IDLE", "HAPPY", "EXCITED", "LOVED", "BORED",
    "ATTENTION_SEEK", "LONELY", "SLEEPY", "SCARED",
    "STARTLED", "RELIEVED", "THINKING", "TALKING",
    "ANGRY", "CONFUSED", "SAD"
};

const char* emotion_state_name(EmotionState state) {
    if (state >= EMOTION_COUNT) return "UNKNOWN";
    return state_names[state];
}

uint8_t emotion_state_priority(EmotionState state) {
    if (state >= EMOTION_COUNT) return 0;
    return state_priority[state];
}

void emotion_state_reset_idle_timer() {
    // TODO: Reset FreeRTOS software timer for boredom cascade
}
