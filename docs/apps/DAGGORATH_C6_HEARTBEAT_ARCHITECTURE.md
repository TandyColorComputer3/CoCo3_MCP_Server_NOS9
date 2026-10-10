# Daggorath C6 heartbeat presentation architecture

**Status (2026-10-03): bounded broker proof implemented; revise before
acceptance.** C5 commit `5648b132093ab8ddd300a38142f8863aa34e4cab`
remains the accepted rollback baseline. The uncommitted C6 code is
investigative and is not an accepted checkpoint.

## 9. Broker proof result

The proof uses one high-priority process (`F$SPrior` 192) as the only issuer
of visible gameplay `PutBlk` calls. Two fixed 16-byte pipes carry one request
and one acknowledgment at a time. No process-local pointer crosses the
boundary: the parent fills CoWin GP buffer 196/1 and cannot refill it until
the broker acknowledges the transfer. The broker owns `/dhb`, GP buffers
196/2 and 196/3, and graphics path 2 inherited at `F$Fork`. Status requests
carry the native generation and phase and are rejected when either is stale.

The first live launch exposed a CMOC calling-convention defect in the small
assembly client: `_dag_broker_start` pushed the broker pointer before the
rate argument when calling `_dag_broker_rate`. Reversing those two pushes
removed guest error 201. Review then found three protocol-field defects in
the broker service: rate and scene destination were read from reserved byte 5
instead of argument byte 3, and successful enable did not latch the broker's
local `enabled` observation. Correcting those fields made the stress gate
exercise real three-tick edges and correctly addressed scene transfers. These
were broker defects; no native-heartbeat or gameplay semantic was changed.

The final target stress gate ran 220 consecutive transitions at the fastest
legal three-tick rate while continuously submitting 512x16 scene and status
requests. Its per-generation ring records generation, tick, PB1, visual
phase, conservative wake ticks, heart-write ticks, end-to-end ticks, active
scene type, and pass/fail. The target returns success only when:

- all 220 generations are consecutive;
- PB1 equals the visual phase on every record;
- no generation is collapsed or duplicated;
- broker wake, prepared-heart `PutBlk`, and 512x16 scene `PutBlk` each use no
  more than one 60 Hz tick;
- end-to-end latency uses no more than two ticks (33.333 ms), within the
  retained 33.34 ms gate;
- there are no deadline failures.

The byte-verified target completed with status 000. The tick measurement is
deliberately conservative and has only 16.667 ms resolution; the earlier
high-resolution prepared-operation maxima remain 4.378 ms for the heart and
6.309 ms for a 512x16 scene. The proof does not replace those measurements
with invented sub-tick precision.

The proof is **not accepted for gameplay activation**. In a fresh restored
EOU run, the exact `dodcgame` artifact completed with status 000 but the
completed-frame observer saw only the transition to black before return to
Term. No initial dungeon, `ATTACK RIGHT`, `!!!`, accelerated heart, or Giant
frame was available for human review. Thus transport stress passed while the
required end-to-end gameplay presentation and human audio/visual acceptance
did not. The next bounded revision must explain that presentation lifecycle
before this broker can replace the foreground path.

Final proof artifacts are:

| Module | Bytes | Data request | CRC | SHA-256 |
| --- | ---: | ---: | --- | --- |
| `dodcgame` | 32,757 | 15,638 | `1EC664` | `a1fb0d6368228b642759a83ac7e39f8828b8fe4bee7b1002a4a75930fbab326f` |
| `dodhbp` | 4,696 | 2,998 | `DA5813` | `fb99a464fa2ec75c629336ee9830d2ebe935909dd237dc4d898e438f57e602c0` |

The game still uses four program blocks, two data blocks, and one CoWin block
(7/8, or 8/8 with exactly one existing overlay). The broker is a separate
one-program-block/one-data-block process. Its direct physical allocation is
16 KiB plus PipeMan path/buffer resources and the small CoWin GP buffers.
The parent module has only 11 bytes before the 32 KiB boundary, which is a
serious integration constraint even though this bounded proof still fits.

The invariant is stricter than matching counts: every native heartbeat edge
must toggle PB1 immediately and cause its corresponding small/large heart to
become visible before the next legal source edge. A visible generation may
not jump from N to N+2. The existing 33.34 ms edge-to-completed-presentation
gate is retained. It is a latency gate, not the heartbeat period.

`MCP/Documents/` and `DOCS_INDEX.md` are absent from this checkout, so no
local manual citation is available for this investigation. OS-9 conclusions
below use the indexed EOU source/runtime record, direct EOU measurements, and
the maintained NitrOS-9 checkout at commit
`f470fa52eb172b59b22c1b722074998cb42de9b1`. The installed EOU CoWin and
GrfDrv are not claimed byte-identical to that checkout.

## 1. Exact starvation mechanism

The VIRQ/PB1 side is not the delay. The measured native path from a deadline
through PB1, phase publication, generation publication, and VIRQ return is
about 58 microseconds; PB1-to-generation publication is about 22 microseconds.
Delay begins after publication, while the single game process is doing other
work or is descheduled.

There are three measured classes:

| Blocking class | Measured evidence | Consequence |
| --- | --- | --- |
| Foreground CPU work | The exact-source trace attributed long gaps to scene rendering, creature batches, status/input work, and vector/raster work. Examples include 68.623 ms in `game_render_with_progress` and 102.355 ms before service in a creature batch. Later C6 trials still showed visible delays in the roughly 75--326 ms range despite more checkpoints; the closest eight-row trial retained a 74.3 ms maximum and three transitions beyond 50 ms. | PB1 changes, but no user-state path is available to submit the matching heart. |
| Synchronous graphics work | Prepared EOU `PutBlk` calls measured 31.087 ms median / 32.420 ms maximum for 512x192, 8.323 / 8.758 ms for 512x32, 6.142 / 6.309 ms for 512x16, and 4.211 / 4.378 ms for a prepared 32x7 heart. | The calling process cannot run heartbeat code until its synchronous call returns. Smaller regions bound this part; they do not solve foreground starvation. |
| OS-9 scheduling and unrelated system work | A controlled GShell trial attributed 71.802--178.268 ms pending intervals to repeated SndDrv/VTIO execution. One apparent 95.137 ms heart write contained five consecutive SndDrv frame samples; it was process descheduling, not 95 ms of continuous GrfDrv execution. | A synchronous call's wall time includes time when its process is not running. Priority and ownership must be validated on the target, not inferred from call size. |

The longest uninterrupted *known graphics primitive* is bounded by the region
size, but the longest interval in which the current foreground design cannot
make heart progress is not: it includes gameplay, renderer, overlay,
scheduler, and system scheduling intervals. Adding more callbacks merely
changes those intervals and has produced both regressions and apparently good
runs that failed human review. Cooperative foreground reconciliation is
therefore not an acceptable architecture.

Durable details and raw measurements are in
[the presentation architecture report](DAGGORATH_PRESENTATION_ARCHITECTURE.md),
[the heartbeat decomposition](assets/daggorath-heartbeat-sync/latency-decomposition.json),
and [the presentation measurements](assets/daggorath-presentation-architecture/presentation-measurements.json).

## 2. OS-9 scheduling and graphics-I/O implications

CoWin `PutBlk` delegates to GrfDrv. The maintained source records
`G.GfBusy`, calls GrfDrv through `L00F7`, and explicitly describes GrfDrv as
non-reentrant around window selection. IOMan queues processes around file
manager/driver calls and wakes the next queued process with `F$Send/S$Wake`.
This establishes serialization machinery; it does not by itself establish a
fixed latency for the installed EOU modules.

A disposable EOU process experiment supplied the needed runtime evidence:

| Path arrangement | Parent full writes | Native edges inside them | Child heart writes begun and completed before parent return |
| --- | ---: | ---: | ---: |
| Inherited/shared descriptor | 40 | 61 | 4 |
| Separately opened path to the same window | 40 | 53 | 0 |

The inherited-descriptor child can execute a small write while the parent is
inside its synchronous write, so a graphics call is not an unconditional
machine-wide scheduling barrier. The behavior is not a guarantee: most
requests did not begin in time, and the two interleaved parent calls stretched
to about 66.5 ms. With a separately opened path, the child did not even reach
its user-space pre-call marker until the parent call returned. Process
priority or a second path therefore cannot, by itself, satisfy the invariant.

The selected design does not rely on concurrent GrfDrv entry. It uses one
graphics-system-call owner and bounds every scene transaction. A dedicated
broker can be scheduled independently of gameplay CPU work; if it must wait
for graphics serialization, it waits behind at most one bounded scene call.

## 3. Cartridge heartbeat timing envelope

Original `COMMON.ASM:CLK30..32` owns one countdown. On expiry it reloads
`HEARTC` from `HEARTR`, toggles PB1, complements `HEARTS`, and deposits the
two heart glyphs during the same IRQ invocation. `HUPDAT.ASM:HUPD00..20`
implements, for a living player:

```
HEARTR = floor(64 * power / (power + 2 * damage)) + 1 - 19
```

`HUPDAT` changes the reload value without restarting the current countdown.
The source faint threshold is `HEARTR <= 3`; three is therefore the fastest
legitimate transition interval reached at the living/faint boundary.

| State/example | HEARTR | Transition interval at nominal 60 Hz | Full small-large-small cycle | Full-cycle rate |
| --- | ---: | ---: | ---: | ---: |
| Initial captured attract state | 46 | 766.7 ms | 1.533 s | 39.1 cycles/min |
| Approach capture | 45 | 750.0 ms | 1.500 s | 40 cycles/min |
| Post-command-10 cartridge capture | 41 | 683.3 ms | 1.367 s | 43.9 cycles/min |
| Half damage example | 14 | 233.3 ms | 466.7 ms | 128.6 cycles/min |
| Severe damage example | 6 | 100.0 ms | 200.0 ms | 300 cycles/min |
| Living/faint boundary | **3** | **50.0 ms** | **100.0 ms** | **600 cycles/min** |

The hard target remains completion within 33.34 ms of every native edge.
That leaves one nominal video tick of margin before the 50 ms fastest legal
next transition. Acceptance records mean, p95, maximum, jitter, missed
deadlines, maximum generation backlog, and collapsed generations. Required
values are zero missed generations, zero collapsed generations, and backlog
never above one while its transition is being presented.

## 4. Architecture comparison

| Option | Decision | Evidence and boundary |
| --- | --- | --- |
| A. Dedicated heartbeat process | **Possibly viable alone; viable as part of the selected broker** | The inherited-path experiment proves cross-process progress is possible, but not deterministic. A heartbeat-only process still races the game's scene writes and can sit behind a long transaction. It becomes viable when it is the sole visible-write owner and scene calls are bounded. |
| B. Separate heartbeat window/overlay | **Rejected as the timing mechanism** | It still reaches the same CoWin/GrfDrv service and adds selection/compositing behavior. The separately opened-path trial showed no request start during a parent full write. A separate window may be useful for layout later, but supplies no timing guarantee. |
| C. Persistent heart buffers/isolated surface | **Viable and required component** | Two preloaded 32x7 GP buffers already make the final mutation only 4.211 ms median / 4.378 ms maximum in the prepared probe. The status scene must carry a phase stamp so stale scene data cannot overwrite the heart. |
| D. Direct video-memory update | **Rejected** | No documented EOU application interface was found for safely mapping and mutating the active GrfDrv screen. GrfDrv maps/manages screen and GP blocks in system context. Direct GIME/MMU or physical-screen writes would race GrfDrv, conflict with task DAT switching, and violate normal Level II ownership. No IRQ-safe proof exists. |
| E. Incremental/off-screen scene construction | **Viable and required component** | The current logical frame and mapped GP strip already separate construction from transfer. A 512x16 prepared transfer is bounded to 6.309 ms in the measured environment. The broker protocol can serialize these requests while servicing the heart first. |
| F. Cartridge drawing granularity | **Possibly viable as a fidelity/performance refinement** | The cartridge writes display memory and heart glyph bytes directly; the port introduced GP-buffer batching. Preserving original semantic draw order is desirable, but translating each primitive into ordinary GFX2 calls would add driver overhead and still share GrfDrv. It does not independently establish a deadline. |

## 5. Recommended architecture

Use a **dedicated high-priority presentation broker process**, combining A, C,
and E:

1. The broker inherits the already owned graphics descriptor and becomes the
   only process that issues visible `PutBlk` operations during gameplay.
2. The broker owns `/dhb`; the same native VIRQ still changes PB1, phase, and
   generation. No graphics call is made in IRQ context.
3. The broker defines and preloads both immutable 32x7 heart GP buffers.
4. The game retains its existing one mapped scene GP buffer. It fills that
   buffer in process memory, then sends a small ready/rectangle/phase-stamp
   message. No framebuffer pointer crosses the process boundary.
5. The broker polls/wakes at one video tick or better, queries the atomic
   heartbeat snapshot first, and presents every pending generation before
   accepting scene work.
6. Scene transfers are at most 512x16 initially. After each transfer the
   broker checks the native generation before another scene request.
7. A status request includes the heart generation/phase embedded in its
   pixels. If it is stale before submission, the broker rejects it for
   repatching rather than displaying it. If an edge occurs during the bounded
   transfer, the broker presents the new heart immediately on return. Thus a
   scene cannot restore a pre-edge heart after a newer heart has completed.
8. Rate changes and enable/disable commands travel over a small control pipe;
   the broker applies them to the one native driver. Gameplay never creates a
   second timer or synthetic edge.

The measured conservative budget is:

```
one tick wake/poll       16.67 ms
512x16 scene worst        6.31 ms   (mutually exclusive branch in most cases)
prepared heart worst      4.38 ms
                         --------
conservative sum         27.36 ms
33.34 ms gate margin      5.98 ms
```

This arithmetic is a design bound to test, not acceptance evidence. The next
prototype must measure actual wake latency, IOMan queue behavior, and the
combined worst case. If it cannot stay below 33.34 ms with margin, reduce the
maximum scene transaction before considering any weaker heartbeat rule.

This design is stronger than foreground reconciliation because gameplay,
command overlays, `dodsched`, compatibility-state copies, vector construction,
parser work, and sound-effect work cannot prevent the broker process from
running. Only OS scheduling and one deliberately bounded graphics operation
remain in the critical path.

## 6. Level II memory impact

The accepted C5 game remains:

```
game process: 4 program + 2 data + 1 mapped CoWin GP block = 7/8
with dodcmd or dodsched:                                     = 8/8
```

The broker is a separate process, so it consumes no logical DAT entry in the
game process and does not change mutual exclusion of `dodcmd` and `dodsched`.
The target budget is one broker program block and one data/stack block. It
needs no mapped scene block: it addresses the game-owned GP buffer by the
inherited window descriptor and GP buffer ID. The two heart buffers remain
small CoWin-owned, unmapped buffers. Parent-side additions are fixed-size pipe
state and a phase-stamped request, expected to remain within the existing two
data blocks.

Projected incremental physical allocation is one 8K program allocation, one
8K broker data/stack allocation, PipeMan buffers/descriptors, and less than
one hundred bytes of heart GP payload rounded by CoWin. This is physical RAM,
not additional game-process DAT occupancy. Exact module/data sizes remain a
prototype acceptance item; no size is claimed before a build.

## 7. Risks and required controls

- **Graphics ownership:** only the broker may issue visible gameplay writes.
  Concurrent writers would reintroduce nondeterministic ordering.
- **Shared GP-buffer race:** the parent cannot refill the mapped buffer until
  the broker acknowledges completion. One-buffer handshaking is mandatory.
- **Stale heart overwrite:** status scene requests are generation stamped and
  rejected if stale. The broker checks again after every scene call.
- **Process scheduling:** priority is not assumed sufficient. One-tick wake
  latency and outliers must be measured during the real attack workload.
- **Path serialization:** the design tolerates serialization only because one
  owner submits bounded calls. It does not depend on concurrent GrfDrv entry.
- **IRQ safety:** VIRQ changes PB1/state and wakes or is polled by the broker;
  it never calls CoWin, GrfDrv, PipeMan, or normal I/O.
- **Lifecycle:** the broker owns `/dhb`, heart buffers, and final PB1 restore.
  startup, cancellation, abnormal child death, window teardown, and repeated
  launch need explicit cleanup tests.
- **Sound effects:** SSC remains a separate credited service. It cannot own,
  delay, or synthesize PB1 edges. Scheduler/system-load outliers remain part
  of the live matrix.
- **Choreography:** `ATTACK RIGHT -> strike -> !!! -> rate change -> later
  frames` remains source ordered. Presentation brokerage changes ownership,
  not game time or command order.
- **Future C6/C7:** overlays remain in the game process and mutually
  exclusive. New UI must submit damage regions through the broker rather
  than adding another visible writer.

## 8. Minimal proof of concept

The existing disposable two-process experiment is sufficient to reject
"high-priority second writer" as a complete solution and to establish that an
inherited descriptor can be used across processes. No new production code was
needed for this decision.

The next bounded prototype, requiring separate approval, should contain only:

- one tiny broker process using the inherited window path;
- the existing preloaded small/large heart buffers;
- one existing mapped scene GP buffer and a one-buffer ready/ack handshake;
- 512x16 scene requests plus a status phase stamp;
- the real native driver at 46, 41, 6, and 3 ticks;
- low-overhead edge, wake, PutBlk-entry/return, and completion timestamps.

It must force edges before, during, and immediately after scene calls, then
run the real Giant/attack workload. A separate window, direct screen write,
new renderer, or gameplay change does not belong in that proof.

## 9. Human-review and acceptance plan

The final review run is one continuous cartridge-derived sequence:

```
initial dungeon
-> EXAMINE
-> PULL RIGHT TORCH
-> USE RIGHT
-> LOOK
-> Giant approach redraws
-> creature attack / GRAWL
-> ATTACK RIGHT
-> WHOOSH / KLINK / BANG
-> "  !!!"
-> accelerated heartbeat
-> post-kill dungeon frames
```

Capture PB1 edge time, native generation/phase, broker wake, heart-call entry,
heart-call return, and completed-frame observation. Run through the fastest
three-tick synthetic rate separately, then the unmodified source rates in the
continuous sequence. Report count, mean, p95, maximum, jitter, deadline misses,
maximum backlog, and collapse count for pre-attack, `!!!`, accelerated, Giant
render, scheduler, overlay, and continued-dungeon intervals.

Acceptance requires zero missed or collapsed generations, every completed
heart alternating in native phase order, maximum call-return completion below
33.34 ms, no white rectangle or reverse heart overwrite, and human confirmation
that sound and completed visible frames remain locked after `!!!`. Matching
counts or eventual convergence is insufficient.
