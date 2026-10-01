# Daggorath Presentation Color Research

## Scope

This note separates four-color presentation from the monochrome Sizzle Demo
work. It records verified facts only; it does not change the current
Daggorath display contract.

## Current production mode

`apps/daggorath/src/presentation.c` uses EOU CoWin graphics type 5. The owned
screen is 640×200×2: one bit per physical pixel, 80 bytes per scanline. The
canonical Daggorath viewport remains 512×192 at `(64,4)`, produced from the
unchanged 256×192 logical framebuffer by two-times horizontal expansion. A
512×32 dirty strip therefore occupies 2,048 bytes.

The current EOU RGB presentation has no verified color-selection operation for
that type. It must remain monochrome. The port does not use NTSC artifact
color as a substitute.

## Type 7 investigation

The current upstream NitrOS-9 CoWin source identifies type 7 as 640×200×4,
with 160 bytes per scanline. Its equivalent 512×32 strip would be 4,096 bytes;
it would still fit in the one 8K logical mapping supplied by `SS.MpGPB`.

In the verified EOU runtime:

- A type-5 palette escape (`ESC $16`) returned `E$UnkSvc` 208.
- Type 7 with the old 2K graphics buffer returned `E$BufSiz` 191, confirming
  that the runtime recognizes the type and requires its larger buffer.
- A correctly-sized, disposable type-7 probe did not reach a trustworthy guest
  status or graphics result before its MAME session ended. This is not evidence
  that type 7 works in the installed EOU environment.

The type-7/palette path is therefore a future presentation-research milestone,
not a dependency of Attract M1B or the monochrome Sizzle Demo.

## Future palette intent

When type-7 support has independent live acceptance, the intended semantic
palette is black background, white or neutral primary graphics, red for
heartbeat/life/danger, and blue for map/magic/navigation. No part of that
palette is simulated in type 5.

## Integrity

Every probe used a fresh disposable floppy and the verified `coco3h`, 2 MB,
RGB, `nos9_ready_v2` setup. The canonical EOU media and save state were not
modified.
