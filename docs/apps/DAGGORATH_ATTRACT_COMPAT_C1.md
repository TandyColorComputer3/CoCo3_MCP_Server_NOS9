# Daggorath Attract Compatibility C1

**Status: ready for human review, not connected to the public attract lifecycle.**

C1 turns the accepted Stone Giant P1 experiment into an isolated, reusable
compatibility subsystem.  The original-derived renderer still works in the
cartridge domain, `256 × 192 × 1 bpp` (6,144 bytes).  Only the EOU adapter
expands that frame to the `512 × 192` monochrome image placed in the existing
type-5 CoWin.  The modern NitrOS-9 game remains unchanged and remains the human
play engine.

The source baseline is the recovered read-only Daggorath tree at commit
`a94326f00ebb16a106b540c58bc2ccf5f7b66dac`.  Fixture identity and pixel facts
are retained in
[`test/fixtures/compat-c1/manifest.json`](../../apps/daggorath/test/fixtures/compat-c1/manifest.json).
The repository rollback baseline for this work is `d910ee4`.

## Boundary and ownership

```text
cartridge state fixture
  -> caller-owned, 256-byte-aligned compatibility state
  -> original-derived PIC Giant renderer
  -> caller-owned 256x192 logical framebuffer
  -> stock-6809 compat expander
  -> one mapped 512x32 CoWin strip
  -> six type-5 CoWin puts
```

The PIC renderer owns no SAM, PIA, GIME, interrupt, cartridge video address, or
OS-9 path.  It receives a three-field context: aligned state pointer, logical
frame pointer, and the source presentation mode.  It saves/restores `CC`, `DP`,
`X`, `Y`, and `U`, temporarily points `DP` at the aligned compatibility state,
and returns an OS-9-style byte status.  Constants, vector data, and tables are
PCR-relative.  The presenter owns the CoWin path and one 2,048-byte
`512 × 32` mapped buffer; it maps once, converts and transfers six strips, then
unmaps and closes during fixture cleanup.

There is deliberately no modern-renderer fallback.  A PIC error fails the
fixture.

## Compatibility state ABI

The current bounded state represents original `$0200..$0F04` as compatibility
offsets `$0000..$0D04`, exactly 3,333 bytes.  The base must be 256-byte aligned
so the original direct-page low byte remains valid.  This is a stable offset
contract for future source-derived routines; C1 does not claim that every byte
in the range has already been assigned a semantic owner.

| Original address/range | Compat offset | Source symbol/use | Owner and access in C1 |
| --- | ---: | --- | --- |
| `$0205/$0207` | `$0005/$0007` | `VCNTRX/VCNTRY` | compatibility core; renderer reads |
| `$020B` | `$000B` | original `FLOP`; adapted logical-frame pointer | adapter initializes; renderer reads |
| `$0223` | `$0023` | `PDIR` | fixture/core state; renderer reads |
| `$022C..$0254` | `$002C..$0054` | fade/vector endpoints, DDA work, scales | renderer mutable scratch |
| `$026E/$026F` | `$006E/$006F` | `RLIGHT/MLIGHT` | fixture/core state; renderer reads |
| `$0273/$0274` | `$0073/$0074` | `HLFSTP/BAKSTP` | fixture/core state; renderer reads |
| `$0275` | `$0075` | `MAGFLG` | renderer mutable source state |
| `$027C` | `$007C` | `DROW` | fixture/core state; renderer reads |
| `$028A/$028B` | `$008A/$008B` | `DIR/RANGE` | renderer/source state |
| `$02C1..$02C3` | `$00C1..$00C3` | division temporaries | renderer mutable scratch |
| `$03D4..$05F3` | `$01D4..$03F3` | packed 17-byte CCB array | compatibility core owns; renderer reads |
| `$09F4...` | `$07F4...` | `NEIBOR` | fixture/core state; right-peek path reads |

The logical framebuffer is separate caller-owned storage.  Physical expansion
never changes this state or the 6,144-byte logical frame.

## Original-derived PIC closure

[`giant-render.asm`](../../apps/daggorath/src/compat/giant-render.asm) contains
only the proven closure needed by beats 019–021:

- `VIEWER`: `SETSCL`, relevant `PDRAW`, `CMRDRW`, and `DRAWIT` behavior;
- `COMCRE`: `CFIND` over packed CCBs;
- `CRETUR`: source `STEP` position arithmetic used by `PDRAW`;
- `VCTLST`: the absolute, relative, `NEW`, and `END` commands used by the
  captured vector lists;
- `VECTOR`: original DDA scaling, division, increment, and raster behavior;
- `DTABAS/VARC/D3`: `FWDCRE`, `RPEEK`, `SGINT2`, and Stone Giant vectors.

Two P1 findings are now explicit invariants:

1. the direct `SETFAD` call boundary preserves `X`, matching the register
   preservation formerly supplied by the original dispatcher path;
2. the source's one-byte `HLFSCL` entry immediately precedes `BAKSCL`; that
   intentional layout/alias relationship is retained rather than normalized.

The module has no hard-coded module base and no direct hardware address.

## Immutable logical oracle

Each fixture starts with the captured cartridge state and the captured
pre-creature frame, then invokes the same production PIC entry.  The regression
checks every byte of the completed 6,144-byte frame.  Pixel contribution means
the XOR between the baseline frame and the oracle frame; it is not a bounding
box-only acceptance test.

| Beat | Mode | Differing bytes vs oracle | Giant pixels | Inclusive logical bounds |
| --- | --- | ---: | ---: | --- |
| 019 | right-side `PDRAW` | **0 / 6,144** | 4 | `(187,93)..(188,105)` |
| 020 | forward `VIEW30` | **0 / 6,144** | 118 | `(85,69)..(162,114)` |
| 021 | forward `VIEW30` | **0 / 6,144** | 251 | `(74,67)..(173,126)` |

The target fixture also runs the actual assembly expander over all six strips
and checks all 12,288 expanded bytes against the independent nibble-doubling
model before it opens the acceptance hold.

## Stock-6809 256-to-512 conversion

[`expand.asm`](../../apps/daggorath/src/compat/expand.asm) uses a 16-entry
nibble table.  For each source byte it looks up the high and low nibbles and
writes two destination bytes; each source bit therefore becomes two adjacent
physical bits.  Eight source bytes are expanded per loop iteration, amortizing
loop control without 6309 instructions.  The leaf is position independent,
uses the CMOC argument convention, and does not own the mapped buffer.

P1's byte-at-a-time C conversion took 25–26 ticks (about 417–433 ms).  Restored
EOU target measurements for the stock-6809 C1 path are:

| Beat | PIC render | Conversion | CoWin transfer | Total | Approx. total |
| --- | ---: | ---: | ---: | ---: | ---: |
| 019 | 0 ticks | 8 ticks | 4 ticks | 12 ticks | 200 ms |
| 020 | 4 ticks | 10 ticks | 4 ticks | 18 ticks | 300 ms |
| 021 | 4 ticks | 8 ticks | 4 ticks | 16 ticks | 267 ms |

Conversion is 2.5–3.25 times faster than P1.  Full-frame CoWin transfer is stable
at four sampled 60-Hz ticks in C1.  The loops do not mask interrupts.  C1 does
not migrate or duplicate heartbeat or audio ownership.

A 6309 path is not warranted yet: the mandatory 6809 path already fits the
observed semantic envelope, while a second implementation would add dispatch
and equivalence risk.

## Cartridge timing rule

For an original presentation event:

- `T_original_event` is its source/cartridge semantic presentation boundary;
- `T_eou_ready` is completion of PIC rendering and expansion;
- `T_eou_present` is completion of the CoWin transfer.

If EOU is ready early, the future compatibility scheduler retains the completed
frame until `T_original_event`.  If it is late, the adapter must be optimized;
an arbitrary compensating sleep is not an acceptable fix.  Exact tick counts
apply to explicit source waits.  Runtime-only boundaries initially allow one
60-Hz frame of capture/observer uncertainty, with wider tolerance requiring
new evidence.

The storyboard places the first Giant contribution near 57.5 s, the recognizable
Giant near 61.4 s, and the nearer redraw near 61.8 s.  The useful intervals are
therefore approximately 24 ticks before beat 019, 234 ticks from 019 to 020, and
24 ticks from 020 to 021.  C1's respective 12-, 18-, and 16-tick pipelines fit
inside those intervals.  C1 does not yet schedule these events in the public
attract loop; this comparison proves presenter feasibility, not lifecycle
integration.

## Full frame versus dirty presentation

The Giant-only changed bounds occupy two 32-row strips for each accepted beat,
so a dirty-strip implementation could reduce isolated conversion and put work.
That is not selected for C1.  Real attract transitions also change corridor,
text, heartbeat, and erase state, and an optimization based only on the Giant
XOR could collapse a source-visible completed frame.  The measured full-frame
path already meets the tightest 24-tick interval.  Dirty regions remain a
future optimization after the full presentation graph supplies authoritative
invalidation regions.

## Live physical acceptance

The current target screenshots include the emulator's two-line capture border.
After the common two-row crop, each target image has **zero differing RGB
pixels** against the compatibility half of its accepted P1 pair.

| Beat | Current target frame | Cartridge / compatibility pair |
| --- | --- | --- |
| 019 | [EOU target](assets/daggorath-attract-compat-c1/019-compat-eou-target.png) | [ORIGINAL \| COMPAT EOU](assets/daggorath-attract-compat-c1/019-original-compat.png) |
| 020 | [EOU target](assets/daggorath-attract-compat-c1/020-compat-eou-target.png) | [ORIGINAL \| COMPAT EOU](assets/daggorath-attract-compat-c1/020-original-compat.png) |
| 021 | [EOU target](assets/daggorath-attract-compat-c1/021-compat-eou-target.png) | [ORIGINAL \| COMPAT EOU](assets/daggorath-attract-compat-c1/021-original-compat.png) |

The [three-state progression](assets/daggorath-attract-compat-c1/giant-progression-019-021.png)
shows the same renderer across the sparse, recognizable, and nearer states.
Cartridge NTSC artifact colors versus EOU RGB monochrome are an intentional
platform difference; geometry and one-bit raster data are exact.

## Module and Level II budget

The standalone acceptance modules embed state, baseline frame, sparse oracle
XOR, target verifier, and a two-second review hold.  Their size is intentionally
larger than the reusable renderer itself.

| Module | Size | CRC | SHA-256 | Data request |
| --- | ---: | --- | --- | ---: |
| `dodc19` | 13,755 (`$35BB`) | `1D4341` | `16dad107ff9ed11ed03d180d4f02435120ec6ed0d95f8e0eecbf7d6c1569a6b1` | 13,359 (`$342F`) |
| `dodc20` | 14,061 (`$36ED`) | `AA81CF` | `d17ab79adb3e9bbeccedf21c351011cdaa91abc7925c91f5a8a154b8e653e730` | 13,359 (`$342F`) |
| `dodc21` | 14,358 (`$3816`) | `D7F4C7` | `fc542313d02b3ee0172649af767b268a5137e1417a549a9b532e896d916c2d97` | 13,359 (`$342F`) |

All are `$11/$80` reentrant, read-only OS-9 6809 program modules with good
ToolShed CRCs.  The largest maps as two program blocks.  The data request maps
as two blocks and leaves 3,025 bytes below 16 KiB.  One 2,048-byte CoWin mapping
uses one additional block:

```text
2 program + 2 process data + 1 CoWin = 5 / 8 DAT blocks
```

The largest acceptance program retains 2,026 bytes below the two-block ceiling.
Its embedded fixture rodata alone is 10,096 bytes at beat 021.  The reusable
PIC Giant closure is 903 bytes, and the stock-6809 expander is 184 bytes; the
presenter contributes 792 code bytes plus 62 bytes of tables/state before
shared OS-9/window wrappers.  Removing fixture and verifier data therefore
preserves the architecture audit's future two-program-block ceiling with ample
room for the next bounded source closure.

## Scope and next step

C1 does not change `/d1/daggorath`, the modern renderer, `dodcmd`, `dodsched`,
heartbeat, credited audio, Wizard, text, AUTTAB, or the attract scheduler.  It
adds no public mode selection and performs no sound event.

The recommended next migration is the **one-buffer compatibility presentation
foundation** around this proven raster core: define original clear/erase,
primary-text, and presentation-boundary semantics on the same 6,144-byte frame
before porting the Wizard or AUTTAB graph.  This follows the selected
Architecture B dependency order and avoids reintroducing a second geometric
model.
