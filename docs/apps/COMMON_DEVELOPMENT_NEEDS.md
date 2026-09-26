# Shared development needs

Research date: 2026-09-26. Recommendations from the [Window Manager feasibility](WINDOW_MANAGER_FEASIBILITY.md), [BASIC09 feasibility](BASIC09_MODERNIZATION_FEASIBILITY.md) and existing [Daggorath Wizard milestone](../reference-projects/DAGGORATH.md). No application or MCP feature was implemented.

## Project boundaries

| Project | First useful result | Distinctive risk |
|---|---|---|
| Window Manager | Inspect one active window; validate/apply foreground/background; save/load a small profile | Queryable state is incomplete; palette/border can be shared; foreign-window ownership cannot be guessed |
| BASIC09 workbench | Preserve text master, compile a small procedure with unchanged BASIC09, execute a checked packed module in RUNB | Editor/compiler/runtime share local 16-bit structures; interactive modes and diagnostics need a real worker protocol |
| Daggorath Wizard | Minimal OS-9 program, verified 640×200 graphics screen, centered original 256×192 viewport, intro, clean exit | Original runtime/hardware takeover cannot be reused; raster/fade/timing/audio fidelity and cleanup must be demonstrated |

These are different applications with common development infrastructure. A reusable window lifecycle layer does not imply that all three need MVKit, the same visual style or a shared application process.

## Overlap matrix

**Required** means needed for the proposed milestone or its verification. **Later** means useful for a subsequent stage, not a reason to delay the first milestone.

| Capability | Window Manager | BASIC09 | Wizard | Evidence / practical boundary |
|---|---|---|---|---|
| Reproducible build recipes and pinned dependencies | Required | Required | Required | [coco-dev](../reference-projects/COCO_DEV.md), [MVKit dependency gaps](../reference-projects/MVKIT_RECONSTRUCTION.md); language build inputs need independent matching |
| Artifact inspection: module identity, type, size, CRC, build provenance | Required | Required | Required | [module evidence](../source-index/RUNTIME_MATCHES.md); names/editions do not prove equality |
| Controlled file transfer to disposable guest storage | Required for profiles/app | Required for sources/modules | Required for app/assets | Filesystem persistence is separate from MAME save states; see [live boot](../architecture/NITROS9_LIVE_BOOT.md) |
| Bounded process execution, status, timeout and cleanup | Required | Required, including language modes | Required | [os9_run](../architecture/NITROS9_RUN.md) establishes shell completion, not arbitrary GUI/BASIC09 process control |
| Complete bounded stdout/stderr transport | Useful for inspector | Required for worker diagnostics | Useful for harness | Current screen/sentinel protocol does not capture complete output; [execution design](../architecture/NITROS9_EXEC_DESIGN.md) keeps this distinction |
| Window capability query and owned-resource lifecycle | Required | Required for multi-pane UI | Required | CoWin/GFX2 sources in Window Manager report; modes/dimensions must be verified on actual EOU |
| Input/events and cancellation | Required | Required | Required | [MVKit event loop](../reference-projects/MVKIT_RECONSTRUCTION.md); callbacks/signals need target-compatible structure and cleanup |
| Graphics capture and geometry assertions | Later for rich UI | Later for rich UI | Required | Wizard offset `(192,4)` and raster evidence; screenshots show presentation, not program health by themselves |
| Memory and module-map measurements | Useful | Required | Required | [memory index](../source-index/MEMORY_MMU.md); distinguish process logical memory from physical RAM |
| ROF symbols/relocations plus linked map support | Useful for native app | Useful for helpers; not BASIC09 I-code | Required for native port debugging | [OS9ROF](../reference-projects/OS9ROF.md); ROF object analysis is not a BASIC09 procedure debugger |
| Audio timing/capture | No immediate need | Later for multimedia programs | Required for faithful intro | Wizard sound adapter is a project-specific capability, not an initial shared priority |

## Recommended next MCP priorities

### 1. Reproducible build and artifact evidence

Provide a constrained build-job interface: recipe identity, target CPU/ABI, compiler/assembler/linker versions, explicit input hashes, output paths, exit status, bounded logs and module metadata. Preserve listing/symbol/map artifacts where produced. Treat OS-9 native modules, ROF objects and BASIC09 packed I-code as distinct formats.

Begin with the working native macOS toolchain. The existing Synology coco-dev container may later serve as an optional worker after read-only discovery of its state/tool versions and an explicit job contract. Its current state is unknown; do not deploy another container. [coco-dev research](../reference-projects/COCO_DEV.md) records components and reproducibility gaps.

**Acceptance gate:** two clean builds of a minimal program have explainable artifact identities; the target module is identified before guest execution. For BASIC09, first distinguish host toolchain builds from a guest language-worker build.

### 2. Disposable guest artifact and run lifecycle

Support explicit transfer destinations and media identity, input/output hashes and retrieval of produced profiles/modules/logs. Use disposable media or an explicitly authorized development output area. Never infer that restoring `nos9_ready_v2` rolls back files. A test session should record the media baseline associated with its state and reject incompatible assumptions.

Reuse existing restore/readiness and `os9_run` where the application genuinely returns to Shell+. Add process-aware execution/cancellation only with a verified guest mechanism: startup acknowledgement, process identity, exit status, timeout policy and cleanup. GUI event loops and BASIC09 B/E/D prompts are not completed shell commands. A timed-out command is not automatically safe to overwrite with another command.

**Acceptance gate:** compile/deploy/run a minimal application, retrieve its result and recover its owned resources after an intentional failure without changing canonical media or claiming save-state filesystem rollback.

### 3. Structured bounded I/O and worker diagnostics

Build on the verified completion protocol while keeping output capture a separate capability. A guest worker using files or verified streams can return diagnostics without relying on the visible screen. Specify framing, maximum bytes, truncation, EOF, cancellation and error-versus-transport status.

For BASIC09, test standard-path routing, startup's closure of extra paths, interactive input/debug states, and packed-module output before designing a headless compiler API. A Shell+ sentinel proves shell completion; it cannot by itself identify a BASIC09 syntax error or report all lost scrolling output. [BASIC09 analysis](BASIC09_MODERNIZATION_FEASIBILITY.md) identifies these boundaries.

**Acceptance gate:** a multi-screen diagnostic result is retrieved completely within a declared limit; malformed/oversized output and hung child cases terminate predictably. Until then, report completion/status separately from output completeness.

### 4. Window lifecycle and graphics verification probes

Create a small future probe suite around public `SS.ScTyp`, `SS.ScSiz`, `SS.FBRgs` queries, DWSet/CWArea/Select, owned window cleanup and input cancellation. Record actual EOU module identities and unsupported services. Test text and graphics windows independently.

This benefits all three projects: it establishes the inspector's API, the workbench's window foundation and the wizard's display/exit boundary. Font/private WInfo discovery can follow only after version checks. For wizard graphics, assert full canvas geometry and the centered viewport separately from character-cell working area; record screenshots and deterministic frame samples where feasible.

**Acceptance gate:** create an owned window, query it, exercise safe state changes, cancel/exit and demonstrate the original terminal is usable with no unrelated window changes.

### 5. Resource and debugging observations

Add read-only measurements tied to a stopped/identified process or a cooperative guest report: logical map, workspace free bytes, module sizes, output buffers and owned window resources. Preserve symbol/build identity. Avoid arbitrary physical memory interpretation as process data.

BASIC09 needs measured editor/compiler/runtime pressure before memory redesign; the wizard needs framebuffer/code/stack budgeting. ROF disassembly/relocations may help native builds, while BASIC09 requires its own procedure/symbol interpretation. General debugger write/control support is not a prerequisite for the first experiments.

## Application plumbing worth sharing

Potentially share a small target-verified library for path/window ownership, query wrappers, validated profile/config parsing, signal/cancellation handling, and cleanup. MVKit's event, menu, document and view contracts are useful study material independent of its theme. Recovery is incomplete and no runtime compatibility certification exists, so do not make MVKit reconstruction a mandatory dependency of every project.

Keep language workspace internals, global window-table inspection and Daggorath's vector/raster algorithms in their respective projects. Do not prioritize a generic GUI framework, hardware RTC work, wider BASIC09 pointers or new transport hardware merely because they are interesting: the overlapping requirements above offer concrete tests first.

## Research gaps that affect scheduling

- BASIC09 source/binary reproducibility and the external `nitros9-languages` implementation remain unresolved.
- Public versus private EOU window API behavior needs a live, disposable probe; a matching upstream filename is insufficient.
- Full stream capture and language worker diagnostics are not supplied by current shell completion.
- MAME snapshots preserve emulator state, not external filesystem history; builds and profiles need an explicit storage lifecycle.
- Manual coverage is unavailable at the repository paths named by AGENTS.md in this checkout. Acquire/identify authoritative ABI documentation before committing to private structures or mapping algorithms.

These are gates for implementation, not claims that the applications are infeasible. The lowest-risk shared starting point is an artifact-identified minimal native application with safe window lifecycle, followed by a separate unchanged BASIC09 compile/RUNB experiment.
