# Daggorath dynamic presentation fidelity M3

**Status:** in progress. This report records the source-derived heartbeat
contract, the correction already validated by deterministic tests, and the
remaining live-observation boundary. It does not claim final acceptance until
an exact-current-build RGB EOU replay and an independent source-cartridge dense
capture have both completed.

The read-only original source is `/Volumes/SEDONA/Projects/daggorath-reference`
at `a94326f00ebb16a106b540c58bc2ccf5f7b66dac`. Its source-built cartridge
identity is recorded in [Attract M1](DAGGORATH_ATTRACT_M1.md). M3 preserves
the existing M2 text, layout, callback, scheduler, RNG, and AUTTAB 1–10 work.

## Original heartbeat contract

`COMMON.ASM:CLK30` runs from the 60 Hz `CLOCK` interrupt. When `HBEATF` is
enabled, it decrements `HEARTC`; on zero it reloads `HEARTR`, toggles PIA PB1
(`P.PIIOB` bit 1), and then, when `HEARTF` is enabled, complements `HEARTS`
and writes the two status glyphs. Thus the audible electrical transition and
the visible phase transition are one interrupt event, in this order:

```text
60 Hz CLOCK → HEARTC reaches zero → reload HEARTR → toggle PB1
          → toggle HEARTS → TXTSTS cell 15 → TXTSTS cell 16
```

`PLOOK.ASM:INIVUX` calls `HUPDAT`, increments `HEARTC` so the first edge is
immediate, then enables both `HEARTF` and `HBEATF`. `HUPDAT.ASM:HUPDAX` stores
`HEARTR = P*64/(P + 2*D) - 19`, using the source byte-zero-as-256 countdown
behavior. This makes health damage affect both the next audible edge and the
next visual phase; it is not a separate animation timer.

`SWCHAR.ASM:SPCTAB` supplies the only two visual phases. The left and right
columns are seven scanlines each:

| `HEARTS` phase | Source glyphs | Left bytes | Right bytes |
| --- | --- | --- | --- |
| 0, small | `I.SHL`, `I.SHR` (`$20/$21`) | `00 00 01 01 00 00 00` | `00 A0 F0 F0 E0 40 00` |
| 1, large | `I.LHL`, `I.LHR` (`$22/$23`) | `00 01 03 03 01 00 00` | `00 B0 F8 F8 F0 E0 00` |

`COMDAT.ASM:TXTSTS` places those columns at text cells 15–16: logical
x=120–135 and y=152–158. `CLOCK` continues while `PLAYER` waits, while text
is emitted, and while normal scheduler tasks run, so the status cells are
updated independently of a new dungeon frame. `PUPDAT.ASM` is separately
responsible for generating and flipping full display frames.

## NitrOS-9 authoritative equivalent

`audio/native/driver.asm:tick` is the only port heartbeat clock. Its VIRQ
increments a video-tick counter, byte-decrements `remaining`, and at zero
toggles PB1, toggles `phase`, increments `edgegen`, and reloads `rate`.
`native_heartbeat_snapshot()` atomically exposes that VIRQ-owned phase and
generation. It does not synthesize elapsed time in foreground code.

`game_heart_patterns()` and `game_render_heart()` use the exact source
SPCTAB geometry above. `presentation.c` preloads both 32×7 physical phase
images before activation and `screen_present_heart()` selects the matching
cached buffer. The physical type-5 placement is x=304, y=156 inside the
640×200 window, which is the documented 2× translation of the source cells.

## Measured pre-correction demo behavior

The pre-M3 candidate (`doddemo` SHA-256
`261f240e94ec8eb2248ec7e9805a160248b85ec1e4136e536825a72f331c2ce6`,
22,029 bytes, data request 10,875 bytes, CRC `5C199F`) was sampled at roughly
125 ms intervals during its resident dungeon/autoplay interval. Cropping the
physical 32×7 heart region at x=304, y=182 in the host screenshot (the capture
has a 26-pixel window origin) found exactly the two source-derived phase
images. It therefore was not a static-glyph or coordinate defect.

At demo rate 46, the source/native edge interval is 46/60 = 766.7 ms. The
observed sequence contained normal 617–639 ms holds, but also repeated
3.755–3.983 s holds. Those spans cover about five native edges. The candidate
also had no progress service installed for `DOD_SCHED_PLAYER_WAIT`: each AUTTAB
word advances the recovered 81 source `CLOCK` jiffies, yet its scheduler
callback was a no-op. This was a source-semantic defect because `CLOCK` changes
`HEARTS` independently of `PLAYER` during that interval.

## Bounded correction under validation

`doddemo` now follows the established normal-game foreground protocol after
heartbeat activation:

1. Render a logical dungeon with a callback that can service only the
   already-presented heart.
2. Snapshot the authoritative native generation and phase.
3. Rewrite the completed status heart from that snapshot.
4. Queue the six strips with a progress callback, flip them, and reconcile a
   newer generation after the flip.
5. Install the existing resident-context-safe progress gateway for scheduler
   jiffies after native heartbeat activation. Every `DOD_SCHED_PLAYER_WAIT`
   jiffy then snapshots and reconciles the native generation in foreground.
6. Reconcile after every one-tick visual dwell. The dwell remains
   presentation-only and does not advance `Game`, RNG, or the logical
   scheduler; this merely permits a VIRQ-owned edge that occurred while the
   process yielded to reach the existing heart-only foreground presenter.

No graphics work moved into VIRQ, no visual timer was added, and a newer
generation is never replayed. `test/demo_heartbeat.c` models all 18 AUTTAB
  word waits (1,458 source jiffies), injects a native edge for every completed
  word, and requires the mapped scheduler progress gateway to reconcile every
  edge. It injects an edge on every active visual dwell tick (480 ticks across
  ten command dwells and the stable hold), and an edge between status
  construction and full-frame flip; each must reach the foreground heart
  presenter and the flipped frame must carry the current phase.

The current reproducible candidate is `doddemo` SHA-256
`4f852198e1da18da3b2e306c36262d8ca53b6d21dd55c27c9234a0b4b5cfa1d7`,
22,045 bytes (`$561D`), data request 10,875 bytes (`$2A7B`), CRC `C88CB0`.
It is an edition-1 reentrant/read-only 6809 object. Two clean builds are byte
identical.

## Stone Giant visibility boundaries

The production viewer walks forward cells and invokes its creature lookup at
each range (`game.c:game_render_with_progress`, matching `VIEWER.ASM`’s
forward and lateral queries). A private deterministic AUTTAB 1–9 rendering
comparison removed only CCB 18 from a copied `Game` and counted changed logical
pixels. This measures actual renderer contribution, not inferred distance.

| Boundary | Player | CCB 18 | Distinct CCB-18 pixels |
| --- | --- | --- | ---: |
| Initial, commands 1–5 | varies; after command 5 `(11,22)` | `(11,25)` through `(9,24)` | 0 |
| Command 6 | `(11,22)` | `(9,23)` | 4 |
| Command 7 | `(11,22)` | `(9,22)` | 0 (occluded on that view) |
| Command 8 | `(10,22)` | `(9,22)` | 2 |
| Command 9 / same-cell attack boundary | `(9,22)` | `(9,22)` | 6 |
| Command 10 post-kill | `(9,22)` | inactive | 0 |

The Giant was therefore correctly absent in early frames and only a small
source-perspective feature when visible. The final evidence set must capture
commands 6, 8, and 9 rather than treating a sparse post-command screenshot as
proof that the renderer omitted the creature.

## Presentation-boundary comparison

| Source cartridge | Portable EOU demo |
| --- | --- |
| `PLAYER` expands one AUTTAB word, invokes `WAIT`, processes characters, then schedules itself every jiffy (`HUMAN.ASM:PLAY30–PLAY99`). | `doddemo` invokes the existing overlay command, performs one `DOD_SCHED_PLAYER_WAIT` for each source word, then a `DOD_SCHED_BOUNDARY`. |
| `PUPDAT` generates the active `DSPMOD` image in `FLOP`, requests `UPDATE`, and waits for `SYNC` (`PUPDAT.ASM:PUPDAX`). | After every accepted command boundary, `doddemo` builds and presents the full current game frame, then uses a fixed visual dwell that does not mutate game, scheduler, or RNG state. |
| `LUKNEW` requests `PUPDAT` only when `NEWLUK` is set, then requeues at its source rate (`COMPLR.ASM:LUKNEW`). | `dodsched` retains the same logical request/queue ordering and reports `dirty`; the demo’s command-boundary frame captures the resulting state. It deliberately does not emulate variable 6809 renderer time. |

The M3 correction changes only foreground heart service during rendering and
strip transfer. It does not change the portable scheduler policy, AUTTAB
state trajectory, RNG, or command/order boundaries.

## Remaining acceptance evidence

- Capture several original-cartridge heartbeat cycles densely enough to show
  both source phases at their 60 Hz edge cadence.
- Run the exact corrected EOU module through AUTTAB 1–10 and compare dense
  heart crops with the same native generation/rate model.
- Curate command-6, command-8, command-9/attack, and post-kill EOU frames.
- The exact current-build RGB replay completed AUTTAB 1–10 with status 000 in
  96.525 seconds. Its heartbeat hashes alternate at the expected roughly
  0.6–0.75-second sampled cadence through normal command rendering, scheduler
  waits, and dwells. Thus the native phase is now visibly reconciled during
  the source `CLOCK` and visual-wait boundaries that the pre-correction demo
  omitted.
- One remaining full-frame hold is genuine: the byte-identical full PNG
  `0ebbd3e…` persisted for 4.247 seconds from 70.894 to 75.141 seconds.
  Decoded pixels include the unchanged completed dungeon and status region;
  this is not a screenshot-cache artifact. It begins immediately after the
  source-semantic command-9 scheduler boundary, where the portable scheduler
  queues the one required `GRAWL` event before command 10.
- A controlled private A/B replay used the same verified `doddemo` image on
  two otherwise equivalent restored RGB sessions. The disk without
  `/d1/dodaudio` (and without a resident `dodaudio` module) completed in
  96.525 seconds and held that frame for 4.247 seconds. Adding the verified
  9,426-byte `dodaudio` service left the same scene static for 11.245 seconds
  and extended completion to 106.552 seconds. The current synchronous
  `audio_present_optional()` path calls `PLAY`/reply then `DRAIN`/reply;
  `backend_drain()` can poll for 180 OS ticks. The missing-service startup path
  also consumes the 4.247-second interval. This proves the first remaining
  dynamic-presentation boundary is optional audio IPC, not renderer cadence,
  the portable scheduler, or observer capture.
- M3 does not silently change general SSC or audio-client policy. Keeping
  native visual reconciliation active while the client synchronously waits for
  optional `dodaudio` requires a nonblocking/pollable audio presentation
  boundary or another explicit architecture decision. Existing audio protocol
  tests encode the current `PLAY`/`DRAIN` behavior, so that correction needs a
  separately approved test-contract change rather than a hidden M3 workaround.

## Audio-concurrency design gate — architecture review required

The six preserved M3 working files were reassessed after the master-plan
checkpoint. Their foreground reconciliation, completed-frame handling, runner
mock and CCB-18 visibility assertions remain within the ratified architecture;
they do not change the native VIRQ, scheduler semantics, SSC recipes or source
RNG. Focused heartbeat, demo, scheduler and audio tests still pass. They do not
resolve the measured `GRAWL` audio wait, so M3 live acceptance remains pending.

### Proposed semantic queue contract

The minimal *desired* contract is one game-owned staging queue and one
`dodaudio` worker-owned playback queue. Initially cap the game queue at eight
semantic event slots, with a three-event batch reserved atomically for
`WHOOSH → KLINK → BANG`; measure the capacity before accepting it as a final
policy. Submission must take bounded foreground work and must not wait for an
effect or `DRAIN`. Accepted source events are FIFO. The worker serializes
foreground effects on the existing SSC channel A and drains each before the
next accepted source event. Accepted events do not overlap, preempt, coalesce
or replay after a newer game phase; the existing backend priority/retrigger
policy remains available to legacy direct clients but must not reorder an
accepted source sequence. An entire semantic batch is accepted or rejected;
partial `WHOOSH → KLINK → BANG` is forbidden.

Backpressure must be explicit. If the fixed queue cannot accept a complete
batch, gameplay and the native heart continue, while the optional audio
session records an observable overflow/failure and its rejected event count;
it must not pretend those events played. No unbounded memory or blocking wait
is permitted to hide overflow. On death, restart, phase chain or cancellation,
stop the active effect, invalidate unstarted queued events with an observable
cancel count, and reap the owned child. An SSC/IPC fault latches audio
unavailable for that session without altering Game, RNG, HEARTC/HEARTR,
heartbeat generation or game exit status. Normal shutdown drains only when a
bounded, separately verified teardown policy permits it; cancellation must
not wait through every optional effect. This is a **design proposal**, not a
claim that the present IPC can implement it safely.

### Why the current IPC cannot yet prove that contract

`audio/client.c:audio_start` calls `F$Fork` and then blocks for an eight-byte
greeting. `audio_present_optional` waits for every `CATALOG_PLAY` reply and
every `DRAIN` reply. `audio/service.c` handles each request serially;
`audio/ssc.c:backend_drain` can sleep for 180 ticks. Removing only the client
`DRAIN` call would let the current backend priority policy replace or coalesce
subsequent effects, so it would not preserve the source event sequence.
Allowing several writes without completion credits would eventually fill the
256-byte pipe or reply pipe and block the sole presenter.

The installed EOU PipeMan identity recorded in
[the source/runtime index](../source-index/RUNTIME_MATCHES.md) is edition 5,
595 bytes, CRC `ECE938`; that audit found the stored development-VHD module
byte-identical to the installed module. Re-extraction of that 595-byte module
for this review gave the same CRC. Its file-manager jump-table entry 10
(`GetStt`, module offset `$0033`) begins `5F 39` (`CLRB; RTS`); entry 11
(`SetStt`, `$0036`) has the same bytes. The provenance-qualified EOU candidate
`/dd/SOURCECODE/ASM/NITROS9/PIPE/pipeman_beta6.asm:93–96` agrees. Thus this
installed PipeMan has no `SS.Ready` byte-count implementation to support a
pollable reply path; a success from `I$GetStt` alone would be misleading.
The pinned upstream `pipeman_named.asm:991–1038` has an `SS.Ready`
implementation, but the source index distinguishes that variant from the
installed EOU PipeMan; it cannot silently be substituted.

`F$Send` is not a drop-in completion-credit channel either: the EOU
`/dd/SOURCECODE/ASM/NITROS9/KERNEL/fsend.asm:58–67` returns `E$USigP` when a
non-wakeup signal is already pending. A signal-based credit protocol therefore
needs an explicit retry/acknowledgement design that coexists with the game's
Shift-BREAK/cancellation interception and never mistakes a lost notification
for an available pipe slot. The synchronous child startup/greeting and final
`F$Wait` also need bounded placement/lifecycle decisions. None is supplied by
the current version-1 client protocol. The existing audio mocks prove the
serial request contract, not nonblocking pipe readiness or credit delivery.

Before implementation, review either (1) a verified EOU-compatible
nonblocking readiness/credit facility with bounded writes, or (2) a dedicated
IPC pump and reliable completion signalling, including startup, cancellation,
child reaping and one-slot Level II mapping effects. Prototype the selected
transport on disposable media, then set a measured queue depth and update the
audio protocol/client/service tests without weakening semantic order. Do not
grow the exactly 8,192-byte `dodsched` module for audio transport.

### SSC session ownership already present

`audio/ssc.c:backend_open` checks ownership, records `$FF01/$FF03/$FF23`,
resets/configures the SSC and selects its routing once. `backend_play` sends
effect commands without toggling those PIA routing bits; `backend_stop` sends
SSC `CF`, and `backend_close` stops, restores the recorded routing and releases
ownership once. The current `dodaudio` process thus already supports the
preferred *startup → enable once → effects/silence → shutdown → restore once*
session lifecycle at the backend level. Native PB1 heartbeat remains a
separate path. This does not erase the documented JoyDrv/MAME speaker-routing
artifact or prove physical SSC output; see [Audio M2](DAGGORATH_AUDIO_M2.md)
and [native heartbeat/SSC evidence](DAGGORATH_AUDIO_M3.md).

**Historical pre-implementation disposition:** audio concurrency architecture
review was required before production IPC changes. The existing
4.247/11.245-second holds, 33.34 ms gate and pending visual captures remain
recorded as failures/open evidence, not reclassified as success. The later
credited-IPC candidate and its new acceptance results follow below.
- Record observer-free timing for meaningful presentation transitions; do not
  use capture request completion as renderer timing.

## Credited IPC candidate and exact-build live boundary (unaccepted)

The subsequent production candidate implements the measured one-credit protocol:
an eight-slot resident semantic FIFO admits complete groups, sends at most one
request, and restores credit only after a matching reply. The worker finishes
SSC playback, writes the reply, then sends the completion notification. It
retries `E$USigP` (233); foreground progress validates the reply and never
waits for an effect to finish. Failure disables optional audio for the process
without changing Game, RNG or native heartbeat state. The existing SSC session
retains routing across effects. `dodsched` is unchanged at exactly 8,192 bytes.
The host audio, gameplay and queue regressions passed; the measured queue
high-water mark was three of eight slots, with no rejected group when the
service was present. This supersedes the *implementation status* of the design
proposal above, but does not establish M3 live acceptance.

The exact tested `doddemo` is 23,488 bytes, data request 10,896 bytes,
CRC `B63FFB`, SHA-256
`7cac0a13bd15b761a00fd9999f765c8e1ab0f5a3a79bf3031f81f7a05515bc7e`.
The exact `dodaudio` is 10,086 bytes, data request 2,142 bytes, CRC `CD93E9`,
SHA-256 `f6a87a81b692ed92a44be1f9431895e3259c173bf96dace006092514c89d5ca5`.
Both controlled `coco3h`/2 MB/RGB runs restored `nos9_ready_v2`, used the
same demo build and completed AUTTAB 1–10 with guest status `000`. The private
observer paired native PB1 edges with the subsequent completed heart PutBlk
generation; its initial displayed generation was outside the edge sample.
The [machine-readable observations](assets/daggorath-dynamic-m3/av-observation.json)
retain every over-gate generation.

| Condition | Edges matched | Median | p95 | Maximum | Over 33.34 ms |
| --- | ---: | ---: | ---: | ---: | ---: |
| `dodaudio` available, not preloaded | 92/92 | 12.651 ms | 20.818 ms | 420.265 ms | 4 |
| `dodaudio` absent | 89/89 | 12.357 ms | 91.579 ms | 425.928 ms | 6 |

No sampled generation was unmatched or superseded. The available-service
run ended with credit restored, queue empty, high-water three, zero rejected
groups and zero recorded transport failures. The absent-service run disabled
optional audio and still exited normally. The first two outliers in each run
were similar (about 420/426 ms and 126/114 ms) before any queued audio;
therefore SSC playback cannot explain those intervals. Sparse foreground-PC
samples during the absent-service outliers include temporary mapped-module
code at `$6000..$7FFF` and OS-9/system code. They do **not** yet identify a
specific overlay, system call or scheduler task. No production correction is
justified by those samples alone. The unchanged 33.34 ms gate **fails**.

The controlled live runs also have no trustworthy captured command-6/8/9
Stone Giant frames. Source-derived isolated renderer tests show the creature
can contribute only a few pixels at those positions, and the user's visual
review reports that the Giant still does not appear in the demo. AUTTAB status
`000` proves command execution, not visual storyboard fidelity. M3 remains
blocked on attribution of the foreground over-gate interval and direct
creature-visibility evidence.

For the wider opening, the current autonomous runner starts at welcome text;
it does not present the cartridge Wizard/copyright sequence, black transition
and full source text/cursor choreography as one continuous scene. The current
`OK` command-result text and rebuilt single-message gameplay display are not
evidence of the cartridge's retained four-row transcript. This is an explicit
storyboard gap, separate from the credited-audio implementation and from the
portable scheduler's intentionally non-emulated bare-metal renderer timing.
