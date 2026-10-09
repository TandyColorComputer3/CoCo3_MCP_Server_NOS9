# Verified NitrOS-9 EOU Level II lessons

This reference consolidates behavior verified while developing the Daggorath C6
heartbeat experiments. It describes the installed EOU target used by this
repository. It does not assert that every NitrOS-9 release behaves identically.

## Evidence labels

| Label | Meaning |
| --- | --- |
| **SOURCE VERIFIED** | Confirmed in an identified source tree or definition file. |
| **TARGET VERIFIED** | Observed directly in the installed EOU runtime. |
| **EXPERIMENTALLY ACCEPTED** | Passed a retained, reproducible experiment under its documented workload. |
| **INFERRED** | Best explanation consistent with evidence, without a complete source/runtime proof. |
| **UNRESOLVED** | Evidence is insufficient for a conclusion. |

The maintained comparison tree is `/Volumes/SEDONA/Projects/nitros9-reference`
at commit `f470fa52eb172b59b22c1b722074998cb42de9b1`. Installed EOU behavior takes
precedence when the trees differ. The source relationship and limits are indexed
in [EOU_UPSTREAM_CROSSWALK.md](../source-index/EOU_UPSTREAM_CROSSWALK.md) and
[RUNTIME_MATCHES.md](../source-index/RUNTIME_MATCHES.md).

## 1. Level II process scheduling and signals

| Finding | Why it matters and guidance | Evidence and limits |
| --- | --- | --- |
| `F$Fork` children inherit the parent's priority; the syscall has no child-priority argument. | If a child must enter the active queue at a lower priority, set the parent's priority before `F$Fork`, fork, then restore the parent before enabling time-critical work. Changing the child after it starts is too late to prevent its initial scheduling age and priority from competing with the parent. | **TARGET VERIFIED** by the ARM/KICK priority experiments; summarized in [heartbeat-wake/README.md](../../apps/daggorath/experiments/heartbeat-wake/README.md#architecture-established-by-the-experiments). Do not infer this from a guessed register argument. |
| `S$Wake` is a notification, not authoritative event state. | Store generation and request state separately. A wake may be coalesced or consumed; foreground code must reconcile authoritative state after every return. | **EXPERIMENTALLY ACCEPTED** in `arm-wake`, `arm-kick`, and `pipeman`. The accepted driver publishes generation before attempting `F$Send` at [driver.asm:591](../../apps/daggorath/experiments/heartbeat-wake/cowin/source/driver.asm#L591). |
| A user-space `ARM -> recheck -> F$Sleep(0)` sequence still has a final lost-wakeup window. | A wake delivered while the process is current can be consumed on the interrupt-return path before the later sleep commits. Another user-space recheck cannot make two separate kernel transitions atomic. C6 uses a one-tick bounded sleep as recovery while generation remains authoritative. | **TARGET VERIFIED** in the rejected indefinite-sleep CoWin run; the accepted bounded behavior is documented in [cowin/README.md](../../apps/daggorath/experiments/heartbeat-wake/cowin/README.md). This is an installed-EOU result, not a universal signal theorem. |
| Scheduler priority reduces interference but does not guarantee a deadline. | Verify the actual `P$Prior` in the Level II process descriptor and measure edge-to-resume latency. Avoid equal-priority helper children during deadline tests. | **TARGET VERIFIED**: receiver priority 128 missed a generation; priority 192 plus creation-time priority-128 clients passed the retained workload. Arbitrary workloads remain unproven. |

Authoritative Level II global offsets used by the external observer were
`D.Proc=$50`, `D.AProcQ=$52`, `D.WProcQ=$54`, `D.SProcQ=$56`, and
`D.PthDBT=$88`. These are **TARGET VERIFIED** against the installed Level II
definitions. Do not reuse Level I offsets. Queue membership must be decoded from
the matching process descriptor definitions rather than inferred from a process
not advancing.

## 2. `F$Sleep` and `F$Send`

- **SOURCE VERIFIED / TARGET VERIFIED:** `F$Sleep` takes its delay in X.
  `X=$0000` requests indefinite sleep until a signal; `X=$0001` yields the
  remainder of the current time slice; timed sleeps use the kernel's
  subtract-one convention. The C-callable wrapper must actually emit the SWI2
  call. The accepted lwasm wrapper is
  [sleep0.asm:1](../../apps/daggorath/experiments/heartbeat-wake/cowin/source/sleep0.asm#L1).
- **TARGET VERIFIED:** an early inline-C attempt emitted no syscall bytes, and
  an uninitialized local was mistaken for error 241. Always inspect generated
  assembly when validating an ABI boundary.
- **EXPERIMENTALLY ACCEPTED:** VIRQ-context `F$Send(S$Wake)` produced 220
  successful heartbeat notifications with no `E$USigP` in the retained
  heartbeat-only proof. The callback saves CC, D, X, and Y and never performs
  graphics or PipeMan I/O; see
  [driver.asm:561](../../apps/daggorath/experiments/heartbeat-wake/cowin/source/driver.asm#L561).
- **EXPERIMENTALLY ACCEPTED:** installed-EOU `F$Sleep(2)` bounds recovery to one
  60 Hz tick in the tested CoWin workload. It is a recovery bound, not permission
  to discard a generation and not a universal scheduling guarantee.

Illustrative lwasm wrapper:

```asm
_sleep_signal
        pshs    y
        ldx     #0
        os9     F$Sleep
        bcs     failed
        clrb
failed  puls    y,pc
```

The exact register saves must match the project's CMOC convention; this example
is illustrative and should be compared with the linked listing.

## 3. PipeMan and IOMan serialization

| Finding | Guidance | Evidence and limits |
| --- | --- | --- |
| Installed EOU PipeMan edition 5 has stub GetStt/SetStt entries. `SS.Ready` success does not report queued bytes or a complete record. | Do not use `SS.Ready` for this installed pipe. Write the complete fixed frame, commit authoritative `requestPending` through KICK, then let the broker claim before reading. | **SOURCE VERIFIED** against `/dd/SOURCECODE/ASM/NITROS9/PIPE/pipeman_beta6.asm:93-96` and **TARGET VERIFIED**; see [DAGGORATH_AUDIO_IPC_ARCHITECTURE.md](../apps/DAGGORATH_AUDIO_IPC_ARCHITECTURE.md#installed-eou-boundary). The upstream named-pipe implementation is different. |
| Reads and writes may be partial. | Loop until all bytes transfer or an error/EOF occurs. Do not treat syscall success as proof that all 16 bytes moved. | **EXPERIMENTALLY ACCEPTED:** fragmented 5+11-byte request and 7+9-byte ACK passed; see [pipeman/README.md](../../apps/daggorath/experiments/heartbeat-wake/pipeman/README.md#protocol-and-timing). |
| IOMan serializes shared path-descriptor access with `PD.CPR` and process I/O queue links. | Avoid multiple readers on one reply endpoint. One client owns the reply read side, and one broker writes replies. Close inherited duplicates that are not part of that ownership. | **TARGET VERIFIED:** the rejected candidate blocked before PipeMan dispatch with broker PID 3 queued behind sleeping owner PID 4; see [rejected candidate README](../../apps/daggorath/experiments/heartbeat-wake/cowin/results/rejected-shutdown-candidate/README.md#first-blocked-dependency). The deeper historical PID-7 dependency remains **UNRESOLVED**. |
| Empty/full pipe waits are real scheduling and resource dependencies. | Use one request in flight, exactly one matching ACK, and authoritative request state. Treat EOF after the client closes as a consequence unless ordering proves otherwise. | **EXPERIMENTALLY ACCEPTED** by the 27-request PipeMan and CoWin checkpoints. |

Illustrative sequencing:

```text
client: write_exact(16) -> KICK -> read_exact(16) -> validate sequence
broker: reconcile_heart -> claim requestPending -> read_exact(16)
        -> validate -> operate -> ACK exactly once -> reconcile_heart
```

## 4. CoWin and GrfDrv graphics buffers

- **TARGET VERIFIED:** a persistent mapped scene buffer needs the installed EOU
  sequence `DefGPB -> GetBlk -> SS.MpGPB`. Defining then mapping without the
  one-time `GetBlk` geometry initialization produced `E$ICoord`; the corrected
  512x8 sequence succeeded. See
  [graphics.c:29](../../apps/daggorath/experiments/heartbeat-wake/cowin/source/graphics.c#L29)
  and [getblk-validation.md](../../apps/daggorath/experiments/heartbeat-wake/cowin/results/getblk-validation.md).
- **TARGET VERIFIED:** GP group/buffer numbers are graphics-system identities,
  while the pointer returned by `SS.MpGPB` is process-local. Pass group/buffer
  identity and coordinates across IPC, never a process-local pointer or path
  number.
- **EXPERIMENTALLY ACCEPTED:** one priority-192 broker exclusively owned the
  graphics path, two persistent 32x7 heart buffers, and one persistent mapped
  512x8 scene buffer. It completed 221 heart PutBlks and 221 independently
  observed raster transitions. Evidence is in
  [single-client-accepted](../../apps/daggorath/experiments/heartbeat-wake/cowin/results/single-client-accepted/).
- **SOURCE VERIFIED / TARGET VERIFIED:** CoWin and GrfDrv serialize graphics
  work; concurrent visible writers do not create a safe ordering contract.
  Use one visible PutBlk owner and bounded scene units.

Mapped-buffer lifecycle:

```text
define GP buffer -> initialize geometry with GetBlk -> map -> verify length
-> write mapped pixels -> PutBlk by broker -> wait for ACK before refill
-> unmap -> kill GP buffer -> close owned path
```

## 5. CMOC and 6809 calling conventions

- **TARGET VERIFIED:** CMOC OS-9 code uses Y as the resident data-base register.
  Every C-callable assembly routine and every reverse callback into resident C
  must restore/preserve the caller's resident Y unless its documented ABI says
  otherwise.
- **TARGET VERIFIED:** `_dag_broker_scene` once returned with Y changed from
  `$0000` to `$0001`. The following `_os_clock` stack check indexed `__stbot,Y`
  and `__mtop,Y` one byte off, falsely reporting `E$MemFul`. The proposed frame
  was safe when Y was correct. Memory growth was therefore the wrong remedy.
- **TARGET VERIFIED:** mapped-module-to-resident calls have two independent
  requirements: restore resident Y, and use a runtime-correct target. The
  scheduler callback required PC-relative target resolution; the command
  overlay's loader-relocated target was already correct but still required Y
  restoration. See
  [DAGGORATH_LEVEL2_MODULARIZATION.md](../apps/DAGGORATH_LEVEL2_MODULARIZATION.md).

Minimal preservation pattern:

```asm
_c_callable_wrapper
        pshs    y
        ; temporary Y use and OS-9 call
        puls    y,pc
```

Audit every success and error exit for exactly one restoration. Preserve default
CMOC function-stack checking; it exposed the corrupted context.

## 6. MMU/DAT memory management

- **SOURCE VERIFIED / TARGET VERIFIED:** Level II maps logical 8 KiB blocks
  through each process's DAT image. Link-map addresses are module-relative until
  translated through `P$PModul` and the live DAT mapping. The established method
  is documented in
  [DAGGORATH_LEVEL2_MODULARIZATION.md](../apps/DAGGORATH_LEVEL2_MODULARIZATION.md).
- **TARGET VERIFIED:** process DAT exhaustion and system/physical allocation
  failure are different. A process can have no free logical DAT slot while the
  system still has RAM; conversely module loading can fail because system mapping
  resources are exhausted even when the target process layout looks small.
- **TARGET VERIFIED:** loading a driver and descriptor separately exhausted the
  tested system mapping path and produced error 237. Loading the same two modules
  as one combined pack succeeded. See
  [pipeman/README.md:49](../../apps/daggorath/experiments/heartbeat-wake/pipeman/README.md#reproduction).
- **EXPERIMENTALLY ACCEPTED:** the CoWin broker used three of eight DAT blocks:
  one program, one mapped CoWin buffer, and one process-data block. This is the
  accepted experiment's observed layout, not a general minimum.

## 7. Driver ownership and lifecycle

Use one identity convention for owner checks. CLAIM stores the current process
PID from `D.Proc/P$ID`; RATE, ENABLE, DISABLE, RELEASE, ARM, DISARM, and KICK must
validate against the same current PID. `PD.PD` is not the process PID.

The accepted lifecycle is:

1. open and claim;
2. configure while owned;
3. publish generation/phase before notification;
4. service and acknowledge each generation without collapsing it;
5. stop new requests;
6. atomically freeze new native generation;
7. snapshot and visibly reconcile every generation produced before freeze;
8. clear ARM/request state, retire owner PID, close paths, and reap children.

This is **EXPERIMENTALLY ACCEPTED** for the retained POCs. Unexpected owner death
outside the tested shutdown protocol remains **UNRESOLVED** for production.

Private operation `$95` returns and clears elapsed 60 Hz ticks. Accumulate its
deltas in a sufficiently wide counter after an initial drain; treating successive
values as absolute time released the original 81-tick gate early. This is
**TARGET VERIFIED** and documented in
[pipeman/README.md:15](../../apps/daggorath/experiments/heartbeat-wake/pipeman/README.md#protocol-and-timing).

## 8. Emulator diagnostics and reproducibility

- Positively identify the executing artifact with module size, data request, CRC,
  SHA-256, and a unique in-memory build marker before interpreting direct memory.
- Read live process data only under the correct process DAT image. Static module
  bytes are not live state.
- Use authoritative Level II offsets from the installed definitions. Level I
  offsets produced a false lost-request diagnosis during C6.
- Prefer cheap fixed memory journals and completed-frame observation. Busy holds
  can starve the Lua bridge and alter scheduling.
- A MAME save state is not a disk snapshot. Restoring memory against changed
  backing media can leave stale RBF directory/allocation state. Pair disposable
  media with its state, remount after restore where required, and cold boot after
  changing staged media. See
  [NITROS9_ARTIFACT_STAGING.md](../architecture/NITROS9_ARTIFACT_STAGING.md).
- Preserve accepted source and binaries before extending a POC. Rebuild twice,
  compare bytes, validate ToolShed CRC, and retain hashes and concise evidence.

## 9. Common failed approaches

| Failed approach | Cause | Replacement |
| --- | --- | --- |
| Use PipeMan `SS.Ready` as queue depth | Installed edition-5 entry is a success stub. | Authoritative `requestPending`, exact framing, and blocking read only after claim. |
| `ARM -> recheck -> F$Sleep(0)` | Wake can be consumed before sleep commits. | Generation authority plus accepted one-tick bounded recovery. |
| Multiple reply readers | Shared path ownership and IOMan `PD.CPR` serialization can block the broker before PipeMan dispatch. | One persistent reply reader. |
| Raise child priority after fork | Child already entered scheduling at inherited priority/age. | `F$SPrior -> F$Fork -> F$SPrior` before heartbeat enable. |
| Define then directly map a scene GP buffer | Geometry metadata was not initialized. | `DefGPB -> GetBlk -> SS.MpGPB`. |
| Increase data request after false `_stkcheck` | Y corruption made stack globals appear invalid. | Preserve resident Y at every C/assembly boundary. |
| Treat `$95` as absolute time | Operation is read-and-clear. | Drain, sum deltas, detect overflow. |
| Load driver and descriptor separately | Tested system mapping resources were exhausted. | One verified combined module pack. |
| Infer process state from host elapsed time | Host sampling and emulator scheduling differ from guest ticks. | Correlate Level II queues, PC/stage, and guest source ticks. |

## Known pitfalls and unresolved questions

- The accepted 16.7 ms CoWin maximum applies to the retained workload only.
- Abnormal broker or owner death cleanup is not production accepted.
- The deeper historical PID-7 I/O queue relationship was not resolved; the
  accepted single-reader architecture removes it from the tested path.
- Installed CoWin/GrfDrv are not claimed byte-identical to the maintained
  upstream checkout.
- Direct active-screen memory mutation has no accepted application ABI and was
  not used.
