# External reference projects: targeted archaeology

Inspected 2026-09-26. This is source/history research, not a build, deployment, framework reconstruction or game port. All six supplied checkouts are **external read-only reference repositories**. Inspect/search/diff source and Git history; do not modify or commit to them without explicit instruction.

## Navigation

| Question | Read |
|---|---|
| What can our existing optional build environment provide? | [coco-dev](COCO_DEV.md) |
| How much MVKit survives, and where? | [MVKit reconstruction evidence](MVKIT_RECONSTRUCTION.md) |
| What application patterns are worth reusing? | [mvedit/mvdraw](MVEDIT_MVDRAW.md) |
| Can object files teach the MCP symbols and relocation? | [OS9 ROF disassembler](OS9ROF.md) |
| What exactly is needed for the first wizard intro? | [Daggorath and Wizard Milestone 1](DAGGORATH.md) |
| How do original ASM, Windows/Linux C ports and our NitrOS-9 implementation relate? | [Daggorath semantic crosswalk](DAGGORATH_C_CROSSWALK.md) |

## Reference identities

All were clean, non-shallow Git checkouts when inspected. Paths are `/Volumes/SEDONA/Projects/<name>`. Full hashes pin these findings; no fetch was performed and no assertion is made about a moving remote HEAD.

| Checkout | Inspected commit | Origin |
|---|---|---|
| coco-dev-reference | `ccdf721ba452d282364f5b37b6225e22011aaf16` | https://github.com/jamieleecho/coco-dev.git |
| mvedit-reference | `9fd15c476aed9cdc67d2d628309470e67951f174` | https://github.com/jamieleecho/mvedit.git |
| mvdraw-reference | `e3994af467556ca00d8ebe18de546a28e949f5b0` | https://github.com/jamieleecho/mvdraw.git |
| os9rof-reference | `a66579f5e3a933b36ae41be96a4aa7f582335d52` | https://github.com/ChetSimpson/OS9ROF.Disassembler.git |
| daggorath-reference | `a94326f00ebb16a106b540c58bc2ccf5f7b66dac` | https://github.com/MichaelSpencerJr/DungeonsOfDaggorath.git |
| nitros9-reference | `f470fa52eb172b59b22c1b722074998cb42de9b1` | https://github.com/nitros9project/nitros9.git |

The historical MVKit snapshot used below is **mvdraw `47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2`**, not its current HEAD. History links identify that revision; they were read locally with `git show`, without checking it out or downloading dependencies.

## Important findings

1. MVKit has **97 recoverable tracked files** in mvdraw history, including 39 C files and 16 headers. This is substantially better evidence than reconstruction from call sites alone. It is not fully build-complete: `mv_document_close.c` is referenced but absent.
2. coco-dev pins many tools and definitions, but its recipe is not proof of the installed Synology image's contents. The existing NAS environment remains available as an optional worker; running/stopped state was not queried. No container was created, deployed, started or stopped.
3. The ROF project explicitly lists 6309 support as a TODO. Its relocation parser is useful research; it is not a verified decoder for all objects produced by our modern toolchain.
4. Daggorath's intro is a bounded vector/data problem, surrounded by a bare-machine runtime that cannot be carried into an OS-9 application unchanged. Its original vector area is 256×152 inside a 256×192 overall layout.
5. Preserve the original coordinates and fade rasterization separately from OS-9 presentation. The requested 640×200 output and `(192,4)` translation are the milestone target, requiring a future live screen-size/graphics-path test on EOU.

## Source precedence and knowledge links

Follow [the source-index navigation](../source-index/README.md): verified EOU runtime is primary for our actual target; indexed guest sources explain candidate EOU behavior; pinned official upstream supplies modern implementation patterns; applicable manuals establish normative ABI/hardware contracts. These application repositories are examples, not replacements for those layers. Some EOU changes entered upstream and others remain divergent.

Useful cross-references: [C/ABI](../source-index/C_DEVELOPMENT.md), [graphics/windows](../source-index/GRAPHICS_WINDOWING.md), [processes/signals](../source-index/PROCESSES_SIGNALS.md), [memory/MMU](../source-index/MEMORY_MMU.md), [modules](../source-index/MODULES.md), [development tools](../source-index/DEVELOPMENT_TOOLS.md), [6309](../source-index/CPU_6309.md), [sound](../source-index/SOUND.md), [runtime fingerprints](../source-index/RUNTIME_MATCHES.md), and [upstream crosswalk](../source-index/EOU_UPSTREAM_CROSSWALK.md).

`MCP/Documents/` and `DOCS_INDEX.md` remain unavailable in this checkout. Hardware facts below are explicitly observations of cited code/definitions, not independently checked manual claims. No graphics, audio, object decoder or application execution was live-tested in this archaeology pass.

## Recommended next MCP development capabilities

In priority order, based on these projects:

1. **Build provenance and artifact inspection:** explicit compiler/assembler/linker versions, CPU/OS target, pinned defs/library identities, argv/cwd, stdout/stderr, exit code, artifact hash, size, module identity, map and listing paths. Keep native macOS first-class; add an optional existing-worker backend rather than replacing it.
2. **Dependency preflight:** detect absent MVKit sources/library, missing cmoc_os9, unpinned fetches and incompatible object formats before starting a build. Separate build outputs from external references and media.
3. **Disposable artifact staging and execution:** use separate test media for generated modules/resources; return module identity and invoke through existing `os9_restore_ready`/`os9_run`. Those tools establish readiness/completion, not full output capture. Do not equate restored RAM with rollback of media writes.
4. **Graphics lifecycle probe:** verify a full 640×200 screen, actual usable bounds, disabled automatic scaling, coordinate markers, graphics-buffer upload/display and terminal restoration on normal exit, error and abort. This is the first prerequisite to Wizard Milestone 1.
5. **Symbol-aware inspection:** detect object dialect first; parse symbols/relocations and correlate linker maps with the guest module/process mapping. Never treat a ROF offset or process logical address as a physical MAME address.
6. **Measured visual/timing/audio tests:** frame snapshots and hashes, elapsed guest/host timing, cooperative cancellation and sound capability discovery. Do not rely solely on fixed sleeps or claim complete stdout capture from screen observations.

These are proposed capabilities, not implemented tools. MVKit recreation and side-panel game features are deferred.

## Reproducible read-only method

Used `git status --porcelain`, `remote -v`, `rev-parse HEAD`, `rev-parse --is-shallow-repository`, `tag --list`, `ls-files`, targeted `rg`/`sed`/file reads, and `git log --all -- <path>`, `ls-tree` and `show`. Historical recovery evidence can be repeated without extracting files:

```sh
git -C /Volumes/SEDONA/Projects/mvdraw-reference log --all --oneline -- mvkit
git -C /Volumes/SEDONA/Projects/mvdraw-reference ls-tree -r --name-only 47b116f -- mvkit
git -C /Volumes/SEDONA/Projects/mvdraw-reference show 47b116f:mvkit/Makefile
git -C /Volumes/SEDONA/Projects/mvdraw-reference show 47b116f:mvkit/src/mv_app_run.c
```

Also compared historical Makefile object prerequisites with tree entries and searched store operands against Daggorath code labels (a limited static check, not a proof about all indirect writes). Inspected the Daggorath license-grant image visually. No `make`, Docker command, NAS command, guest command, media mount or write was performed. No upstream tree was copied into this repository. Only these six research documents were created; no commit.

Validation: all 114 local/historical links in these documents resolved; all six external checkouts retained their initial HEAD and clean status. `git diff --check` passed. A tracked-path search found no MVKit tree at any of the six current HEADs; the recovery evidence is in mvdraw history.
