#ifndef DISPLAY_H
#define DISPLAY_H

// =============================================================================
// Display — CO5300 QSPI AMOLED driver + LVGL + GIF playback
// =============================================================================

void display_init();
void display_update();
void display_set_brightness(uint8_t level);

#endif // DISPLAY_H
