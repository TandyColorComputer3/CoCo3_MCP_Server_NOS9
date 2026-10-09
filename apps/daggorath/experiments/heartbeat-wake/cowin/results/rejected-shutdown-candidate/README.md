# Rejected shutdown candidate: request-2 PipeMan stall

This directory preserves the exact CoWin shutdown-reconciliation candidate and
the read-only Level II observations from its intermittent request-2 stall. It is
diagnostic evidence, not an accepted experiment artifact.

## Candidate identity

- `wcowin-map3`: 7,414 bytes
- data request: 702 bytes
- module CRC: `5DF640` (good)
- SHA-256: `ecdb82e85decbafe2dfd467fac209a8bce5cef846619b8f61c26ea0cf043ac05`

The candidate adds one shutdown-tail call to `reconcile()` after atomic freeze
and before `reconcile_final()`. The normal broker loop is instruction-identical
to the bounded-sleep baseline.

## First blocked dependency

The saved failing trace does not show an early process exit. On request 2,
sender PID 4 is sleeping while the shared reply system path has `PD.CPR=4`.
Broker PID 3 then issues the first seven-byte ACK `I$Write`. IOMan sees the
occupied `PD.CPR`, links PID 3 behind PID 4 in the I/O queue (`P$IOQP=4`), and
sleeps the broker before PipeMan's write entry can execute. PID 4 itself has
`P$IOQP=7`, so the saved evidence is an I/O serialization chain rather than a
simple empty-pipe reader sleep.

The failing observer did not record PipeMan's private read/write packets or
buffer pointers. Its buffer occupancy therefore cannot be reconstructed
authoritatively, and the reason PID 7 precedes PID 4 remains unresolved. What
is proven is the first blocked dependency: broker PID 3 cannot enter PipeMan's
write routine because IOMan's shared-path `PD.CPR` is owned by sleeping PID 4.
This excludes the broker's ACK write being blocked by PipeMan buffer-full logic
at that first boundary, because PipeMan `Write` has not yet been dispatched.

`request2-passing-pipe.csv` is a new read-only observation of the exact same
candidate. In that passing run `PD.CPR` is clear when request 2 begins. The
reply pipe is empty (`PD.NxtI == PD.NxtO`, occupancy zero), PID 4 is its waiting
reader, the broker deposits all 16 ACK bytes, PID 4 becomes runnable, and the
buffer returns to occupancy zero. This ordering difference explains the
intermittency at the observed boundary without claiming the deeper PID 7
dependency has been identified.

Installed EOU evidence used for interpretation:

- PipeMan edition 5, CRC `ECE938`, 595 bytes.
- IOMan edition 13, CRC `40D032`, 2,593 bytes.
- EOU source candidates indexed at
  `/dd/SOURCECODE/ASM/NITROS9/PIPE/pipeman_beta6.asm` and
  `/dd/SOURCECODE/ASM/NITROS9/MODS/ioman_beta5.asm`.

No heartbeat, shutdown, PipeMan, IOMan, request framing, graphics, or process
priority behavior was changed during this diagnosis.

## Files

- `wcowin-map3`: exact rejected executable.
- `wakekick.c`, `.s`, `.lst`, and `wcowin-map3.map`: exact source and linked
  build evidence.
- `candidate-assembly.diff`: shutdown-only assembly delta.
- `process-failing.csv`: blocked run.
- `process-passing.csv`: successful run of the same executable.
- `request2-passing-pipe.csv`: decoded reply-path and PipeMan private state for
  a successful request-2 ACK from the exact candidate.
- `memory-failing.csv`, `exit-events-failing.csv`, and visual/raster CSVs:
  supporting external observations.
- `SHA256SUMS`: hashes of every preserved file in this directory.
