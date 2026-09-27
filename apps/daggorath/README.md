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
