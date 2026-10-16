# Protected presenter experiment: recovery archive

This archive preserves what can currently be verified about Daggorath Stages
3E.16–3E.16B.2. Architecture C proposed one presenter process as the sole
CoWin/PutBlk owner, with a separate client. It is **not accepted**: the
presenter/client sources and exact modules were lost with their disposable
`/private/tmp` directories. Graphics activation and the actual Select branch
remain unresolved. This archive is not a runnable reproduction yet.

## Current status

| Stage | Result | Evidence here |
| --- | --- | --- |
| 3E.16 | Architecture C conditionally feasible | Summary only |
| 3E.16A | Not accepted; 10 matching heart rasters met 33.34 ms, ownership unproven | Summary only |
| 3E.16B | Needs information; client status 000, graphics activation unverified | Summary only |
| 3E.16B.1 | Graphics activation failed; 1,499 frames stayed in Shell+ text mode | Summary only |
| 3E.16B.2 | CoWin Select code identified; actual branch unobserved | Verified static reference and summary |

The stage summaries are transcriptions of the Stage 3E.16B.3 preservation
brief and the Stage 3E.16B.2 diagnostic result. They are not original reports
or raw event logs. See [RESULTS.md](RESULTS.md) and [RECOVERY.md](RECOVERY.md).

## Tools and recovery workflow

The historical target used CMOC, lwasm, ToolShed 2.2, and MAME 0.289
`ample-289-u2` with `coco3h`/HD6309E, 2 MB RAM, and RGB
`screen_config=1`. Exact CMOC/lwasm versions, command lines, include paths,
source order, linker inputs, stack reserve, and module data requests for
`protpr`/`protcl` have not been recovered. Do not infer them from another
experiment.

The presently installed tools report CMOC 0.1.90 and lwasm/lwtools 4.22;
these are **current installation identities**, not verified original build
identities. ToolShed 2.2 confirms the preserved CoWin reference CRC as Good.
The missing `protpr`/`protcl` CRCs cannot be independently rechecked.

1. Restore the original private sources, build scripts, observers, launcher,
   and bounded reports into this archive, keeping their original bytes.
2. Record each file's provenance and digest in `artifact-manifest.json`; run
   `python3 build/verify_archive.py` to validate `SHA256SUMS` and the manifest.
3. Run `python3 build/reproduce.py --out OUTDIR`. It currently exits with a
   missing-dependency error. Only replace that guard when the exact original
   build inputs and commands are independently recovered and documented.
4. Verify resulting module name, attributes, data request, CRC, and SHA-256
   with ToolShed `os9 ident`. Stage through binary-safe ToolShed to disposable
   media, extract, and compare byte-for-byte before launching.
5. Use a private MAME port/config and disposable boot, D1, and VHD copies.
   Validate `coco3h`, HD6309E, 2 MB RAM, RGB `screen_config=1`, Shell+
   readiness, and `pwd` status 000 before running the client. Do not fall back
   to shared MCP media.
6. Calibrate observer readiness, active graphics mode, DAT mappings, and
   runtime module bytes before selection breakpoints. Collect the boundaries
   listed in `RECOVERY.md`, stop the guest, and terminate the private session.

No original private launcher or observer was recovered. The existing
`mixed-scene-harness` launcher/observer is a different experiment and must
not be substituted without a documented adaptation and calibration. No MAME
run or heartbeat acceptance test was performed for this archive.
