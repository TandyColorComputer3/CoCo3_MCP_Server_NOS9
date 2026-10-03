# Daggorath M3: Level II audio IPC architecture study

**Decision: select a one-credit, reply-then-signal audio worker transport.**
This is a transport architecture result, not M3 production implementation or
heartbeat/presentation acceptance. The six existing M3 working-tree files
remain untouched. The ratified [EOU master plan](DAGGORATH_EOU_MASTER_PLAN.md)
governs placement and fidelity. Generic IPC observations below are separate
from Daggorath's semantic event policy.

## Boundary to solve

Current `audio/client.c:audio_present_optional()` waits for a PLAY reply and a
DRAIN reply per event. `audio/ssc.c:backend_drain()` can wait 180 OS ticks, so
the optional audio path can stop foreground heart presentation for seconds.
The backend already acquires S/SC routing once at startup and restores it once
at shutdown. Native PB1 heartbeat remains independent. Transport, credit and
completion signalling are the remaining problems; changing S/SC routing is
not part of this study. The measured M3 example is recorded in [Dynamic
Presentation M3](DAGGORATH_DYNAMIC_PRESENTATION_M3.md).

## Installed EOU mechanism inventory

| Mechanism | Evidence and consequence |
| --- | --- |
| `/pipe`, installed PipeMan | The [runtime match](../source-index/RUNTIME_MATCHES.md) identifies edition 5, 595 bytes, CRC `ECE938`, with a byte-identical stored beta6 binary. Its `GetStt` and `SetStt` entry stubs are `CLRB; RTS` (offsets `$33` and `$36`), matching `/dd/SOURCECODE/ASM/NITROS9/PIPE/pipeman_beta6.asm:93–96`. Thus `SS.Ready` is **not** a byte-count or reliable reply readiness test here. The same candidate source allocates a 256-byte pipe page (`:143–165`) and sleeps in its full-write path (`:426–455`). A successful `I$GetStt` or an uncredited series of writes cannot establish nonblocking transport. The upstream named-pipe variant has different facilities; it is not the installed module. |
| `F$Send` / `F$Icpt` | EOU candidate `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fsend.asm:58–67` tests the single `P$Signal` slot and returns `E$USigP` if a non-wakeup signal is already pending. The pinned upstream `level1/modules/kernel/fsend.asm:145–172` agrees. `ficpt.asm` installs one process handler and data pointer, so an audio signal must share the existing cancellation interception in `apps/daggorath/src/os9.c:55–84`. `F$Send` is a wake/notification, not a queued message stream or a count of free bytes. Signal delivery and races still need a live EOU test. |
| `F$Sleep` | Pinned upstream `level1/modules/kernel/fsleep.asm` documents `X=0` as indefinite sleep and `X=1` as yielding the current slice. The EOU signal/pipe source uses sleep for blocking. Sleeping in the game to await audio credit would violate the presentation requirement; a worker may sleep or retry. Polling sleep in the game would create latency and is not a credit protocol. |
| Ordinary process data | Level II changes each process DAT image; a pointer in the game's data mapping is not a valid shared address in `dodaudio`. See [memory/MMU source index](../source-index/MEMORY_MMU.md) and [Level II modularization](DAGGORATH_LEVEL2_MODULARIZATION.md). A raw `Game *` or ring pointer cannot cross this IPC boundary. |
| Data/named modules | `F$Link` shares a physical module but consumes a logical 8 KiB mapping while linked. The established reentrant/read-only modules keep mutable state in caller-owned data, not in shared module bytes. An ordinary data module containing a mutable ring has no proven cross-process write/synchronization contract here. It would also consume the gameplay overlay slot. Module naming alone does not provide a shared mutable queue. |
| `F$SRqMem`, `F$AllRAM`, `F$MapBlk` | `F$SRqMem` is used by system modules for system memory, not shown as an ordinary user-process shared-memory API in the indexed examples. Pinned upstream `level2/modules/kernel/fallram.asm` allocates physical 8 KiB blocks and `fmapblk.asm` maps specified blocks into **the calling process** through `F$FreeHB`/`F$SetImg`. This suggests a possible explicitly mapped shared block, but ownership, user access, cleanup and simultaneous visibility on installed EOU are unproven. One whole gameplay DAT slot would be occupied while mapped; the normal game already needs that slot alternately for `dodcmd` and `dodsched`. It cannot be made permanent under the current mapping policy. |
| Alarm/event mechanisms | The [installed Clock match](../source-index/RUNTIME_MATCHES.md) and EOU Clock source index show `F$Alarm`; upstream `level2/modules/clock.asm` exposes one alarm packet and sends a signal. That is timed signalling, not a general event/credit queue. No installed EOU event semaphore/message-queue facility with the needed semantics was established by the source index or this review. Do not assume later OS-9 facilities. |
| Separate pump process | `F$Fork`, pipes and signals support the *possibility* of a pump with its own process DAT. Existing `dodaudio` already proves a separate worker and blocking backend can run. A pump can absorb `PLAY`/`DRAIN` waits, but its input pipe still needs a bounded producer credit protocol; it does not by itself make the game's `I$Write` nonblocking. |

Source provenance matters: the EOU assembly paths above are indexed candidate
source except where the installed binary has been compared directly. Pinned
upstream is `/Volumes/SEDONA/Projects/nitros9-reference` at
`f470fa52eb172b59b22c1b722074998cb42de9b1`; it supplies implementation
context, not automatic proof of the installed EOU kernel.

## Candidate architectures

| Architecture | Verified OS support | Can the game block? | Placement and memory | Failure isolation / cancellation | Assessment |
| --- | --- | --- | --- | --- | --- |
| A. Queue in `dodaudio`, game owns bounded admission queue | Existing two pipes, worker and one-outstanding eight-byte protocol exist. A disposable installed-EOU test below proves the reply-then-signal credit handshake without `SS.Ready`. | The measured synthetic foreground stayed active during a 60-tick worker hold; its credited write/receive each crossed at most one 60 Hz boundary. Startup must precede the heartbeat-critical interval. | One small resident admission queue plus metadata; no extra game DAT slot or new process. `dodaudio` code changes and a versioned protocol extension. | Missing/dead worker leaves credit at zero until a bounded foreground timeout disables optional audio. Cancellation must not wait for DRAIN. | **Selected transport architecture**, subject to production visual-latency acceptance and explicit overflow policy. |
| B. Queue in resident game plus a lightweight pump function in that process | Ordinary data queue is straightforward; installed PipeMan lacks readiness. | Calling a pump that performs blocking PLAY/DRAIN or reads replies blocks the *same* presenter. A one-credit asynchronous transport reduces to A's notification problem. | Queue data is small but resident code must fit `dodgame` policy; no new DAT mapping. | Poor isolation for blocking IPC; gameplay must own more lifecycle. | Reject as a standalone solution. Game-local admission queue remains useful with A or C. |
| C. Separate audio pump process between game and unchanged `dodaudio` | `F$Fork`, pipes and process isolation are established. | Pump can wait through PLAY/DRAIN; game input pipe can still fill without credits. Same bounded-write/ack experiment required. | Third process/module and at least one more 256-byte pipe page, extra stacks/data and path descriptors; its *actual* module/data sizes require a build. No permanent game DAT slot. | Best separation from backend failure, but two children to start, cancel and reap; pump cannot receive new pipe events while synchronously draining unless its pipe has headroom or it owns an additional queue/reader. | Fallback if A cannot meet timing or needs to preserve `dodaudio` byte-for-byte. Not automatically preferable. |

No candidate needs or may add code to the exactly 8,192-byte `dodsched`.
The current `dodgame` is 25,376 bytes with 11,088 requested data bytes;
its 30,720-byte policy margin is 5,344 bytes. `doddemo` is 22,045 bytes
with 10,875 data bytes. These are **pre-design** measurements, not projected
post-implementation artifacts. A provisional eight-slot queue containing
two-byte `(sound, gain)` entries is 16 payload bytes; even a 32-byte total
queue/metadata allowance would leave normal-game data at 11,120 bytes,
within two 8 KiB blocks. That says nothing about code-size growth or stack
use, which must be measured from actual builds. Existing `/pipe` uses a
256-byte page per pipe; two current pipes imply at least 512 bytes of pipe
buffers, subject to the installed manager's allocation/lifecycle. A third
process adds its own program/data/stack mapping, not a permanent game DAT
mapping. Stock 6809, 6309, MAME and real EOU must all use the same OS IPC
contract; no CPU-specific spin/atomic instruction is proposed.

## Selected signal and credit protocol

The selected A protocol keeps **at most one eight-byte request and
one eight-byte reply outstanding**. The game first admits a complete semantic
group into a local bounded FIFO, then sends one frame only while it owns a
credit. The worker processes it, writes the reply, then sends a user-defined
signal meaning *reply/credit state changed*. The game's existing intercept
records a flag without C calls or I/O; the foreground later reads exactly
one reply and restores credit. Because notification follows the reply write,
the foreground need not poll `SS.Ready`. The next frame is sent only after
that reply is consumed. PLAY followed by DRAIN can remain two asynchronous
transactions; a new auto-draining command is an option, not a prerequisite.

The worker must not drop a failed `F$Send`: `E$USigP` means another signal
occupies the recipient's pending slot. It retries after yielding while the
game remains independent; invalid/dead PID terminates the optional session.
A successful signal is not an event count. The private test initially used
read-then-clear of a notification byte; that has a lost-arrival race and was
discarded before the final stress run. The selected handler increments a
**monotonic byte counter**. Foreground assembly reads it without clearing,
compares against its last-seen value captured *before submission*, and reads
the reply only on change. With one outstanding request, one completion
notification per reply and no old paths reused after cancellation, the byte
cannot wrap between comparisons. `F$Icpt` retains Shift-BREAK signals 2/3 in
their separate cancellation byte. If the process exits during a pending
request, cancellation closes paths and terminates/reaps the child without
draining the effect. Startup/greeting belongs before the heartbeat-critical
interval or must be delegated; that placement remains a production task.

The one-credit invariant bounds queued pipe data to one frame on each side.
The installed PipeMan's full-write sleep cannot be reached through normal
queue capacity with this invariant and fresh pipes; actual short-write
timing was measured below. Signals are notifications of reply state, never a
carrier for individual sound IDs or an unbounded queue.

## Daggorath admission and backpressure policy still to decide

- FIFO and serialized playback are the conservative source-semantic baseline;
  no implicit coalescing. `WHOOSH → KLINK → BANG` must be admitted as one
  **atomic three-event reservation** and played as three ordered events, so
  partial acceptance cannot misrepresent Combat M1. Do not collapse these
  into one effect or assume overlap from the current backend policy.
- When capacity is insufficient, rejecting the newest **whole group** is the
  simplest explicit bounded policy, but whether dropping a combat group is
  acceptable is a product/fidelity decision. Reserving three slots for combat
  may avoid that case at the cost of other sounds. Priority drops or stale
  cancellation require source/product rules and cannot be silently invented.
- Phase changes can cancel **unstarted** queued events; an already offloaded
  S/SC effect needs explicit STOP/cancellation semantics. Transport failure
  disables optional audio for the process lifetime without changing `Game`,
  RNG, heartbeat generation, parser continuation or exit status.
- An eight-slot depth is an initial candidate based on the known four-event
  boundary below, not a proven worst-case source burst or an accepted product
  overflow policy. Measure queue occupancy under actual demo and gameplay
  workloads before fixing it in production.

## Disposable installed-EOU experiment

The experiment used private CMOC 0.1.90 6809 OS-9 programs `audprobe` and
`audwork`, the existing eight-byte audio frame/client/IPC wrappers compiled
unchanged, the installed EOU PipeMan and kernel, `coco3h`/2M/RGB, private
copies of the EOU boot media and `nos9_ready_v2`, and a byte-extracted,
SHA-verified disposable `/d1` floppy. The final private producer was 6,286
bytes (SHA-256 `8666e765deefa6c31a685d64a3765ebc3557f517414d2cdafb244221fd3d4211`);
the worker was 2,278 bytes (SHA-256
`2f39cc298090316d517ca8980d2bb21c24ee64927e46f29b94c28b6653834a22`).
These are **probe** sizes, not production size estimates. The corrected
notification-counter run used those exact staged modules. Private source,
disk, screenshots and JSON responses remain under `/private/tmp/dod-audio-ipc-exp/`.
Early probe revisions with an absolute pathname accidentally embedded as
the module name, and one that failed to preserve CMOC U around `F$Icpt`,
were rejected as disposable harness defects; no IPC conclusion uses them.

The producer's timed loop samples the verified EOU 60 Hz clock, counts one
synthetic presentation/service per elapsed tick, admits only when credit is
one, and records clock-boundary crossings around `I$Write` and reply read.
It prints *after* the timed loop. One clock-boundary crossing gives a coarse
upper bound below two 60 Hz periods (33.34 ms) for the sampled interval; it
is not a sub-tick measurement and is **not** an actual CoWin heart PutBlk
latency result.

| Private case | Exact EOU observation | Consequence |
| --- | --- | --- |
| Reply → signal, worker delays 0, 1, 3 or 60 ticks | Three matched sequence-numbered replies in each case, one notice per reply, credit restored once. At 60 ticks/request: 183 synthetic generations and 183 services, max service gap 1 tick, max request-write span 1 tick, max reply-read span 0 ticks; guest status 000. | Worker service duration did not stop producer foreground progress. |
| Signal → reply, 3-tick service plus 3-tick post-signal hold | Three replies eventually matched, but reply read blocked 5–6 ticks and max synthetic service gap was 7 ticks; guest status 000. | **Reject** this ordering. Notification before reply is unsafe for the visual-latency gate. |
| Pending-signal collision | Worker sends unrelated user signal 130, immediately tries completion 128; actual `F$Send` returns `E$USigP` 233 and worker retries after sleep. Final 32-request stress run: 32/32 replies, `badDelta=0`, 35 retries encoded as deliberate worker exit status 35, max foreground gap/write/read each 1 tick. | Pending signal is recoverable by worker-side retry; status 35 is a probe retry count, **not** a gameplay failure or raw OS-9 error. |
| Repeated ordinary credit | 32/32 matched replies at 1-tick artificial delay, `badDelta=0`, credit 1 at finish, max foreground gap/write/read 1 tick; guest status 000. | No loss or double credit seen in this bounded repetition. |
| Worker death before read, before reply, or after reply but before signal | Each kept producer credit at 0; no stale reply was consumed. The private 120-tick timeout fired (probe status 246) while 121 synthetic generations/services continued, max gap 1 tick. | The game cannot detect death solely from a missing signal; a finite **foreground-checked** deadline must disable optional audio without waiting. A written reply without its notification does not restore credit. |
| Phase cancellation: idle, three locally pending unsent events, delayed in-flight request | Private `audio_cancel` returned child cancellation status 3; the probe returned 000. The local three-event group was discarded before submission; in-flight credit stayed 0. | Unstarted events can be removed locally. Cancelled pipe/session state must be discarded, not reused. |
| Reply written and notification received, before reply consumption | Probe observed notification 1 while credit was 0, cancelled without reading the reply, reaped child with status 3 and returned 000. | Cancellation need not restore credit or consume a stale reply; a new session must start with new pipes and sequence state. |

The producer alternated synthetic work and `F$Sleep(1)`, so notifications
were exercised while it was doing normal foreground work and while sleeping.
The source `F$Send` pending-signal case was also forced and observed. The
probe did **not** enter CoWin/GFX2 during a send or run actual native heartbeat
presentation. Those remain production acceptance tests; this transport
selection does not waive the unchanged 33.34 ms gate.

### 6809 ownership and atomicity

The signal handler's `INC` of one byte is a single 6809 memory instruction;
foreground loads that byte through explicit assembly because CMOC 0.1.90
does not supply `volatile` semantics relied upon here. It never clears the
counter, so a signal between two foreground instructions cannot erase a
notice. The foreground alone owns credit, sequence, FIFO indices and group
admission; the interrupt handler does not inspect or modify those fields.
Byte head/tail/count changes therefore need no interrupt exclusion under
this owner split. A future cross-context 16-bit head/tail/count or a handler
that reads a multi-field queue would require an explicit atomic protocol or
short critical section; none is part of the selected design. Sequence bytes
may wrap across transactions because only one request is outstanding and
old paths/replies are discarded on session cancellation.

### Queue depth and backpressure recommendation

The current source-backed scheduler fixture has one `GRAWL` before command
10; the ordinary Combat M1 fixture then emits the atomic
`WHOOSH → KLINK → BANG` group (`test/scheduler_attract.c:97–108`). Thus the
largest known immediate combined admission at that boundary is **four**
semantic events, with a largest indivisible group of **three**. Four slots
are the minimum to accept both without delay. Recommend **eight two-byte
slots as an initial bounded product candidate**: 16 payload bytes, four
slots beyond that measured fixture burst, plus byte head/tail/count and
session/credit metadata. Eight is not an observed worst-case source burst;
the existing scheduler event array can hold more, and live queue occupancy
must be measured before treating eight as universally sufficient. Whole
groups must be reserved atomically; an overflow must be reported explicitly.
Whether to reject a newest group, reserve combat capacity or cancel stale
unstarted events remains a source/product policy decision for M3
implementation, not a hidden effect of this transport experiment.

### Production implementation and validation plan

1. Initialize the optional audio worker and greeting before native heartbeat
   activation where the phase lifecycle permits; never perform a blocking
   fork/greeting at a resident heart service point. Preserve silent gameplay
   on failure and one-attempt-per-process behavior.
2. Version the audio protocol. Keep one request/reply in flight. Worker writes
   the full reply, then sends user notification, retrying `E$USigP` with a
   yield. The game handler preserves 2/3 cancellation and increments the
   byte notice counter for the assigned audio signal.
3. Foreground compares the notice counter with its pre-submission snapshot,
   reads and validates the matching eight-byte reply, then restores its one
   credit. No `SS.Ready` or audio-duration sleep in the game. A bounded
   foreground-checked missing-notification deadline disables optional audio.
4. Put the admission FIFO in resident process data, not `dodsched` and not a
   permanently mapped shared module. Start with an eight-slot candidate only
   after an explicit overflow policy review. A complete three-event combat
   group must be accepted or rejected together; serialized worker PLAY/DRAIN
   remains source-sequence-safe.
5. Add host/ABI tests for notification ordering, pending-signal retry,
   monotonic counter race, one-credit invariant, sequence wrap, atomic group
   admission, overflow, startup/death, cancellation at all tested phases and
   cleanup. Then run exact-build EOU heartbeat/CoWin tests with optional audio
   active, all Daggorath/MCP regressions and real-hardware acceptance tiers.

No new game DAT slot or process is required by A. The proposed 16-byte FIFO
payload plus byte metadata remains well inside the existing two-block data
allocation (normal-game baseline 11,088/16,384 bytes); actual generated code
and data requests must be measured at implementation. `dodsched` remains
exactly 8,192 bytes. The current two pipe buffers remain 256 bytes each under
installed PipeMan; no third pump process or pipe is selected. This is a
transport selection, **not** permission to weaken the 33.34 ms visual gate or
to treat the synthetic probe as final presentation acceptance.

## Implemented candidate and live acceptance limit

The subsequent M3 candidate follows the selected one-credit ordering. Its
game-owned eight-slot FIFO admits whole semantic groups; one request is sent
at a time; the worker drains playback, writes a matching reply, then sends
the completion signal (retrying installed EOU `E$USigP` 233). Foreground
progress validates sequence identity before restoring credit. Optional-audio
failure disables the queue for the rest of the process. It does not change
Game, RNG or native heartbeat ownership. Host protocol, queue, gameplay and
service tests passed. `dodgame` is 26,870 bytes with an 11,109-byte data
request; `doddemo` is 23,488 bytes with a 10,896-byte data request;
`dodaudio` is 10,086 bytes with a 2,142-byte data request. `dodcmd` remains
7,004 bytes and `dodsched` remains exactly 8,192 bytes. These retain the
Level II program/data and one-slot overlay bounds.

The exact-build available-service demo completed AUTTAB 1–10, guest status
`000`, with queue high-water three of eight, zero rejected groups and zero
recorded transport failures. The service-absent control also completed status
`000` with optional audio disabled. Both nevertheless exceeded the unchanged
33.34 ms heart presentation gate: maxima 420.265 ms with service and
425.928 ms without. Their earliest large outliers preceded queued sound and
closely reproduced in the no-service control. Therefore the credited IPC
candidate is implemented and functionally exercised, but M3 is **not live
accepted**. These observations do not identify the specific mapped routine
or OS-9 call behind foreground starvation; see [M3 live measurements](DAGGORATH_DYNAMIC_PRESENTATION_M3.md#credited-ipc-candidate-and-exact-build-live-boundary-unaccepted).
