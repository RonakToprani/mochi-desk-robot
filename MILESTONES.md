# Mochi Desk Robot — Build Milestones

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

- [ ] **M2 — Touch**
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
