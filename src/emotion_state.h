#ifndef EMOTION_STATE_H
#define EMOTION_STATE_H

// =============================================================================
// Emotion State Machine — Mochi personality core
// =============================================================================

#include <stdint.h>

// All possible emotion states (priority in comments)
typedef enum {
    EMOTION_IDLE = 0,           // Priority 0 — default
    EMOTION_HAPPY,              // Priority 3
    EMOTION_EXCITED,            // Priority 4
    EMOTION_LOVED,              // Priority 4
    EMOTION_BORED,              // Priority 1
    EMOTION_ATTENTION_SEEK,     // Priority 2
    EMOTION_LONELY,             // Priority 2
    EMOTION_SLEEPY,             // Priority 1
    EMOTION_SCARED,             // Priority 8
    EMOTION_STARTLED,           // Priority 7
    EMOTION_RELIEVED,           // Priority 3
    EMOTION_THINKING,           // Priority 5
    EMOTION_TALKING,            // Priority 6
    EMOTION_ANGRY,              // Priority 7
    EMOTION_CONFUSED,           // Priority 3
    EMOTION_SAD,                // Priority 1
    EMOTION_COUNT
} EmotionState;

void emotion_state_init();
void emotion_state_set(EmotionState new_state);
EmotionState emotion_state_get();
uint8_t emotion_state_priority(EmotionState state);
void emotion_state_reset_idle_timer();

#endif // EMOTION_STATE_H
