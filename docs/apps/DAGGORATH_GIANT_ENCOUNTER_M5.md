# Daggorath Giant Encounter M5

## Current disposition: modern-engine attract remains visually divergent

The bounded [overlay-slot repair](DAGGORATH_M5_OVERLAY_SLOT.md) supersedes the
initial 207 blocker below. The heartbeat pack now uses a retained nonmapping
load. This restores the scheduler/command slot without changing gameplay or
adding a phase. The public continuation now executes AUTTAB commands 1--10, but
M5 is **not** accepted through beat 026.

The final continuous modern-engine review used public `/d1/daggorath`, an exact
staged `dodintro` SHA-256
`4b7f4a30cf6b5589559efe0341b077aa77a8d6f2772fd7de8d2ce6aa0fe225dc`,
and a completed-frame observer at 59.92 frames/second. The uninterrupted run
lasted 192.20 seconds and established:

- native heartbeat frames alternate at 0.734--0.784 seconds around the source
  46-jiffy rate, then approximately 0.551 seconds after the source rate changes
  to 33 jiffies;
- the former multi-second EXAMINE/inventory heart freeze is gone because long
  overlay text and resident primary-text rendering expose bounded progress
  points to the one authoritative native heartbeat;
- source `LUKNEW/PUPDAT` behavior issues exactly nine presentation requests in
  the tested AUTTAB prefix: seven resident dungeon redraws and two overlay-view
  deferrals while `dodcmd` cannot coexist with `dodsched`;
- the visible path reaches Wizard, PREPARE, map, lone-dot dungeon, EXAMINE,
  torch use, first MOVE, shield and sword states;
- beat 019 contributes sparse creature pixels, but beat 020 never becomes a
  human-recognizable Stone Giant, and the next two source-requested approach
  redraws still omit the cartridge's recognizable creature geometry.

Consequently GRAWL, command-10 combat presentation and audio choreography are
not accepted by this evidence. A command occurring in authoritative state does
not substitute for a missing visible storyboard beat. Dense AVI, observer code,
staging media and contact sheets remain disposable and are not repository
content.

The exact regression contract is nine source presentation requests, seven safe
resident flips, two overlay-view deferrals and seventeen total command/source
flips. `dodcmd` and `dodsched` never coexist, and rendering remains forbidden
while `dodcmd` owns the eighth DAT slot.

## Initial M5 attempt — historical failure record

2026-10-02 Pacific. Phase A is committed; M5 remains an uncommitted candidate.
No push. No beat 027 or AUTTAB 11–17 work.

The oracle remains the [cartridge storyboard](DAGGORATH_CARTRIDGE_STORYBOARD.md)
and [ratified master plan](DAGGORATH_EOU_MASTER_PLAN.md).
A continuous public `/d1/daggorath` run reached the map, then returned to Term
with **207, Process Memory Full**, before initial dungeon/EXAMINE in this candidate.
This is a Level II mapping blocker, not evidence about Giant geometry or sound.

## Phase A checkpoints

| Full commit | Message | Files | Insertions/deletions |
|---|---|---:|---:|
| `0920c082843f35a3502478771e5b7e9c2747dce3` | feat: add nonblocking Daggorath audio queue | 27 | +1302/−58 |
| `cba96e959cbf0896c56474262252b6ef26666586` | feat: integrate Daggorath cartridge opening | 60 | +1154/−15 |
| `7d473cfc48d3ae96824acca797ffd306b0bcec2e` | perf: optimize Daggorath maze RNG on 6809 | 7 | +250/−3 |

Mixed files were staged as semantic subsets; the completed checkpoint sequence
preserved the original working source content. Intermediate audio and opening
states were also built/tested. Reports/evidence accompany their implementation.
Immediately after these commits only excluded `cfg/` and `snap/` remained.
The exact checkpoint file inventory is at the end of this report.

## Current uncommitted candidate

- `build_opening.py`: link the established command/scheduler hosts, context-safe
  gateways and credited audio queue into public `dodintro`.
- `src/gameplay/demo.c`: extend the inherited-window opening with real scheduler
  and command modules, bounded AUTTAB 1–10, persistent primary text, EXAMINE view
  persistence, and existing native-heartbeat/optional-audio integration.
- `primary-text.c/.h`: ASCII adaptation into the existing source character/cursor/
  scroll system; ordinary prompt handling remains owned by that system.
- New `test/public_m5.c` and `test_public_m5.py`: exercise the public runner with
  real Game/command/scheduler bodies and host substitutes for OS/mapping boundaries.
- This report and a small evidence set. No raw recording or private observer is
  repository content. No existing test assertion/mock was changed for M5.

The candidate omits standalone `doddemo`'s 36-tick visual review dwell in the public
command path, retains the portable scheduler's explicit logical waits, and moves
M4's bounded final review hold to the end. These candidate choices have **not**
passed live M5 choreography acceptance. Standalone `doddemo` remains byte-identical.
No production correction was made after the first mapping blocker was isolated.

## Continuous run and first failure

Exact staged production `dodintro` SHA-256:
`a33367730bf3791085bed389b0386e36ba72e3f9fa4434a8ec553c54276cbdd7`.
Every staged module was extracted and compared with its host artifact.
Environment: fresh restored disposable EOU, `coco3h`, 2 MB, RGB, public launch;
no manual internal-module preload. Completed AVI frames run at 59.922748 fps.

| Observed boundary | Zero-based frame | Seconds from recording start |
|---|---:|---:|
| First map detected in late-opening window | 4367 | 72.877 |
| Last completed map frame | 5044 | 84.175 |
| First returned Term frame | 5045 | 84.192 |
| Terminal error text fully available, curated frame | 5393 | 89.999 |

These times include restore/launch lead-in; they are **not** normalized cartridge
beat timings or individual syscall measurements. Map detection was restricted to
frames after 60 seconds to exclude emulator-reset/initial-shell images.
The later error frame is separately selected after the terminal text completed.
No M5 renderer, heartbeat-latency, or audio-onset duration can be measured from
this failed run.

A second disposable build added compact stage/result storage and printed it only
after normal graphics cleanup. It produced:

```text
M5 BOUNDARY 3 ERROR 207 LINK 207 CALL 255 UNLINK 255
Error #207 - Process Memory Full
```

| Stage | Actual evidence |
|---|---|
| Retained command-module load | Returned success |
| Retained scheduler-module load | Returned success |
| First `DOD_SCHED_INIT` | Entered `game_scheduler_call` |
| Scheduler `F$Link` | Raw returned error **207** |
| Mapped scheduler ABI/body | Not entered; private `CALL=255` sentinel unchanged |
| Per-call `F$UnLink` | Not entered; private `UNLINK=255` sentinel unchanged |
| Caller | Preserved error, ran cleanup, returned 207 |

`255` is a diagnostic unvisited value, **not an OS-9 error result**. The diagnostic
was a distinct 29,678-byte module (also four program blocks), SHA-256
`0fd93ffc68907c0590c4b0de99565c5c0935422cda40c6e7c2a932fdce082342`.
It was separately extracted/byte-verified. Its measurements establish control
flow/error attribution, not fine-grained wall-clock latency. Production failure
was independently observed with the uninstrumented candidate.

### Narrowest demonstrated cause and source evidence

The first temporary scheduler mapping fails. Neither mapped scheduler execution
nor the reverse callback gateways caused this observed failure.

Authoritative reference inspected: external `nitros9-reference` commit
`f470fa52eb172b59b22c1b722074998cb42de9b1`:

- `defs/os9.d:1268–1276` defines `E$MemFul` at 207, “Process Memory Full”.
- `level1/modules/ioman.asm:1636–1657`: `FLoad` loads and then calls `F$ELink`,
  returning mapped module/entry pointers. This differs from its nonmapping
  `FNMLoad` entry.
- `level1/modules/kernel/flink.asm:129–185`: Level II link processing allocates
  process DAT space for the module.
- Current `opening-heartbeat.c` performs `F$Load /d1/dhbpack` (`os9 $01` confirmed
  in generated assembly), retains its ownership until cleanup, and does not
  perform an intervening `F$UnLink`. The host shim's generated `103F00` instruction
  confirms the failing scheduler operation is `F$Link`, with B error preserved.

**Inference requiring a DAT check:** the extra program block in public `dodintro`,
combined with the heartbeat pack's retained mapping, can consume the nominal
free overlay slot. M4 `dodintro` used three program blocks; M5 uses four. The
nominal four program + two data + one CoWin = seven calculation does not account
for that retained `F$Load` mapping. No complete live DAT snapshot was taken in
this run, so the exact occupant of each logical slot is not claimed as measured.

Next bounded investigation: observe actual DAT ownership before the failing link
and verify the heartbeat pack's mapping/reference lifecycle. Preserve mandatory
scheduler failure handling and heartbeat ownership. Do not suppress 207, bypass
the scheduler, remove stack checking, or enlarge the mapping architecture merely
to advance the demo. This report does not implement that repair.

## Beat classification

For this stopped run, MISSING means no corresponding completed EOU frame was
produced. For beats after 013 it records non-reachability only; their individual
geometry/timing/audio defects were not evaluated.

| Beat | Cartridge reference | EOU primary status | Evidence/disposition |
|---|---|---|---|
| 013 | EXAMINE inventory and transcript | MISSING | Map → Term 207 before EXAMINE |
| 014 | PULL RIGHT TORCH | MISSING | Not reached; no individual fidelity assessment |
| 015 | USE RIGHT | MISSING | Not reached |
| 016 | Lit LOOK | MISSING | Not reached |
| 017 | First MOVE | MISSING | Not reached |
| 018 | Shield | MISSING | Not reached |
| 019 | Sword and first Giant pixels | MISSING | Not reached |
| 020 | Recognizable Giant | MISSING | Not reached |
| 021 | Nearer Giant | MISSING | Not reached |
| 022 | Large Giant | MISSING | Not reached |
| 023 | Giant occlusion | MISSING | Not reached |
| 024 | Giant reappearance | MISSING | Not reached |
| 025 | ATTACK RIGHT with visible Giant | MISSING | Not reached |
| 026 | Post-kill | MISSING | Not reached |

No MATCH is inferred from the host AUTTAB test. The first failed requested beat
is 013, although startup aborts before re-presenting beat 012 in this candidate.

## State, Giant, heartbeat and audio

- **Authoritative state:** demo initialization completed sufficiently to draw the
  full map. No complete live Game/RNG dump was taken at failure. Independent exact
  initialization/RNG/Combat regressions pass; that does not prove every live byte.
- **Logical renderer:** beat-013 EXAMINE rendering was not called live. Host runner
  verifies its retained `.EXAMINE` transcript, cursor and blank-row state.
- **Physical presentation:** map then Shell+ error, not a valid beat-013 frame.
- **Giant:** no live first-pixel, recognizable, growth, occlusion, reappearance,
  attack or post-kill evidence obtained. No claim that this candidate fixes the
  reported invisible Giant.
- **Heartbeat:** `dodintro` native open/rate/enable occurs after the failed scheduler
  initialization and was not reached. Pack loading did occur. No gameplay-cycle,
  PB1/visual-phase or 33.34-ms presentation-gate measurement is available.
- **Audio:** queue startup is after the failing boundary and was not reached.
  GRAWL/WHOOSH/KLINK/BANG submission, audible onset, ordering, credit, high-water,
  retries, overflow, rejected groups and transport-failure metrics are **N/A for
  M5**, not measured zeros. Earlier queue tests remain passing independent evidence.

## Curated evidence

- [ORIGINAL EXAMINE | actual EOU failure](assets/daggorath-giant-m5/013-original-eou-blocked.png)
- [Last visible production map](assets/daggorath-giant-m5/013-last-visible-eou-map.png)
- [Production Term/error frame](assets/daggorath-giant-m5/013-eou-memory-error.png)
- [Separate private link diagnostic](assets/daggorath-giant-m5/013-private-link-diagnostic.png)
- [Frame/time selection metadata](assets/daggorath-giant-m5/capture-boundary.json)

The paired image uses untouched 640×236 screens with a label bar; the right side
is explicitly failure evidence, not a substitute EXAMINE capture. Dense AVIs,
observer scripts, staging disks and private diagnostic source remain under
`/private/tmp`, excluded from commits. Both private sessions were stopped.

## Initial candidate module and data budget (before slot repair)

All numbers below are measured artifacts, not code-size estimates. Callable
modules have no standalone process-data request; their state remains caller-owned.

| Module | Bytes | Data request | Program blocks | Data blocks | CRC |
|---|---:|---:|---:|---:|---|
| dodgame | 27,287 | 11,109 | 4 | 2 | `8BB638` |
| dodcmd | 7,004 | 0 | 1 | 0 | `C3B942` |
| dodsched | 8,192 | 0 | 1 | 0 | `DA8721` |
| doddemo | 23,905 | 10,896 | 3 | 2 | `9A932C` |
| daggorath | 863 | 1,573 | 1 | 1 | `7CE837` |
| dodwiz | 24,562 | 7,855 | 3 | 1 | `A25DCA` |
| dodintro | 28,269 | 11,027 | 4 | 2 | `8D1A81` |
| dodaudio | 10,086 | 2,142 | 2 | 1 | `CD93E9` |

`dodgame` has 3,433 bytes below policy 30,720; `dodcmd` has 1,188 bytes below
8,192; `dodsched` is exactly 8,192 and has **no growth margin**. Public `dodintro`
data remains within two blocks. File-size limits pass, but the live one-free-slot
requirement **fails** at first scheduler link. These are distinct gates.

Phase-A `dodintro` was 22,568 bytes / 10,924 data / CRC `1C53C0`; candidate growth
is +5,701 module bytes and +103 data bytes, crossing three → four program blocks.

| Module | SHA-256 (current candidate; all except dodintro unchanged from Phase A) |
|---|---|
| dodgame | `d9b7e4b98b3bd6631bd0b5dacd4fda34465eaeda6de00ea8532564a2d806956a` |
| dodcmd | `fbf2424f991afe7610a815113b1084372afa495272587be187b9a1872726b946` |
| dodsched | `82deac540cd172f9fdd99d26b2a6989c3d8193265ad60b8b2ded35d594bff6b0` |
| doddemo | `e6d16db714768a8708ece38d75a9580b6e797cc877dd18681769d16630b2b0ab` |
| daggorath | `cb950a95d2c2352fee61943600ccccfdf0e42f219c69551a606e6baecb72c7bb` |
| dodwiz | `8dfe734d862f621c07b60db5badc62e3ec5282cc4fbe57ab8250957f625b3c3f` |
| dodintro | `a33367730bf3791085bed389b0386e36ba72e3f9fa4434a8ec553c54276cbdd7` |
| dodaudio | `f6a87a81b692ed92a44be1f9431895e3259c173bf96dace006092514c89d5ca5` |

## Initial candidate regression and integrity record

- Phase A: **39/39 Daggorath scripts**, **MCP 115/115**, TypeScript build pass.
- M5 candidate: **40/40 Daggorath scripts**, including new public runner coverage.
  Existing exact maze/RNG, Combat, Bag, EXAMINE, scheduler, audio and lifecycle
  checks remain intact. No existing tests were weakened or edited for M5.
- New host runner proves commands 1–10, transcript after EXAMINE, nonoverlapping
  command/scheduler calls, cleanup, player `(9,22)`, power 6136 and CCB18 dead.
  Its simulated mapping cannot prove Level II free-slot availability; live fails.
- Two clean builds of all eight Phase-A modules were byte-identical. Two clean M5
  opening builds are byte-identical; ToolShed/module CRC checks pass. Standalone
  demo remains byte-identical. No MCP/TypeScript sources changed after validation.
- `git diff --check` and local report/documentation links checked.
- Canonical media/save state unchanged; `63SDC.VHD` remains mode `0444`.
- Private MAME/observer sessions stopped; no private MCP experiment remains.
  The user's normal configured MCP connector is not an experiment and is preserved.

Canonical SHA-256 values:

| File | SHA-256 |
|---|---|
| `63SDC.VHD` | `db2f0f444073d3b88a3610a63ec9f7d2e2dd4299347181faa6b2b2aa9637de2c` |
| `63SDC-MCP-DEV.VHD` | `4c6bdc0cd2f68aa57a9c75f75f15fe429f30926aa522917dd8d14aa1b9ae622e` |
| `63EMU.DSK` | `9a51adb8656b9f003c7f822352c291487362f1b4c43aac1a16672d5d2bcb4c40` |
| `nos9_ready_v2.sta` | `a0f4ee094db471c596b9ecb9826922f80d982d2571fe7e72672a255ebca2effa` |

## Remaining M4 polish debt

Preserved in the opening checkpoint; none was silently declared resolved:

1. Final Wizard dissolve → PREPARE still has substantial unexplained/EOU-only
   latency (14.035-second observed interval; no new attribution here).
2. PREPARE → map improved from 20.543 to approximately 9.16 seconds after the
   source-equivalent 6809 RNG leaf, versus 6.442 seconds cartridge reference.
3. Heartbeat-pack acquisition remains approximately 2.7 seconds.
4. Black → dungeon remains slower than cartridge (earlier 1.201-second evidence).
5. Four-row blank-cell rendering has an optimization opportunity (earlier 0.783 s).
6. Beat-012 heart's two-pixel difference needs correctly phase-matched review.
7. Wizard fade buzz/KABOOM remain future opening-audio work.

No opening-polish changes were made to address this M5 blocker.

## Phase A exact classified file inventory

Paths below are repository-relative. Mixed files legitimately occur in multiple
checkpoints; their staged subsets were separately reviewed. Disposable `/private/tmp`
material was never staged. Excluded cfg/snap files were preserved, not deleted.

| Path | Checkpoint/category |
|---|---|
| `apps/daggorath/build_demo.py` | M3 audio: build infrastructure; maze RNG: build infrastructure |
| `apps/daggorath/build_gameplay.py` | M3 audio: build infrastructure; maze RNG: build infrastructure |
| `apps/daggorath/src/audio/audio.h` | M3 audio: intended production |
| `apps/daggorath/src/audio/client.c` | M3 audio: intended production |
| `apps/daggorath/src/audio/event.c` | M3 audio: intended production |
| `apps/daggorath/src/audio/ipc.c` | M3 audio: intended production |
| `apps/daggorath/src/audio/ipc.h` | M3 audio: intended production |
| `apps/daggorath/src/audio/queue.c` | M3 audio: intended production |
| `apps/daggorath/src/audio/service.c` | M3 audio: intended production |
| `apps/daggorath/src/gameplay/demo.c` | M3 audio: intended production; M4 opening: intended production |
| `apps/daggorath/src/gameplay/main.c` | M3 audio: intended production |
| `apps/daggorath/src/os9.c` | M3 audio: intended production |
| `apps/daggorath/src/platform.h` | M3 audio: intended production |
| `apps/daggorath/test/audio_client.c` | M3 audio: test |
| `apps/daggorath/test/audio_queue.c` | M3 audio: test |
| `apps/daggorath/test/audio_service.c` | M3 audio: test |
| `apps/daggorath/test/demo_heartbeat.c` | M3 audio: test |
| `apps/daggorath/test/demo_runner.c` | M3 audio: test |
| `apps/daggorath/test/gameplay_lifecycle.c` | M3 audio: test |
| `apps/daggorath/test/gameplay_text.c` | M3 audio: test |
| `apps/daggorath/test/scheduler_attract.c` | M3 audio: test |
| `apps/daggorath/test_audio_queue.py` | M3 audio: test |
| `apps/daggorath/test_combat.py` | M3 audio: test |
| `apps/daggorath/test_demo_heartbeat.py` | M3 audio: test |
| `docs/apps/DAGGORATH_AUDIO_IPC_ARCHITECTURE.md` | M3 audio: documentation/evidence |
| `docs/apps/DAGGORATH_DYNAMIC_PRESENTATION_M3.md` | M3 audio: documentation/evidence |
| `docs/apps/assets/daggorath-dynamic-m3/av-observation.json` | M3 audio: documentation/evidence |
| `apps/daggorath/build.py` | M4 opening: build infrastructure |
| `apps/daggorath/build_opening.py` | M4 opening: build infrastructure; maze RNG: build infrastructure |
| `apps/daggorath/src/gameplay/opening-heartbeat.c` | M4 opening: intended production |
| `apps/daggorath/src/gameplay/opening-heartbeat.h` | M4 opening: intended production |
| `apps/daggorath/src/gameplay/opening-map.c` | M4 opening: intended production |
| `apps/daggorath/src/gameplay/opening-map.h` | M4 opening: intended production |
| `apps/daggorath/src/gameplay/opening-phase-module.asm` | M4 opening: intended production |
| `apps/daggorath/src/gameplay/primary-text.c` | M4 opening: intended production |
| `apps/daggorath/src/gameplay/primary-text.h` | M4 opening: intended production |
| `apps/daggorath/src/logical.h` | M4 opening: intended production |
| `apps/daggorath/src/main.c` | M4 opening: intended production |
| `apps/daggorath/src/opening-launch.c` | M4 opening: intended production |
| `apps/daggorath/src/opening-module.asm` | M4 opening: intended production |
| `apps/daggorath/src/original/logical.c` | M4 opening: intended production |
| `apps/daggorath/src/phase-chain.c` | M4 opening: intended production |
| `apps/daggorath/src/phase-chain.h` | M4 opening: intended production |
| `apps/daggorath/src/presentation.c` | M4 opening: intended production |
| `apps/daggorath/src/presentation.h` | M4 opening: intended production |
| `apps/daggorath/test/lifecycle.c` | M4 opening: test |
| `apps/daggorath/test/wizard_clock.c` | M4 opening: test |
| `apps/daggorath/test_opening_m4.py` | M4 opening: test |
| `apps/daggorath/test_phase_chain_m4.py` | M4 opening: test |
| `apps/daggorath/test_wizard_clock.py` | M4 opening: test |
| `docs/apps/DAGGORATH_CARTRIDGE_OPENING_M4.md` | M4 opening: documentation/evidence |
| `docs/apps/DAGGORATH_CARTRIDGE_STORYBOARD.md` | M4 opening: documentation/evidence |
| `docs/apps/DAGGORATH_EOU_MASTER_PLAN.md` | M4 opening: documentation/evidence |
| `docs/apps/DAGGORATH_M4_OPENING_LATENCY_ROOT_CAUSE.md` | M4 opening: documentation/evidence |
| `docs/apps/DAGGORATH_M4_OPENING_TIMING_ATTRIBUTION.md` | M4 opening: documentation/evidence |
| `docs/apps/DAGGORATH_M4_VISUAL_AUDIT.md` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/002-first-strokes.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/004-wizard-complete.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/005-welcome.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/007-fade.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/008-near-black.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/009-prepare.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/010-map.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/012-dungeon-prompt.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/clock-capture/002-t31.5.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/clock-capture/003-t35.5.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/clock-capture/004-t37.5.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/clock-capture/005-t39.5.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/clock-capture/006-t41.5.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/clock-capture/007-t44.5.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/clock-capture/008-t48.5.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/clock-capture/009-t64.5.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/clock-capture/010-t83.5.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/clock-capture/011-t86.5.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/clock-capture/012-t87.5.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/visual-audit/002-original-current-eou.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/visual-audit/003-original-current-eou.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/visual-audit/004-original-current-eou.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/visual-audit/005-original-current-eou.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/visual-audit/006-original-current-eou.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/visual-audit/007-original-current-eou.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/visual-audit/008-original-current-eou.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/visual-audit/009-original-current-eou.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/visual-audit/010-original-current-eou.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/visual-audit/011-original-current-eou.png` | M4 opening: documentation/evidence |
| `docs/apps/assets/daggorath-opening-m4/visual-audit/012-original-current-eou.png` | M4 opening: documentation/evidence |
| `apps/daggorath/src/gameplay/game.c` | maze RNG: intended production |
| `apps/daggorath/src/gameplay/maze-random.asm` | maze RNG: intended production |
| `apps/daggorath/test_maze_oracle.py` | maze RNG: test |
| `docs/apps/DAGGORATH_M4_MAZE_OPTIMIZATION.md` | maze RNG: documentation/evidence |
| `cfg/coco3h.cfg` | excluded: unrelated/disposable; excluded |
| `cfg/default.cfg` | excluded: unrelated/disposable; excluded |
| `snap/coco3h/0006.png` | excluded: unrelated/disposable; excluded |
| `snap/coco3h/0007.png` | excluded: unrelated/disposable; excluded |
| `snap/coco3h/0005.png` | excluded: unrelated/disposable; excluded |
| `snap/coco3h/0004.png` | excluded: unrelated/disposable; excluded |
| `snap/coco3h/0010.png` | excluded: unrelated/disposable; excluded |
| `snap/coco3h/0000.png` | excluded: unrelated/disposable; excluded |
| `snap/coco3h/0001.png` | excluded: unrelated/disposable; excluded |
| `snap/coco3h/0003.png` | excluded: unrelated/disposable; excluded |
| `snap/coco3h/0002.png` | excluded: unrelated/disposable; excluded |
| `snap/coco3h/0009.png` | excluded: unrelated/disposable; excluded |
| `snap/coco3h/0008.png` | excluded: unrelated/disposable; excluded |
| `snap/private/tmp/dod-cart-recovery/gtime-screen.png` | excluded: unrelated/disposable; excluded |
