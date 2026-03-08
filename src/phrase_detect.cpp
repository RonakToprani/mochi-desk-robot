// =============================================================================
// Phrase Detection implementation — ESP-SR
// WakeNet9 (wake word) + MultiNet6 (command phrases)
// =============================================================================

#include "phrase_detect.h"
#include <Arduino.h>

void phrase_detect_init() {
    // TODO: M7 — Load WakeNet9 + MultiNet6 models from flash into PSRAM
    // TODO: M7 — Initialize AFE pipeline (noise suppression + VAD)
    // TODO: M7 — Configure command phrase set via gen_sr_commands.py:
    //   "Hey Mochi" (wake word)
    //   "I love you" / "Good morning" / "Good night" / "Go to sleep"
    //   "Wake up" / "Are you hungry" / "Good boy" / "How are you"
    Serial.println("[PhraseDetect] Init placeholder");
}

void phrase_detect_update() {
    // TODO: M7 — Feed mic stream through AFE → WakeNet → MultiNet pipeline
    // On wake word: play confirm_chirp, enter THINKING state, 5s timeout
    // On command match: trigger emotion response from lookup table
}

int phrase_detect_get_command() {
    // TODO: M7 — Return last detected command index
    return -1;
}
