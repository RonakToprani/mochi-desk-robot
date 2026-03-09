# Mochi Desk Robot — Build Milestones

## Session History

### Session 1 — Project Setup + Touch Driver (Complete)
- Created mochi-desk-robot repo with PlatformIO structure
- Implemented CST820 touch controller with 5 zones and 7 gestures
- Emotion state machine with priority system (16 states)
- Placeholder stubs for display, audio, IMU, phrase detection

### Session 2 — Emotion Tag Sync (2026-03-08)
- **Goal**: Intercept Xiaozhi WebSocket emotion packets and trigger GIF switch
- **Status**: VERIFIED ON HARDWARE. All hooks working. GIF switching instant (0ms).

**Build environment:**
- ESP-IDF v5.5.2 (upgraded from v5.1.4)
- Board: Waveshare ESP32-S3-Touch-AMOLED-1.32 (8MB flash, 8MB PSRAM)
- Partition: 8m.csv (3MB app + 2MB assets), binary 2.8MB (8% free)
- Build path: must avoid spaces in path (linker bug)

**Files created/updated:**
- `src/mochi/mochi_emotion.h` — 15-state MochiEmotion enum with string/emoji mappers
- `src/mochi/mochi_display.h` — GIF display engine interface
- `src/mochi/mochi_display.cc` — FreeRTOS task (Core 0), mutex-protected GIF switching
- `xiaozhi-patches/session2-emotion-sync.patch` — Patches for xiaozhi-esp32:
  - application.cc: LLM emotion intercept, state change hooks, timestamped logging
  - board file: mochi_display_init() call after LCD panel creation
  - CMakeLists.txt: added mochi/ source and include dirs
- `tools/generate_placeholder_gifs.py` — 15 color-coded placeholder GIFs (150x150)

**Hardware verification results (8 runs):**
- EMOTION_TAG → GIF_LOAD: 0ms (same millisecond, instant)
- STATE:speaking → EMOTION_TAG: +220-330ms (protocol behavior, not a bug)
- Emotions verified: happy, neutral, laughing, funny, surprised, thinking
- Emoji fallback verified: laughing→😆→excited, funny→😂→excited, surprised→😲→startled
- Heap stable: 87-126KB free SRAM, min 73KB across all interactions
- No panics, no watchdog, no stack overflow

**Protocol discovery:**
- Xiaozhi server sends STATE:speaking BEFORE the emotion tag arrives
- Emotion packet arrives 220-330ms after TTS starts (1470ms for tool calls)
- This means EMOTION_TAG cannot precede STATE:speaking with current protocol
- Workaround for Session 3: buffer TTS until emotion arrives, or accept slight delay

**Known issues (deferred to Session 3):**
- SPIFFS not mounted (xiaozhi uses mmap_assets format, not VFS SPIFFS)
- ESP-IDF nano printf does not support %lld (timestamps show "ldus")
- Custom wake word ("Hey Mochi") needs Multinet model setup
- Server-side AI personality responds in Chinese (needs xiaozhi portal config)

**Edge cases verified on hardware:**
- Missing GIF file → GIF_MISSING logged, falls back to idle, graceful skip
- Missing emotion field → falls back to kThinking
- Emoji fallback → unknown emotion string triggers emoji lookup
- Back-to-back state changes → no crash, mutex-protected
- WiFi disconnect → enters config mode cleanly, reconnects after setup

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

- [x] **S2 — Emotion Tag Sync** (Session 2 — VERIFIED ON HARDWARE)
  - Intercept WebSocket emotion packets in application.cc
  - Map emotion strings + emoji to 15 MochiEmotion states
  - GIF display engine with FreeRTOS task on Core 0
  - Verified: GIF_LOAD triggers in 0ms after EMOTION_TAG (instant)
  - Protocol note: emotion arrives 220-330ms after TTS starts
