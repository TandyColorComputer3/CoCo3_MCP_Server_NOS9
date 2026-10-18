# Stage 3E.16D.25F.5 accepted type-7 raster checkpoint

This additive experimental archive preserves the exact D.25F.5 `ppown25c` / `ppclient25c` presenter pair, the Lua observer that ran with it, and the evidence for **type-7 rendered-raster content and three-frame stability**. The current-run parent/child lifecycle also completed with Shell+ status 000. This is not a DHeartbeat, gameplay, strict deadline, or physical host-viewer acceptance result.

Read [ACCEPTANCE.md](ACCEPTANCE.md), [RASTER_VALIDATION.md](RASTER_VALIDATION.md), [OBSERVER.md](OBSERVER.md), [BUILD.md](BUILD.md), and [LIMITATIONS.md](LIMITATIONS.md). `PROVENANCE.json` identifies the original path, size, and SHA-256 of every copied file. `SHA256SUMS` verifies every archived file except itself. From this directory run `shasum -a 256 -c SHA256SUMS`; then compare the manifest's path set with the actual archive files. No MAME or build run is needed to validate the archive.

## Graphics contract

The accepted disposable parent owns the visible CoWin type-7 window and all GP/PutBlk presentation. The logical screen is 640×200, four colors, two bits per pixel: 160 packed bytes per row and 32,000 bytes of screen backing. Its GP 196/4 holds a 32×8, 64-byte pattern. Each row is `00 00 55 55 AA AA FF FF`; the accepted presentation covers logical X=64–95, Y=64–71. The parent first draws the same pattern at Y=32 as an activation marker. The child sends 16-byte framed requests; the parent performs PutBlk and replies with ACKs. These facts are grounded in `source/ppgraphics.c`, `source/pp25c_protocol.h`, `source/ppown3.c`, the D.25E independent runtime calibration, and the D.25F.5 guest logs. The repository's referenced `MCP/Documents` manual directory is absent in this checkout, so the EOU-specific contract is reported from identified source and verified runtime evidence.

The protected heartbeat rectangle remains X=304–335, Y=156–162. This static pattern test did not establish that other graphics writes exclude it.

## Archive boundaries

The accepted original modules are in `artifacts/`; source and accepted linked build inputs are in `source/` and `build/accepted-c/`. The exact observer is `observer/accepted/observer.lua`. Evidence includes full first-frame raw pixels, the bounded event ledger, all guest diagnostic logs, three separate matching crops, and the original D.25F.5 report. The independent D.25E coordinate calibration result is under `evidence/independent-calibration/`.

The D.23 accepted type-5 lifecycle archive is a separate baseline and was not changed. All paths and hashes in this new archive are new files. No ROM, disk/VHD, save state, MAME binary, generated cache, or 26 MB bridge wire dump is included. The wire dump is ancillary to the archived parsed host results and raw crop evidence; its original SHA-256, size, and path are recorded in [LIMITATIONS.md](LIMITATIONS.md).
