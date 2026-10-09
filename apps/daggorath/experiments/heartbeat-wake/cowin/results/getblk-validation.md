# CoWin GetBlk correction validation

## Correction

The private scene setup now emits the installed EOU GetBlk packet
`1B 2C C4 04 00 00 00 00 02 00 00 08` after DefGPB and before SS.MpGPB.
This initializes group 196 buffer 4 as a persistent 512x8 GP buffer. Repeated
scene requests populate its 512-byte mapped payload and issue PutBlk only.

## Passing boundary

- GetBlk: success.
- SS.MpGPB: success, pointer `$C4A0`, length 512.
- Formerly failing scene PutBlk: success, raw result 0.
- Post-scene heartbeat reconciliation: reached and succeeded.
- ACK: both fragments written, result 0.
- Raster: the 512x8 destination contains the expected alternating expansion
  of `$44` and `$11`; no unintended heart/status overwrite was observed.
- Status-overlap requests 8 and 15 completed. The heart raster continued to
  alternate through generation 81. Its two post-overlap hashes were
  `7E7A6B39` and `3684CBFD`.

## First heartbeat invariant violation

Request 22 is the intentional stale-generation control. It was claimed and
ACKed with the expected stale result. At its completed ACK boundary, native
generation was 82 and the last completed/observed visual generation was 81.
The next client request was committed, but the driver state shows
`requestPending=1`, 23 KICKs accepted, and only 22 claims. No further visual
heart update occurred. Native/PB1 generation continued to 1898 while shown
generation remained 81.

The accepted prefix has 81 individually observed transitions, no duplicate,
missed, or phase-error counters, no failed KICK signal, no `E$USigP`, maximum
generation delta 1, and maximum edge-to-observation latency 1 source tick.
The complete mixed test fails because generation 82 and all later generations
were not presented. `$BC` was not returned before foreground progress ceased.

Testing stopped at this first invariant violation. The shutdown request,
atomic freeze, final reconciliation, buffer cleanup, and child reaping were
not reached; the private MAME session was stopped externally.

## Result

**REVISE.** The one-time GetBlk correction is correct and resolves `$BD /
E$ICoord`. The next unresolved semantic boundary is the transition after the
stale request ACK, where request 23 remains pending and heartbeat service no
longer runs. No production or accepted-checkpoint source was changed.
