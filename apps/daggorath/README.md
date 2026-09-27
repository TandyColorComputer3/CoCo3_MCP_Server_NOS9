# Daggorath Wizard M1 (`dodwiz`)

Native NitrOS-9 Level II, 6809-compatible CMOC application. Only the original wizard
introduction is ported. [Provenance](PROVENANCE.md) identifies every adapted source
component. [Live report](../../docs/apps/DAGGORATH_WIZARD_M1.md) records measured
fidelity and limitations.

```
src/original/data.h           generated original vector/font/message data
src/original/provenance.json  pinned source hashes and segment locations
src/original/logical.c        private 256x192 raster/text renderer
src/logical.h                renderer boundary
src/main.c                   absolute cartridge-derived deadlines, cancellation
src/playback.c               lossless build-time frame cache decoder
prepare_frames.py             executes unchanged logical renderer on host
src/presentation.c/.h        owned OS-9 window and buffer; 2× horizontal, (+64,+4) only here
src/os9.c, platform.h        verified CMOC syscall/signal wrappers
src/window-path.c            owned /w open/close
src/module.asm               explicit dodwiz module name/edition
```

Build and tests (repository root):

```sh
python3 apps/daggorath/build.py --out /private/tmp/daggorath-m1/build \
  --cmoc /usr/local/bin/cmoc --lwasm /usr/local/bin/lwasm --lwlink /usr/local/bin/lwlink \
  --os9 /Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9
python3 apps/daggorath/test.py
python3 apps/daggorath/test_lifecycle.py
python3 apps/daggorath/test_presentation.py
python3 apps/daggorath/test_playback.py
```

`import_data.py` regenerates selected data only from the pinned external reference;
the ordinary build uses the checked-in generated header and needs no external
checkout. The build record retains actual sources, libraries, tools and output
identity. `-O0` and no 6309-specific instructions/optimization are intentional.

Stage through `node MCP/dist/os9-stage-cli.js` onto a **fresh** artifact floppy.
Cold boot new copies of the EOU media with that floppy attached. Make/restore a
session-only ready checkpoint paired with those image bytes, then:

```json
{"command":"load /d1/dodwiz"}
{"command":"dodwiz","timeout_ms":120000,"allow_graphics":true}
{"command":"dodwiz cancel","timeout_ms":120000,"allow_graphics":true}
{"command":"date"}
```

`cancel` self-sends OS-9 signal 3 partway through fade-in, exercising the handled
signal path without racing MCP keyboard injection. User signals 2/3 use the same
flag/cleanup path. Forced termination cannot guarantee cleanup. The application
must start with stdout attached to the verified Term.

No guest files are written. The selected GP buffer (196/1) is **exclusively
allocated** with DefGPB; an existing buffer causes an error, never replacement.
Only successfully owned resources are freed. A fixed namespace may cause a safe
allocation failure on another setup; general buffer allocation policy is deferred.
The logical framebuffer owns 6144 bytes. An exclusively mapped 12288-byte GP
buffer presents it at 512×192, origin (64,4), on one owned type-5 640×200 screen.
The 64-pixel side bands remain black. Public SS.MpGPB maps the buffer; F$ClrBlk
unmaps it before KillBuff. The clock adapter performs read-only F$CpyMem of the
verified EOU Level II clock packet and checks it against F$Time. This private
clock-layout dependency is target-specific, not a portable timing API.

The host C compiler generates a 13282-byte RLE cache from the unchanged renderer;
all 19 cached frames are checked byte-for-byte. Playback prepares frames ahead of
absolute deadlines and sleeps cooperatively. See the live report for measured
15.220-second cartridge / 15.487-second port timing and remaining differences.

Sound is silent in M1. Original buzz/explosion event boundaries are retained for a
future OS-managed audio backend. No hardware writes, private interrupts, BASIC ROM
calls, gameplay or side-panel UI are included.

## Gameplay M2 status display

The original left/right hand names and heart now occupy logical rows 152–159.
The existing torch subset also accepts `PULL RIGHT TORCH` and `USE RIGHT`.
Build/preload the matching `dhbpack`: Gameplay M2 queries its latched native
heartbeat phase (GetStat `$90`, B=0/1); older drivers are rejected.

New checks: `python3 apps/daggorath/test_status.py` and
`python3 apps/daggorath/test_status_phase.py`.
See [Gameplay M2](../../docs/apps/DAGGORATH_GAMEPLAY_M2.md) for exact cartridge
comparisons, lifecycle results, refresh limits, memory budget and complete
regression results.

## Gameplay M3 bag/hand operations

`dodgame seed0` supports original `PULL LEFT/RIGHT <generic>` or
`PULL LEFT/RIGHT <adjective> <generic>`, and `STOW LEFT/RIGHT`.
Unique original-table prefixes work: `P R W SW` pulls the starting wooden sword
into the right hand; `S R` stows it. `P L P T` pulls the pine torch; existing
`USE LEFT` lights/stows it. Carrying a sword does **not** implement combat.
Other commands retain their M1/M2 bounded adapters; `EXIT` is the OS-9 extension.

Build with `python3 apps/daggorath/build_gameplay.py --out MCP/work/gameplay-m3/build`;
use the [disposable artifact workflow](../../docs/architecture/NITROS9_ARTIFACT_STAGING.md),
preloading the matching heartbeat pack and gameplay module before activation.
Never install to the canonical EOU VHD or introduce active floppy I/O during play.
Run `python3 apps/daggorath/test_bag.py` in addition to the complete existing suite.
[Gameplay M3](../../docs/apps/DAGGORATH_GAMEPLAY_M3.md) records dependencies,
original-cartridge comparisons, live acceptance and memory limits.

## Gameplay M4 floor objects

Original GET/DROP now operates on real OCB owner/location state:
`P R SW`, `D R`, `G L W SW`, `S L`, `P R SW`, `D R` demonstrates a round trip
using the starting sword. Light the torch first (`P L T`, `USE LEFT`) to see
floor objects. GET does not require visibility; it requires an empty selected
hand and a matching unowned object in the current cell. It does not auto-stow.

Build with `python3 apps/daggorath/build_gameplay.py --out MCP/work/gameplay-m4/build`;
use the same disposable artifact workflow and resident preload sequence.
Run `python3 apps/daggorath/test_floor.py` plus all existing suites.
See [Gameplay M4](../../docs/apps/DAGGORATH_GAMEPLAY_M4.md) for source semantics,
cartridge comparisons, live results, memory and I/O boundaries.

## Gameplay M5: EXAMINE

Original room/backpack examination and LOOK display restoration are implemented.
EXAMINE does not parse a separate BAG/hand/object qualifier; trailing words are
ignored as in the original handler. Inventory remains authoritative packed state.
See [M5 results and live acceptance](../../docs/apps/DAGGORATH_GAMEPLAY_M5.md).

```sh
python3 apps/daggorath/test_examine.py
```
