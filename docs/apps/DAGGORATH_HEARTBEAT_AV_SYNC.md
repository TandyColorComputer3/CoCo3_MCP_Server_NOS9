# Daggorath heartbeat audiovisual synchronization: measured stop gate

**Status, 2026-09-28:** the bounded 512×32 dirty-strip implementation remains uncommitted and is **not accepted**. Its first Term trace passed the unchanged 33.34 ms engineering gate, but the required exact-build workload run found three lit-render outliers: **34.381, 36.994, and 45.640 ms**. All 89 measured generations were individually presented and none were superseded, but 3/89 exceeded the gate. Per the acceptance stop rule, no production correction, GShell run, SSC run, or commit followed.

## Evidence and source relationship

The read-only original cartridge checkout is at commit `a94326f00ebb16a106b540c58bc2ccf5f7b66dac`; the maintained NitrOS-9 checkout consulted by the existing driver work remains `f470fa52eb172b59b22c1b722074998cb42de9b1`.

The original cartridge source at `/Volumes/SEDONA/Projects/daggorath-reference/COMMON.ASM:CLK30–CLK32` decrements `HEARTC` on its interrupt path. On zero it reloads `HEARTR`, toggles PIA `P.PIIOB` bit 1, then, if `HEARTF` is enabled, complements `HEARTS` and calls `TXTDPB` for both heart glyphs **in that same interrupt invocation**. `/Volumes/SEDONA/Projects/daggorath-reference/COMTXT.ASM:TXTDPB/DPB10` writes seven rows of each glyph directly to the selected display buffer. `/Volumes/SEDONA/Projects/daggorath-reference/PLOOK.ASM:INIVUX` initializes and enables the visual/audio heartbeat. Thus the source has one countdown and one transition, with the two visible glyph writes following the physical output edge; it does not schedule a separate graphics heartbeat. The cartridge's exact pixel scanout latency was **not** measured live here. At nominal 60 Hz, a completed display-memory write can wait up to the next ~16.67 ms video scan opportunity.

The native authority is unchanged: `apps/daggorath/src/audio/native/driver.asm:tick` toggles `$FF22` bit 1, then phase and `edgegen` in one bounded VIRQ callback; GetStat `$96` snapshots the pair atomically. At baseline, `screen_present_heart` called `screen_flip`, issuing a full-window GFX2 `PutBlk`; `game_render_with_progress` only offered a foreground checkpoint between long vector draws. Those baseline observations refer to the matching 28,950-byte `dodgame` module, CRC `50683B`. The repository's older `MCP/Documents/` and `DOCS_INDEX.md` pointers are absent; the original source, indexed EOU source/runtime evidence, and installed MAME trace are used with their limits stated.

## Read-only measurement method

A private observer appended to a complete disposable MCP Lua bridge watched the exact linked module's symbol-relative function entries and `presentedGeneration` write, the native driver's deadline, `$FF22` write, phase/generation writes, and the next MAME video-frame notifier. All intervals use MAME `machine.time`, including normal guest preemption. The first trace covers GShell-launched idle dark gameplay, a normal `TURN LEFT`, blocked `MOVE`, `PULL LEFT TORCH`, `USE LEFT`, a lit turn and lit movement. A second run counted vector-line entries and watched guest BEL writes while repeating the torch/turn path. Both used only the paired disposable boot floppy and VHD clone from the [accepted GShell checkpoint](DAGGORATH_EOU_INTEGRATION.md). The [machine-readable summary](assets/daggorath-heartbeat-sync/baseline-summary.json) retains the counts, category boundaries, quantiles, and caveats. Full JSONL traces remain in `/private/tmp/dod-gshell-live/` and are not Git assets.

**Completion definition:** The `presentedGeneration` write occurs in `main.c` *after* synchronous `screen_flip`/GFX2 `PutBlk` returns. It is therefore a measured completed-call boundary, not merely a request. It does not directly timestamp the visible pixel on a monitor. The next video-frame notifier followed this write by **1.090–16.043 ms** in the refined run (median **9.085 ms**); that bounds the next scan opportunity, not optical pixel appearance.

The first `$FF22` write established the observer's initial bit state; subsequent tick-body writes were counted as physical transitions. A PB1 write during shutdown was excluded by its different program counter. Extra writes to the same logical driver-static addresses under other MMU mappings were discarded by requiring a source-consistent generation/phase sequence around the deadline. Presentation samples were accepted only when the game module mapping and owned path were valid.

## Baseline timing

The long GShell run recorded **465** source-rate deadline/generation events. Every adjacent deadline was separated by the previous loaded rate byte: **zero countdown-spacing mismatches** in this interval, including rate changes from 46 to values 37–42. For **464 directly observed tick-body `$FF22` transitions**, deadline→audio edge was **30.730 µs median** (maximum **33.524 µs**); audio edge→phase write was **6.705 µs**, and edge→generation write was **15.086 µs**. The audio and logical authority are aligned within the same VIRQ. The failure is subsequent foreground presentation.

| Measured guest interval | Native generations | Exact generation completed: median / maximum | Superseded before separate completion | Longest wait for a *newer* completed frame |
| --- | ---: | ---: | ---: | ---: |
| Idle, dark resident dungeon | 104 | 261.915 / 626.856 ms | 0 | — |
| Dark `TURN LEFT` | 27 | 267.717 / 591.404 ms | 1 | 852.686 ms |
| Dark blocked `MOVE` | 25 | 338.489 / 503.107 ms | 0 | — |
| `PULL LEFT TORCH` and ordinary text/command work | 29 | 292.797 / 863.717 ms | 2 | 1,255.596 ms |
| `USE LEFT`, first lit redraw | 49 | 385.271 / 695.023 ms | 35 | 5,610.973 ms |
| Lit `TURN LEFT` | 39 | 354.626 / 555.030 ms | 29 | 6,372.585 ms |
| Lit `MOVE` and subsequent resident lit redraws | 192 | 338.018 / 681.911 ms | 160 | **9,142.191 ms** |

“Superseded” means the first later completed frame carried a *newer* generation. The sound edge for that earlier generation had **no separately completed corresponding visual heart transition** in this trace. One final generation was right-censored at normal EXIT. Across the entire first run, **237** generations completed exactly (median **290.346 ms**, maximum **863.717 ms**), **227** were superseded, and one had no later completion before exit. These are GShell-launched observations; the prior Term M6 measurements establish similar slow full-redraw stages but are not a separate Term audiovisual distribution. The visible heart appearing frozen during lit play, reported by the user during this test, is consistent with the measured supersession. It is not an audio-cadence failure.

The refined run isolated the small heart path: heart pixel expansion/dispatch→`screen_flip` entry took **3.163 ms median** (maximum **4.268 ms**), then full-window `PutBlk` entry→completed-generation write took **32.068 ms median** (maximum **32.240 ms**) in 53 matched calls. Even with an immediately available foreground process, this path costs about **35 ms** before the next video scan. The current 6809-compatible mapping loop is therefore a minor fraction of heart-only call cost; an HD6309 `TFM` or wider-copy optimization of those few bytes would not address the multi-second waiting time or full-window `PutBlk`. No 6309 assembly change is justified by these measurements alone.

The second lit run measured nine completed scenes of **2.333–6.262 s**. Its longest scene had 81 vector-line entries but only 14 progress callbacks. Other lit scenes had only **two** progress callbacks over **4.155–4.802 s**; the longest gap between progress checkpoints was **4.665 s**. Even a callback after every `wizard_line` call would not yet meet a frame-scale target: maximum gaps between successive vector-line entries were **335–701 ms** across these scenes. The original-derived line rasterizer and its caller currently execute for long bounded stretches without a presentation opportunity. A tiny status-region upload alone cannot fix that scheduling gap.

## Acceptance gate established before a correction

The original writes the heart in the same IRQ following the sound edge, with only normal video scanout remaining. The target guest has a **60 Hz / 16.67 ms** video quantum. A future correction should make the generation requested within **one video tick** of its native edge and finish the corresponding status-region display write within **two ticks (33.34 ms)**, including lit redraw and supported three-tick damaged cadence. It must complete **every** healthy and damaged generation before the next native deadline, with no superseded phase, and leave the sound edge/countdown unchanged. A further frame notifier may add up to one video tick before scanout; this is an engineering gate for an audiovisual trial, not a claim that a particular millisecond number has been psychophysically proven on real hardware. The original cartridge should be sampled under the same MAME display path before calling the threshold a fidelity match. The baseline's best completed-generation latency (**40.697 ms**) missed this two-tick gate; the exact-source correction trace below also misses it.

## Bounded correction trial

The uncommitted code keeps the VIRQ as the only deadline, sound, phase, and generation authority. `screen_present_heart` now issues the verified CoWin `GPLoad` packet for a separately owned 32×7 buffer and a 32×7 `PutBlk`, while also patching the mapped full buffer. The first attempt mapped a second GP buffer and failed live with E$MemFul **207** before heartbeat activation; the packet upload avoids that second mapping and respects the 64K process address-space limit. The small completed call is **18.327 ms median**, versus the baseline full-window `PutBlk` **32.068 ms median**. No HD6309 copy optimization was justified by the measured approximately 3 ms logical heart preparation.

Foreground progress callbacks were added within long vector raster loops, full-frame expansion (eight-row chunks), text and UI row preparation, and creature tick catch-up. The creature simulation still updates on the source-defined sixth video tick; presentation may check the VIRQ generation on each processed tick. Before and after a full flip, `main.c` reconciles to the latest atomic driver generation. Historical generations are not queued. A callback updates only the heart of the last complete displayed frame, never the incomplete dungeon image. The final logical frame is returned to its phase-zero template after progress callbacks; the presenter applies the live phase before display. The existing exact-frame test detected and verified that final restoration. These changes add no VIRQ graphics work, long interrupt masking, or CPU waveform loop.

The [machine-readable trial summary](assets/daggorath-heartbeat-sync/trial-summary.json) contains each module identity and measured section. The directly comparable final **live-tested** Term trial is the 30,940-byte module, CRC `AFA858`, with a dark idle, `PULL LEFT TORCH`, `USE LEFT`, lit `TURN LEFT`, lit `MOVE`, and `EXIT`. Among **122** observed deadlines with subsequent completion, **122** completed their own generation: median **30.223 ms**, p95 **143.090 ms**, maximum **174.149 ms**, **zero superseded**. The preceding UI-checkpoint trial had 126/126 exact completions, median **34.903 ms**, p95 **153.795 ms**, maximum **181.848 ms**. Both are a substantial improvement over the separate GShell baseline, but neither meets the two-tick gate. Trial sections use command-enqueue boundaries and should not be mistaken for precise command-completion boundaries. Exact optical pixel presentation and acoustic output were not measured.

The long tail persisted after rasterizer and per-video-tick checkpoints. The previous trace had a native deadline followed by **149.85 ms** without a foreground progress callback, then an approximately 18 ms small transfer; another idle deadline waited **131.78 ms**. The exact source and operation causing those gaps were unproven at that point. The following deeper trace resolves the major paths and the blocking GFX2 boundary. The two-tick gate, damaged-rate run, GShell run, and SSC coexistence remain open. **Stop without committing or declaring synchronization complete.**

## Exact-source latency decomposition and architectural stop

The final 30,953-byte `dodgame`, CRC `EFF41B`, including the exact-frame restoration fix, was copied byte-for-byte into the **disposable** VHD while MAME was stopped, then cold-booted as `coco3h -ramsize 2M` with the full private MCP bridge. The `test_gameplay_input_render.py` exact-frame test passed before launch; this live run confirms that the final-source build starts, draws and returns to Term with guest status `000`. The private observer added no guest writes. It used MAME 0.289 Lua [memory read/write taps](https://docs.mamedev.org/luascript/ref-mem.html) and a [frame notifier](https://docs.mamedev.org/luascript/ref-common.html), paired with the exact linked module symbols and the driver listing. All timings below use MAME `machine.time`. The [per-generation decomposition](assets/daggorath-heartbeat-sync/latency-decomposition.json) records every one of the 113 observed deadlines and the 52 completions beyond the 33.34 ms gate. A frame-PC observation is a 60 Hz sample, **not** proof of the first instruction the process executed.

`driver.asm:tick` is at module offset `$0255`, the native `$FF22` write at `$0290`, and the callback return at `$02B9` in the verified 701-byte driver listing. The trace records the VIRQ stack-write entry and stack-read return around each edge. For the 52 outliers with sufficient native-base observation, the deadline followed entry by **8.940 µs**, the audio edge followed the deadline by **30.730 µs**, phase followed audio by **6.705 µs**, generation followed audio by **15.086 µs**, and VIRQ returned **58.108 µs** after the deadline. These source/machine boundaries place the excess after publication, in foreground service. Adjacent native deadlines exactly matched the previous loaded rate byte in all 112 comparisons (90 deadlines at rate 46, 23 at rate 40); no cadence change was measured.

| Exact-final-source Term measurement | Result |
| --- | ---: |
| Deadline → completed corresponding generation | 113/113 exact; median 31.821 ms, p95 153.793 ms, maximum 302.292 ms |
| Superseded generations | 0 |
| Completed small heart call | median 18.263 ms, p95 19.364 ms, maximum 21.171 ms |
| Synchronous full-frame `PutBlk`/`I$Write` | 59 calls; median 32.010 ms, p95 32.832 ms, maximum 35.159 ms |
| Outliers beyond 33.34 ms | 52/113 |

The event-boundary attribution for **all 52** outliers is in the JSON: **20** had a batch of two or more `move_one` calls before the next service checkpoint, **10** crossed status/input/UI preparation, **16** crossed vector/raster work, **3** crossed other scene-render work, and **3** arrived during a synchronous full-frame GFX2 `PutBlk`. These are source-function interval attributions, not claims that each function consumes the entire recorded delay. The idle generation 3 spent **100.608 ms** across six `move_one` calls before its checkpoint at 102.355 ms and completed at 124.114 ms. A later lit-movement generation spent **276.848 ms** across 18 `move_one` calls, reached its checkpoint at 279.357 ms, and completed at **302.292 ms**. Individual `move_one` calls had 15.470 ms median and 24.380 ms maximum. Status rendering had 66.559 ms median and 98.792 ms maximum. These intervals show `dodgame` executing substantial foreground work after the VIRQ; they cannot all be attributed to an unscheduled process. Some 60 Hz CPU samples fall in OS-9/kernel code, so the trace does not allocate every microsecond between preemption and system-call work.

The **decisive bound** is independent of those CPU loops. Three deadlines occurred inside the paired full-frame `PutBlk`/`I$Write` interval. Their residual blocked times were **27.402, 24.242, and 21.730 ms**, with corresponding heart completions **49.856, 46.331, and 43.635 ms** after the deadline. The first of those had no foreground checkpoint before the full write returned. A deadline immediately after a representative full-frame write begins has a minimum path of approximately **32.010 + 18.263 = 50.273 ms**, before any dispatch, pixel preparation, or video scanout. The maximum measured full write implies an even higher possible bound. Thus a single foreground presenter making that blocking call **cannot guarantee** the established 33.34 ms gate for all edge phases. The small heart call itself remained bounded in this trace; it did not cause the 100–300 ms outliers. Optical pixel and acoustic transducer timestamps were not measured, so synchronous call completion remains the stated proxy.

Additional checkpoints inside creature, status, or vector work could shorten their specific gaps, and a narrow HD6309 optimization might help a measured CPU hotspot. Neither solves an edge that arrives while the same foreground process is blocked for approximately 32 ms in GFX2. The next architecture decision is whether the gate should be revisited with an explicit perceptual/scanout study, or whether a separately scheduled presenter with **verified** CoWin window/buffer ownership can issue the small update while the game process is blocked. Another possible direction is replacing the blocking full-frame path with a verified chunked/async presentation method. None is implemented or assumed safe here. The original driver, heartbeat cadence, and 33.34 ms gate remain unchanged.

The source-condition labels in this trace are bounded by MCP input **enqueue** times; buffered keystrokes completed later under the heavy read-tap observer. Function-entry and return events provide the stronger operation attribution. The architectural stop occurred before separate short/long SSC trials, GShell repetition, Shift-BREAK cancellation, an active FDC tap, and strict post-exit `date`/`pwd`. They remain acceptance requirements for any later architecture.

## First bounded dirty-strip implementation

The accepted architecture experiment was implemented without changing the native heartbeat deadline, sound edge, phase, or generation logic. The game still has one process and one exclusive GFX2 owner. The logical image remains 256×192, and presentation remains a 512×192 image at `(64,4)` with the two 64-pixel side regions unchanged.

The former mapped 12,288-byte viewport GP buffer is replaced by one reusable mapped 512×32 strip (2,048 bytes). A full image queues six vertical strips; a status/input update queues the bottom two. Each strip is expanded in four eight-row chunks, with authoritative-heartbeat service before the strip, after every expansion chunk, and after the synchronous `PutBlk`. The queued-mask operation is additive: preparing a UI update cannot accidentally discard an already queued full redraw.

Strip four contains logical heart rows 152–158. Immediately before that strip is submitted, its heart pixels are patched from the latest logical frame. This establishes the stale-frame invariant:

> A completed standalone heart update can never be replaced by an older heart from a subsequently completed strip. Before a heart-intersecting strip is written, that strip is reconciled to the newest authoritative phase/generation; after every strip, foreground service checks the authority again.

The visible heart represents current state. If several native generations occur before foreground service, the code converges directly to the latest generation and never replays obsolete phases.

The 32×7 heart uses two exact, unmapped CoWin GP buffers, one for each source-derived phase. The first occurrence of each exact 28-byte pattern uses `GPLoad`; subsequent heartbeat events use only the selected small `PutBlk`. This preserves exact pixels and avoids mapping another buffer into the Level II process. The earlier attempt to map a second graphics buffer returned E$MemFul 207 live; the final implementation maps only the 2,048-byte strip. It does not claim a complete process free-memory figure.

The remaining foreground starvation sources were measured and given bounded service points: the 6,144-byte logical clear, vector rasterization, status and message glyphs, input underlay rows, UI preparation, and creature/object/line-of-sight scans. A read of PIA PB1 is used only as a cheap foreground hint that an edge may have occurred. Any change triggers atomic native GetStat `$96`; PB1 never derives phase or generation and is not a second heartbeat clock. Graphics calls remain outside VIRQ context.

### Exact-build Term measurement

The live module was 33,042 bytes, data allocation 10,737 bytes, edition 1, CRC `C4778E (Good)`, SHA-256 `6d6b75533dfb51579713da14f40498c8cd39dc508f2e385c830b8a2850286d68`. The [machine-readable dirty-strip result](assets/daggorath-heartbeat-sync/dirty-strip-final.json) is derived from the first complete `game_main` through `native_removed` interval of the private trace.

| Exact dirty-strip Term run | Result |
| --- | ---: |
| Authoritative generations completed exactly | 73/73 |
| Intentionally superseded generations | 0 |
| Edge/generation → completed matching heart | median 14.136 ms; p95 22.000 ms; max 29.905 ms |
| Results beyond 33.34 ms | 0 |
| 512×32 synchronous strip transfers | 72 |
| Strip transfer | median 9.179 ms; p95 9.349 ms; max 9.587 ms |
| Heart-only completed call | median 8.078 ms; p95 8.975 ms; max 25.103 ms |
| Six-strip `screen_flip` | median 337.758 ms; max 368.615 ms |
| `game_render_with_progress`, dark resident scenes | median 578.332 ms; max 587.434 ms |

All six strip positions were transferred in the run. The bottom two appeared more often because status/input updates queue them independently. The high first-use heart maximum includes initial GP-buffer loading; once both patterns were cached, the normal event path was the bounded prepared-buffer write. Completion is still the synchronous GFX2 return proxy, not optical scanout.

This result is directly comparable to the exact-source monolithic trace (31.821/153.793/302.292 ms, 52/113 over the gate). An intermediate strip build that still loaded the heart on every event reached 23.253 ms median, 32.995 ms p95, and 36.777 ms maximum, with 3/60 over the gate. Caching both exact phases supplied the final margin without changing source pixels, audio timing, or generation semantics.

The total foreground image path is not optimized here. The architecture investigation measured approximately 183 ms for 6,144→12,288-byte horizontal expansion and 136 ms for a 12,288-byte process-memory copy. The strip implementation removes the separate whole-image copy, but its observed six-strip flip remains roughly 338 ms and a dark render roughly 578 ms. A full dark render plus presentation is therefore about 0.9 s in this instrumented run. No HD6309 optimization was made. Expansion and copying remain separate future targets after correctness and complete live acceptance.

Normal guest teardown printed status `000` and produced exactly one `native_removed` event. The private HTTP wrapper returned 500 after the shell marker had already appeared; subsequent strict MCP `date` and `pwd` commands both returned structured status `000`. This distinguishes guest exit/Term recovery from a private long-RPC wrapper failure.

### Acceptance still open

The exact-build Term distribution demonstrates that the bounded architecture can satisfy the gate in that run. It does not complete the requested matrix. The private keyboard observer did not deterministically reach the intended lit/movement state on a subsequent attempt, so no exact-build claim is made for lit redraw, movement/turn, creature-active redraw, or heartbeat occurrence at every strip position. Exact-build GShell, Shift-BREAK status `003`, concurrent short/long SSC effects, repeated launch, and active resident FDC observation were also not rerun. Existing host regressions and previously accepted live checkpoints cover their behavior before this exact module, but they are not substituted for current live evidence. The implementation therefore remains uncommitted.

## Final acceptance attempt: workload gate failure

A lower-overhead private observer and command-by-command input synchronization corrected the earlier harness ambiguity without changing production. The exact 33,042-byte, CRC `C4778E` module was verified on the disposable VHD before launch. MAME again ran `coco3h` with 2 MB. The session began from `os9_restore_ready`, loaded the disposable `dhbpack`, and launched through graphics-aware `os9_run`.

The synchronized sequence reached the initial dark screen, pulled and used the torch, completed a lit turn, completed movement, and remained in lit resident gameplay while creatures advanced. A [snapshot after movement](assets/daggorath-heartbeat-sync/dirty-strip-lit-move.png) shows the expected lit vector scene and intact status/side regions. Input-enqueue times bound the broad intervals; exact command completion is not inferred from enqueue alone. The outlier attribution below instead uses native deadlines, vector-render function events, heart-call entry/return, strip-write intervals, and matching generation completion.

The [machine-readable stop result](assets/daggorath-heartbeat-sync/dirty-strip-workload-stop.json) records the exact figures:

| Exact-build Term workload | Generations | Exact presentations | Superseded | Median | p95 | Maximum | Over 33.34 ms |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Initial dark resident interval | 13 | 13 | 0 | 15.660 ms | 29.410 ms | 29.410 ms | 0 |
| Torch-use processing interval | 8 | 8 | 0 | 14.935 ms | 25.649 ms | 25.649 ms | 0 |
| Lit-turn processing interval | 13 | 13 | 0 | 14.254 ms | 19.972 ms | 19.972 ms | 0 |
| Lit movement and subsequent resident creature redraw | 55 | 55 | 0 | 14.301 ms | **34.381 ms** | **45.640 ms** | **3** |
| **Combined** | **89** | **89** | **0** | **14.424 ms** | **29.410 ms** | **45.640 ms** | **3** |

The three failures were generations 46, 80, and 81. None arrived during a 512×32 strip write. All occurred while the foreground was in lit logical vector-render work before the next effective heartbeat service:

| Generation | Deadline → observed vector work | Deadline → heart call | Heart call | Completed latency |
| ---: | ---: | ---: | ---: | ---: |
| 46 | 20.890 ms | 37.499 ms | 8.141 ms | **45.640 ms** |
| 80 | 8.903 ms | 25.343 ms | 9.038 ms | **34.381 ms** |
| 81 | 11.667 ms | 28.107 ms | 8.886 ms | **36.994 ms** |

The bounded 32×7 write remained small. The excess was accumulated before that call, in the lit logical renderer/checkpoint path. Generation 46 occurred during the movement redraw; generations 80 and 81 occurred during later creature-triggered lit redraw work. This is a measured foreground service/checkpoint limitation, not stale-generation replay, strip-transfer blocking, native cadence drift, or SSC ownership.

Two other native deadlines did occur inside synchronous strip-six (`y=164`) writes, with 6.725 and 8.376 ms remaining in those writes; both matching generations completed under the gate. The acceptance run had not forced deadlines through the other five positions when the workload gate failed. Following the explicit stop rule, that matrix was not continued.

The resident I/O tap recorded only expected software-clock accesses at `$FF50/$FF51`; active rb1773/FDC accesses in `$FF40..$FF4F` and `$FF74..$FF76` were **zero**. No obsolete generation was displayed after a newer one. The final rendered lit snapshot showed no observed tearing, stale strip, heart/status corruption, or side-region overwrite, but intermediate visual acceptance is incomplete because the run stopped on latency.

Redraw performance remains a separate concern. Thirty-six status-only two-strip flips measured 114.990 ms median, 132.712 ms p95, and 134.562 ms maximum. Eleven full six-strip flips measured 351.531 ms median and 368.528 ms maximum. Representative synchronous strip writes totaled about 51.5–56.9 ms per full flip; most of the remaining flip interval is CPU horizontal expansion, heartbeat checkpoints/dispatch, preemption, and private observer overhead. The initial dark logical render measured 587.589 ms. The measured lit-movement logical render spanned 6.533 s under instrumentation before its 351.531 ms flip. These are completed operation intervals, but the trace does not assign every microsecond or establish an uninstrumented user-visible value. No 6309 optimization was attempted.

The failure happened before exact-build GShell, real Shift-BREAK cancellation, short/long SSC coexistence, repeated launch, and all-six-strip forced alignment. Those are not reported as passing. The disposable MAME instance was stopped cleanly after preserving the trace. Production source and tests were not changed in response to the failure.

## Regression and limits

The exact-final-source module returned to Term with `DODGAME ... EDGE 113 PRESENTED 113 MAX_PHASE_LAG 1 ... STATUS 0` and one `native_removed` event in the trace; the private MAME host then returned `{"ok":true}` from `coco_stop`. The `MAX_PHASE_LAG 1` counter is a sampled maximum pending generation, not evidence of zero latency. The accepted GShell trace separately recorded child status `000` and VIRQ teardown. This correction was **not** tested live under GShell, Shift-BREAK cancellation, three-tick damaged heartbeat, concurrent short/long SSC effects, or active FDC tap in the exact-source run. Those acceptance cases remain mandatory before a heartbeat commit. Normal shell return is evidenced by the [exact-source Term screenshot](assets/daggorath-heartbeat-sync/final-source-term-exit.png). The prior private MCP `os9_run pwd` call was rejected at preflight because that private session had not called `os9_restore_ready`; strict post-exit shell statuses are still unverified.

In the refined GShell run, the watched GShell graphics-buffer BEL byte (`$07`) had **zero** writes while the game was active. The user heard repeated system bells during the longer trial. Those sounds were not timestamp-matched to the observer, so their source remains **unresolved**; they must not be presented as proof of background GShell keyboard duplication or as native heartbeat audio. The accepted GShell trace has a positive desktop BEL control and negative controls for particular game commands.

**Current validation:** All **19** `apps/daggorath/test_*.py` scripts passed (**237** documented checks), including exact frame equality, strip packet/pixel equality, UI-mask preservation, lifecycle, native heartbeat, audio M1/M2/M3, and gameplay regressions. `npm test` passed **115/115**, `npm run build` passed, `git diff --check` passed, and all ten local links in the two changed reports resolve. Two clean gameplay builds are byte-identical: `dodgame` **33,042 bytes**, data allocation **10,737 bytes**, edition **1**, CRC **`C4778E (Good)`**, SHA-256 **`6d6b75533dfb51579713da14f40498c8cd39dc508f2e385c830b8a2850286d68`**. Earlier baseline `dhbpack` reproducibility and identity are unchanged but were not rebuilt for this presentation-only slice.

**Canonical media and state integrity:** SHA-256 remained `db2f0f444073d3b88a3610a63ec9f7d2e2dd4299347181faa6b2b2aa9637de2c` for stock `63SDC.VHD` (mode `0444`), `4c6bdc0cd2f68aa57a9c75f75f15fe429f30926aa522917dd8d14aa1b9ae622e` for development `63SDC-MCP-DEV.VHD`, `9a51adb8656b9f003c7f822352c291487362f1b4c43aac1a16672d5d2bcb4c40` for `63EMU.DSK`, and `a0f4ee094db471c596b9ecb9826922f80d982d2571fe7e72672a255ebca2effa` for canonical `nos9_ready_v2`. Stock remains mode `0444`; only private cloned media were mounted. The heartbeat changes, report, and trial summary remain **uncommitted**.

## Lit-vector checkpoint investigation and correction

The remaining failures were narrowed with disposable instruction-boundary taps against the exact `C4778E` build. The blocking unit was not lighting, clipping, a framebuffer store, or a strip transfer. It was the setup before `wizard_line_progress` reached its existing two-pixel callback:

- `gameplay/game.c:scale` was called four consecutive times for each vector. CMOC lowers each signed scale to 32-bit helper calls; live intervals between scale entries were approximately 5.8–7.2 ms each.
- `original/logical.c:wizard_line_progress` then constructed the fixed-point endpoints and computed `sx` and `sy` with two generated 32-bit divisions before its first old callback. That setup took approximately 11–13 ms.
- A deadline arriving near the start of those operations could therefore wait through four scales plus line setup. This explains the measured 25–37 ms pre-heart delays without implicating the 8–9 ms heart PutBlk.

With a measured steady-state heart-operation worst case of 9.038 ms, the unchanged 33.34 ms gate leaves **24.302 ms** for scheduling and uninterrupted foreground work. The correction places service at natural source boundaries: before each vector, after the first two coordinate transforms, and at `wizard_line_progress` entry before its generated 32-bit setup. No logical coordinate, fade sample, framebuffer pixel, native VIRQ operation, cadence, or sound edge changes.

The exact corrected module is 33,178 bytes, data allocation 10,737 bytes, edition 1, CRC `1F237B (Good)`, SHA-256 `73c9e48f34765b840e1dae76fbfc39f5e89d828a9a3000d657f9fb993dd52953`. Two clean builds were byte-identical.

The deterministic initial-dark → torch pull/use → lit turn → lit move → resident creature workload passed the gate. Extended resident/repeated-turn sampling retained the same bound. The compact evidence is [vector-checkpoint-result.json](assets/daggorath-heartbeat-sync/vector-checkpoint-result.json), and the final lit frame is [vector-checkpoint-lit.png](assets/daggorath-heartbeat-sync/vector-checkpoint-lit.png).

| Corrected exact build | Result |
| --- | ---: |
| Authoritative generations presented exactly | 298/298 |
| Superseded generations | 0 |
| Median edge → completed matching heart | 14.210 ms |
| p95 | 22.161 ms |
| Maximum | **29.709 ms** |
| Beyond 33.34 ms | **0** |
| Active rb1773/FDC accesses | **0** |

Only `$FF50/$FF51` software-clock activity was observed in the resident I/O range. The screenshot shows intact canonical geometry, status area, heart, and side reserves. Completion remains synchronous small-PutBlk return rather than optical scanout.

The trace also identifies later HD6309 optimization work, separate from this synchronization fix. `scale` spends most of its generated body in general 32-bit helpers even though its measured operand range can be proven much smaller; a verified 16-bit formulation may remove that overhead. Fixed-point line setup needs more than signed 16 bits and is a candidate for bounded native 6309 `Q`/`MULD`/`DIVQ` work. Horizontal 1×→2× expansion is a transform, so raw `TFM` cannot perform it by itself; lookup/wider stores are candidates. Plain copies can use `TFM`. None of those optimizations was implemented here.

Host validation passed all 19 Daggorath scripts (237 documented checks), 115/115 MCP tests, TypeScript build, reproducible game builds, and `git diff --check`. Strict post-run `date` and `pwd` both returned status `000`. The disposable game returned to Term before a fresh ready-state restore. Exact-build GShell, Shift-BREAK `003`, and concurrent short/long SSC cases still require a final live matrix before this heartbeat work can be committed; they are not inferred from the passing Term workload.

## Final exact-build GShell acceptance stop

The final GShell AIF trial used the same 33,178-byte, CRC `1F237B`
artifact and the disposable `gshell_trace_idle` environment. The private
observer retained the discovered child module/data identity across Level II
process-map switches and required the module-header signature before treating
the child as active. This corrected instrumentation only; no guest or
production source changed.

The run covered dark resident play, torch pull/use, a lit turn, movement, and
subsequent lit creature processing. The
[machine-readable stop result](assets/daggorath-heartbeat-sync/gshell-final-acceptance-stop.json)
and [last resident frame](assets/daggorath-heartbeat-sync/gshell-final-acceptance-stop.png)
preserve the evidence.

| Exact-build GShell workload | Result |
| --- | ---: |
| Authoritative generations presented exactly | 227/227 |
| Superseded generations | 0 |
| Median edge to completed matching heart | 14.054 ms |
| p95 | 28.107 ms |
| Maximum | **108.451 ms** |
| Beyond 33.34 ms | **11** |
| Active rb1773/FDC accesses | **0** |

All six strip positions were exercised, the side regions remained intact in
the captured frame, and resident I/O in the watched ranges was limited to the
expected `$FF50/$FF51` software-clock accesses. This confirms that the stale
generation and multi-second starvation defects stayed fixed, but the unchanged
33.34 ms gate **did not pass under GShell**.

The trace separates two remaining failure classes. Ten failures spent most of
their excess interval before the foreground heart call began (26.209--94.989
ms before entry in the longer cases); the available instrumentation cannot
further distinguish child scheduling, the SCF/input wait path, or an
unobserved blocking call for seven of those ten. Two failures overlap known
logical render/progress work before service. One distinct generation reached
the heart call in 9.281 ms but its synchronous small-buffer GFX2 write took
95.123 ms, producing the 108.451 ms maximum. Normal small writes in the other
failures took about 4.09--5.40 ms. Therefore the failed bound cannot be
attributed solely to missing source checkpoints: the GShell run contains both
late foreground dispatch and one exceptional synchronous graphics-path stall.

The user independently identified the repeated system bell as text delivered
to GShell indiscriminately. During this bounded rerun, MCP text injection was
used only after the Daggorath graphics window was visibly active; GShell
selection/launch used the keyboard-mouse F1 path. The bell observation remains
a harness/input-routing constraint and is not heartbeat audio evidence.

Following the required stop rule, the game and disposable MAME instance were
closed cleanly. Repeat launch, accepted Shift-BREAK cancellation, short/long
SSC coexistence, final regression rerun, and commit were not performed after
the failure and are not claimed. The implementation remains uncommitted, and
the 33.34 ms gate remains unchanged.

## Controlled Term/GShell cause investigation

The earlier stop established a launch-correlated difference, not a GShell
cause. GShell is the parent/launcher; `dodgame` owns a separately opened
graphics path. A controlled follow-up therefore used the exact same `1F237B`
module, private VHD contents, `dhbpack`, MAME configuration, dirty-strip
implementation, source-derived seed/workload, and read-only observer in three
runs. Each environment began from a freshly restored state epoch. Results are
preserved in
[term-gshell-ab.json](assets/daggorath-presentation-architecture/term-gshell-ab.json).

| Controlled run | Generations | Superseded | Median | p95 | Maximum | Over 33.34 ms |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| Term, deterministic dark/lit/movement workload | 382/382 | 0 | 13.946 ms | 21.099 ms | 37.163 ms | 1 |
| GShell launch plus leaked-text/system-bell workload | 362/362 | 0 | 14.016 ms | 22.206 ms | 178.268 ms | 6 |
| GShell mouse launch, quiescent, no text injection | 128/128 | 0 | 14.076 ms | 23.330 ms | 36.462 ms | 2 |

The Term and quiescent-GShell medians and p95 values are effectively the same
at the measurement precision. The quiescent control contained no SndDrv frame
samples and no failure after the two first-use generations. Continued GShell
residency by itself therefore did **not** reproduce the long tail.

### Actual long-tail cause

The scheduler-attribution observer sampled the currently mapped module at each
video frame while a heartbeat generation remained pending. The four long
foreground-start failures in the bell-producing GShell trial contained four,
five, ten, and five consecutive samples in `SndDrv`, followed by VTIO. Their
completed latencies were 71.802, 90.672, 178.268, and 88.693 ms. This is the
EOU system-bell/`SS.Tone` path consuming execution, consistent with the user's
direct observation that indiscriminately injected text caused audible bells.
No equivalent SndDrv samples occurred in the matched Term or quiescent-GShell
controls.

The anomalous synchronous-write case is also resolved more narrowly. Its heart
call began 79.084 ms after the native edge. The small write then remained
outstanding for 95.137 ms, but five consecutive frame samples during that
interval were mapped to `SndDrv`, not GrfDrv or `dodgame`. The wall-clock call
duration therefore includes process descheduling while a graphics request was
outstanding; it is not evidence that CoWin/GrfDrv executed continuously for 95
ms. Ordinary writes in the surrounding failures took about 4.14--5.42 ms.

### Runtime contract and remaining bound

The game-visible ownership contract matched across launch paths: module base
`$6000`, process data base `$0000`, owned local graphics path 3, return path 1,
one mapped strip buffer, and the same native heartbeat driver. The earlier AIF
probe established the actual inherited difference: the GShell child was PID 4
with parent GShell PID 3 and inherited W3 on paths 0--2; the Term control
inherited Term on paths 0--2. Before the Term launch, `procs` showed the active
shell plus the three startup window shells. This investigation did not obtain
a complete, simultaneous kernel process-descriptor snapshot for every frame,
so it does not claim exact priority/age equivalence.

The dirty-strip/checkpoint architecture remains sound for the demonstrated
steady path: no generation was superseded and the long bell-related tail does
not originate in vector checkpoints or stale-strip reconciliation. The strict
gate is still not universally proven. Both GShell controls had startup
first-use failures at generations 2 and 13 (36.462 and 35.126 ms), and the
instrumented Term run had one 37.163 ms failure involving SCF while a write was
outstanding. These are concrete warmup/scheduling cases; they were not fixed in
this investigation.

The next bounded implementation should first prevent host automation from
duplicating gameplay text into the resident GShell path, then explicitly
prewarm/measure both exact heart buffers before accepting heartbeat service.
Neither change is made here. A future transient mouse menu remains a live dirty
region and must avoid bell-producing input leakage. A modal help/configuration
window may intentionally pause gameplay and can use ordinary CoWin behavior
without the live heartbeat gate, provided ownership and restoration remain
explicit.

### BFX2/CFX2 prior art boundary

The EOU inventory contains only the compiled 2,381-byte
`/dd/SOURCECODE/ASM/BASIC09/bfx2_ver1` module. Its export table exposes the
ordinary GFX2 operations, including `DefBuff`, `GPLoad`, `Get`, and `Put`, but
no inspected source or documentation establishes a faster PutBlk, dirty-update
algorithm, buffer mapping technique, or 6309 acceleration. No CFX2 source or
documentation was found in this repository, the EOU source index, current
`nitros9-reference`, or the inspected CoCo development/MV references. Those
sources are missing; no optimization claim is inferred from their names.

## Final-acceptance startup stop

Final acceptance stopped before changing production because the first-use
uncertainty resolves to a required initialization change.

Generation 2 in the quiescent GShell control is the first request for the
second exact heart phase. `screen_open` currently defines only GP buffer
`196/2`; `screen_present_heart` then defines `196/3`, performs its first
28-byte `GPLoad`, and finally issues `PutBlk` after heartbeat service has
already started. The three synchronous writes measured 9.061, 8.845, and
4.997 ms. Together with 9.523 ms before heart-call entry, this produced the
36.462 ms failure. The equivalent Term occurrence happened to complete in
29.578 ms; that timing difference does not remove the initialization work.

Generation 13 was different. It used one already cached buffer write taking
5.059 ms, but presentation did not begin for 26.209 ms; frame attribution
sampled `dodgame` and then GrfDrv. It is a scheduling/foreground-service
sample, not GP-buffer allocation. The single instrumented Term failure was
also cached: 28.131 ms before entry and a 4.985 ms write, with SCF sampled
while the write was outstanding. It did not reproduce in the prior 121- or
298-generation Term runs, so current evidence classifies it as rare ordinary
SCF/scheduler interaction rather than a repeatable production rendering
defect. The strict gate remains unchanged.

The smallest proposed production correction is to create **and GPLoad both**
exact 32×7 phase buffers as part of presentation initialization, before
`native_heartbeat_enable`. Initialization must not visibly PutBlk either
phase, change `presentedGeneration`, or invent a heartbeat. Runtime
`screen_present_heart` would then select and PutBlk an already valid phase;
its existing cache comparison and cleanup ownership remain intact. This is
the natural ownership boundary because `screen_open` already creates the
first heart buffer and runs before native heartbeat activation.

No such change was made. Per the requested decision gate, the final Term and
GShell workloads, lifecycle/cancellation matrix, SSC coexistence, regressions,
and ready-to-commit decision were not run. The private host also still needs a
confirmed game-window input gate: command posting must wait for the observed
owned game path/window rather than enqueue text against whichever selected
window happens to be active. The quiescent mouse-launch control proves the
bell-free baseline but is not a substitute for that full input harness.

## Separated workload/lifecycle checkpoint

The accepted two-heart-buffer initialization correction produced the exact
production `dodgame` module at 33,765 bytes, data size 10,765, edition 1, CRC
`CB25FE`, and SHA-256
`e175f961030e5e54c63c296d864a2d1329418914f7aff29185713bc9a4d7be00`.
Its complete deterministic Term workload presented 172/172 generations with
zero supersession: 14.075 ms median, 22.717 ms p95, 27.535 ms maximum, no
result beyond 33.34 ms, and zero active FDC accesses.

GShell input automation is now excluded from timing acceptance. One real,
input-ready-gated host character still correlated with five `SndDrv` samples.
Two attempted instruction-level Lua hooks logged bytes but did not reproduce
the real OS-9/CMOC `game_input` semantics and are rejected as evidence.
Production input handling was not changed.

A disposable workload module replaced only `game_input`; it linked the exact
production main loop, parser, state, creature scheduler, logical renderer,
presentation layer, native heartbeat client, and OS wrappers. It supplied
`PULL LEFT PINE TORCH`, `USE LEFT`, `TURN RIGHT`, and `MOVE`, followed by
resident empty-input polls. The resulting lit/navigation and creature-active
sample completed six full scene renders and presented 123/123 authoritative
generations with zero supersession: 13.995 ms median, 20.901 ms p95, 26.894
ms maximum, zero beyond 33.34 ms, and zero active FDC accesses. The disposable
input provider is not repository or production code.

Separate GShell AIF lifecycle testing observed normal child status `000`, one
VIRQ removal, zero callbacks after that removal, graphics return to the same
GShell desktop, and a second launch/`000` exit. The private observer retained
its `removed` latch when the second heartbeat became active, so its repeated
run post-removal callback count is invalid. The second child status is valid;
the zero-callback repeated-run gate must be rerun with that observer latch
reset. Physical EXIT input produced the already-characterized bell path and
is not included in timing statistics.

[Machine-readable checkpoint](assets/daggorath-heartbeat-sync/separated-acceptance-checkpoint.json)
records the valid measurements and remaining gates. Exact-build Shift-BREAK
`003`, clean repeated teardown observation, SQUEAK/PHASER coexistence, strict
post-run `date`/`pwd`, and the final regression matrix remain open. The change
is not ready to commit.

## Final remaining-gate attempt: bounded stop

The corrected private repeated-run observer now clears its removal latch before
classifying the first callback of a new registration. Two real GShell launches
then produced two independent native-heartbeat registrations and removals with
**zero callbacks after either final removal**. The already accepted normal child
statuses remain `000`, `000`. A separate exact-artifact lifecycle run used the
established physical MAME SHIFT-then-BREAK chord (BREAK two video frames after
SHIFT, both released after eight frames). It returned guest status **003**,
removed the VIRQ, produced no later callback, restored the GShell desktop, and
permitted another launch. Repeating the chord on that launch again returned
`003` with a second clean registration/removal pair. No instruction-level input
substitution is used as evidence.

SSC coexistence used a disposable wrapper that started the unchanged
`dodaudio` service before entering the unchanged production game main. This
avoids falsely charging the service's approximately six-second initialization
to heartbeat presentation. The disposable `game_input` provider then issued
ordinary semantic SQUEAK and PHASER requests. The private MAME taps observed
SSC commands `$D8` and `$CA`, followed by the normal `$CF` shutdown/stop writes.
Across both effects, native heartbeat produced **93** generations and all 93
were presented, with zero supersession and zero active rb1773/FDC accesses.
Latency to the established PutBlk-return/generation-completion boundary was
13.970 ms median, 21.864 ms p95, and **35.508 ms maximum**. One generation
exceeded the unchanged 33.34 ms gate.

That failure is attributed, not hidden. Generation 4's native edge/publication
occurred at emulated time 119.701868. A production logical-render progress
callback ran 0.791 ms later, but did not service the pending generation. Heart
presentation began 26.620 ms after the edge; its bounded write entered at
30.403 ms, returned at 35.434 ms, and the generation record completed at
35.508 ms. SQUEAK had been sent 1.232 seconds before this edge and PHASER had
not yet been sent, so neither SSC command transmission nor an active effect
transition explains the delayed foreground service. The measured defect is a
remaining checkpoint/service gap in the production logical draw path. The
heart write itself remained bounded.

The game subsequently restored Term, removed the native heartbeat, and emitted
no post-removal callbacks. Strict `date` and `pwd` were attempted through the
private MCP, but that cold-boot-only MCP correctly rejected them at preflight
because `os9_restore_ready` had not established its shell-ready session token;
they are not reported as passing in this run. Since the presentation gate
failed first, the final regression/build matrix was not repeated and no
production change was made. The milestone is **not ready to commit**.

[Machine-readable final-stop evidence](assets/daggorath-heartbeat-sync/final-acceptance-stop.json)
records the accepted checkpoints, lifecycle evidence, SSC commands, exact
failed-generation decomposition, and remaining shell/regression gates.

## Bounded logical-draw correction and final evidence

The 35.508 ms SSC-coexistence outlier was reproduced as CPU work before the
line renderer's first progress callback.  After the existing checkpoint, the
generated CMOC code performed the second endpoint's two 32-bit coordinate
scales and then initialized the line renderer's two 32-bit fixed-point
accumulators.  That uninterrupted sequence accounts for the measured 25.829
ms interval from the preceding callback to `screen_present_heart` entry.  The
SSC timeline excludes an audio cause: SQUEAK preceded the edge by 1.232 s and
PHASER had not been issued.

The bounded correction adds one generation-aware foreground checkpoint after
the second endpoint is scaled and before fixed-point line setup.  It does not
change vectors, rasterization, heartbeat cadence, VIRQ work, SSC ownership, or
GFX2 ownership.  A checkpoint may present only the latest authoritative
generation; it never replays an obsolete generation.

Two corrected exact-code SSC trials covered 180 generations.  Trial 1 was
88/88 with 13.882/21.665/28.879 ms median/p95/maximum; trial 2 was 92/92 with
13.545/20.717/26.872 ms.  Both had zero superseded generations, zero results
above 33.34 ms, zero active FDC accesses, and the expected SSC command sequence
`D8 CA CF CF` (SQUEAK, PHASER, and normal stop/shutdown).  This exceeds the
93-generation sample in which the rare failure was found.

The corrected module is `dodgame`, edition 1, re-entrant OS-9 6809 object,
33,815 bytes (`$8417`) with 10,765 bytes (`$2A0D`) of data, CRC `9D2CD2`, and
SHA-256 `c4054c761987664e17c58a81a842afefced3221fe241855ba6626708b20c51f6`.
Two clean builds were byte-identical.  All 20 Daggorath test scripts (237
checks) passed; MCP tests passed 115/115 and the TypeScript build passed.

The previously accepted real GShell lifecycle evidence remains applicable:
two AIF launches exited `000`, and the corrected private observer recorded a
fresh registration and final removal for each run with zero callbacks after
each removal.  Real SHIFT-then-BREAK cancellation returned `003`, removed the
VIRQ, restored GShell, and allowed another launch.  The bounded rendering
checkpoint does not participate in launch, input, or teardown.  A fresh
attempt to automate another mouse launch was rejected because it would reuse
the known host-input path that can also ring GShell's bell; no contaminated
timing sample is substituted for the clean lifecycle evidence.

After restoring `nos9_ready_v2`, strict `date` and `pwd` calls both completed
with status `000` and a verified ready prompt.  Canonical hashes remain
`db2f0f...de2c` (stock VHD), `4c6bdc...622e` (development VHD),
`9a51ad...c40` (boot DSK), and `a0f4ee...effa` (`nos9_ready_v2`); the stock
VHD remains read-only.  The exact machine-readable correction/acceptance
record is [final-corrected-acceptance.json](assets/daggorath-heartbeat-sync/final-corrected-acceptance.json).

The heartbeat synchronization implementation is ready for commit review.
Future work remains separate: the repeating cartridge attract loop, leading
`.` command convention, all four message rows, optional Classic Mode border,
mouse/menu UI, map/inventory presentation, surgical HD6309 rendering work,
and eventual 6809 back-port investigation.
