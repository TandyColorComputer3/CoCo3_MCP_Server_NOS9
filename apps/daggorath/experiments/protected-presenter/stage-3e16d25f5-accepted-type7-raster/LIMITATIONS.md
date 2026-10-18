# Unaccepted requirements and preservation limits

The D.25F.5 result does **not** accept:

A. Geometric exclusion of scene/status graphics from the protected heart rectangle X=304–335, Y=156–162.
B. Generation-aware heart-phase reconciliation.
C. Independent heartbeat presentation under gameplay workload.
D. Prevention of obsolete visible heart phases.
E. Absence of artificial visual phase catch-up.
F. Actual PB1-edge-to-completed-raster latency.
G. The strict 33.34 ms visual deadline.
H. Physical host-viewer publication timing; `video:snapshot_pixels()` alone does not establish it.
I. The timing cost of optional four-color type 7 relative to the accepted type-5 lifecycle.

The D.25F.5 observer's 3.0-second provisional later label is not a guest sleep-return timestamp. Its eight-frame coarse cadence cannot exclude a shorter transient that begins and ends between samples. No system-wide process inventory was recorded to assert zero orphans; matching F$Wait PID/status and Shell+ restoration are the supported lifecycle observations. The exact D.25C compiler command line was not recovered, although exact source, generated assembly, object inputs, linker section descriptions, maps, and two identical accepted build outputs are attributable and preserved. [BUILD.md](BUILD.md) distinguishes a candidate reconstruction recipe from a verified byte-exact rebuild.

## Excluded large or host-specific material

The boot floppy, VHD, D1 image, EOU-ready save state, ROMs, MAME executable, and bridge wire dump are not included in Git. They are not required to verify the archived binary identities, guest logs, raw crops, parsed host results, or the result classification. The D.25F.5 bridge wire dump remains at `/private/tmp/dag-3e16d25f5-phase-20261018/session-r1/evidence/bridge-responses.raw.jsonl`, size 26,966,105 bytes, SHA-256 `f3b2f1de0c48be02472091f83b1c367440f9701209dcb066242865808e6d5ee0`. This path is temporary and **is not the preservation mechanism**; the parsed result, array-probe proof, lifecycle events, full initial snapshot and raw crops are archived here. A fresh wire dump requires a new, explicitly authorized private run with the documented launcher and verified media, and cannot be reconstructed from this archive alone.

## One proposed next experiment

Use a disposable type-7 presenter pair with the accepted single parent graphics owner and unchanged lifecycle. First display a known heart-region pattern at X=304–335, Y=156–162. Then submit an overlapping scene/status update through the same parent-owned graphics path and compare exact raw heart pixels before and after the update, alongside the non-heart pixels that must change. A pass requires zero writes to the protected rectangle and correct updates elsewhere. This is a proposal only; no source, module, or MAME run for it is included here.
