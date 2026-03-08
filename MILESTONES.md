# Mochi Desk Robot — Build Milestones

## Session History

### Session 1 — Project Setup + Touch Driver (Complete)
- Created mochi-desk-robot repo with PlatformIO structure
- Implemented CST820 touch controller with 5 zones and 7 gestures
- Emotion state machine with priority system (16 states)
- Placeholder stubs for display, audio, IMU, phrase detection

### Session 2 — Emotion Tag Sync (2026-03-08)
- **Goal**: Intercept Xiaozhi WebSocket emotion packets and trigger GIF BEFORE TTS
- **Status**: Code complete, syntax validated. Full build/flash pending ESP-IDF upgrade.

**Files created:**
- `src/mochi/mochi_emotion.h` — 15-state MochiEmotion enum with string/emoji mappers
  - EmotionFromString(): maps Xiaozhi emotion strings (smile, sad, fear, etc.)
  - EmotionFromEmoji(): maps 50+ emoji characters to MochiEmotion values
  - MochiEmotionGifPath(): resolves SPIFFS GIF paths
- `src/mochi/mochi_display.h` — GIF display engine interface
- `src/mochi/mochi_display.cc` — FreeRTOS task (Core 0), mutex-protected GIF switching
- `xiaozhi-patches/session2-emotion-sync.patch` — Patches for xiaozhi-esp32:
  - application.cc: LLM emotion intercept + state change hooks with timestamped logging
  - board file: mochi_display_init() call after LCD panel creation
  - CMakeLists.txt: added mochi/ source and include dirs

**Timing verification (by code analysis):**
- LLM emotion packet arrives via WebSocket → parsed in OnIncomingJson callback
- `Schedule()` pushes emotion handler to FIFO deque
- TTS start packet arrives later → pushes SetDeviceState(kDeviceStateSpeaking) to same deque
- FIFO ordering guarantees: EMOTION_TAG log → GIF_LOAD log → STATE: speaking log

**Blocking issue:**
- ESP-IDF >= v5.5.2 required for xiaozhi-esp32 build; local install is v5.1.4
- Next session: upgrade ESP-IDF, full build, flash, serial log verification

**Edge cases handled:**
- Missing emotion field → falls back to kThinking
- Unknown emoji → falls back to kIdle
- Back-to-back emotion packets → second wins (no crash, mutex-protected)
- Missing GIF file on SPIFFS → logs warning, falls back to idle.gif
- Same emotion re-set → no-op (skip redundant GIF restart)

---

## Phase 1: Offline Autonomous Desk Pet

- [ ] **M0 — Dev Environment**
  - Arduino IDE + PlatformIO + board flashing working
  - Install arduino-esp32 board package, all libraries from spec
  - Flash blink example, confirm serial monitor output
  - Test: Serial prints "Hello Mochi"

- [ ] **M1 — Display**
  - GIF animation playing on round AMOLED
  - Init Arduino_GFX with CO5300 QSPI driver
  - Decode one happy.gif from SPIFFS via AnimatedGIF
  - Display centered on black background (200x200 GIF on 466x466 screen)
  - Test: See Mochi face animated on screen

- [x] **M2 — Touch** (Session 1)
  - Touch zones triggering different animations
  - Init CST820 on I2C, map X/Y to 5 zones
  - Trigger HAPPY on head tap, LOVED on long-press
  - Test: Tap top -> happy anim; hold center -> loved anim

- [ ] **M3 — IMU**
  - Physical gestures changing state
  - Init QMI8658 via SensorLib, 50Hz polling on Core 1
  - Implement pick-up / shake / face-down / tilt detection
  - Test: Shake device -> angry anim; lay face-down -> screen dims

- [ ] **M4 — Speaker**
  - Pre-recorded sounds playing on events
  - Init ES8311 via I2C + I2S (confirm I2S pins from schematic first!)
  - Load happy_chirp.pcm from SPIFFS, play on tap touch event
  - Test: Tap -> hear chirp sound from speaker

- [ ] **M5 — Microphone**
  - Mic RMS fright detection working
  - Enable mic path in ES8311, read I2S mic stream
  - Compute RMS, trigger SCARED on loud noise (threshold ~8000)
  - Test: Clap near device -> scared animation + yelp sound

- [ ] **M6 — State Machine**
  - Full emotion state machine integrated
  - Wire all sensors into unified EmotionState enum + transition logic
  - FreeRTOS timer for boredom cascade (5min -> 10min -> 20min)
  - Test: Leave 5min -> bored; 10min -> attention seeking; 20min -> lonely

- [ ] **M7 — Phrase Detection**
  - ESP-SR wake word + command phrases
  - Integrate ESP-SR component (WakeNet9 + MultiNet6)
  - Flash models, generate phoneme tables for core phrases
  - Test: Say "Hey Mochi, I love you" -> LOVED state + sound

- [ ] **M8 — Time Awareness**
  - Time-of-day behaviors active
  - NTP sync at boot, time slot checks (morning/afternoon/night)
  - Morning groggy state, night auto-sleep at 10pm+
  - Test: Boot in morning -> groggy; after 10pm -> auto sleep

- [ ] **M9 — Polish**
  - Full Phase 1 complete
  - Tune all thresholds (touch debounce, IMU sensitivity, mic RMS)
  - Add all 14 sound clips, ensure all 15 GIFs loaded
  - Test 48hr continuous run for stability
  - Test: Full personality loop feels alive and responsive

## Phase 2: Xiaozhi Integration (LLM + Cloud)

- [x] **S2 — Emotion Tag Sync** (Session 2 — code complete, pending build)
  - Intercept WebSocket emotion packets in application.cc
  - Map emotion strings + emoji to 15 MochiEmotion states
  - GIF display engine with FreeRTOS task on Core 0
  - Verify: emotion GIF switches BEFORE TTS audio starts
