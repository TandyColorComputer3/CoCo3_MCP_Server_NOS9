# Protected presenter experiment: recovery archive

This archive preserves what can currently be verified about Daggorath Stages 3E.16–3E.16B.2. Architecture C proposed one presenter process as the sole CoWin/PutBlk owner, with a separate client. It remains **not accepted**: the exact presenter source and binary, original client binary, full observer/launcher set, raw event logs, and live display ownership evidence remain unavailable.

## Current status

| Stage | Result | Evidence here |
| --- | --- | --- |
| 3E.16 | Architecture C conditionally feasible | Summary only |
| 3E.16A | Not accepted; 10 matching heart rasters met 33.34 ms, ownership unproven | Summary only |
| 3E.16B | Needs information; client status 000, graphics activation unverified | Summary and recovered source/build transcript |
| 3E.16B.1 | Graphics activation failed; 1,499 frames stayed in Shell+ text mode | Summary only |
| 3E.16B.2 | CoWin Select code identified; actual branch unobserved | Verified static reference and summary |

The stage summaries remain secondary summaries, not original reports or raw event logs. Recovery findings are appended to [RESULTS.md](RESULTS.md). The exact original `protcl` source and historical build script are now recovered from the accessible Codex session; the final `protpr` source is not. This archive is still not a runnable reproduction.

## Recovery result

- Recovered original client source: `source/protcl/client.c`, SHA-256 `a9be9cce523524644ac8bdc2d4465fec3de5efee7b49be64a78ab58600177b7b`; its hash matches the session’s immediate post-write record.
- Recovered original build-script source: `build/original-build.py`; it records the historical CMOC source order/options, stack reserve, and ToolShed check. It has absolute host paths and is preserved as historical evidence, not as a portable reproduction tool.
- Recovered presenter precursor: `source/protpr/presenter-base-stage3e10.c`, SHA-256 `cb1ac3a812a2784e19598d1eddf2dde2da49e090ecaf4382558f780775c6eed0`, matching the source copied at Stage 3E.16B start. One complete and one truncated presenter patch fragment are retained as evidence.
- Missing: final presenter source, both original binaries, binary/map/log artifacts, exact source-dependency hashes, and exact compiler/assembler versions. No build was attempted because the presenter source and historical dependency revisions are not established.

See [RECOVERY.md](RECOVERY.md) for source provenance and the first missing prerequisites. The archive status is **PARTIALLY RECOVERED / NOT REPRODUCIBLE**.

## Tools and environment

The historical target used CMOC, lwasm, ToolShed 2.2, and MAME 0.289 `ample-289-u2` with `coco3h`/HD6309E, 2 MB RAM, and RGB `screen_config=1`. The recovered build script shows exact source ordering and flags, including a 1,024-byte stack reserve and `DAG_HEARTBEAT_CONTROL_PLANE` for `protpr`. The script’s original absolute paths and live dependencies were not retained in a portable form.

The presently installed tools report CMOC 0.1.90 and lwasm/lwtools 4.22; these are current identities, not verified original build identities. ToolShed 2.2 validated the recovered CoWin reference CRC as Good. The `protpr`/`protcl` CRCs remain historical session results; their binary bytes were not recovered for independent revalidation.

## Safe next recovery step

Recover the complete original `presenter.c` and the exact source revisions or hashes for its linked project dependencies. Then recover or rebuild in two isolated directories, validate each module with ToolShed, and compare to the historical SHA-256. Do not use the incomplete presenter patch, the Stage 3E.10 predecessor alone, or a different experiment’s observer as a substitute. No MAME launch or heartbeat acceptance test was performed for this archive.
