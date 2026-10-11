# Original-cartridge heartbeat timing reference

**Status:** Stage 3E.0 accepted measurement evidence, preserved 2026-10-11.

This directory preserves the read-only Dungeons of Daggorath original-source
cartridge heartbeat study. It is an evidence archive, not production code and
not a change to C6 acceptance criteria. The technical interpretation is in
[`DAGGORATH_CARTRIDGE_HEARTBEAT_TIMING.md`](../../../../docs/apps/DAGGORATH_CARTRIDGE_HEARTBEAT_TIMING.md).

## Artifact identity

- ROM size: 8,192 bytes
- ROM SHA-256: `35e6a77354dcf1a3048f276824b7a0f9f759115fdd40603664cebfb3a7da6571`
- source commit: `a94326f00ebb16a106b540c58bc2ccf5f7b66dac`
- MAME: 0.289, Ample build `ample-289-u2`
- machine: `coco3h`, 2 MiB, RGB

The ROM was source-built and byte-verified. It was not independently
authenticated against a retail dump and is not included here.

## Preserved evidence

`evidence/` retains the original bytes of:

- `observer.lua` and `analyze.py`;
- `summary.json`, `analysis-output.txt`, and `tearing-examples.json`;
- `source-reference.txt`;
- idle and stress `events.csv`, `frames.csv`, `heartbeat-correlation.csv`, and
  `raster-ledger.csv`;
- run-specific observer metadata and verified MAME configuration files;
- representative idle and combat large/small completed rasters.

`SHA256SUMS` is the **unchanged original 549-entry Stage 3E.0 manifest**. It
uses paths relative to the original `/private/tmp/c6-stage3e0` tree and also
lists intentionally excluded bulk evidence. `PRESERVED_SHA256SUMS` identifies
the subset committed here.

## Approved manifest exception

The original manifest SHA-256 is:

`df2627708cf5df1b7c4e3d567cb77a3c9cca2079b702de7c1773b4adccf75ab1`

Verification produced 548 matches and one mismatch:

| Original path | Original expected SHA-256 | Current post-run SHA-256 |
| --- | --- | --- |
| `cfg/coco3h.cfg` | `dd49931d25415110e410521a0fd377fde13174a6895fd2482adbd1832c334481` | `c2721b91c6ee47a68331503ea77f9d31a38a2b7dfbb4e522a83bd53da0a4582f` |

The original expected bytes are unavailable. MAME rewrote this mutable
configuration when residual calibration processes exited. The current bytes
are retained as `evidence/post-run/coco3h.cfg`; they are explicitly a post-run
artifact and are not claimed to reproduce the original manifest entry.

Every measurement CSV, script, summary, audio file, and raster listed by the
original manifest passed integrity verification independently of this
configuration mismatch. This approved exception does not repair or replace
the original manifest and prevents a claim of full byte-identical environment
reproducibility.

## Excluded artifacts

The following were deliberately not committed:

| Artifact | Recorded SHA-256 or disposition |
| --- | --- |
| Original ROM | `35e6a77354dcf1a3048f276824b7a0f9f759115fdd40603664cebfb3a7da6571` |
| `idle/original-idle.wav` | `11be6d94bfcb28727e305f32329013eeec6c159a5880f0d5ad1af48c5793f378` |
| `stress/original-stress.wav` | `d09895aef12f279bd5d44efa9696b2ab4337925802bba1eb80184b94b32a4472` |
| Bulk raster sequence | identities remain in the original `SHA256SUMS` |
| Raw raster streams | temporary analysis intermediates, excluded from the original manifest |
| MAME executable, disks, NVRAM, and emulator state | disposable environment; not committed |

## Reproduction

The accepted runs used MAME 0.289 with `coco3h`, 2 MiB RAM, RGB output, the
identified cartridge, no disk, and no restored save state. `observer.lua`
accepts `STAGE3E_ROOT`; its default remains the original private calibration
path. `analyze.py` intentionally retains the original absolute
`/private/tmp/c6-stage3e0` root. The preserved scripts have not been rewritten
for portability.

To reproduce the full analysis, reconstruct the original directory layout,
provide the separately obtained byte-identical ROM and excluded WAVs, and run
the preserved analyzer. Because the original top-level `cfg/coco3h.cfg` bytes
are unavailable, this repository supports reproduction of the measurement
method and retained numerical analysis, not a claim of the full original
environment's byte identity.

The retained per-event CSVs contain 128 idle and 128 stress events. The
accepted result is 256 correlated generations with zero missing, duplicate,
obsolete, extra, or partial completed-heart transitions and no accumulating
drift.
