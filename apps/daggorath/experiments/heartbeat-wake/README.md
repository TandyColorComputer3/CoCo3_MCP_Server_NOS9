# Daggorath heartbeat wake experiments

These modules preserve three private Level II experiments that validated an
independent heartbeat wake mechanism. They are deliberately outside the
production build and are inactive unless built and installed by hand.

Current status:

- `arm-wake`: **ACCEPTED**
- `arm-kick`: **ACCEPTED**
- `pipeman`: **ACCEPTED**
- CoWin visual presentation: **NOT YET ACCEPTED**
- production C6: **NOT YET ACCEPTED**

## Accepted results

### `arm-wake`

At heartbeat rate 3, the test observed 220 native generations, 220 PB1
transitions, 220 foreground generations, and 220 successful wake
notifications. Maximum generation delta was one. There were no missed or
duplicate generations, phase errors, failed `F$Send` calls, or `E$USigP`
errors.

The checked-in reference binaries are retained because they are the exact
artifacts accepted during live EOU testing:

| Module | Bytes | Data | CRC | SHA-256 |
| --- | ---: | ---: | --- | --- |
| `DArm63` (`darmbeat-race`) | 1,139 | 103 | `D99F46` | `7bdd98930980211aeb0c49695029f2fa755d3063771b7a868028d16ab3213ebc` |
| `wakerace` | 2,404 | 549 | `700C98` | `301b32f54d7f206ac0cb3cc0131afa62abb5e31426ebb0324e90c1ed1deb97bc` |

### `arm-kick`

The combined test observed 220 native generations, 220 PB1 transitions, and
220 foreground generations. It committed, accepted, and claimed 26 requests
exactly once. It recorded 218 successful heartbeat wake notifications, 24
successful KICK wake notifications, and two KICKs that needed no signal.
Maximum generation delta was one. It recorded no missed or duplicate
generations, phase errors, request collisions, request errors, failed signals,
or `E$USigP` errors.

| Module | Bytes | CRC | SHA-256 |
| --- | ---: | --- | --- |
| `wakekick` | 3,748 | `48560D` | `3c1f05777e3c7d2d1648c8475cacd6b7e9697f3506806d390d2c440d9dc25036` |
| `kickchild` | 1,990 | `0B5EC7` | `b730263be76dfc1ca78e7e6ee963973d07ee92fb29914a7282713f9095042a73` |

The accepted disposable disk itself is intentionally excluded. Its recorded
SHA-256 was `2877820fbd3572a7ab832f2fdaa5c2414ed49e436ade371e201789954e159171`.

## Architecture established by the experiments

The native VIRQ changes PB1, changes the logical phase, increments the
authoritative generation, and conditionally sends `S$Wake` when the receiver
is armed. Foreground code reconciles that generation before other work,
atomically claims `requestPending`, arms against the current generation, and
enters indefinite sleep only after an atomic recheck. `S$Wake` is a
notification; it is never the heartbeat authority.

Installed EOU Level II behavior observed by these experiments:

- `F$Sleep` receives its argument in X; `X=$0000` sleeps indefinitely until a
  signal.
- `F$Send(S$Wake)` works from the inspected VIRQ context.
- OS-9 has one pending signal byte, so unconditional wake signaling is unsafe.
- The armed handshake avoids unnecessary competing signals.
- The receiver ran at priority 192 and senders at priority 128.
- `F$Fork` inherits the parent's priority and has no child-priority argument.
  The harness used `F$SPrior`, then `F$Fork`, then restored priority before
  heartbeat enable. Creation-time priority matters because scheduler age is
  initialized from priority.
- C-callable assembly preserves resident Y. Critical syscall wrappers must be
  checked in emitted assembly; one earlier inline `F$Sleep` attempt emitted no
  bytes.

## Installed EOU PipeMan transport

`pipeman` preserves the accepted follow-on transport experiment. It observed
221 native generations, 221 PB1 transitions, and 221 foreground generations,
with maximum generation delta one. All 27 fixed-size PipeMan requests were
ACKed exactly once, including fragmented 5+11-byte request and 7+9-byte ACK
transfers. The exact 81-tick request-free gate and atomic freeze/final
reconciliation shutdown passed. See `pipeman/README.md` and its retained
results.

## Historical incomplete transport work

`pipeman-wip` preserves the earlier unaccepted lineage. Installed EOU
PipeMan edition 5 has stub GetStt/SetStt entry points: `SS.Ready` succeeds
without reporting queued-byte readiness. It cannot be used to establish that
a complete 16-byte request is available.

The WIP writes a complete fixed-size request before KICK, claims
`requestPending` before reading, loops over partial reads/writes, and services
heartbeat first. It is **not accepted**. Its mandatory 81-tick request-free
gate never ran correctly because private driver operation `$95` returns and
clears elapsed 60 Hz ticks; the harness incorrectly treated successive values
as absolute timestamps and released its sender after eight ticks. No PipeMan
220-generation acceptance result exists in that historical directory. Its
defect remains unchanged; the corrected accepted lineage is `pipeman`.

## Reproduction

Prerequisites:

- CMOC 0.1.90
- lwtools 4.22
- ToolShed 2.2 for module identity inspection
- NitrOS-9 reference checkout at commit
  `f470fa52eb172b59b22c1b722074998cb42de9b1`

Run `build/rebuild-accepted.sh`. It builds into a temporary directory, checks
the exact accepted SHA-256 values, and leaves the source tree unchanged.

The live tests depended on the disposable `coco3h`, 2 MiB, RGB EOU environment
and the `nos9_ready_v2` restored state. The experimental driver and `/da63`
descriptor must remain separate from production DHeartbeat and `/dhb`.

## Validation limits

These results prove wake and foreground observation only. They do not prove
CoWin `PutBlk` timing, audible/visual synchronization, the 33.34 ms graphics
deadline, or production Daggorath compatibility. Production heartbeat,
gameplay, broker code, media, and accepted C5 fixtures are unaffected by this
directory.
