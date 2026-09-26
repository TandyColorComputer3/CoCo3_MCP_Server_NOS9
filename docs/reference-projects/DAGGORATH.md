# Dungeons of Daggorath: source map and Wizard Milestone 1

[Navigation](README.md). Inspected source commit **`a94326f00ebb16a106b540c58bc2ccf5f7b66dac`**. This is a port investigation only; nothing was assembled, run, copied into the MCP implementation or installed in EOU.

## Provenance and fidelity boundary

[DAGGORATH.ASM](/Volumes/SEDONA/Projects/daggorath-reference/DAGGORATH.ASM) includes the recovered source units and declares `pragma nodollarlocal,6809`. Its notes explain reconstruction from assembler listings and recovered macros; [missing-macros.asm](/Volumes/SEDONA/Projects/daggorath-reference/missing-macros.asm) identifies non-original macro definitions. The repository includes an original-listing PDF and [grant_of_license.png](/Volumes/SEDONA/Projects/daggorath-reference/grant_of_license.png); the latter is a 2002 grant naming Michael J. Spencer Jr. from Douglas J. Morgan. Preserve this evidence and the original copyright notices; this pass does not assert a blanket modern open-source license or distribution clearance. The scan was not independently checked line-by-line against all reconstructed source.

This is 6809 source, not a 6309-optimized implementation. Preserve its arithmetic and timing intent before considering CPU-specific optimizations. An HD6309 host does not make its hard-coded interrupt frames OS-9-compatible.

## Program map

| Responsibility | Source and entry points |
|---|---|
| Composition | `DAGGORATH.ASM` includes definitions, initialization, private runtime/SWI, display/text/math, world/gameplay, sound, tables and vectors in that order |
| Entry/init | [ONCE.ASM](/Volumes/SEDONA/Projects/daggorath-reference/ONCE.ASM): ORG `$C000`, ONCE/DEMO → COMINI → DEMO10; GAME uses GAME10. COMINI pushes the selected continuation before returning from initialization. |
| Private runtime | [COMMON.ASM](/Volumes/SEDONA/Projects/daggorath-reference/COMMON.ASM): scheduler/TCB queues, SAM setup, CLOCK IRQ, time counters, keyboard buffers, cassette helpers |
| Service dispatch | [COMSWI.ASM](/Volumes/SEDONA/Projects/daggorath-reference/COMSWI.ASM): SWISER dispatches SWI+inline service byte; SW2SER redirects SWI2 through BASIC's table. SWITAB offsets are stored near the end of SOUNDS.ASM. |
| Display interpreter | [VCTLST.ASM](/Volumes/SEDONA/Projects/daggorath-reference/VCTLST.ASM): VCTLSX, VCTABS, VCTREL, VCTNEW/JMP/JSR/RTS, scaling and VECTOR calls |
| Rasterizer | [VECTOR.ASM](/Volumes/SEDONA/Projects/daggorath-reference/VECTOR.ASM): VECTOR digital differential analyzer, DIVIDE, INCRE, NEGD, BITMSK |
| Clear/text | [CLEAR.ASM](/Volumes/SEDONA/Projects/daggorath-reference/CLEAR.ASM), [TXTSER.ASM](/Volumes/SEDONA/Projects/daggorath-reference/TXTSER.ASM), [COMTXT.ASM](/Volumes/SEDONA/Projects/daggorath-reference/COMTXT.ASM), [EXPAND.ASM](/Volumes/SEDONA/Projects/daggorath-reference/EXPAND.ASM), [SWCHAR.ASM](/Volumes/SEDONA/Projects/daggorath-reference/SWCHAR.ASM) |
| Wizard behavior | [MISC.ASM](/Volumes/SEDONA/Projects/daggorath-reference/MISC.ASM): WIZIX/WIZIX0, WIZI10/WIZI20, WIZOX/WIZO10, WIZZES, WAITX |
| Wizard data | [D4.ASM](/Volumes/SEDONA/Projects/daggorath-reference/D4.ASM): WIZ1 crescent decorations → V$JMP WIZ0 body. WIZ2 star variant is not used by DEMO10. |
| World/view | VIEWER/VARC/VERT, DGNGEN/NEWLVL/MAPPER, DTABAS/D3/D4/VOBJ; separate from minimal intro |
| Input/gameplay | HUMAN/PARSER/TOKEN, COMPLR, PATTK/PCLIMB/PEXAM/PGET/PINCAN/PLOOK/PREVEA/PTURN/PUSE/PZTAPE; not required as complete modules for the intro |
| Initial state/layout | [CD.ASM](/Volumes/SEDONA/Projects/daggorath-reference/CD.ASM), [COMDAT.ASM](/Volumes/SEDONA/Projects/daggorath-reference/COMDAT.ASM): globals, descriptors, screen addresses, initial centroid and interrupt-vector installation |

Do not call the original COMINI inside OS-9: it installs its own stack, programs PIAs/SAM, clears `$0200..$3FFF`, establishes DP=$02, creates its own tasks and overwrites RAM interrupt vectors.

## Exact intro sequence

In ONCE.ASM `DEMO10`:

1. Set AUTFLG and call IRQSYN (original hardware IRQ synchronization).
2. Set X to **WIZ1**, set FADFLG and invoke **WIZIN0**, mapped to **WIZIX0**. This secondary entry preserves the copyright/status line established by COMINI; WIZIX would also clear it and disable heartbeat.
3. Output two packed messages through OUTSTI: “I DARE YE ENTER...” and “...THE DUNGEONS OF DAGGORATH!!!”. Source comments vary in a few spellings, so use the packed data/decoder as the fidelity reference rather than copying every comment literally.
4. Invoke WAIT twice, then WIZOUT, blank the alternate graphics area, flag UPDATE and synchronize.
5. Original code continues into GAME20/autoplay dungeon creation. **The milestone ends before this transition.**

WIZIX0 clears the primary text area and sets VXSCAL/VYSCAL to `$80/$80` (unity under the radix-7 arithmetic). Fade-in draws with B=32,30,…,0. WIZZES copies B to VCTFAD/NOISEV, clears the alternate vector region, processes the display list, requests a flip and synchronizes. VECTOR increments VCTFAD internally and plots one dot per resulting fade interval; this is **spatial dotted fading**, not simply palette brightness. WIZI20 plays A$EXP1. Fade-out first plays the explosion and then draws B=0,2,…,30. Faithful rendering should preserve these rules, including raster rounding/clipping.

WAITX's header says 1.5 seconds, but implementation loads **81** and executes one SYNC per iteration; its inline comment says 1.35 seconds. COMMON.ASM describes the IRQ as 60 Hz. Use the 81-tick count as implementation evidence, not the inconsistent heading. Cycle-loop sound and render time also contribute to total presentation time; exact wall-clock playback has not been measured here.

## Coordinates, rasterization and original screen layout

VCTLST documents **y,x byte pairs**, with control codes V$RTS=$FA through V$NEW=$FF. V$REL encodes two signed nibble deltas, each multiplied by two; zero returns to absolute mode. The reconstructed SVORG/SVECT macros use misleading SVX/SVY variable names relative to their emitted y,x order—follow bytes and VABS20, not macro-variable names. WIZ1 contains a pointer to WIZ0, which requires relocation or conversion to an offset in an OS-9 module.

The interpreter scales relative to centroid **(x=128,y=76)** from COMDAT, using radix-7 factors. VECTOR takes 16-bit endpoints, chooses max(abs(dx),abs(dy)) as length, computes fractional increments and maintains three-byte accumulators with half-unit initial rounding. It clips through X high-byte and display-base/end checks, maps a pixel into a **32-byte scanline**, and sets/clears a bit using BITMSK and VDGINV. A generic OS-9 line primitive may choose different endpoint/rounding rules and will not automatically reproduce its fading.

| Original region | Verified source definition |
|---|---|
| Whole logical screen | 256×192 at one bit/pixel: 32×192 = **6,144 bytes** (derived from stride and COMDAT region endpoints) |
| Two screen starts | CD: D0$BAS=$1000, D1$BAS=$2800; full 6,144-byte intervals end at $2800/$4000 |
| Vector/main area | D0.LEN/D1.LEN = 32×19×8 = **4,864 bytes**, i.e. rows 0–151 |
| Status region | COMDAT STSVDB rows 152–159 |
| Primary message region | COMDAT PRIVDB rows 160–191 |
| RAM globals | CD ORG $0200, followed by buffers/world/TCB/object storage below stack top PDL=$1000 |
| Program origin | ONCE ORG $C000; no complete linked size/map was produced in this pass |

Preserving the **256×192 layout** includes original status/message areas; it does not mean enlarging the wizard's own 152-line drawing region or moving the centroid to y=96.

### Requested output geometry

Proposed unscaled mapping into a **verified full 640×200** OS-9 screen:

```text
host_x = logical_x + 192     logical_x: 0..255 → 192..447
host_y = logical_y +   4     logical_y: 0..191 →   4..195
```

This leaves x=0..191 and 448..639 (192 columns each), and four rows above/below. Use 16-bit output coordinates; never store x+192 back into an 8-bit original-coordinate field. Reserve side areas without designing inventory/status/automap or any other enhanced UI.

The geometry is the user's target, **not an observed live EOU mode in this pass**. Current upstream [level2/cmds/grfdrv.asm](/Volumes/SEDONA/Projects/nitros9-reference/level2/cmds/grfdrv.asm) contains L086A.25 640×200 and L086A.24 640×192 tables; [level2/coco3/modules/cowin.asm](/Volumes/SEDONA/Projects/nitros9-reference/level2/coco3/modules/cowin.asm) handles SS.ScSiz. Therefore query actual usable screen size after setup, and do not assume `SCREEN_TYPE=5` or a framed 80×25 application automatically provides the requested canvas. mvdraw's cell-aligned CWArea approach cannot itself express a y=4 pixel origin; use explicit pixel translation and appropriate clipping/presentation.

## Hardware, ROM and multitasking boundaries

These addresses/semantics are grounded in CD/ONCE/COMMON/SOUNDS source definitions and comments; no separate manual verification is implied.

| Original dependency | Intro relevance | OS-9 boundary required |
|---|---|---|
| PIA$0=$FF00, PIA$1=$FF20 and register offsets | Startup, keyboard strobe, DAC and interrupt configuration | Do not touch from the application; use installed input/audio services |
| SAM loop at `$FFC0` through addresses below `$FFD4` | Select original display mode/page in SAM/CLOCK | OS-9 owns display/device state; use graphics/window API |
| RAM vectors V$SWI2=$0103, V$SWI=$0106, NMI=$0109, IRQ=$010C | RAMDAT copies JMP stubs to SW2SER/SWISER/CLOCK | Do not install private vectors; replace calls with ordinary module routines and OS-9 services |
| ORCC/ANDCC, SYNC/CWAI, own stack and DP | Initialization, service frames, pacing and flip | Use process-owned data/stack, proper ABI and cooperative wait/signal handling |
| SWI2 → BASIC dispatch `$A000`; POLCAT there | Normal keyboard scan; autoplay directly strobes PIA and rewrites saved PC to GAME | Use SCF input/readiness and an application cancellation path; never reuse SWI2 BASIC dispatch under OS-9 |
| BASIC cassette calls and `$A027` restart | Whole-game save/load, not wizard core | Exclude from milestone; later use RBF file operations rather than ROM cassette routines |
| Direct screen writes and dual-screen offset | VECTOR/CLEAR/TXTDPB; text deposits to both buffers | Use private framebuffer(s) and a verified OS-9 upload/display path |
| CPU-cycle audio loops/DAC output | Fade buzz and explosion | Translate to an OS-managed sound operation or explicitly scoped audio backend; no private IRQ audio |

### Self-modifying code and stack tricks

No instruction-stream self-modification was identified in the traced wizard renderer. A limited scan of direct store/inc/dec/clear/com operands against code labels also found no direct code-label stores; this is **not** proof about every indirect write in the game. Proven dynamic behavior includes installing executable JMP stubs in RAM, SWISER/SW2SER updating saved return PCs/register slots, CLOCK rewriting its return PC on autoplay keypress, and subroutines consuming inline parameter bytes via stack return addresses. `SKIP2` opcode tricks and FCB prefix tricks are static instruction encodings, not by themselves self-modification.

These conventions assume original 6809 frames and shared globals. Rewrite their calling boundaries explicitly; do not retain fixed `10,S`/`12,S` frame offsets inside a 6309 OS-9 application merely because the algorithm works on a cartridge.

## Intro sound dependencies

COMMON `CLK20` toggles NOISEV and writes scaled values to the PIA1 DAC for the opening buzz; its comment describes 30 Hz under the original 60 Hz IRQ. MISC `WIZI20` selects **A$EXP1** → SOUNDS `SOUNDI/SOUNDX` → **KABOOM**. KABOOM uses **THUDD** (stored in SWCHAR.ASM), BOOMER/BOOM1/BOOM2, SNSUB3/SNSUB2, SNOISE, SNOUT and SNWT1K/SNWAIT. SNOUT writes `$FF20`; SNWAIT is a cycle-counted loop. Thus the visible intro is not originally silent, and wholesale copying these routines would monopolize hardware/timing.

The milestone should include an explicit sound adapter and preserve these event points. Bit-accurate sound is not demonstrated by a generic beep. If the first rendering probe is silent, label it a probe rather than claiming a faithful complete intro. Consult [sound sources](../source-index/SOUND.md) and installed driver capabilities before choosing playback encoding; no suitable full-fidelity EOU audio transport was validated here.

## Rendering strategies

| Strategy | Benefit | Fidelity/performance limits | Compatibility requirement |
|---|---|---|---|
| A. Decode vectors → GFX2/cgfx drawing commands | Small guest framebuffer requirement; OS-managed drawing; mvdraw demonstrates drawing/clip primitives | Native line rasterization can differ from VECTOR, especially sparse fade dots; many per-pixel commands could be slow; drawing is not automatically atomic | Verify chosen C/assembly interface and EOU service support; BASIC09 GFX2 is not a mandatory dependency for C/assembly |
| B. Software 256×192 1-bpp framebuffer → 640×200 presentation | Preserves 32-byte stride, original rasterizer, fonts, clipping and sparse fade; original coordinates remain intact | 6,144 bytes per logical buffer (12,288 for two), plus module/stack/data; transferring a frame still needs an OS-9 transport | Keep buffers process-owned; don't write assumed physical screen memory. At 1 bpp a full 640×200 destination is 16,000 bytes, a derived budget, not an allocated buffer claim. |
| C. OS-9 GET/PUT/GPLoad buffering | Cache/upload regions or wizard frames; reduce visible redraw; upstream GrfDrv has GetBlk/GPLoad/PutBlk paths | Needs verified buffer headers, color depth, group/id ownership and cleanup; size/transfer overhead; GET captures pixels, not source vectors | Query/test on frozen EOU; no blanket promise of tear-free/vblank-synchronized PUT |
| D. Modern application-screen services | Current upstream Co3HiRes offers allocated/mapped/show/freed application screens | Promising modern backend, but requires matching VTIO/CoWin/provider ABI; not proven resident in EOU | Do not upgrade frozen EOU to obtain it. See [modernization notes](../source-index/MODERNIZATION_NOTES.md). |

**Recommended first implementation direction:** B for logical rendering, with a small validated C-style GPLoad/PutBlk presentation adapter if EOU supports the necessary upload path. This is a proposed B+C combination, not a tested API recipe. Start by probing allocation/upload/placement/cleanup and measuring latency. A is useful as a simpler geometry comparison but is not automatically a pixel-faithful fade implementation. Do not make direct GIME/MMU writes the fallback when a graphics service remains unverified.

## Concrete proposed Wizard Milestone 1

```text
original WIZ1/WIZ0 + intro text/font + fade/raster algorithms
    ↓ adapt labels, pointers, data allocation and calling conventions
minimal OS-9 program
    ↓ open/select graphics context; retain terminal/option state
640×200 graphics screen (query and verify dimensions)
    ↓ translate presentation only by (+192,+4)
centered 256×192 logical viewport
    ↓ fade in → messages/hold → fade out, with abort/sound adapters
wizard intro
    ↓ release graphics/audio resources, restore options and terminal
clean F$Exit
```

### Exact source/data dependency set

| Needed material | Original locations / boundaries | Reuse decision |
|---|---|---|
| Sequence specification | ONCE `DEMO10` through final ZFLOP/UPDATE/SYNC, **before GAME20**; copyright packed bytes in COMINI if retaining original status notice | Re-express as application state machine; exclude COMINI and world creation |
| Wizard vectors | D4 **WIZ1** crescent sublists and **WIZ0** body through its SVEND; no WIZ2 required | Preserve coordinates/data. Relocate V$JMP pointer or encode a relative table reference |
| Data encoding | missing-macros SVORG, SVECT, SVNEW, SVEND; CD V$ constants | Preserve emitted semantics; flag reconstructed macro provenance |
| List interpretation | VCTLST **VCTLSX**, VCTL10/VCTDIS, VCTABS/VABS00/10/20, VCTNEW/VCTEND, VCTREL, ASCALX/ASCALY/SCAL helpers, NEWOLD; VCTJMP for WIZ1→WIZ0 | Adapt private variables and calls. VCTJSR/VCTRTS can remain in a bounded general decoder but are not required by the selected WIZ1/WIZ0 path. SETFAX lighting is not needed for explicit intro fade values. |
| Rasterizer | VECTOR **VECTOR**, VECTxx, INCRE, DIVIDE, NEGD, BITMSK; COMTXT **LSLD5** chain; PATTK **ASRD7/ASRD3** shared shift chain | Preserve arithmetic/plot semantics in a private 32-byte-stride buffer; take helpers only, not attack gameplay |
| Fade/controller | MISC **WIZIX0, WIZI10, WIZI20, WIZOX/WIZO10, WIZZES, WAITX** | Preserve progression/event points; replace SWI, UPDATE, SYNC and delay mechanisms |
| Initial renderer data | COMDAT centroid 128/76, selected DSP/status/primary descriptor semantics; CD VXSCAL/VYSCAL, VCTFAD/FADCNT, endpoints/accumulators, DRWFLG/TX/TY and scratch values | Build fresh process data; do not copy RAMDAT's vector or absolute-address writes |
| Clear | CLEAR **CLEAR**, ZFLOPX/CLRPRX/CLRSTX semantics | Rewrite as bounded private buffer clears; retain vector/status/text separation |
| Exact text/font | ONCE packed intro/status strings; TXTSER **TXTSTI/TXTSTR/TXTCHR**; EXPAND **EXPANX/EXPA00/GETFIV/FIVDSP** and CHARxx helpers; COMTXT **TXTXXX/TXTDPB/TXTSCR** and control handlers; SWCHAR **SWCTAB/SPCTAB** | Reuse data/decoding/font shapes if preserving original appearance. Replace SWI-frame returns, absolute bases and `G6.LEN` second-buffer addressing. Token/game parser not needed. |
| Sound reference | MISC WIZI20; COMMON CLK20 buzz; SOUNDS **KABOOM/BOOMER/SNSUB3/SNSUB2/SNOISE/SNOUT/SNWT1K/SNWAIT**; SWCHAR **THUDD** | Translate into a verified OS-managed adapter. Do not invoke original DAC/IRQ loops. |
| New OS-9 shell | Module entry, private memory, graphics lifecycle, I/O completion, cooperative ticks, cancellation, cleanup and **F$Exit** | New code required later; none implemented in this pass |

World generation, player/creature queues, inventory, combat, cassette I/O, heartbeat gameplay and autoplay continuation are outside this milestone. The task is not a full port.

### Acceptance gates for later implementation

1. Build identifies target/compiler/defs and module identity; logical vectors remain byte-/coordinate-traceable to this revision.
2. A graphics probe proves 640×200 usable bounds and displays logical corner markers at (192,4) and (447,195), with no coordinate scaling or wrap.
3. Wizard crescent/body, original message/status layout and fade sequence render correctly. Compare logical framebuffer snapshots independently from output-driver timing.
4. Pacing yields to OS-9; input can cancel at every stage; sound event behavior is explicitly verified or reported incomplete.
5. Normal completion, abort and injected setup failure all restore a healthy terminal and return an explicit status through F$Exit. Merely exiting without restoring terminal state is insufficient.
6. Use separately authorized disposable test media; do not mutate frozen EOU media or assume a save-state restore undoes disk writes.

## Knowledge-layer connections

Use [graphics/windowing](../source-index/GRAPHICS_WINDOWING.md) for `/dd/SOURCECODE/ASM/VEFIO-WINFO/vefio.asm`, GLIB and GFX2 examples; [processes/signals](../source-index/PROCESSES_SIGNALS.md) for cooperative waits/abort; [memory/MMU](../source-index/MEMORY_MMU.md) for logical/physical separation; [modules](../source-index/MODULES.md) for module structure; [sound](../source-index/SOUND.md) for driver/client patterns. Current upstream CoWin/GrfDrv and Co3HiRes are modern implementation references, while [EOU runtime fingerprints](../source-index/RUNTIME_MATCHES.md) and [crosswalk](../source-index/EOU_UPSTREAM_CROSSWALK.md) limit compatibility claims. The historical graphics examples and modern private driver internals are not automatic application ABI authority.
