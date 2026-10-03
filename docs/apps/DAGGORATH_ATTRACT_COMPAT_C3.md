# Daggorath Attract Compatibility C3

**Status: Wizard-through-PREPARE compatibility sequence ready for human
review; separate review executable; stops at PREPARE.**

C3 is the first continuous Architecture-B choreography. It executes the
original-derived Wizard vector path, the accepted C2 text path, both source
waits, the Wizard fade and the first `PREPARE!` presentation. It does not run
maze generation, heartbeat loading, scheduler startup, audio IPC, map
construction or AUTTAB.

The read-only source baseline is
`/Volumes/SEDONA/Projects/daggorath-reference` at commit
`a94326f00ebb16a106b540c58bc2ccf5f7b66dac`. C2 was checkpointed as
`d5ce41a` (`feat: add cartridge-compatible Daggorath text presentation`).

## Original call graph and ported closure

The source path is:

```text
ONCE:DEMO10
  -> MISC:WIZIN0/WIZI10/WIZZES
       -> CLEAR:ZFLOPX
       -> D4:WIZ1, including its V$JMP to WIZ0
       -> VCTLST/VECTOR
       -> UPDATE/SYNC
  -> ONCE:OUTSTI welcome part I
  -> ONCE:OUTSTI welcome part II
  -> MISC:WAITX (81 IRQ jiffies)
  -> MISC:WAITX (81 IRQ jiffies)
  -> MISC:WIZOUT/WIZOX/WIZO10/WIZZES
  -> CLEAR:ZFLOPX, UPDATE, SYNC
  -> ONCE:GAME20 -> MISC:PREPAX
```

[`import_compat_wizard.py`](../../apps/daggorath/import_compat_wizard.py)
imports the 84 ordered `WIZ1`/`WIZ0` vector segments and the packed status and
welcome data. Checked-in generated data makes ordinary builds independent of
the external source checkout. [`wizard-render.asm`](../../apps/daggorath/src/compat/wizard-render.asm)
ports the source DDA vector leaf to position-independent 6809 code. It uses the
same 3,333-byte, 256-byte-aligned compatibility image proven by C1, preserves
the caller's `CC`, `DP`, `X`, `Y` and `U`, and receives the source-domain
`256 x 192 x 1 bpp` frame explicitly.

The implementation does not store or reveal cached Wizard rasters. For every
source fade value it clears the viewport, runs all source vectors in source
order, presents the result and observes the `WIZZES` visibility boundary.
PCR-relative vector and bit-mask tables replace original absolute addresses.

## One-buffer adaptation

The original `ZFLOPX`/flip arrangement makes an alternate logical screen the
next visible screen. C3 needs one caller-owned logical frame because no source
operation observes both buffers simultaneously in this bounded path. Each
`WIZZES` event constructs the complete next visible viewport in that frame,
while the copyright/status row remains untouched. The live output's complete
6,144-byte logical states match the source-derived oracle, including clear and
persistence boundaries, so C3 has no semantic need for a second logical frame
or DAT slot.

The C2 glyph/deposit implementation is reused unchanged. A small regional-cell
entry exposes the same exact glyph operation to full-screen source cells:
copyright occupies row 19 (logical y=152), and `PREPAX` writes `PREPARE!` at
cell `(12,9)`. Welcome text still passes through C2 character semantics one
character at a time; it is not a precomposed bitmap.

## Exact framebuffer oracles

The generated manifest records the source commit, all 84 source locations and
SHA-256 values for seven complete 6,144-byte logical frames. The byte-verified
target module was observed in a continuous live EOU run. Thresholding its
logical display area produced these results:

| Boundary | Live frame / elapsed | Differing logical bytes |
| --- | ---: | ---: |
| first visible strokes, fade 32 | 17 / 2.091 s | **0 / 6,144** |
| emerging Wizard, fade 16 | 42 / 5.075 s | **0 / 6,144** |
| complete Wizard, fade 0 | 68 / 8.248 s | **0 / 6,144** |
| complete two-line welcome | 77 / 9.335 s | **0 / 6,144** |
| representative fade-out, fade 14 | 124 / 15.073 s | **0 / 6,144** |
| near-black viewport, copyright retained | 153 / 18.461 s | **0 / 6,144** |
| `PREPARE!`, copyright retained | 154 / 18.591 s | **0 / 6,144** |

The six intervening stable Wizard/welcome states and the successive fade
states show that formation order is visible rather than reduced to a final
image. The complete welcome first changes at 8.367 s and reaches its exact
oracle at 9.335 s, a 0.968-second visible formation interval in this observer
run. The target then preserves the complete welcome through 11.937 s.

## Timing

The cartridge figures are from the committed completed-frame trace and the
more precise Wizard flag trace. C3 elapsed time is normalized to its first
exact visible-stroke frame. The MCP image observer samples at roughly 8.5 Hz,
so sub-sample differences are reported as observer bounds. The two `WAITX`
calls are independently asserted as exactly 81 target sleep ticks each.

| Boundary | Cartridge ticks / ms | C3 ticks / ms | Delta | Status |
| --- | ---: | ---: | ---: | --- |
| first stroke -> emerging reference frame | approx. 144 / 2,399 | approx. 179 / 2,984 | +585 ms | sampled reference frames; C3 full-frame adapter is slower |
| emerging -> complete | approx. 149 / 2,490 | approx. 190 / 3,173 | +683 ms | same formation order and exact endpoint; adapter cost remains visible |
| complete -> welcome complete | 84 / 1,402 | approx. 65 / 1,087 | -315 ms | exact source text order; no completed-bitmap substitution |
| visible welcome formation | bounded by the 84-tick interval | approx. 58 / 968 | inside source interval | C2 per-character path observed continuously |
| `WAITX` #1 | **81 / nominal 1,350** | **81 / nominal 1,350** | 0 ticks | exact source wait |
| `WAITX` #2 | **81 / nominal 1,350** | **81 / nominal 1,350** | 0 ticks | exact source wait |
| welcome complete -> clear | 162 / 2,704 | observed 164 / 2,736 | +32 ms | within observer boundary uncertainty |
| welcome clear -> final blank, including second explosion boundary and fade | approx. 373 / 6,224 | approx. 383 / 6,390 | +166 ms | same ordered fade states; sound backend deferred |
| final fade -> near-black | source `ZFLOP/UPDATE/SYNC`, about 2 ticks | one presented boundary | not separately resolved | exact near-black raster established |
| near-black -> PREPARE | approx. 3 / 50 | observed <=8 / 130 | <=+80 ms | one observer interval; no EOU setup or multi-second gap |

The critical product result is the last row: the only work between the final
blank presentation and PREPARE is the source-equivalent regional clear/cell
write and CoWin presentation. No modern game initialization occurs there.

## Performance attribution

The adapter measurements use the same production leaves already isolated by
C1 and C2:

| Operation | Target cost / evidence |
| --- | --- |
| Wizard PIC vector execution | approximately 10 sampled ticks per representative fade frame, inferred after subtracting the separately measured presentation and one source visibility tick from the continuous intervals |
| 256-to-512 compatibility expansion | 8-10 ticks for six strips, C1 isolated target measurement |
| full CoWin transfer | 4 ticks for six puts, C1 isolated target measurement |
| one welcome text cell presentation | 0 conversion ticks + 1 transfer tick, 2 ticks including the surrounding clock quantum, C2 isolated target measurement |
| fade presentation | one full conversion/transfer, normally 12-14 ticks, plus the explicit one-jiffy visibility boundary |
| PREPARE rendering/presentation | eight shared C2 cell deposits followed by one full conversion/transfer; observed near-black-to-complete-PREPARE bound 130 ms |

Module/floppy load and review-program startup are excluded. The full-frame
Wizard adapter is slower than the cartridge at several formation boundaries,
but it retains every original visible state and remains a bounded future dirty-
region optimization problem. No arbitrary delay compensates for the cost.

## Copyright, clear and audio findings

The inverse `COPYRIGHT  DYNA MICRO  MCMLXXXII` row persists through formation,
welcome, primary clear, fade, near-black and PREPARE. Only the viewport is
cleared by the final bounded `ZFLOP` adaptation. This matches the corrected
cartridge evidence; the map is the later operation that replaces the display.

The source semantic audio boundaries are now explicit:

1. `COMMON:CLK20` Wizard buzz begins before/with fade-in;
2. `MISC:WIZI20` invokes `A$EXP1` after formation;
3. `MISC:WIZOX` invokes a second `A$EXP1`, then fade buzz;
4. buzz stops after the last fade step.

C3 records those four events at their source locations but intentionally uses
a no-op adapter. Credited asynchronous integration and S/SC timbre remain
future work; no synchronous audio wait is inserted into visual timing.

## Module and Level II budget

| Module | Size | CRC | SHA-256 | Data request |
| --- | ---: | --- | --- | ---: |
| `dodcopen` | 5,344 (`$14E0`) | `8C2A08` | `1113ce775568223ee85570d089b676d20d6452777a614f0d81018227d0886b4c` | 11,324 (`$2C3C`) |

ToolShed identifies it as a valid `$11/$80` reentrant/read-only 6809 program.
It uses one program block, two process-data blocks and one CoWin mapping:

```text
1 program + 2 data + 1 CoWin = 4 / 8 DAT blocks
```

The process data contains 3,333 bytes of aligned compatibility state, one
6,144-byte logical framebuffer, 1,536 bytes of added stack, and the remaining
311 bytes of globals/alignment. It leaves 5,060 bytes below the 16-KiB/two-data-
block boundary. Presenter scratch is the existing 2,048-byte mapped CoWin
buffer and consumes the one CoWin slot rather than process data. The selected
Architecture-B ceiling of two program blocks, two data blocks and one CoWin
mapping therefore holds with one program block unused.

## Review material

Each pair is **original cartridge on the left, live C3 EOU on the right**.
Capture borders and RGB/monochrome phosphor appearance are platform/capture
differences; logical one-bit acceptance uses the complete frame comparison
above.

- [continuous C3 opening clip](assets/daggorath-attract-compat-c3/c3-opening.mp4)
- [selected original cartridge beats](assets/daggorath-attract-compat-c3/original-selected-beats.mp4)
- [selected synchronized original/C3 review](assets/daggorath-attract-compat-c3/original-eou-selected-review.mp4)
- [first strokes](assets/daggorath-attract-compat-c3/pairs/00-first-strokes-original-eou.png)
- [emerging Wizard](assets/daggorath-attract-compat-c3/pairs/01-emerging-original-eou.png)
- [complete Wizard](assets/daggorath-attract-compat-c3/pairs/02-complete-original-eou.png)
- [welcome](assets/daggorath-attract-compat-c3/pairs/03-welcome-original-eou.png)
- [welcome cleared](assets/daggorath-attract-compat-c3/pairs/04-welcome-cleared-original-eou.png)
- [fade](assets/daggorath-attract-compat-c3/pairs/05-fade-original-eou.png)
- [near-black](assets/daggorath-attract-compat-c3/pairs/06-near-black-original-eou.png)
- [PREPARE](assets/daggorath-attract-compat-c3/pairs/07-prepare-original-eou.png)
- [selection and zero-difference ledger](assets/daggorath-attract-compat-c3/selection.tsv)

The clean review executable was extracted from the disposable disk and matched
the host artifact SHA-256 before launch. The continuous capture returned to
Term after its three-second post-PREPARE review hold. A concurrent MCP command-
completion request timed out after display ownership returned, so this run is
accepted for frame/state evidence rather than used as an exit-status oracle.
The original video in the two compact review movies is deliberately a sequence
of the committed cartridge frames rather than a fabricated continuous clip;
the 190-second source-ROM AVI was disposable and is not retained in the
repository. Its frame times and hashes remain in the cartridge storyboard
manifest.

## C4 boundary (not implemented)

C4 begins after the accepted PREPARE frame. Its source closure is
`GAME20:NEWLVL` (level/maze/creature initialization), the `DEMDAT` object loop
through `OBIRTH/OCBFIL`, `GAME40:MAPPER`, two `WAITX` calls, the map-to-black
`ZFLOP/UPDATE/SYNC` sequence, and `GAME50:INIVUX/PROMPX` for the initial dungeon
and lone-dot transcript.

C4 can reuse the C1 frame/presenter, C2 text/clear path and C3 source-tick
controller. The already proven deterministic RNG/maze work is a semantic
oracle, but it is not silently imported into the compatibility executable.
Map construction, initialization ownership and their original framebuffer
oracles remain to be ported and reviewed.

The accumulated evidence supports eventual sharing of original-derived vector
rasterization, glyph/text, regional clear and timing primitives with human
play. Maze/RNG sharing remains an architectural question for C4 and later; C3
does not migrate or replace the modern human-game engine.
