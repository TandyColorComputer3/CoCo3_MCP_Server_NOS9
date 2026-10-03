# Daggorath Attract Compatibility C2

**Status: text/presentation foundation ready for human review; not connected to
the public attract lifecycle.**

C2 extends the committed C1 cartridge-layout renderer with the original
primary-text dependency closure, region-specific clears and bounded physical
presentation. The compatibility core still writes a caller-owned
`256 × 192 × 1 bpp` logical frame. CoWin remains the only physical display
owner and expands the source image horizontally to the existing monochrome
type-5 surface.

The read-only source baseline is
`/Volumes/SEDONA/Projects/daggorath-reference` at commit
`a94326f00ebb16a106b540c58bc2ccf5f7b66dac`. The exact source hashes, decoded
welcome strings, oracle cursors and logical-frame hashes are retained in
[`manifest.json`](../../apps/daggorath/test/fixtures/compat-c2/manifest.json).
C1 was checkpointed first as commit
`c4224d8a11bb8b2eb9249335aeaa159befd451d3`.

## Source closure and adaptations

| Source | Behavior retained in C2 |
| --- | --- |
| `TXTSER.ASM:TXTCHR/TXTSTR/TXTSTI` | Character dispatch and packed-string source order. Packed welcome data is expanded at build time; target rendering receives the original display codes. |
| `COMTXT.ASM:TXTXXX/TXTBS/TXTCR` | Printable deposit, cursor advance, wraparound backspace, and CR down-one-row/home-column semantics. |
| `COMTXT.ASM:TXTDPB` | Exact `SWCTAB`/`SPCTAB` glyph selection and seven top-to-bottom scanline writes; the eighth cell scanline is untouched. |
| `COMTXT.ASM:TXTSCR` | Rows 1–3 move up and the last 32×8 row clears using the current inverse byte; cursor resumes at cell 96. |
| `CLEAR.ASM:CLRPRX/CLRSTX/CLEAR` | Primary, status, viewport and whole-logical-screen region clears. Primary clear homes its cursor. |
| `CLEAR.ASM:ZFLOPX` | The same whole-screen operation can target whichever caller-owned frame represents the alternate screen. C2 does not invent a second physical owner. |
| `HUMAN.ASM:HMAN30/HMAN40`, `MISC.ASM:M$CURS/M$ERAS/PROMPX` | Character zero erases the old cursor, delete is backspace–space–backspace, underline cursor is write-then-backspace, and the ordinary prompt is CR then dot. |
| `COMDAT.ASM:TXTPRI` | Four 32-character rows at logical y=160, 168, 176 and 184. |
| `ONCE.ASM:DEMO10` | Exact two compressed welcome strings and their output order. |

[`text.asm`](../../apps/daggorath/src/compat/text.asm) is position independent,
uses PCR-relative glyph data, changes no hardware register or direct page, and
preserves `CC`, `DP`, `X`, `Y` and `U`. Cartridge video bases become an explicit
logical-frame pointer. The operation byte makes region ownership visible at the
ABI rather than translating every source clear into a physical-window clear.

[`import_compat_text.py`](../../apps/daggorath/import_compat_text.py) is the
auditable import step. Normal builds use its checked-in generated glyph table
and fixtures and do not depend on the external source checkout.

## Exact logical oracle

The independent Python model decodes the packed original font and strings and
produces complete 6,144-byte frames. The target demonstration executes the PIC
assembly and compares the result after every operation. Ordinary glyph calls
compare all seven bytes they may touch; scrolls, clears and named boundaries
compare all 6,144 bytes.

| Boundary | Cursor | Differing logical bytes |
| --- | ---: | ---: |
| blank four-row area | 0 | **0 / 6,144** |
| CR + lone dot | 33 | **0 / 6,144** |
| underline cursor, then backspace | 33 | **0 / 6,144** |
| cursor erased with display code zero | 34 | **0 / 6,144** |
| first welcome prefix | 40 | **0 / 6,144** |
| first welcome line complete | 64 | **0 / 6,144** |
| second welcome prefix | 72 | **0 / 6,144** |
| both welcome lines complete | 95 | **0 / 6,144** |
| four-row fill before overflow | 127 | **0 / 6,144** |
| source scroll | 96 | **0 / 6,144** |
| source primary clear | 0 | **0 / 6,144** |

The blank glyph is not redrawn across all 128 cells after a clear. That follows
the source naturally: `CLRPRX` clears the region, and subsequent text emits only
requested characters. It avoids the modern renderer's measured blank-cell cost
without introducing a compatibility-only shortcut.

## Visibility boundaries and timing

The source routines write each glyph immediately in seven-scanline order, but
the recovered completed-frame trace contains no stable frame for every
individual welcome character. It does establish source order and stable partial
strings. C2 therefore applies every character to the logical frame in source
order and presents the four retained runtime/oracle boundaries: first prefix,
first line, second prefix and completed welcome. It does not replace those
operations with a precomposed final bitmap.

The completed-frame observer ran at `268435455/4479692` frames/s
(approximately 59.9227 Hz). In the final byte-verified run:

| C2 boundary | Frame | Time | Interval from prior formation boundary |
| --- | ---: | ---: | ---: |
| first welcome prefix | 1671 | 27.885904 s | — |
| first line complete | 1695 | 28.286420 s | 24 frames / 0.401 s |
| second prefix | 1718 | 28.670247 s | 23 frames / 0.384 s |
| welcome complete | 1749 | 29.187580 s | 31 frames / 0.517 s |

Prefix-to-complete is 78 frames, about **1.302 seconds**, inside the original
1.402-second Wizard-complete-to-welcome-complete observation. A disposable
target measurement of the bounded cell path recorded **2 sampled ticks total**:
0 sampled conversion ticks, 1 CoWin transfer tick and one surrounding clock
quantum. This is comfortably below each retained formation interval. The
partial path updates 16 bytes in the already mapped 2,048-byte physical strip,
then transfers the strip. Host tests prove its pixels equal a full conversion.

The standalone review program deliberately adds one-second holds so humans can
inspect dot, cursor, erase, fill, scroll and clear. Those holds are not cartridge
timing. Its pre-window target self-test also exercises every region clear and
the alternate-frame contract, so absolute launch-to-text timestamps are not an
opening oracle. C3 must schedule the opening's two source `WAITX` calls at 81 jiffies
each; C2 neither implements nor substitutes for them.

## Live review evidence

The final executable is SHA-256
`a08c600fcccd3e3a5b81342045ac198c98730df849849b5e8b0b75b3a529463d`.
It was extracted from the disposable OS-9 disk and compared byte-for-byte with
the host build before launch. The disk entry had the same `--e-rewr` executable
attributes as the C1 controls. Thresholded monochrome comparison reports
**zero differing pixels for all eleven selected states**. Raw capture black
levels differ from the generated oracle, so the accepted comparison is the
one-bit raster rather than RGB voltage levels.

- [Selection times](assets/daggorath-attract-compat-c2/selection.tsv)
- [Welcome formation sequence](assets/daggorath-attract-compat-c2/c2-welcome-formation.gif)
- [Blank](assets/daggorath-attract-compat-c2/pairs/00-blank-oracle-eou.png)
- [Lone dot](assets/daggorath-attract-compat-c2/pairs/01-lone-dot-oracle-eou.png)
- [Underline cursor](assets/daggorath-attract-compat-c2/pairs/01b-dot-cursor-oracle-eou.png)
- [Cursor erased](assets/daggorath-attract-compat-c2/pairs/01c-erased-cursor-oracle-eou.png)
- [First prefix](assets/daggorath-attract-compat-c2/pairs/02-welcome-first-prefix-oracle-eou.png)
- [First line](assets/daggorath-attract-compat-c2/pairs/03-welcome-first-line-oracle-eou.png)
- [Second prefix](assets/daggorath-attract-compat-c2/pairs/04-welcome-second-prefix-oracle-eou.png)
- [Welcome complete](assets/daggorath-attract-compat-c2/pairs/05-welcome-complete-oracle-eou.png)
- [Four-row fill](assets/daggorath-attract-compat-c2/pairs/06-four-row-fill-oracle-eou.png)
- [Scroll](assets/daggorath-attract-compat-c2/pairs/07-scroll-oracle-eou.png)
- [Clear](assets/daggorath-attract-compat-c2/pairs/08-clear-oracle-eou.png)

The left side of each pair is generated from the source-derived logical oracle;
the right side is the exact final EOU target run. These are not mislabeled as
independent original-cartridge screenshots. The committed cartridge storyboard
remains the player-visible oracle for the complete opening.

## Module and Level II budget

The C2 acceptance module embeds two 6,144-byte frames, the verifier and review
holds. Its data request therefore represents the deliberately heavy test
program, not the eventual integrated state allocation.

| Module | Size | CRC | SHA-256 | Data request |
| --- | ---: | --- | --- | ---: |
| `dodctxt` | 5,901 (`$170D`) | `AC40CC` | `a08c600fcccd3e3a5b81342045ac198c98730df849849b5e8b0b75b3a529463d` | 13,882 (`$363A`) |
| current `dodc19` | 14,429 (`$385D`) | `DBA651` | `6d40a38f7a4b2191a4353d8d5e6afa240d7f631d2c8150cbb0a0de9e76a169c7` | 13,359 (`$342F`) |
| current `dodc20` | 14,735 (`$398F`) | `F46A19` | `d8b98de62e4e3fce99ddbe645c7b6e23a66643cc14261b50321da53f73ef258f` | 13,359 (`$342F`) |
| current `dodc21` | 15,032 (`$3AB8`) | `AFB19A` | `e57d1112550627cd370092f164e0b3d12f68d971b84976d1c48c47b8f5db43b3` | 13,359 (`$342F`) |

All are `$11/$80` reentrant, read-only 6809 program modules with good ToolShed
CRCs. `dodctxt` maps as one program block and two data blocks. The largest
current C1 acceptance module remains within two program blocks. Either isolated
acceptance path uses one 2,048-byte CoWin mapping:

```text
C2 review: 1 program + 2 data + 1 CoWin = 4 / 8
C1 review: 2 program + 2 data + 1 CoWin = 5 / 8
future combined compatibility target: <=2 program + 2 data + 1 CoWin = <=5 / 8
```

The current C2 data request leaves 2,502 bytes below 16 KiB. No third program
block or second CoWin mapping was introduced.

## C3 Wizard entry plan

C3 can begin at the original `ONCE.ASM:DEMO10` opening boundary without
rediscovering text:

1. enter `MISC.ASM:WIZIN0/WIZI10/WIZI20` with `D4.ASM:WIZ1` through the C1
   compatibility state/frame and presenter;
2. issue the two `DEMO10` packed messages through C2 `TXTCHR` in exact source
   order;
3. schedule both `WAITX` calls at 81 logical jiffies;
4. use `MISC.ASM:WIZOX/WIZO10/WIZZES` for fade-out, preserving the copyright
   status region while clearing only primary text;
5. apply the alternate logical-screen `ZFLOPX` operation and present the final
   blank boundary;
6. hand off immediately to `MISC.ASM:PREPAX` through the existing phase
   controller/CoWin owner.

The adapters required are: the existing aligned compatibility state, one
6,144-byte logical frame, original vector/table imports, the C1/C2 CoWin
presenter, an explicit 60-Hz logical-boundary scheduler and semantic audio event
calls for the source opening sounds. C3 must preserve the source copyright
region and presentation reasons. It must not revive the modern multi-second
Wizard-to-PREPARE setup gap.

C2 does not implement Wizard vectors, opening sound, AUTTAB, the compatibility
scheduler, map, heartbeat, Giant scheduling or public `/d1/daggorath`
integration.
