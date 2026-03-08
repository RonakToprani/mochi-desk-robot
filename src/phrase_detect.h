#ifndef PHRASE_DETECT_H
#define PHRASE_DETECT_H

// =============================================================================
// Phrase Detection — ESP-SR WakeNet9 + MultiNet6
// On-device wake word + command recognition (no WiFi required)
// =============================================================================

void phrase_detect_init();
void phrase_detect_update();

// Returns the last detected command index, or -1 if none
int phrase_detect_get_command();

#endif // PHRASE_DETECT_H
