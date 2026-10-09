# Daggorath C6 architecture decisions

**Status:** the ARM/S$Wake, ARM/KICK, PipeMan, and CoWin mixed-scene designs are
accepted experimental checkpoints. Production C6 gameplay and cartridge
choreography are **not yet accepted**.

The accepted CoWin artifact is commit
`c519893c70512aa3e08a203148e9e160a8241253`. Its compact result is
[validation.json](../../apps/daggorath/experiments/heartbeat-wake/cowin/results/single-client-accepted/evidence/validation.json).

## Governing invariant

One native logical heartbeat edge must update PB1 sound, phase, generation, and
the corresponding visible heart without loss, duplication, collapse, or drift.
The visible PutBlk must complete within 33.34 ms. Generation state is authority;
signals are notifications. A scene may never overwrite a newer heart phase.

## Selected design

| Decision | Reason | Evidence status |
| --- | --- | --- |
| Native VIRQ generates heartbeat edges. | Preserves source timing independent of foreground gameplay. The callback performs only bounded PB1/state work and notification. | **EXPERIMENTALLY ACCEPTED**: 221 native/PB1 edges in the CoWin run. Driver edge path: [driver.asm:576](../../apps/daggorath/experiments/heartbeat-wake/cowin/source/driver.asm#L576). |
| PB1 changes on the native edge. | Sound and visual phase derive from the same event; no later process synthesizes sound timing. | **EXPERIMENTALLY ACCEPTED** for transition counts. Audible human-game acceptance remains a production gate. |
| One priority-192 presentation broker. | It can run independently of priority-128 gameplay and is the only visible PutBlk writer. | **EXPERIMENTALLY ACCEPTED** under the retained workload; arbitrary system load is not guaranteed. |
| One persistent priority-128 gameplay-style client. | Prevents equal-priority competition and removes competing reply readers. | **EXPERIMENTALLY ACCEPTED**: PID 4 was the sole reply reader; PID 3 the sole writer. |
| ARM/S$Wake/KICK notifications. | ARM suppresses unnecessary competing signals; KICK commits request state after the complete frame write. Neither notification is authoritative. | **EXPERIMENTALLY ACCEPTED** in the three predecessor checkpoints and CoWin POC. |
| `F$Sleep(2)` bounded recovery. | The installed kernel can consume a wake before a later indefinite sleep. One-tick sleep guarantees another reconciliation opportunity without hiding a generation. | **TARGET VERIFIED** and **EXPERIMENTALLY ACCEPTED**; maximum observed latency was one frame. |
| Authoritative generation and phase. | Detects missed, duplicate, collapsed, or wrong-phase service even when signals coalesce. | **EXPERIMENTALLY ACCEPTED**: final 221/221/221 and maximum delta 1. |
| `$BC` collapse protection. | A delta greater than one means a visible generation would be lost. Termination is safer than silently presenting only the latest phase. | **EXPERIMENTALLY ACCEPTED** as a guard; it remained zero in the final run. |
| Fixed 16-byte PipeMan request/ACK frames. | Small, versioned, deterministic protocol; exact reads/writes handle fragmentation. | **EXPERIMENTALLY ACCEPTED** including 5+11 request and 7+9 ACK fragmentation in the PipeMan checkpoint. |
| Exactly one reply reader. | Avoids shared reply-path `PD.CPR` and I/O queue serialization observed with multiple readers. | **TARGET VERIFIED** failure and **EXPERIMENTALLY ACCEPTED** correction. |
| Broker-exclusive visible PutBlk ownership. | Serializes status, scene, and heart writes in one phase-aware order. | **EXPERIMENTALLY ACCEPTED**: 221 heart PutBlks and mixed requests. |
| Two persistent 32x7 heart GP buffers. | Heart updates issue only a small prepared PutBlk and cannot create a white rectangle by transferring a larger status block. | **EXPERIMENTALLY ACCEPTED** with authentic Daggorath heart rasters. |
| One persistent mapped 512x8 scene buffer. | Bounds each scene transfer and avoids repeated define/load/map overhead. | **EXPERIMENTALLY ACCEPTED** after `DefGPB -> GetBlk -> SS.MpGPB`. |
| Eight-row presentation units. | Bounds graphics ownership intervals while matching the logical renderer's strip model. | **EXPERIMENTALLY ACCEPTED** for transport/presentation mechanics; cartridge choreography remains unaccepted. |
| Atomic freeze and final visual reconciliation. | Prevents shutdown from abandoning a generation produced just before disable. `shownGeneration` advances only after successful heart PutBlk. | **EXPERIMENTALLY ACCEPTED**: shutdown legitimately finished at 221/221/221. |
| 33.34 ms deadline. | At the fastest legal three-tick heartbeat, this retains margin before the next edge and prevents accumulated drift. | **EXPERIMENTALLY ACCEPTED** under this workload: maximum about 16.7 ms. No universal guarantee is claimed. |

## Runtime flow

```text
VIRQ: PB1 -> phase -> generation -> conditional S$Wake

client: write_exact(request, 16) -> KICK -> read_exact(ACK, 16)

broker:
  reconcile every pending heartbeat generation
  claim requestPending
  read and validate one request
  reject stale status generation, or perform one bounded PutBlk
  reconcile heartbeat again
  ACK exactly once
  ARM; recheck authority; F$Sleep(2) when idle
```

The accepted implementation is
[wakekick.c](../../apps/daggorath/experiments/heartbeat-wake/cowin/source/wakekick.c).
`reconcile()` presents the heart before recording the generation
([lines 66-72](../../apps/daggorath/experiments/heartbeat-wake/cowin/source/wakekick.c#L66));
`service_request()` generation-stamps status requests and checks again after a
scene transfer ([lines 27-65](../../apps/daggorath/experiments/heartbeat-wake/cowin/source/wakekick.c#L27));
shutdown freezes, visibly reconciles, disarms, releases, and closes
([lines 103-111](../../apps/daggorath/experiments/heartbeat-wake/cowin/source/wakekick.c#L103)).

## Graphics ownership and buffers

The broker creates a CoWin path and owns all visible writes. Group 196 contains:

| Buffer | Shape | Purpose |
| --- | --- | --- |
| 1 | 512x8 status staging in the POC | Status-overlap validation. |
| 2 | 32x7 | Heart phase 0. |
| 3 | 32x7 | Heart phase 1. |
| 4 | 512x8, persistently mapped | Scene strip payload. |

The mapped scene initialization is implemented at
[graphics.c:83](../../apps/daggorath/experiments/heartbeat-wake/cowin/source/graphics.c#L83).
The client must not refill a mapped scene payload before its ACK. Group/buffer
identity is global to the graphics system; the mapped pointer and path number
remain local to their process.

## Process, DAT, and resource model

The accepted broker process ran at priority 192; its client ran at 128 from its
first active-queue insertion. Its observed DAT image was
`0010:333E:333E:333E:333E:333E:0018:0007`:

- one program block;
- one mapped CoWin scene-buffer block at logical block 6;
- one process-data block at logical block 7;
- 3/8 logical DAT blocks occupied.

The retained artifacts are:

| Artifact | Bytes | Data | CRC | SHA-256 |
| --- | ---: | ---: | --- | --- |
| `wcowin-map3` | 7,298 | 702 | `56422B` | `1946e92a6e3755469a50c00c56ffd02ed6de08193c54e9f57d236c361b2f5102` |
| `kickchild` | 3,103 | 37 | `4F8C58` | `1f00dc3e172149e6a598407edf7bfde929b2c9bfd7bfb97cc67ae0618c8e3099` |
| `darmpack-cowin` | 1,415 | combined pack | driver `39AA22`; descriptor `7DB914` | `09f2422266dff2914fdce8568403160ad1eeb88bfcdb26d71b0e9881aacc101d` |

These are experimental resource measurements. Production integration must
recalculate process and system memory rather than copying them as a budget.

## Rejected alternatives

| Alternative | Why rejected |
| --- | --- |
| Foreground-only heartbeat reconciliation | Gameplay, rendering, overlays, and synchronous graphics can delay presentation across multiple edges. |
| Heartbeat service only when a scene request arrives | Legitimate 81-tick request-free waits accumulate unseen generations. |
| PipeMan `SS.Ready` polling | Installed edition 5 does not expose queue readiness. |
| Indefinite `F$Sleep(0)` after user-space ARM/recheck | Lost-wakeup race remains between final recheck and committed sleep. |
| SCF driver operation that sleeps atomically while holding the call | SCF retains `V.BUSY`; another process cannot reach KICK through the same device. |
| Multiple reply readers or short-lived clients | Shared endpoint serialization blocked ACK progress before PipeMan dispatch. |
| Multiple visible graphics writers | Ordering between scene and heart becomes nondeterministic and stale status can restore the wrong heart. |
| Full-status heart redraw | Larger transfer caused visible isolation/white-rectangle behavior and increased deadline risk. |
| Direct framebuffer/GIME mutation | No accepted Level II application ABI or ownership proof; it would race GrfDrv and DAT switching. |
| Disable `$BC`, increase retries, or collapse to latest generation | Conceals a real missed visible transition and violates the core invariant. |

## Accepted experimental result

The final run recorded:

- 221 native generations;
- 221 PB1 transitions;
- 221 completed heart PutBlks;
- 221 independently verified raster transitions;
- maximum visual latency of one source frame, approximately 16.7 ms;
- 26/26 gameplay requests and ACKs;
- 1/1 shutdown request and ACK;
- zero missed, duplicate, collapsed, or phase-error generations;
- zero failed `F$Send` and zero `E$USigP`;
- correct stale rejection and status-overlap behavior;
- clean final reconciliation with no orphan process.

This is **EXPERIMENTALLY ACCEPTED** for the retained workload. It does not prove
arbitrary-load real-time guarantees, production gameplay integration, audible
human-review synchronization, or cartridge choreography.

## Known pitfalls

- Verify actual process priority and creation-time age; intended priority is not
  evidence.
- Preserve CMOC resident Y in every C-callable assembly wrapper.
- Never send process-local pointers or path numbers through IPC.
- Never mark a generation shown until the corresponding PutBlk succeeds.
- Keep one request outstanding and one reply reader.
- Treat PipeMan reads/writes as potentially partial.
- Reconcile after scene PutBlk because an edge may occur during the call.
- Freeze native generation before the final shutdown snapshot, then visibly
  reconcile the last produced generation.
- Pair emulator state with unchanged disposable media; a restore does not roll
  back disk contents.

## Production integration checklist

- [ ] Preserve the accepted C5 heartbeat fixtures unchanged.
- [ ] Port the experimental protocol without changing native edge timing.
- [ ] Audit all C/assembly boundaries for Y preservation and emitted syscalls.
- [ ] Verify broker 192 and gameplay 128 in live process descriptors.
- [ ] Verify one persistent reply reader and no inherited duplicate endpoints.
- [ ] Verify exact 16-byte framing, partial transfer loops, and sequence matching.
- [ ] Verify persistent heart buffers contain the accepted two phases.
- [ ] Verify `DefGPB -> GetBlk -> SS.MpGPB` and exact 512-byte mapping.
- [ ] Enforce one scene payload owner until ACK.
- [ ] Reject stale status scenes and repatch after overlapping transfers.
- [ ] Keep `$BC`, maximum generation delta 1, and 33.34 ms deadline gates.
- [ ] Exercise an exact 81-tick request-free interval.
- [ ] Exercise accelerated post-`!!!` heartbeat with concurrent scene traffic.
- [ ] Measure native, PB1, completed PutBlk, and visible raster independently.
- [ ] Test cancellation, broker death, client death, window failure, and repeated launch.
- [ ] Atomically freeze, visibly reconcile, disarm, release, close, and reap.
- [ ] Verify no orphan modules, paths, processes, or post-close wake.
- [ ] Complete continuous human review of sound/heart synchronization and cartridge choreography.

## Unresolved production questions

1. How should unexpected broker death retire driver ownership without weakening
   generation accounting?
2. Does the production workload retain the measured deadline under concurrent
   audio, scheduler, overlay, and disk activity?
3. What is the final production module/data/DAT budget after integration?
4. Can production shutdown and cancellation retain exact visual reconciliation
   through every error path?

The general Level II findings and their evidence limits are recorded in
[NITROS9_EOU_LEVEL2_LESSONS.md](../technical/NITROS9_EOU_LEVEL2_LESSONS.md).
