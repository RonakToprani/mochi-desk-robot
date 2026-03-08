#ifndef AUDIO_H
#define AUDIO_H

// =============================================================================
// Audio — ES8311 codec (I2S speaker + mic)
// =============================================================================

#include <stdint.h>

void audio_init();
void audio_play_sound(const char *name);
void audio_set_volume(uint8_t vol);     // 0-100
void audio_set_mic_gain(uint8_t gain);  // 0-100
float audio_get_mic_rms();

#endif // AUDIO_H
