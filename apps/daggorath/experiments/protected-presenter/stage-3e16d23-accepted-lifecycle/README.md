# Stage 3E.16D.23 accepted presenter lifecycle

This directory is an additive, curated preservation of the exact D.23
parent/child presenter lifecycle and its source/build evidence. It does not
replace or edit the earlier protected-presenter recovery archive.

The accepted run is the tested **type-5** graphics configuration. It is a
process-lifecycle and graphics-restoration baseline; it is not a heartbeat
timing acceptance result. D.24's type-8 / 320×200×16 review is in
[`GRAPHICS_16COLOR_FEASIBILITY.md`](GRAPHICS_16COLOR_FEASIBILITY.md).

## Contents

- `artifacts/`: exact host binaries `ppown20` and `ppclient3`.
- `source/`: the parent and child source snapshots and the shared IPC,
  graphics, OS-9, file I/O, and sleep support used by those binaries.
- `build/parent/`, `build/child/`: retained generated listings, maps, link
  inputs, build logs, and the historical child build script.
- `evidence/phaseA/`: writable-media qualification evidence.
- `evidence/phaseB/`: the accepted single-run lifecycle, status, observer,
  and cleanup evidence, plus ToolShed module identification and source
  provenance.
- `BUILD.md`: toolchain, source provenance, and reproduction guidance.
- `ACCEPTANCE.md`: the recorded results, scope, and limitations.
- `provenance.json`: machine-readable artifact and source identities.
- `SHA256SUMS`: checksums for every other file in this directory.

Large disposable disks, VHDs, ROMs, MAME binaries, and save states are not
included. The repetitive 3.1 MiB raw console-read trace also remains outside
Git; its path and hash are recorded in `provenance.json`. The curated command
completion, observer lifecycle, and parent checkpoint records are preserved.
Temporary evidence paths in provenance are historical source locations, not
runtime dependencies of this archive.

## Reuse constraints

Keep the parent as the sole graphics/window owner. The tested lifecycle uses
one forked client, PipeMan-framed requests and ACKs, the existing B=4 child
memory request, parent `F$Wait`, and parent cleanup/restoration. Do not use this
baseline as evidence that sustained heartbeat presentation or 16-color mode
works.

No guest rebuild or MAME launch was performed while preparing this archive.
