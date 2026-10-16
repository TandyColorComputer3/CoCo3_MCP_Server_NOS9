# Recovery ledger

Search performed for `protpr`, `protcl`, and the Stage 3E.16 private directory
names in `/private/tmp`, `/Volumes/SEDONA/Projects`, the local coding toolchain,
existing Daggorath archives, and available Codex session/attachment records.
The named private directories are absent. No matching source or exact binary
was recovered; local disk-image search found no embedded `protpr`/`protcl`
name. Git history does not contain the protected-presenter experiment.

## Found

- The installed EOU boot CoWin module and the preserved `cowin_beta6` binary
  compare byte-for-byte. The reference assembly source and assembly listing
  were extracted without editing from the EOU `/dd/SOURCECODE` VHD into
  `references/`. See `references/cowin-select.md` for the verification boundary.
- Stage 3E.16B.3's brief summarizes the earlier results. Its original report
  and event files are not present. The Stage 3E.16B.2 static diagnostic can be
  repeated from the preserved CoWin reference; its live trace was blocked.

## Missing

- Original `protpr` and `protcl` source and exact modules (expected module
  identities are in `artifact-manifest.json`).
- Original build commands, flags, include paths, map files, compiler/assembler
  version records, and scripts.
- Graphics ledger, PB1/raster observer, Select tracer, private bridge/launcher,
  configuration template, and bounded raw event logs.
- Original Stage 3E.16, 16A, 16B, and 16B.1 reports.

Nothing has been reconstructed as though it were an original. Two isolated
reproduction attempts currently stop before compilation because the exact
source and build procedure are missing. Artifact status: **NOT REPRODUCIBLE**.

## Resume Stage 3E.16B.2

Recover the exact original client and presenter files first. Validate their
module digests and CRCs and compare staged guest copies. In a new private RGB
EOU session, establish the runtime CoWin base and DAT mapping, validate Select
instruction bytes, then capture calling PID, process/window paths, old and new
`P$SelP`, `G.CurDev`, the branch at module offset `$0578`, carry/B on return,
and display registers before/after. Do not infer foreground ownership or the
branch from a successful status code or visible text alone. A child-process
restriction and a correction remain unproven until this trace exists.
