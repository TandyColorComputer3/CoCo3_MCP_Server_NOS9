# Preserved findings and limits

The stage summaries under `evidence/` are **secondary summaries**, not original
raw measurements. Their provenance is recorded in `artifact-manifest.json`.

CoWin static verification on the local EOU media identified module `CoWin`,
edition 2, size 7,537 bytes, CRC `3C70C2` Good, type/attributes `$C1/$80`.
The boot module equals the `/dd/SOURCECODE/.../cowin_beta6` binary byte for
byte. The listed Select instruction bytes at `$0559–$05A1` match that binary.
The runtime module base was not captured in Stage 3E.16B.2.

The installed Select code uses the current caller (`D.Proc`) and its
`P$SelP`. After recording the newly selected path, it compares the old path's
device with global `G.CurDev`. A mismatch branches to a clear-error return
without display activation. This is a **possible** explanation for the
presenter's reported successful Select and unchanged text display, not proof
that the presenter took that branch. See `references/cowin-select.md`.

No new MAME run, heartbeat timing acceptance, or audio/raster measurement was
performed while preparing this archive. The exact presenter/client artifacts,
process IDs, window ownership, raw Select status, and live display handoff
remain unverified.
