# Daggorath Attract Compatibility C5: connected gameplay

**Status:** preferred C5 execution boundary reached and accepted by live human
visual/audio review; C4 and C5 remain uncommitted pending checkpoint audit.

## Connected source path

C5 starts with the exact C4 level-two state and reconnects the next foreground
graph instead of manufacturing milestone frames:

```text
HUMAN.ASM:PLAYER/PLAY20/PLAY30
  -> TOKEN.ASM:AUTTAB token words
  -> HUMAN line buffer and CR dispatch
  -> mapped dodcmd parser/command operation
  -> COMPLR.ASM/CROSS-derived dodsched queue boundary
  -> CRETUR.ASM:CMOVE/CWALK creature progression
  -> source presentation request
  -> resident logical renderer plus C1 PIC Giant vectors
```

The reference source is `/Volumes/SEDONA/Projects/daggorath-reference` commit
`a94326f00ebb16a106b540c58bc2ccf5f7b66dac`.  The controlling evidence is
`HUMAN.ASM` (`PLAYER`, `PLAY20`, `PLAY30`, `PLAY50`, `HUMAN`, `HMAN70`),
`TOKEN.ASM:AUTTAB`, `COMMON.ASM:SCHED/CLOCK`, `COMPLR.ASM:LUKNEW`, and
`CRETUR.ASM:CMOVE/CMOV20/CWALK`.

`src/compat/c5.c` retains the source boundary at which autoplay and hardware
keyboard characters meet.  AUTTAB words receive the source PLAYER wait before
each expanded word, and every character enters the same line buffer and CR
dispatch.  A real keyboard command advances CLOCK by one logical jiffy before
the foreground scheduler boundary.  Only the hardware keyboard acquisition is
adapted to the process-owned NitrOS-9 `/w` path.

Command leaves and queue leaves use the existing source-derived production
implementations in `dodcmd` and `dodsched`.  Their retained-load, temporary
link, resident-context callback, return and unlink contracts are unchanged.
`dodcmd` and `dodsched` alternate in the one free DAT slot and are never mapped
together.

## Deterministic trajectory

The C5 regression begins with the complete C4 oracle.  It checks every command
boundary through the first visible Giant closure, including RNG, player state,
CCB 18 and its source countdown.  The values below are after the command and
its scheduler boundary.

| AUTTAB | Command | Player | Damage | RNG | Stone Giant 2 | Countdown |
| ---: | --- | --- | ---: | --- | --- | ---: |
| 1 | `EXAMINE` | `(12,22)` north | 0 | `A5 B1 C9` | `(11,25)` | 4 |
| 2 | `PULL RIGHT TORCH` | `(12,22)` north | 0 | `F9 16 E8` | `(10,25)` | 17 |
| 3 | `USE RIGHT` | `(12,22)` north | 0 | `50 DC 8F` | `(9,25)` | 17 |
| 4 | `LOOK` | `(12,22)` north | 0 | `8B 30 EE` | `(9,25)` | 4 |
| 5 | `MOVE` | `(11,22)` north | 6 | `4B DB 79` | `(9,24)` | 17 |
| 6 | `PULL LEFT SHIELD` | `(11,22)` north | 5 | `C1 AA E4` | `(9,23)` | 17 |
| 7 | `PULL RIGHT SWORD` | `(11,22)` north | 4 | `D2 D4 58` | `(9,22)` | 17 |
| 8 | `MOVE` | `(10,22)` north | 10 | `84 D4 3F` | `(9,22)` | 4 |

There are 15 source word waits and nine foreground boundaries including the
initial boundary.  CCB 18 remains active type 5 and arrives through scheduler
movement; its position is never patched.  After AUTTAB 8, the target accepts
lowercase `look` from the private graphics path, normalizes it through HUMAN,
dispatches `LOOK` through `dodcmd`, advances CLOCK one jiffy, runs the tenth
boundary, redraws, and returns normally after the bounded review hold.

## Presentation boundary

The corridor/status base uses the established logical gameplay renderer.  C5
suppresses that renderer's type-5 creature drawing, derives the source view
fields from the authoritative Game/CCB state, and invokes the C1 original PIC
Giant vector closure.  Thus the Giant geometry is cartridge-derived and its
successive positions are selected by naturally evolved creature state.

PLAY30's 81-jiffy WAIT occurs before every expanded word.  C5 advances the
source CLOCK by those 81 ticks, waits the same 81 real 60-Hz jiffies, writes
the word plus its source trailing space through HUMAN, and presents that word
before advancing.  The accepted dense run shows `EXAMINE`, then `PULL`, `PULL
RIGHT`, and `PULL RIGHT TORCH` as distinct visible states rather than exposing
the complete command at once.

`VIEWER:VIEW32/PDRAW` is also retained at the first Giant boundary.  After
AUTTAB 6 the Giant is at `(9,23)`: it is absent from the range-two forward cell
and present in the range-two right-adjacent cell.  The exact C1 right-peek
vector mode is therefore drawn.  After AUTTAB 7 it moves to `(9,22)` and the
ordinary forward vector mode takes over.  This recovers the partial
around-the-corner frame rather than jumping directly to a complete Giant.

## Protected heartbeat boundary

The native `/dhb` VIRQ handler is the sole logical heartbeat authority.  At
each expiration it toggles PIA PB1 (`$FF22` bit 1), toggles the latched visual
phase, increments the 32-bit edge generation, and reloads the countdown in one
bounded interrupt handler.  C5 never advances the visual heart from a second
timer.

The resident presenter samples that atomic phase/generation state.  A changed
generation renders the matching phase and transfers only the 32-by-7 heart
rectangle from one of two preloaded CoWin buffers.  It samples again after the
transfer; if a VIRQ edge occurred during PutBlk, it immediately reconciles the
new generation.  Frequent progress callbacks from VECTOR endpoint work, the
PIC line core, six-strip presentation, parser/overlay work, scheduler work and
one-jiffy sleeps bound the latency without exposing unfinished logical frames.
The PB1 bit is a cheap unchanged-edge gate; the driver snapshot remains the
authority after a change.

Startup enables CLK30 immediately before first dungeon construction, matching
the source ordering.  Heart-only transfers are gated until the first complete
status strip is installed, preventing the former isolated heart and white
rectangle.  The accepted assembly renderer completes that first construction
inside the initial heartbeat period.  Cleanup disables both presentation gates
before closing the native driver.

This relationship is a protected compatibility invariant for C6 and later:
one native logical edge produces one PB1 transition and one matching visible
phase, with bounded reconciliation and no accumulated drift.  The deterministic
[heartbeat trace](../../apps/daggorath/test/fixtures/compat-c5/heartbeat-trace.csv) covers repeated
VECTOR checkpoints and an edge during PutBlk; it does not encode emulator audio
latency.

This is a connected gameplay acceptance boundary, not a claim that every
intermediate text or corridor raster is already cartridge exact.  The current
EXAMINE result still uses the modern text-screen adapter, and a completed-frame
capture can observe CoWin's non-atomic strip transfer between stable logical
states.  Those are bounded presentation-adapter differences; they do not
alter the C4 state, command result, RNG, scheduler, CCB movement, word timing,
or the source Giant vectors.  Completing AUTTAB 9-17, combat and the remaining
connected text presentation is outside this C5 stop boundary.

## Live Level II result

The final artifact was byte-verified after staging on disposable media and run
on `coco3h`, 2 MB, RGB, restored `nos9_ready_v2`.  From the exact C4 dungeon it
showed word-at-a-time AUTTAB input, command-driven equipment changes, multiple
corridor redraws, the right-peek Giant at 72.26 seconds, and the first complete
forward Giant at 77.36 seconds.  The final 100-second review run used no input
injection and continued through multiple recognizable forward/close Giant
frames.  Human review accepted the word pacing, LOOK-to-dungeon transition,
partial peek, status-before-heart ordering, artifact-free heart rectangle, and
heart/PB1 synchronization.

- [EXAMINE boundary](assets/daggorath-attract-compat-c5/01-examine.png)
- [lit corridor](assets/daggorath-attract-compat-c5/02-lit-corridor.png)
- [equipment and approach](assets/daggorath-attract-compat-c5/03-equipment-approach.png)
- [right-peek Giant](assets/daggorath-attract-compat-c5/04-first-giant.png)
- [post-human-LOOK Giant](assets/daggorath-attract-compat-c5/05-human-look-giant.png)
- [clean Shell+ return](assets/daggorath-attract-compat-c5/06-clean-return.png)

## Level II budget

| Artifact | Program | CRC | SHA-256 | Data request |
| --- | ---: | --- | --- | ---: |
| `dodcplay` | 30,869 (`$7895`) | `B65698` | `6c620c267c78e827db22bea41983167713ac3dbb45f1b71e8c23d38204c2a0ae` | 15,600 (`$3CF0`) |
| `dodcmd` | 7,062 (`$1B96`) | `F344B1` | `3891bca111b269ede9a530e7a8076b098afbd8a2aa812838ab7326973190b40d` | one temporary block |
| `dodsched` | 8,192 (`$2000`) | `DA8721` | `82deac540cd172f9fdd99d26b2a6989c3d8193265ad60b8b2ded35d594bff6b0` | one temporary block |

C5's connected production leaves increase the compatibility executable from
the C4 two-block research target to four program blocks.  Process data remains
two blocks.  The live maps remain valid: normal execution is four program plus
two data plus one CoWin block (**7/8**); either command or scheduler execution
uses the final block (**8/8**).  Size growth is therefore recorded as placement
debt rather than misclassified as a semantic incompatibility.  Future work can
extract connected view/command leaves without changing this accepted state
trajectory.

The C5-only `DAG_COMPAT_HEARTBEAT` build boundary keeps heart buffers and
transfer code out of C1-C4 artifacts.  Normal C5 occupancy remains
`4 program + 2 data + 1 CoWin = 7/8`; exactly one of `dodcmd` or `dodsched`
temporarily consumes the eighth block.

## C5 acceptance boundary

C5 reaches the preferred boundary: exact C4 state becomes a living scheduler
state, a recognizable source-vector Giant is produced by normal CCB movement,
and a genuine human command is accepted and executed through the same
HUMAN/parser/game path.  No game state, RNG, creature position or framebuffer
milestone is forced to obtain that result.

Known bounded differences remain: CoWin six-strip transfers are not atomic as
a full 256-by-192 cartridge framebuffer update, the EXAMINE screen uses the
modern adapter, and host audio/display delivery latency is outside the logical
edge contract. AUTTAB 9-17, combat, repeat lifecycle, and broader human play
are deliberately outside C5.
