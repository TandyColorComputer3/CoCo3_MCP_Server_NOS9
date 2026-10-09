# One-tick bounded broker sleep validation

Status: **REVISE**.

## Candidate

The only guest-source change from the preserved GetBlk mixed-scene baseline is
the argument supplied by the existing CMOC-safe sleep wrapper:

```asm
        ldx     #2
        os9     $0a             * F$Sleep: installed EOU subtract-one timing
```

The previous value was `#0` (indefinite sleep). Driver ARM/KICK/S$Wake,
generation accounting, `$BC`, PipeMan framing, priorities, CoWin operations,
and scene ownership are unchanged.

Two clean builds were byte-identical. The exact staged guest module matched
the host build byte for byte and ToolShed reported a good module CRC:

- `wcowin-map3-bounded-sleep`: 7,369 bytes, data request 702 bytes,
  CRC `C0745C`, SHA-256
  `11af4a8b20143be186b6ae77db49b78b5e5b91604362c2348f21616c8cf3db41`.
- `kickchild-bounded-sleep`: 2,790 bytes, data request 37 bytes,
  CRC `49ABFC`, SHA-256
  `47ce2b3d329aec88c857309a0f2b4897b3a43f4a1beaf5c368c67d7edc98af0d`.
- The driver, descriptor, and combined pack are byte-identical to the
  preserved CoWin baseline.

## Passing gates

- The request-free PLAYER wait completed before the first KICK. The first
  KICK/claim occurred at native generation 29, after the source-authoritative
  81-tick accumulator completed; generations 1 through 28 were serviced with
  no gameplay request.
- The generation-82 regression passed. Generation 82 was presented, the
  intentional stale request 22 was ACKed, and request 23 was subsequently
  claimed and ACKed. The generation delta never exceeded one.
- The run reached generation 220 with 220 native generations, 220 PB1 edges,
  220 successful heart PutBlks (the driver records a generation as observed
  only after `graphics_heart()` and observation SetStat succeed), no misses,
  duplicates, phase errors, `$BC`, failed signal, or `E$USigP`.
- All 26 gameplay-style requests and the shutdown request were claimed and
  ACKed exactly once. Twenty-five scene requests were accepted; the single
  deliberately stale request was rejected. Status-overlap and mapped-buffer
  reuse paths completed.
- Maximum generation delta was one and maximum edge-to-PutBlk-completion
  latency was one source tick.

The external raster log contains one generation-correlated sample for every
generation from 1 through 220. Its boundary-only sampler recorded 217
immediate hash changes over that interval; it cannot see a raster change that
lands between two shown-generation samples. Consequently this run does not
upgrade the previously accepted heartbeat-only 220-transition evidence into
a new claim of 220 independently observed mixed-traffic raster transitions.

## First proven invariant failure

Shutdown produced native generation 222 after generation 221 had been
presented. The existing `reconcile_final()` path detected the pending
generation but called only observation SetStat `$98`; it did not call
`graphics_heart()`. The driver therefore ended with native/PB1/observed counts
`222/222/222`, while the completed-frame heart raster for generation 222
remained byte-identical to generation 221 (`3684CBFD`, 128 lit pixels).

This silently accounts a generated heartbeat without a matching visible heart
transition. It violates the shutdown requirement and the project-wide
one-to-one heartbeat invariant. Validation stopped at this boundary. No
correction was made.

## Runtime counters and overhead

- VIRQ callbacks: 738; native/PB1 edges: 222.
- ARM calls / bounded-sleep returns: 416 / 416.
- Successful heartbeat wake sends: 209.
- KICK calls / accepted / claimed: 27 / 27 / 27.
- KICK wake sends / no-signal active deliveries: 14 / 13.
- Same-tick KICK observations: 13; collisions: 0.
- Maximum latency: one source tick; maximum generation delta: one.
- Broker lifetime sampled externally: about 12.55 seconds.
- Level II DAT image while active:
  `000D:333E:333E:333E:333E:333E:0018:0007`.

The bounded loop executed 416 sleep/return cycles, about 33.1 cycles per
second over the observed broker lifetime. Of these, 223 had successful
heartbeat or KICK wake sends; the remaining returns were bounded timeout or
already-pending-work paths. This is measured scheduling activity, not a CPU
cycle benchmark.

Persistent GP definitions remain unchanged: status group/buffer `196/1`,
heart buffers `196/2` and `196/3`, and mapped 512-byte scene buffer `196/4`.

## Evidence

- `bounded-sleep-visual.csv`: generation-correlated raster samples.
- `bounded-sleep-boundaries.csv`: selected live Level II process boundaries.
- `bounded-sleep-final-counters.json`: final driver counters.
- `bounded-sleep-artifacts.sha256`: preserved source, artifacts, and evidence
  identities.

Recommendation: **REVISE** the CoWin experiment's final visual reconciliation
before acceptance. Production and accepted experiment checkpoints remain
unchanged.
