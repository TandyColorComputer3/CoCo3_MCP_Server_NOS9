# Recovery ledger

## Search and outcome

The named Stage 3E.16B disposable directories and the specifically identified later MAME run roots are absent. The exact `protpr`/`protcl` binaries were not recovered from the targeted `/private/tmp`, project, toolchain, archive, and Codex session locations. No matching guest disk or VHD remains among the identified paths.

Accessible Codex session history does preserve certain original tool arguments and outputs. This changes the prior all-missing source assessment to **PARTIALLY RECOVERED**; it does not make the experiment reproducible.

## Recovered original files and provenance

- `source/protcl/client.c` (1,957 bytes; SHA-256 `a9be9cce523524644ac8bdc2d4465fec3de5efee7b49be64a78ab58600177b7b`) was extracted from the complete Add File patch for `/private/tmp/c6-stage3e16b-protected/src/client.c` in the accessible 2026-10-15 Stage 3E.16B Codex session. A subsequent session command hashed that file and recorded the same digest. It is classified `ORIGINAL_VERIFIED`.
- `build/original-build.py` (2,340 bytes; SHA-256 `6f06b39e745c2feeed3c99c576b3e74cedd503cd741f71b779f5fd3efa326dd0`) was extracted from the complete Add File patch for `/private/tmp/c6-stage3e16b-protected/build.py`; the session records invoking it for both builds. It reveals exact commands/source order, but its path literals are host-specific and it references project files whose historical hashes were not saved.
- `source/protpr/presenter-base-stage3e10.c` (13,338 bytes; SHA-256 `cb1ac3a812a2784e19598d1eddf2dde2da49e090ecaf4382558f780775c6eed0`) is the exact Stage 3E.10 source copied at the start of Stage 3E.16B. It is a predecessor, not the final presenter.
- `evidence/source-recovery/presenter-change-1.patch` preserves the complete first patch argument. `presenter-change-2-truncated.txt` preserves the second patch argument as it appears in the transcript; the transcript itself contains an omission marker, so this is incomplete and must not be applied as a whole.
- `evidence/source-recovery/build-record.md` transcribes the session’s build, `cmp`, and ToolShed results. The transcript reports two builds byte-identical and gives expected module metadata. This is not a substitute for the absent original binaries, logs, maps, or dependency-hash JSON.

The session-reported final presenter SHA-256 is `1bde874594657782aa8b8c75b7e6bd4d0ab3f94b2e8df7628756f2aba8c3cb3e`; no bytes matching it were recovered.

## Still missing

- Final `presenter.c` and exact `protpr` binary.
- Exact `protcl` binary. The original client source is recovered, but its binary is not.
- Original observer and launcher revisions, raw timing/graphics/Select logs, map files, build logs, staged disk, and dependency-hash record.
- Exact historical CMOC and lwasm versions. Current installed versions are not historical proof.
- Hashes/bytes for the linked project sources as they existed in the Stage 3E.16B build.

ToolShed’s historical “Good” CRC output is recorded in the session transcript, but cannot be rerun on missing modules.

## Build dependencies recorded by the recovered script

The script invoked `cmoc --os9 -O2 --intermediate --verbose`, an intermediate directory, include paths for `src`, `src/compat`, and `src/audio`, and output basenames `protcl`/`protpr`. Both builds reserved 1,024 bytes of OS-9 stack; `protpr` additionally used `-DDAG_HEARTBEAT_CONTROL_PLANE`. Link/source order was:

- `protcl`: recovered `client.c`, `heartbeat-broker-client.asm`, `opening-heartbeat.c`, `audio/ipc.c`, `os9.c`.
- `protpr`: missing final `presenter.c`, `native_heartbeat.c`, `native-heartbeat-wait.asm`, `audio/ipc.c`, `window-path.c`, `os9.c`.

The historical script called ToolShed 2.2 `os9 ident`; CMOC and lwasm version strings and hashes of linked project dependencies were not recorded. The recovered client calls the broker API and has no direct graphics path or PutBlk operation. The exact historical broker header revision defining the 16-byte framing was not archived or hash-pinned, so the frame size is not independently verified for that build. The original build script points at absolute `/private/tmp`, project, and toolchain paths. Preserve it as evidence; do not run it as a portable reproduction procedure. The first build blocker is the missing exact `presenter.c`; even after recovering it, historical dependency revisions and compiler/assembler identities must be established before claiming exact reproducibility. No build was attempted.

## Reproduction status

`build/reproduce.py` intentionally refuses to build. It does not substitute a new presenter or use current project inputs to synthesize historical binaries. The recovered script and client compile as Python/C source respectively, but no build was attempted. Classification: client source `ORIGINAL_VERIFIED`; presenter predecessor `SOURCE_ONLY`; build script recovered from session; both modules `MISSING`; experiment **PARTIALLY RECOVERED / NOT REPRODUCIBLE**.

## Stage 3E.16B.2 continuation

First recover the complete presenter source and its original dependency identities. Then validate module hashes/CRCs and staged copies. In a new private RGB EOU session, establish runtime CoWin base and DAT mapping, validate Select instruction bytes, and capture caller PID, process/window paths, old and new `P$SelP`, `G.CurDev`, the branch at module offset `$0578`, carry/B on return, and display registers before/after. Do not infer foreground ownership or the branch from successful status or visible text alone. A child-process restriction and correction remain unproven until this trace exists.
