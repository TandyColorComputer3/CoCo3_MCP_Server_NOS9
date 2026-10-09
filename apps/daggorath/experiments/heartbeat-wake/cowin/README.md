# Reconstructed CoWin mapped-scene heartbeat experiment

Status: **ACCEPTED EXPERIMENTAL COWIN HEARTBEAT AND MIXED-SCENE POC**.

Production C6 gameplay and cartridge choreography are **NOT YET ACCEPTED**.

This private Level II experiment extends the accepted PipeMan heartbeat POC
with broker-owned CoWin presentation. It preserves persistent 32×7 heart
buffers in group 196 buffers 2 and 3, plus a persistent mapped 512×8 scene
buffer in group 196 buffer 4. `SS.MpGPB` supplies the process-local payload
mapping; scene requests write the mapped payload and issue only PutBlk.

The source was reconstructed by replaying the recorded private CoWin changes
on top of the committed accepted PipeMan checkpoint at Git commit
`c429482ec33d002944213a89ceb7242013bee478`. The resulting `wcowin-map3` and
`kickchild` hashes match the hashes recorded by the historical build, although
the missing historical files cannot be compared directly. This is a hash-record
match, not a claim that a direct comparison with the deleted file occurred.

## Reproduction

Prerequisites: CMOC 0.1.90 and lwtools 4.22.

```sh
apps/daggorath/experiments/heartbeat-wake/cowin/rebuild.sh
apps/daggorath/experiments/heartbeat-wake/cowin/rebuild-heartbeat-only.sh
apps/daggorath/experiments/heartbeat-wake/cowin/rebuild-diagnostic.sh
```

The scripts build outside the repository under `${TMPDIR:-/tmp}`. The first
reconstructs the baseline, the second delays scene traffic until generation
220 for the heartbeat-only gate, and the third adds memory-store-only return
diagnostics. ToolShed 2.2 `os9 ident` validates the retained OS-9 module
headers and CRCs. See
`results/reconstruction.md` and `results/artifacts.sha256` for identities.

## Ownership and timing

The priority-192 broker owns the graphics path and is the only PutBlk writer.
One persistent priority-128 client owns the request/reply endpoints; this avoids
multiple readers competing for the shared reply path. Native VIRQ generation,
PB1 transitions, ARM/`S$Wake`/KICK, `$BC` generation-collapse protection, and
the 33.34 ms visual deadline remain unchanged. Installed-EOU `F$Sleep(2)` gives
one-tick bounded recovery from a consumed wake notification.

The protocol uses fixed 16-byte PipeMan request and ACK frames with one
outstanding request. Complete request writes precede KICK; the broker claims
`requestPending`, reads the complete frame, validates it, performs the graphics
operation, and writes exactly one matching ACK. Stale generation requests are
rejected. A scene operation that overlaps the status area reconciles and
repatches the authoritative heart phase before acknowledgment.

The broker defines persistent 32x7 heart buffers and a persistent 512x8 scene
buffer. Scene initialization follows `DefGPB -> GetBlk -> SS.MpGPB`; clients
write the process-local mapped payload while the broker alone issues PutBlk.
Shutdown stops new requests, atomically freezes native generation, performs a
real final heart PutBlk for every generation produced before the freeze, then
retires ARM, ownership, paths, and child processes.

The live broker DAT image was
`0010:333E:333E:333E:333E:333E:0018:0007`: one program block, one mapped CoWin
buffer block at logical block 6, and one process-data block at logical block 7,
for 3/8 occupied DAT blocks. The driver requests 127 data bytes; `wcowin-map3`
requests 702 bytes; `kickchild` requests 37 bytes.

## Historical reconstruction result

The reconstructed heartbeat-only gate passed 220 native/PB1/completed-heart
transitions with 220 independently observed raster transitions. The first
mapped scene request did not pass: its PutBlk command returned `$BD` /
`E$ICoord`, after which the ordinary error path cleaned up the process. The
working production mapped-buffer path initializes buffer geometry once with
GetBlk before mapping; this reconstruction only DefGPBs and maps buffer 196/4.
That missing initialization is the smallest proposed correction and has not
been implemented here.

This experiment is isolated from production build targets and from the
accepted `arm-wake`, `arm-kick`, and `pipeman` sources. It is not production
C6. No result here weakens the accepted C5 heartbeat fixtures.

## Historical GetBlk validation result

The experimental scene buffer now follows the installed EOU sequence used by
the working production strip path: DefGPB, a one-time 512x8 GetBlk into group
196 buffer 4, then SS.MpGPB. The formerly failing first scene PutBlk and its
ACK both return zero, and external raster inspection confirms the alternating
expanded bytes at the requested 512x8 destination.

The mixed test is not accepted. Heart presentation alternates correctly
through generation 81, including status-overlap requests. Immediately after
the intentionally stale request is ACKed, native generation 82 is produced
but is not reconciled. A following request remains pending and unclaimed while
native/PB1 generation continues. The externally observed final state was
native generation 1898, shown generation 81. Testing stopped at this first
heartbeat invariant violation; shutdown was not reached. See
`results/getblk-validation.md` and the retained compact CSV evidence.

## Historical one-tick bounded-sleep candidate

The subsequent bounded candidate changes only the existing sleep wrapper from
indefinite `F$Sleep(0)` to installed-EOU one-tick `F$Sleep(2)`. It resolves
the generation-82 indefinite-sleep regression and completes the mixed workload
through generation 220 with maximum generation delta and PutBlk-completion
latency both one source tick. All 27 requests are claimed and ACKed.

That candidate was **REVISE**, because a generation produced at shutdown was
accounted without a matching heart PutBlk. It remains preserved as historical
evidence in `results/bounded-sleep-validation.md` and
`results/rejected-shutdown-candidate/`.

## Accepted single-client result

The corrected single-client lineage is preserved under
`results/single-client-accepted/`, including source, rebuild script, exact
binaries, hashes, raster transitions, request/ACK evidence, process inventory,
and validation summary. Two clean builds reproduce the retained artifacts
byte-for-byte.

The accepted run measured:

- 221 native generations and 221 PB1 transitions;
- 221 completed heart PutBlks and 221 independently verified raster changes;
- maximum edge-to-raster observation latency of one completed source frame
  (about 16.7 ms);
- 26/26 gameplay requests and ACKs plus 1/1 shutdown request and ACK;
- zero missed, duplicated, collapsed, or phase-error generations;
- zero failed `F$Send`, zero `E$USigP`, and maximum generation delta 1;
- correct stale-generation rejection and status-overlap presentation;
- atomic freeze with final visual reconciliation and no orphan processes.

This result accepts the experimental CoWin heartbeat and mixed-scene POC. It
does not accept production C6 gameplay, cartridge choreography, or universal
real-time behavior under arbitrary Level II workloads.
