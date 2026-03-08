#ifndef DISPLAY_H
#define DISPLAY_H

// =============================================================================
// Display — CO5300 QSPI AMOLED driver + LVGL + GIF playback
// =============================================================================

#include "emotion_state.h"

void display_init();
void display_update();
void display_set_brightness(uint8_t level);
void display_show_emotion(EmotionState state);

#endif // DISPLAY_H
