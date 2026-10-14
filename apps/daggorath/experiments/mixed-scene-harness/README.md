# Stage 3E.13 mixed-scene harness

This directory defines a new deterministic mixed-scene test baseline. It does
not recreate the unavailable Stage 3E.10T temporary workload or claim its
results. It contains no guest binary, ROM, disk image, VHD, save state, or
credential. The harness has **not** been run in MAME; this is build/configuration
preparation, not heartbeat or visual acceptance.

## Contents

- `workload/status-client.c` — finite client using the existing synchronous
  broker and C6 presenter APIs. Rate and presentation count are compile-time
  options.
- `observer/observer.lua` — disposable derivative of the preserved 3E.10D
  observer, with bounded in-memory PB1 and completed-raster records and explicit
  graphics lifecycle commands.
- `launcher/private-host.mjs` — private MCP host wrapper; creates a fresh
  temporary root, copies user-supplied media/state, composes a private Lua
  bridge, uses private ports, and exposes only the required MCP controls.
- `launcher/config.example.json` — machine-local input paths to fill in outside
  the repository (copy to `config.json`; the latter is intentionally ignored).
- `launcher/validate-config.mjs` — static private RGB/config generation check;
  does not start MAME.
- `launcher/stage-modules.sh` — binary-safe staging to a *new disposable copy*
  of a D1 image, with ToolShed identity and extracted-byte checks.
- `build/build.py`, `build/two_clean_builds.py` — isolated CMOC build and
  byte-for-byte two-build check.
- `build/validate_observer.py` — static API/safety checks and classifier-control
  checks; not a substitute for executing the Lua observer in MAME.
- `ACCEPTANCE.md` — gates still required before mixed-scene evidence is valid.
- `artifact-manifest.json` — accepted binary identities, source provenance,
  toolchain and baseline references.

## Workload and protocol

The client calls `dag_compat_present_open()`, `opening_heartbeat_modules_open()`,
`dag_broker_start()`, `dag_broker_snapshot()`, repeated
`dag_compat_present_progress()` calls, `dag_broker_stats()`, and the existing
close/unlink routines. These declarations are in
`apps/daggorath/src/compat/present.h`,
`apps/daggorath/src/compat/heartbeat-broker.h`,
`apps/daggorath/src/gameplay/opening-heartbeat.h`, and
`apps/daggorath/src/audio/ipc.h`; implementations are listed with hashes in
`build-result.json` after a build. No new pipe protocol or broker operation is
added. The existing client wrapper exchanges fixed 16-byte frames and validates
the matching operation/sequence ACK before returning. Calls are synchronous,
so this single process has at most one outstanding request.

The harness counts the startup RATE/ENABLE ACKs, an initial snapshot, presenter
scene transactions (including stale snapshot/retry exchanges), STATS, and
SHUTDOWN. The count is an application-level accounting of successful API
returns and sequence changes, not an independent packet sniffer. The broker
wrapper validates ACK magic, operation, and sequence. Any unexpected sequence
delta or scene-write accounting is treated as workload failure (`188`). The
normal exit path closes the broker, module links and presenter path, then writes
a summary to the guest terminal.

The deterministic test pattern varies the status-strip pixels outside the
heart while exercising the existing status/heart-overlap strip. The overlap is
the presenter’s 8-row strip sourced from rows 152–159 and drawn at y=156–163;
the presenter fetches current heartbeat state and patches the heart before
issuing its SCENE request. The 32x7 heart occupies x=304–335, y=156–162. The
status fixture is not authentic artwork and is intended only to exercise the
existing segmentation and overlap behavior.

## Build and verify

Prerequisites already used by this repository: CMOC, lwasm, lwlink, and the
ToolShed 2.2 `os9` executable. Defaults point to the verified local ToolShed
path; override with `--os9` when needed. Build output directories must be new
and empty.

```sh
python3 apps/daggorath/experiments/mixed-scene-harness/build/build.py \
  --out /private/tmp/msh13-one --rate 46 --presentations 8
python3 apps/daggorath/experiments/mixed-scene-harness/build/two_clean_builds.py \
  --rate 46 --presentations 8
python3 apps/daggorath/experiments/mixed-scene-harness/build/validate_observer.py
node apps/daggorath/experiments/mixed-scene-harness/launcher/validate-config.mjs
```

The build result records module size, SHA-256, ToolShed ident output, build
command, source/header SHA-256 values, versions and executable hashes for the
compiler, assembler, linker, and ToolShed. Use rates 46, 45, or 41. The output
module name is the basename `msharness`; it must be staged under that name.
The verified default build is 9,892 bytes, data request 7,397 bytes, CRC
`1590C2` (Good), type/attributes `$11/$80`, and re-entrant. The linked program
image extends to `$2662` (two 8 KB program DAT blocks); the data request is one
8 KB data block. The linker map, not total module size minus data request,
establishes the program footprint. Added stack reserve is 1,024 bytes.

For staging, supply an existing read-only source D1 disk copy, a **new** output
path, and a directory containing `msharness`, `build-result.json`, the exact
accepted `dodhbp`, and `dhbpack`:

```sh
apps/daggorath/experiments/mixed-scene-harness/launcher/stage-modules.sh \
  /path/to/disposable-source-d1.dsk /private/tmp/msh13-d1.dsk /path/to/module-output
```

The script makes a separate output copy, uses ToolShed without line-ending
conversion, sets owner/public read+execute attributes, verifies CRCs, extracts
each module, and compares extracted bytes against host binaries. It refuses to
overwrite the target disk. Do not pass canonical media.

## Private MAME configuration

Copy `launcher/config.example.json` to an ignored local `launcher/config.json`
and replace the angle-bracket input placeholders with existing trusted source
paths. Required inputs are a boot floppy, a disposable D1 floppy containing
the staged modules, a VHD, and a documented EOU-ready state file. The launcher
copies all four inputs to a fresh temp root before launch. It does not read
`MCP/.env`; it calls the repository MCP config/controller with explicit
overrides, a private bridge port and private state/config directories. The D1
test image is mounted on MAME flop2 (`/d1`).

Before delegating `coco_start`, the wrapper writes and verifies the private
`coco3h.cfg` XML with `screen_config=1`. It requires MAME 0.289, config label
`ample-289-u2`, machine `coco3h`, HD6309E target, RGB and 2 MB. MAME’s command
line receives `coco3h` and `-ramsize 2M`; HD6309E is the machine’s selected CPU,
not a separate command-line override. The MAME version string alone does not
prove the Ample build suffix; that label is an operator-supplied config value
and must be independently verified before a real run.

The configured ready-state file must come from the established documented EOU
workflow. No ready-state, disk, or VHD inputs are bundled here. The launcher is
therefore not ready to start until those external inputs are supplied and
verified. It exposes only `coco_start`, `coco_stop`, `coco_status`,
`coco_mount_flop`, `os9_restore_ready`, and `os9_run`, plus its private observer
commands; it does not modify MCP production settings.

After filling `config.json`, start the private wrapper with
`node launcher/private-host.mjs launcher/config.json`. It prints the selected
loopback HTTP port and private run root. Send JSON POSTs to
`http://127.0.0.1:<httpPort>/rpc`: `{"name":"coco_start","arguments":{}}`,
then `{"name":"os9_restore_ready","arguments":{}}`, then
`{"name":"coco_mount_flop","arguments":{"drive":"flop2"}}`, followed by
`{"name":"msh_observer_ready"}`. Only when readiness reports `ready`,
`tapInstalled`, and `frameRegistered` as true, submit
`{"name":"os9_run","arguments":{"command":"msharness","allow_graphics":true,"timeout_ms":120000}}`.
Drain remaining records with `msh_observer_drain`; stop MAME with
`{"name":"coco_stop","arguments":{}}` or Ctrl-C. Do not reuse a temp run
root or attach the wrapper to a production MCP process.

## Observer lifecycle and evidence limits

The observer preserves the archived 3E.10D methods: a narrow `$FF22` write tap,
driver generation/phase reads through the known system-driver DAT mapping, and
`video:snapshot_pixels()` for published frames. It does not call
`screen:pixel()`, encode images, write files, or traverse process descriptors.
Its 4096-event buffer is bounded; the host drains batches and writes JSONL
outside callbacks. The readiness endpoint reports observer setup, tap and frame
notifier registration, plus six static classifier controls.

Lifecycle is set by the private host when an `os9_run`-scoped
`read_text_console` response reports EOU text mode unsupported, and returns to
text mode. This is a bridge display-mode signal, not process identity and not a
heart-pixel heuristic. It proves a transition away from EOU’s supported text
mode, but by itself cannot distinguish the intended graphics window from every
other possible non-text display mode. If that ambiguity occurs in a run, label
the graphics interval indeterminate and do not accept raster timing.

The classifier’s phase, old-phase, blank, partial-row, and unknown controls are
checked statically and asserted by the Lua initialization. Since this observer
has not yet been loaded by MAME in this checkpoint, Lua runtime compatibility,
callback execution, PB1 events, lifecycle delivery, raster timing, and observer
overhead remain unverified. No timing acceptance is claimed.

## Provenance and dependencies

The archived 3E.10D source remains unchanged at
`apps/daggorath/experiments/c6-stage3e-weekend-preservation/stage-3e10d/compare-observer.lua`.
Its previously recorded SHA-256 is in `artifact-manifest.json`. This observer
is a new derivative, not the archived 3E.10T observer. Historical temporary
`dag_private_status_only()` source/binary was not found in the searched
preservation tree and is not recreated: this harness uses the documented
public presenter and broker API instead.

Build dependencies outside this directory are explicit source/header paths
inside this repository and are hashed into each `build-result.json`. Runtime
dependencies are the two exact accepted heartbeat artifacts, the established
MCP `dist` build and source modules, installed MAME/ROMs, ToolShed, disposable
media, and a documented EOU ready state. No contents of `/private/tmp` are
assumed by the source build. The exact fixed heartbeat pack was recovered in a
separate disposable evidence directory and its hash is recorded. That temporary
location is provenance only, not a required staging path: provide any copy
matching the manifest hash to staging. The pack contains `DHeartbeat` (1,382
bytes, 64-byte static data, CRC `FBC38A`, Good) and `dhb` (39 bytes, CRC
`BF478C`, Good); ToolShed `ident` confirmed embedded names, descriptor-to-driver
reference, module types and attributes. If the byte-identical pack cannot be
supplied, stop and recover it through the documented Stage 3E.12A process
rather than rebuilding from current production sources.

The private host imports these MCP modules from `MCP/dist`: config,
bridge-server, mame-process, tools, and toolchain. Recreate that repository
build with `npm run build --prefix MCP` from the repository root; the source
files and lock/configuration inputs used by the wrapper are hash-listed under
`launcherSourceDependencies` in `artifact-manifest.json`. The corresponding
`MCP/dist` output and installed Node dependencies are runtime build products,
not copied into this experiment.
