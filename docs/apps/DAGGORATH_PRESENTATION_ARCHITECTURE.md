# Daggorath presentation architecture: bounded feasibility investigation

**Status (2026-09-28): Decision A has a first bounded implementation, but final acceptance failed.** The 33.34 ms audio-edge-to-completed-heart gate remains unchanged. A favorable exact-build Term run passed, but the representative lit workload measured three failures at 34.381–45.640 ms. All 89 generations were presented individually with no supersession. The measured excess occurred before the small heart call during lit logical vector rendering, rather than in a strip transfer. The implementation remains uncommitted and unchanged pending a separate correction decision.

## Scope and evidence

The target was Ample MAME 0.289, `coco3h`, 2M, RGB, and the verified EOU Level II environment. All experiments used disposable copies of the boot floppy and development VHD. MAME `machine.time` supplied the common emulated-time clock. Phase stores immediately before and after guest `I$Write` calls establish synchronous call boundaries; they do not establish the exact optical scanout instant.

The exact-source heartbeat baseline remains 113/113 generations presented, zero superseded, median 31.821 ms, p95 153.793 ms, and maximum 302.292 ms. Three native edges that occurred during a synchronous 512×192 `PutBlk` completed visually 43.635–49.856 ms later. See [the latency decomposition](assets/daggorath-heartbeat-sync/latency-decomposition.json).

EOU behavior takes precedence here. Source evidence came from indexed `/dd/SOURCECODE/ASM/NITROS9/SCF/cowin_beta6.asm`, the installed module observations in [RUNTIME_MATCHES.md](../source-index/RUNTIME_MATCHES.md), disposable runtime probes, and the EOU Level II Technical Reference. The modern comparison checkout was `/Volumes/SEDONA/Projects/nitros9-reference` at commit `f470fa52eb172b59b22c1b722074998cb42de9b1`, especially `level2/coco3/modules/cowin.asm` and `level2/cmds/grfdrv.asm`. The indexed EOU source is not claimed byte-identical to the installed modules: installed CoWin is edition 2, 7,537 bytes, CRC `3C70C2`; installed GrfDrv is edition 14, 8,167 bytes, CRC `199209`.

Machine-readable results are in [presentation-measurements.json](assets/daggorath-presentation-architecture/presentation-measurements.json). Private probe source and raw traces remain under `/private/tmp/dod-av-live/`; they are disposable instrumentation, not application source or durable evidence assets.

## Current graphics path and ownership

The game owns one type-5 640×200 `/w`. Its logical framebuffer is 256×192 packed pixels (6,144 bytes); the presentation layer expands it horizontally to the canonical 512×192 central viewport (12,288 bytes), leaving 64 pixels on each side. The current full viewport is an owned mapped GP buffer followed by synchronous GFX2 `PutBlk`. The unaccepted heartbeat work uses a separate 32×7 owned GP buffer loaded with `GPLoad` and displayed with `PutBlk`.

EOU CoWin `PutBlk` (`cowin_beta6.asm:PutBlk/L0849/L00F7`) delegates through GrfDrv. Its dispatch uses a system task and the shared `G.GfBusy`; the `Select` commentary says GrfDrv is non-reentrant. Upstream CoWin preserves that organization and comment. These sources make serialization plausible, but only runtime evidence can distinguish driver serialization from ordinary OS-9 scheduling on the installed EOU image.

## Forced mid-transfer heartbeat experiment

### Method

A disposable parent process issued 40 prepared 512×192 `PutBlk` calls. A child observed the actual native `/dhb` VIRQ generation at a forced one-tick rate and issued 96 prepared 32×7 writes. The rate was chosen only to force events inside the approximately 31 ms transfer. It is not a proposed gameplay cadence.

The observer recorded full-write request and synchronous return, the native `$FF22` heartbeat edge/generation, and small-write request and synchronous return. Two path arrangements were tested:

1. **Inherited/shared descriptor:** the child used a duplicate of the parent's selected graphics path.
2. **Separately opened descriptor:** the parent opened a second path to the same named `/w`, duplicated that descriptor for the child, and verified both path names before the trial.

Both runs returned guest status `000`, reaped the exact child PID, killed owned GP buffers, closed the owned window, released the heartbeat driver, and restored Term.

### Results

| Arrangement | Full writes | Native edges inside full calls | Small requests begun and completed before that full call returned | Full median / p95 / max | Prepared heart median / p95 / max |
| --- | ---: | ---: | ---: | ---: | ---: |
| Inherited/shared descriptor | 40 | 61 | **4** | 31.088 / 32.420 / 66.553 ms | 4.137 / 5.531 / 37.512 ms |
| Separately opened same-window descriptor | 40 | 53 | **0** | 31.087 / 32.048 / 32.420 ms | 4.137 / 5.147 / 37.512 ms |

The four inherited-path overlaps occurred in two full calls. Each full call was preempted twice and stretched to about 66.5 ms. In one call the small requests began 34.195 and 50.364 ms after the full request and completed 38.332 and 54.501 ms after it; the second call had the same pattern within measurement precision. Thus a small same-descriptor write **can** execute and complete while the original process remains inside its synchronous full `I$Write`.

The small-call maxima in the table include process descheduling while the synchronous call was outstanding; the isolated prepared-region curve below has a 4.378 ms maximum. This distinction is why a helper cannot be credited with a scheduling guarantee merely from its typical call cost.

No small request even reached its user-space pre-call phase marker during a separately opened-path full write. The small request followed the preceding full completion by 1.280–6.857 ms in the paired samples.

### What the experiment proves

- The prior no-overlap observation was not proof of unconditional CoWin/GrfDrv serialization. An inherited/shared descriptor did interleave when OS-9 scheduled the child.
- The inherited result does not provide a bound: most heartbeat generations did not cause a small call to begin inside the active full call, and the interleaved full call became roughly twice as long.
- The separately opened-path result cannot distinguish “child was not scheduled” from “driver/path work was queued.” Its user-space request marker never occurred during the full call, so it does not prove a lower-level CoWin queue.
- A presenter process that exclusively owns GFX2 still cannot interrupt its own synchronous full call. Two concurrent writers could sometimes interleave, but that is nondeterministic and conflicts with the preferred single-owner model.

Consequently a dedicated process alone is not evidence-backed as the heartbeat fix.

## Prepared dirty-region timing curve

The corrected probe defined, measured, and killed one GP buffer at a time. It made 40 prepared `PutBlk` calls per region, with one OS-9 tick between calls. The final 512×192 point comes from the separately opened-path forced trial because the standalone size process could not reallocate that large GP buffer after exercising the preceding sizes; that failed allocation returned EOU error 237 and contributes no timing claim. The full point uses the same dimensions, prepared-buffer packet, and 40-call measurement boundary.

| Region | Payload bytes | Median | p95 | Maximum |
| --- | ---: | ---: | ---: | ---: |
| 32×7 | 28 (GP allocation rounded to 32) | 4.211 ms | 4.211 ms | 4.378 ms |
| 512×8 | 512 | 5.051 ms | 5.051 ms | 5.218 ms |
| 256×16 | 512 | 5.283 ms | 5.450 ms | 5.450 ms |
| 512×16 | 1,024 | 6.142 ms | 6.142 ms | 6.309 ms |
| 512×32 | 2,048 | 8.323 ms | 8.490 ms | 8.758 ms |
| 512×64 | 4,096 | 12.685 ms | 12.852 ms | 13.105 ms |
| 512×96 | 6,144 | 17.472 ms | 17.483 ms | 17.639 ms |
| 512×192 | 12,288 | 31.087 ms | 32.048 ms | 32.420 ms |

The equal-payload comparison, 512×8 versus 256×16, differs by only 0.232 ms at the median. Total bytes dominate; width/row shape adds a smaller measurable cost.

These are prepared-buffer `PutBlk` times. The current production-style heart operation also prepares pixels and performs `GPLoad`; its measured complete call path was about 18.263 ms median and 21.171 ms maximum. With that existing heart path, a 512×64 maximum plus heart maximum is about 34.276 ms before checkpoint overhead, so 64-row chunks do not provide a safe 33.34 ms argument. A 512×32 maximum plus heart maximum is about 29.929 ms, leaving roughly 3.4 ms for foreground recognition and dispatch. This is narrow but plausible and must be validated live. If two preloaded small/large heart GP buffers later remove per-event `GPLoad`, the budget improves, but that has not been implemented or tested and cannot be assumed.

## CPU, expansion, copying, and GFX2 decomposition

A separate disposable process used the current 6809-compatible nibble-expansion algorithm and bracketed only the called operation. Forty repetitions produced:

| Operation | Input/output | Median | p95 | Maximum | Potential HD6309 benefit |
| --- | --- | ---: | ---: | ---: | --- |
| Logical-to-physical horizontal expansion | 6,144 → 12,288 bytes | 182.876 ms | 182.886 ms | 182.994 ms | Yes, but this is a transform; `TFM` alone cannot duplicate packed pixels. Lookup/wider-store assembly may help. |
| Process-memory `memcpy` | 12,288 → 12,288 bytes | 136.058 ms | 136.068 ms | 136.344 ms | Yes; a bounded native-mode copy/`TFM` comparison is justified later. |
| Prepared GFX2 `PutBlk` | 512×192, 12,288 bytes | 31.087 ms | 32.048 ms | 32.420 ms | Not from application rendering code. It is synchronous driver/path work. |

The exact-source deep trace previously observed `screen_prepare_progress` at roughly 287–323 ms and whole foreground rendering from about 332 ms to several seconds. Those intervals include checkpoints, callbacks, game drawing, and preemption; the isolated figures above identify the expansion and memory-copy costs without claiming cycle-exact cartridge timing.

The decomposition yields two separate conclusions:

1. HD6309 work could materially reduce construction/expansion latency and improve how often the foreground reaches presentation service points.
2. It cannot remove the indivisible GFX2 transfer time. A 6309 optimization is complementary to dirty-region presentation, not a substitute for it.

Upstream `level2/cmds/grfdrv.asm:L0C98/L0CA2` conditionally uses HD6309 `TFM` for some screen/memory copying. The installed GrfDrv is not source-matched, so this report does not claim that the EOU binary uses those branches.

## Bounded dirty-region architecture

Routine monolithic 512×192 presentation should be replaced, subject to an implementation milestone, by one graphics owner issuing ordered dirty rectangles with a conservative maximum indivisible viewport strip of **512×32** until live evidence supports a larger bound.

The required invariant is:

> Before any dirty strip that intersects the heart/status rectangle is submitted, its pixels must be reconciled to the newest authoritative heartbeat generation. After every strip return, heartbeat service runs before another strip begins. A standalone heart write also updates the compositing source state, so a later stale strip cannot overwrite it.

If multiple heartbeat generations occur, only the latest authoritative phase is presented; obsolete intermediate visual states are not replayed. This matches the recovered source relationship: the heart display represents current VIRQ-owned phase, while the same event creates the native audio edge.

Exact visual output is preserved by splitting the same canonical 512×192 image into vertical rectangles without changing its pixels, origin, scaling, logical framebuffer, or 64-pixel side reserves. Unchanged regions need no transfer. Dungeon, message/status, future menu, and later side panels can use separate damage rectangles within the one owned 640×200 window.

The implementation must still prove:

- construction loops reach a heartbeat checkpoint frequently enough;
- the 512×32 strip plus current complete heart path stays below 33.34 ms under Term and GShell scheduling;
- strip boundaries do not tear or change exact frame results;
- heart generation reconciliation prevents stale overwrite;
- dirty bookkeeping does not exceed the 64K process space;
- cancellation and GShell/Term ownership remain exact.

A future exclusive presenter process remains compatible with this region protocol and might isolate rendering from input/game scheduling. It is not required by the present evidence and would add cross-process framebuffer transport, Level II mapping, IPC latency, child lifecycle, and window ownership risks. If later foreground scheduling still violates the gate after bounded strips and bounded construction checkpoints, the same dirty-message protocol is the appropriate input to a presenter investigation.

`GCSet` is a graphics-cursor selection operation, not a verified display-page flip. GP buffers are retained offscreen storage followed by synchronous copying. No documented EOU page switch was established as a replacement for `PutBlk` in this pass.

## Architecture decision

**Decision A — bounded dirty-region presentation can plausibly meet the unchanged 33.34 ms gate.**

The evidence does not support Decision B: a second process was not reliably able to preempt a full transfer, and an exclusive presenter would still block inside that transfer. Decision C is premature because dirty strips have a plausible single-owner budget. Decision D is not supported because measured 512×32 and smaller prepared writes are well below the lower-level bound; the path cost scales mainly with transferred bytes rather than imposing a fixed ~31 ms floor.

The smallest next implementation is therefore a single-owner, generation-aware dirty-strip presenter, with 512×32 as the initial maximum viewport operation and service/reconciliation between operations. It must undergo fresh live heartbeat acceptance before any commit. This investigation stops here; it does not implement that design.

## Implementation result

The first implementation follows Decision A and does not add a presenter process:

- one process retains exclusive ownership of the graphics path;
- one mapped 2,048-byte 512×32 GP buffer is reused for ordered strips;
- full viewport updates are six strips, while status/input updates queue the bottom two;
- expansion is divided into eight-row CPU chunks with heartbeat service points;
- every strip return is followed by an authoritative generation check;
- strip four is reconciled immediately before transfer so it cannot overwrite a newer standalone heart;
- two unmapped 32×7 GP buffers cache the exact small/large phase pixels, avoiding a per-event `GPLoad` after warmup;
- obsolete intermediate generations are discarded in favor of the latest atomic GetStat `$96` snapshot.

The final exact-build Term result is retained in [dirty-strip-final.json](assets/daggorath-heartbeat-sync/dirty-strip-final.json). Prepared 512×32 transfer measurements predicted 8.323 ms median and 8.758 ms maximum; the integrated trace measured 9.179 ms median, 9.349 ms p95, and 9.587 ms maximum across 72 calls. The small additional cost is compatible with foreground dispatch and tracing. Six-strip flips measured 337.758 ms median and 368.615 ms maximum because each strip also includes CPU expansion and intervening service. Dark `game_render_with_progress` measured 578.332 ms median and 587.434 ms maximum. The implementation prioritizes bounded heartbeat progress; it has not received an HD6309 optimization pass.

The integrated heartbeat result was 14.136 ms median, 22.000 ms p95, and 29.905 ms maximum, with 0/73 beyond 33.34 ms. This changes the architecture conclusion from merely plausible to demonstrated for the measured Term interval. It does not establish a universal bound until the outstanding exact-build workloads are measured.

The follow-up workload did establish that the first implementation does not yet provide the required universal bound. Across initial dark, torch, lit turn/movement, and subsequent creature-active lit work, 89/89 generations completed exactly with zero superseded, but three exceeded the gate: 34.381, 36.994, and 45.640 ms. The complete summary is [dirty-strip-workload-stop.json](assets/daggorath-heartbeat-sync/dirty-strip-workload-stop.json).

None of the three failures overlapped a synchronous strip transfer. Their heart-only calls remained 8.141–9.038 ms; the calls began 25.343–37.499 ms after their native deadlines. Function evidence places that preceding interval in lit logical vector rendering before the next effective foreground heartbeat service. This narrows the remaining problem to CPU-side service/checkpoint latency. It does not justify changing strip height, adding a presenter process, delaying native audio, or weakening the gate without a new bounded design decision.

The run stopped before forcing a deadline through every strip position. Two observed deadlines inside strip six completed under the gate. Exact-build GShell, cancellation, and SSC coexistence were not run after the gate failure, in accordance with the stop requirement.

The 64K process constraint remains explicit. Mapping a second graphics buffer returned E$MemFul 207 during development. The final design maps only the 2KB strip; the tiny heart buffers reside in CoWin-owned storage and are loaded by packet. The final module is 33,042 bytes with a 10,737-byte data allocation, CRC `C4778E`. These figures do not include a claim about all runtime stack or system allocations.

### Future presentation classes

Later UI work should distinguish three policies without changing the present heartbeat authority:

1. **Live transient overlays**, including a future mouse-triggered menu bar, must coexist with gameplay/audio and use bounded dirty regions plus current-generation reconciliation.
2. **Modal overlays/windows**, such as configuration or help, may intentionally pause gameplay. A paused modal display does not need to meet the live 33.34 ms heartbeat-presentation constraint.
3. **Alternate map/inventory presentation** may later use the reserved side regions or an owned full-screen/windowed mode. It should compose through the same damage model rather than creating unrelated concurrent writers.

No menu, side panel, map, inventory, combat, attract mode, alternate window contract, or boot-module requirement is implemented here.

## Integrity

Only disposable cloned media were mounted for the implementation trial. Production Daggorath source is modified but uncommitted; MCP source is unchanged. No canonical media were mounted or modified, and no commit was made.

## Vector-work bound after strip implementation

The remaining dirty-strip gate failures were traced to CPU work before the line renderer's first callback: four generated 32-bit coordinate scales followed by two generated 32-bit fixed-point divisions. Individual scale intervals were about 5.8–7.2 ms and line setup about 11–13 ms. This was independent of strip/GFX2 serialization.

The 9.038 ms measured steady-state heart-call worst case leaves 24.302 ms of the 33.34 ms gate for foreground dispatch and work. Natural vector checkpoints now separate pairs of scale operations from line setup. The exact corrected workload measured 14.210 ms median, 22.161 ms p95, and 29.709 ms maximum across 298 generations, with no superseded or over-gate result. See [vector-checkpoint-result.json](assets/daggorath-heartbeat-sync/vector-checkpoint-result.json).

This validates the single-owner dirty-strip architecture for the measured Term workload. It does not remove the need for the outstanding exact-build GShell, cancellation, and SSC live matrix before commit. Broad HD6309 optimization remains a separate redraw-performance milestone.

## GShell acceptance boundary

The exact `1F237B` artifact subsequently failed the unchanged gate under a
GShell AIF launch. The complete dark/lit/movement/resident workload presented
227/227 authoritative generations with no superseded state, but measured
14.054 ms median, 28.107 ms p95, and 108.451 ms maximum; 11 generations
exceeded 33.34 ms. See
[gshell-final-acceptance-stop.json](assets/daggorath-heartbeat-sync/gshell-final-acceptance-stop.json).

The bounded strips and generation reconciliation remain correct: every strip
position was exercised, no stale generation was observed, and active FDC
access remained zero. That first trace did not establish whether the long tail
came from GShell, scheduling, or the graphics subsystem. The controlled A/B
below supersedes that causal uncertainty.

### Controlled cause result

The [machine-readable A/B comparison](assets/daggorath-presentation-architecture/term-gshell-ab.json)
uses the exact `1F237B` module and identical frame-boundary scheduler
attribution. Term measured 13.946/21.099/37.163 ms median/p95/max with one
failure. A quiescent mouse-launched GShell run with no injected text measured
14.076/23.330/36.462 ms with two startup failures and no later failure. It had
no SndDrv samples. GShell residency alone did not reproduce the 68--178 ms
tail.

The bell-producing GShell workload did. Four long delayed-service generations
showed 4--10 consecutive 60 Hz samples in SndDrv. In the exceptional 95.137 ms
outstanding heart write, five samples were SndDrv while the request was
outstanding. This distinguishes wall-clock process descheduling from 95 ms of
continuous GrfDrv work. The actual differentiator was leaked input invoking
EOU's system-bell/`SS.Tone` path, consistent with the user's audible report,
not a different game graphics path.

The single-owner dirty-strip architecture therefore remains the appropriate
base. Host input routing must stop delivering game command text to resident
GShell. First-use heart-buffer warmup and rare SCF scheduling remain outside
the universal 33.34 ms proof and need a separate bounded correction before
commit. No production or MCP source was changed in this investigation.

Future transient menus remain bounded live dirty regions. Modal overlays may
pause gameplay and use normal CoWin calls without inheriting the live
heartbeat constraint. The available BFX2 artifact is a compiled BASIC09
extension exposing ordinary GFX2 operations; no source-backed optimization was
found. No CFX2 material was present in the inspected project/reference trees.

### Startup-buffer decision gate

The final acceptance investigation found that the second exact heart phase is
still allocated lazily after heartbeat activation. Its first call defines GP
buffer `196/3`, GPLoads the 28-byte phase, and PutBlks it; the measured three
writes plus dispatch reached 36.462 ms in the quiescent GShell control. This is
not a dirty-strip or GShell-residency defect, but it prevents a deterministic
startup bound.

The recommended bounded correction is to define and GPLoad both immutable
heart-phase buffers during `screen_open`, before the VIRQ heartbeat is enabled.
It should perform no visible heart PutBlk and no generation bookkeeping.
That change belongs to production initialization, so it was not implemented
under the requested stop condition. Final acceptance remains stopped pending
explicit approval for that correction.

## Separated deterministic-workload result

A disposable test module replaced only the external `game_input` provider and
executed the exact production main/parser/game/creature/render/presentation
and heartbeat objects. Its lit turn, movement, creature-active and resident
sample presented 123/123 generations, with zero supersession, 13.995 ms
median, 20.901 ms p95, 26.894 ms maximum, no result above the unchanged 33.34
ms gate, and zero active FDC accesses. This demonstrates the single-owner
512x32 dirty-strip architecture under the required production rendering
workload without GShell keyboard/BEL contamination.

GShell remains separately evidenced as the AIF launcher and parent of the
game-owned graphics window. Physical host text can activate the GShell/system
bell path and is excluded from presentation timing conclusions. Instruction-
level Lua input substitution was rejected because it did not reproduce guest
input semantics. Remaining lifecycle, cancellation, SSC coexistence, and
regression gates are listed in the
[checkpoint JSON](assets/daggorath-heartbeat-sync/separated-acceptance-checkpoint.json).

## Remaining-gate SSC result

The accepted single-owner, 512x32 dirty-strip architecture remains unchanged.
A final disposable SQUEAK+PHASER coexistence run presented 93/93 authoritative
heartbeat generations with zero supersession and zero active FDC accesses.
Median/p95 completion were 13.970/21.864 ms, but one logical-draw occurrence
completed at 35.508 ms and therefore failed the unchanged 33.34 ms gate.

The outlier was upstream of the small GFX2 transfer: a progress callback ran
0.791 ms after publication, yet the pending heart was not serviced until the
heart operation began 26.620 ms after publication. The small write then
completed at 35.434 ms. This points to a remaining logical-render service gap,
not monolithic GFX2 transfer, SSC ownership, a dedicated-presenter need, or an
FDC interaction. Production was deliberately left unchanged pending review of
that exact checkpoint path. See the
[final-stop record](assets/daggorath-heartbeat-sync/final-acceptance-stop.json).

## Final bounded checkpoint result

The remaining 35.508 ms failure was not a strip-transfer or SSC defect.  The
measured uninterrupted unit was the second endpoint's two generated CMOC
32-bit scales followed by the line renderer's two fixed-point accumulator
initializations, before `wizard_line_progress` could make its first callback.
The smallest correction places a generation-aware service point between the
endpoint scales and line setup.  This preserves the accepted architecture:
one graphics owner, 512x32 strips, immutable 32x7 heart buffers, foreground
GFX2 calls, and VIRQ-owned heartbeat authority.

Across two corrected SQUEAK/PHASER trials, all 180 generations were presented,
none were superseded, and the worst completion was 28.879 ms.  The two p95
values were 21.665 and 20.717 ms.  No corrected result exceeded the unchanged
33.34 ms gate and active FDC access remained zero.  The result confirms that
the dirty-strip design remains sufficient; no presenter process, interrupt
graphics, larger refactor, or 6309 rewrite is justified for this defect.

The future presentation classes remain unchanged: bounded live overlays,
intentionally paused modal displays, and alternate map/inventory views using
the reserved side regions or an owned full-screen/window view.  Planned
attract-mode, four-row text, command-dot, Classic Mode border, mouse/menu,
HD6309 optimization, and 6809 compatibility work are outside this milestone.
