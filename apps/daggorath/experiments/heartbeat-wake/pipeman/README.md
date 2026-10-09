# Accepted PipeMan heartbeat transport POC

This directory preserves the accepted private Level II PipeMan transport
experiment. It extends the accepted ARM/`S$Wake`/KICK mechanism with one
fixed 16-byte request and reply in flight. It is outside the production build.

## Protocol and timing

The client writes all 16 request bytes before KICK. The broker reconciles the
heartbeat first, atomically claims `requestPending`, then performs the fixed
read and sends the matching ACK. `ipc_read()` and `ipc_write()` loop over
partial transfers. The accepted run explicitly used a 5+11-byte request and
7+9-byte ACK. Installed EOU PipeMan edition 5 `SS.Ready` is not used.

Driver operation `$95` returns and clears elapsed 60 Hz ticks. `wakekick.c`
drains it once, accumulates subsequent deltas in a 16-bit counter, checks
overflow, and does not release the first sender until at least 81 ticks have
accumulated. The independent observer measured 84 ticks from heartbeat enable
to the first KICK: three setup/drain ticks followed by the required 81-tick
request-free interval.

## Shutdown protocol

After the shutdown request is ACKed and both children are reaped, the owner:

1. atomically freezes heartbeat generation with `$94`;
2. snapshots the final authoritative generation and phase;
3. observes any generation produced before freeze;
4. takes a confirming snapshot and requires exact generation/phase equality;
5. marks the diagnostic complete, releases ownership, and closes paths.

The driver implements `$94` by masking interrupts around `enabled=0`, so no
new native generation can appear between freeze and the final snapshots. The
accepted run ended at 221 native generations, 221 PB1 transitions, and 221
foreground observations. No generation was reset, hidden, or abandoned.

## Reproduction

Prerequisites are CMOC 0.1.90 and lwtools 4.22. Run:

```sh
apps/daggorath/experiments/heartbeat-wake/pipeman/rebuild.sh
```

The script builds in `${TMPDIR:-/tmp}` and checks every generated artifact
against `results/accepted.sha256`. ToolShed 2.2 `ident` was used to validate
the retained OS-9 modules and CRCs.

The live validation used `coco3h`, 2 MiB, RGB, restored `nos9_ready_v2`, the
combined `DArm63`+`da63` module pack, broker priority 192, and sender priority
128. Loading the driver and descriptor separately can exhaust the available
system DAT mapping slots; the accepted run loaded `darmpack-pipe` once.

## Limits

This checkpoint proves independent heartbeat wake and fixed PipeMan transport.
It does not prove CoWin `PutBlk` timing, visible-heart synchronization, audio
and video lock, the 33.34 ms graphics deadline, or production C6 gameplay.
Those remain separate acceptance gates.
