# Daggorath opening and attract mode: source and MAME reference

This is an implementation plan, not a port of the opening. The reference is the read-only recovered source at `/Volumes/SEDONA/Projects/daggorath-reference`, commit `a94326f00ebb16a106b540c58bc2ccf5f7b66dac`. Its `DAGGORATH.ASM` built with `lwasm --format=raw` into an 8 KiB ROM (SHA-256 `35e6a77354dcf1a3048f276824b7a0f9f759115fdd40603664cebfb3a7da6571`). Controlled MAME 0.289 runs of **that source-built ROM** provide the timings and images below. This is reproducible recovered-source behavior, **not independent authentication of a retail cartridge dump**. The existing [Wizard M1](DAGGORATH_WIZARD_M1.md) is a separately measured partial port.

## Reproduction boundary

The ROM was assembled outside this repository with `/usr/local/bin/lwasm` from lwtools 4.22: from the reference checkout, `lwasm --format=raw --output=/private/tmp/daggorath-attract/original.rom DAGGORATH.ASM`. It then ran as `coco3h` at 2 MB with RGB and no disk attached. The launch command was:

```text
/Applications/Emulators/Ample.app/Contents/MacOS/mame64 coco3h -window -skip_gameinfo -ramsize 2M -rompath '/Users/magneto-optimus/Library/Application Support/Ample/roms' -cfg_directory /private/tmp/daggorath-attract-full/cfg -cart /private/tmp/daggorath-attract/original.rom -wavwrite /private/tmp/daggorath-attract-full/audio.wav -autoboot_delay 0 -autoboot_script /private/tmp/daggorath-attract-full/capture.lua
```

The isolated MAME configuration used the verified RGB input port `:screen_config` with value `1`. Lua sampled video frames and source state; MAME's WAV output was captured without guest disk I/O. Three runs covered no input, a key during autoplay, and the full framebuffer/audio trace. Timing is MAME emulated time from launch, at approximately 59.923 video frames/s, not wall-clock time. The raw per-frame trace and WAV remain disposable under `/private/tmp/daggorath-attract-full/`; the representative images and a short unaltered-time opening audio excerpt are curated [here](assets/daggorath-attract/).

## Source-defined sequence

| Order | Original source | Behavior |
| --- | --- | --- |
| Reset | `ONCE.ASM:ONCE/DEMO/COMINI` | Initializes PIA, RAM, video and copyright/status text, then enters `DEMO10`. No separate title-menu routine was found on this path. |
| Wizard | `ONCE.ASM:DEMO10`; `MISC.ASM:WIZIN0/WIZI20/WIZOUT`; `D4.ASM:WIZ1` | Sets `AUTFLG`, synchronizes the IRQ, fades in the original wizard vectors, emits the first explosion, draws the two encoded welcome messages, waits twice, emits the second explosion and fades out. The fade retains the copyright/status text as the source requests. |
| Transition | `ONCE.ASM` immediately after `WIZOUT` | Blanks/flips the display, chooses demo level 2 (the third level), and supplies `DEMDAT`. |
| Preparation | `ONCE.ASM:GAME20/GAME30`; `COMSWI.ASM:PREPAR`; `MISC.ASM` preparation text; `COMDAT.ASM:DEMDAT` | Shows `PREPARE!`, builds the level, then creates and bags the demo objects: iron sword, pine torch and leather shield. |
| Map | `ONCE.ASM:GAME40`; `MAPPER.ASM:MAPPER` | Disables the ordinary scheduler while showing the complete demo map, waits twice, then resumes and synchronizes. This map is part of autoplay, not the ordinary new-game path. |
| Dungeon | `ONCE.ASM:GAME50`; `PLOOK.ASM:INIVUX`; `TOKEN.ASM:AUTTAB`; `HUMAN.ASM:PLAYER` | Initializes the resident view/status and heartbeat, draws a prompt, and lets the scripted demo command stream run through the scheduler. `AUTTAB` begins `EXAMINE`, `PULL RIGHT TORCH`, `USE RIGHT`, `LOOK`, movement/equipment/attack commands, then waits and restarts the demo. |
| User interruption | `COMMON.ASM:CLOCK/CLK50`; `ONCE.ASM:GAME/GAME10` | **Any keyboard depression in autoplay** rewrites the interrupted return address to `GAME`, beginning a fresh normal game. This is not a stage-advance or a character passed through to the demo. `GAME10` establishes the normal start location and `GAMDAT`; `GAME40` skips the demo map when `AUTFLG` is clear. |

The normal game begins dark. Its initial torch is carried but not lit; `PUSE.ASM:PUSE12` ignites a used torch and requests source sound `A$TORC` (`CHUCK`). The automatically illuminated corridor is therefore an **autoplay consequence** of scripted `USE RIGHT`, not a universal opening state.

## Measured visual timeline

The transitions below are sampled frames of the source-built ROM. They are not claims about an authenticated retail cartridge or an exact scanline boundary.

| Frame | Emulated time | Observation |
| ---: | ---: | --- |
| 102 | 1.701 s | First visible wizard fade frame |
| 395 | 6.590 s | Wizard geometry complete |
| 479 | 7.992 s | Both welcome messages complete |
| 641 | 10.696 s | Messages clear |
| 760 | 12.682 s | Fade-out begins its visible phase |
| 1014 | 16.920 s | Final blank frame |
| 1017 | 16.970 s | `PREPARE!` visible |
| 1403 | 23.412 s | Demo map visible |
| 1568 | 26.166 s | Heartbeat enable state observed |
| 1577 | 26.316 s | Demo dungeon view visible |
| 1679 | 28.018 s | Scripted `EXAMINE` begins displaying |
| 2180 | 36.379 s | `PULL RIGHT TORCH` visible |
| 2350 | 39.216 s | First lit 3D corridor, after `USE RIGHT` |

The no-input trace continued through frame 3600 (about one minute) in autoplay. A separate run injected `X` at frame 1800 (30.037 s), while `EXAMINE` was active. `AUTFLG` cleared by frame 1804 (30.104 s), and the normal initial position was established by frame 1833 (30.588 s). A dark playable dungeon was captured by frame 2400. This verifies the any-key restart path in one controlled condition; it does **not** establish the first input-ready frame or response to every key at every stage. The source's `GAME50 → INIVU → PROMPT → SCHED` path identifies when ordinary command processing can begin.

The curated full-trace images are frames 480 (messages), 1080 (`PREPARE!`), 1500 (map), 1680 (`EXAMINE`), and 2400 (lit corridor). The normal dark-dungeon image is frame 2400 of the separate key-interruption run.

![Wizard messages](assets/daggorath-attract/wizard-messages.png)
![Preparation](assets/daggorath-attract/prepare.png)
![Autoplay map](assets/daggorath-attract/autoplay-map.png)
![Autoplay EXAMINE](assets/daggorath-attract/autoplay-examine.png)
![Autoplay lit corridor](assets/daggorath-attract/torch-corridor.png)
![Normal dark dungeon after interruption](assets/daggorath-attract/normal-dark-dungeon.png)

## Audio and timing relationships

`COMMON.ASM:CLOCK/CLK20` emits the original roughly 30 Hz DAC wizard buzz while `NOISEF` is set during the fades. `MISC.ASM:WIZI20` requests `A$EXP1` (`KABOOM`) after the fade-in and before fade-out. The captured MAME audio contains two strong explosion-like intervals and fade-associated buzz, consistent with that call order; the source is the evidence for exact call identity. The opening audio excerpt is [available here](assets/daggorath-attract/opening-source-build.mp3). It is a source-built CoCo DAC recording, **not** output from the port's SSC backend. The demo's `USE RIGHT` later requests `A$TORC` (`CHUCK`); subsequent autoplay commands may request other data-dependent effects, which this pass did not isolate event by event. `PLOOK.ASM:INIVUX` enables the heart and its native PB1 heartbeat during dungeon entry; the source heartbeat audio and visual heart share the clock path rather than independent schedulers.

The two `WAIT` calls after the welcome messages and two map waits are part of the cadence. Wizard M1 already has a separately measured fade/content comparison; extending it must preserve its logical frames and canonical 512×192 presentation, and must place any later sounds at the source-defined transitions without inserting new foreground delays. Audio fidelity of the remaining attract sequence has not been established under NitrOS-9.

## Smallest faithful implementation slice

Add a bounded opening continuation **after the existing Wizard**: preserve its completed fade-out, show the source-derived `PREPARE!` transition and demo map in the same owned 640×200 screen, then return safely to Term. Reuse the original PREPAR/map data and timing boundaries; compare source-built ROM frames at those states, including the 512×192 centered viewport and original message geometry. Keep source logic, 256×192 framebuffer, and presentation translation separate. Acceptance should include exact source-frame comparisons, transition timestamps, no new audio delay, handled cancellation, status `000`/`003`, and a healthy subsequent shell. A later slice can initialize the autoplay dungeon, scripted object state and command stream. Do not imply the bounded slice is a complete attract mode or connect all SSC sounds merely because their catalog exists.

Before implementing even this slice, confirm memory/ownership for retaining the opening screen through the map, and whether PREPAR/map can be ported without pulling in the full original scheduler. The observed any-key restart is a separate interaction boundary. Full original autoplay duration, every later sound, and first normal input-ready frame remain open measurements. Gameplay combat, menu, and side panels are outside this proposal.
