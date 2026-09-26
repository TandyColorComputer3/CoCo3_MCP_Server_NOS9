# BASIC09 modernization feasibility

Research date: 2026-09-26. **Implementation archaeology and proposed experiments; no language implementation, media or reference repository was changed.**

## Evidence, identity and limits

Consult [BASIC09 index](../source-index/BASIC09.md), [memory/MMU](../source-index/MEMORY_MMU.md), [modules](../source-index/MODULES.md), [runtime observations](../source-index/RUNTIME_MATCHES.md) and [upstream crosswalk](../source-index/EOU_UPSTREAM_CROSSWALK.md).

Source abbreviations below are actual guest paths:

- **B** = `/dd/SOURCECODE/ASM/BASIC09/basic09_ver101.asm`.
- **D** = `/dd/SOURCECODE/ASM/BASIC09/basic09.d`.
- **R** = `/dd/SOURCECODE/ASM/BASIC09/runb_ver101.asm`.
- Build/config companions: `basic09defsfile`, `makebasic09`, `makerunb`; arithmetic companions `basic09.real.add.63.asm`, `basic09.real.mul.63.asm`, `basic09.real.div.63.asm` in that directory.

Labels are more durable than line numbers. These files were read from the existing host-side CR-normalized Pass 1 extraction, without opening the VHD for writing. B combines historical Microware/Motorola code, disassembly labels, later changes and speculative comments. A comment saying “maybe”, “I think” or “6809/6309 mod” is not evidence that a proposed optimization was implemented. Findings below follow instructions and referenced structures, not every annotation.

Previously inspected **disk modules**, not a new live BASIC09 session:

| Module | Edition | Module bytes | CRC | Evidence meaning |
|---|---:|---:|---|---|
| Basic09 in `/dd/CMDS/basic09` | 25 | 22,774 | `69C082` | Installed executable module; file also contains helpers |
| RunB in `/dd/CMDS/runb` | 25 | 11,569 | `F9C7B3` | Installed runtime module; file also contains helpers |

B and R declare edition 25. This does **not** establish a reproducible binary match. Rebuild/CRC comparison and behavioral tests remain prerequisites for modifying either.

### What current official upstream actually provides here

Official checkout `/Volumes/SEDONA/Projects/nitros9-reference`, commit `f470fa52eb172b59b22c1b722074998cb42de9b1`, does not contain the current BASIC09 compiler/runtime implementation. Its [README](/Volumes/SEDONA/Projects/nitros9-reference/README.md) delegates languages to sibling `nitros9-languages`; [recipes/coco3/basic09/recipe.mak](/Volumes/SEDONA/Projects/nitros9-reference/recipes/coco3/basic09/recipe.mak) includes `basic09 runb` and samples under `$(LANGUAGES)/basic09`. That sibling is not among the available reference checkouts. The local `basic09.hp` and `runb.hp` help files establish launch intent, not internal architecture.

Therefore “current upstream BASIC09 versus EOU BASIC09” remains unresolved. Current upstream kernel/module/window implementations can inform process cooperation, but cannot be substituted for a missing language implementation. `MCP/Documents/` and `DOCS_INDEX.md` are also absent here; ABI/manual confirmation remains a recorded gap rather than invented citations.

## 1. Architecture map

```text
B start / command dispatcher
  ├─ LOAD text → per-line compiler → workspace I-code + symbols
  ├─ EDIT → same per-line compiler → relocate/mutate same workspace
  ├─ LIST / SAVE ← formatted reconstruction from workspace
  ├─ binder → resolve statement/symbol references
  ├─ PACK → compact/transform procedure → OS-9 I-code module file
  └─ RUN → procedure lookup/link → invocation/storage frame
                   → statement/expression interpreter → OS-9 I/O

RUNB: separate executable containing runtime machinery;
      linked/loaded procedures still execute inside RUNB's address space.
```

| Area | Source entry points | Verified behavior |
|---|---|---|
| Startup | B `start`, `L07B5`, `L0870`; R `start`, `L0222` | Initialize direct-page state, workspace/directory/stack pointers, signal intercept and absolute JMP dispatch vectors; establish command I/O paths. B optionally loads/runs a named procedure. |
| Commands/modes | B command table near `L00DC`, command loop, `L1590` | Separate B/E/D prompt modes and command sets for system, editing and debugging. An OS shell prompt detector does not recognize these as shell readiness. |
| Editor | B `L1590`, `L1606`, `L19B1`, `L1A10` | Select/create an editable procedure, compile incoming lines, locate insertion points, move I-code and adjust pointers/sizes. Syntax-error tokens can remain in the editable representation. |
| Source representation | B `L1606`, `L1677`, `L0CF4`, `L0D02`; D token/symbol definitions | Editable source is tokenized/intermediate procedure data with names and line information, not just an array of original text lines. LIST/SAVE reconstruct text. |
| Parser/compiler | B `M.COMPIL` → `L1CA5`/`L1CB5`, `L1E29`, `L2430`, `L252A` | Function dispatch, line processing, name scanning and token lookup feed workspace I-code. Exact interfaces are private, not a standalone compiler API. |
| Binder | B `L255A`, `L256A`, `L2692`, `L30A0` | Statement/type/symbol processing and reference resolution; called by editing/loading/PACK paths. Do not treat it as an independently callable linker. |
| Procedure records | D `P.*`; B `L0EFD`, `L0F91`, `L0FB0` | Procedure directory, headers and link/load logic track workspace and OS-9 modules; workspace movement updates directory entries. |
| PACK | B `L0B51` through packing/output path, `SMASH` | Transforms an editable procedure into compact Sbrtn+ICode form, updates header/size, computes parity/CRC and writes module bytes. Details below. |
| Execution | B `M.STMTS` → `L31E8`; `M.EXPRSN` → `L3C09`; RUN `L3A8A` onward | Statement and expression interpreters operate on active I-code and variable storage; RUN resolves a procedure and constructs execution state. |
| Module loading | B `L0F91`/`L0F96` | Tries `F$Link`, falls back to `F$Load`, stores module pointer. This is different from loading source text. |
| Source loading | B `L0AC3`, `L0B2D`, `L0B3C`, `L1606` | `I$Open`/`I$ReadLn`, PROCEDURE recognition and line compilation; a 256-byte input buffer is used. Do not infer an unrestricted modern text format. |
| Variables | D `S.*`, `P.VARC`, `P.DSCB`; B binder and runtime initialization | Typed symbol/descriptive records and per-invocation storage; runtime arrays/strings/records consume local memory in addition to source/I-code. |
| Errors/debugging | B `L1287`, `L39FB`, `L3A14`, `SP.ERR`; D `U.ERRA`, `U.ERRS` | Saved error code, per-invocation trap state, stack restoration, printing/debug transitions. No external structured debugger protocol is present in these paths. |
| Files/processes | B `L3970`, `L397D`, `L39A0`, `L39BC`, I/O interpreter paths | OS-9 paths provide file I/O. SHELL forks/waits; CHAIN replaces the process image. BASIC09 RUN is not SHELL/Fork. |

### Editor/compiler/runtime coupling

At startup B installs six JMP vectors: `M.COMMAND`, `M.COMPIL`, `M.BINDER`, `M.STMTS`, `M.EXPRSN`, `M.CNVIO`. These refer to code within the loaded language module and share its direct-page state. `L1606` calls compiler function 10, examines generated tokens and invokes `L19B1` to change the active procedure. `L0FB0` moves workspace contents and updates procedure-directory addresses. This is tight in-process coupling, not a text editor talking to an existing compiler service.

An external editor can replace the human editing experience through source files while leaving that coupling intact internally. Extracting the compiler or exposing incremental compile/debug RPC would require a deliberate BASIC09 interface change.

## 2. Memory map and pressure

### Source-defined layout, relative to process data base

B and R reserve the following initial layout. These are **relative process-data offsets**, not physical CoCo addresses:

| Offset / field | Source evidence | Role / caution |
|---|---|---|
| `$000..$0FF` | D direct-page definitions; startup clear | Globals, flags, 16-bit pointers and installed dispatch jumps |
| `$100..$1FF` | `u0100`, `I.IOBG` | Temporary I/O buffer |
| `$200..$2FF` | `u0200` | Reserved area; source annotation is uncertain. Do not assign it a confident undocumented purpose. |
| `$300..$3FF` | `u0300`, `I.OPBG`, initial S | Initial stack/operand staging region; execution changes stack state |
| `$400..$4FF` | `u0400`, `G.DIRA`, startup clear | Procedure/module pointer directory with two-byte entries |
| `$500` onward | `G.PRCA`, `I.STBG` startup | Dynamic procedure/workspace region. The declaration names a `$500` I-code buffer, but it is not an additional permanently isolated buffer beyond the dynamic start. |
| Declared initial data extent | `u0600 rmb $2000-.` for Level II | 8 KiB default request; not a fixed maximum or the entire process image. Level I branch uses `$1000`. |

B `L0870` computes available workspace as:

```text
G.VARS = G.WSPA + G.WSPS - G.PRCA - G.PRCS
```

B `MEM` (`L0918`) checks the occupied region, calls `F$Mem`, and updates workspace size. The interpreter, linked modules, stack, variable frames and OS mapping constraints still occupy the same logical process address space. Two megabytes of physical RAM does not turn these 16-bit logical addresses into a larger flat workspace. See the EOU `KERNEL/fmem.asm`, `fmapblk.asm`, `fdatlog.asm` evidence in the [memory index](../source-index/MEMORY_MMU.md).

### Structures that constrain separation or banking

| Structure | Actual fields | Consequence |
|---|---|---|
| Workspace globals | D `G.WSPA`, `G.WSPS`, `G.DIRA`, `G.PRCA`, `G.PRCS`, `G.VARS` | Two-byte base/size/directory state; workspace relocation is explicitly managed. |
| Active interpreter/compiler state | D `I.APRC`, `I.ASTR`, `I.ICBG`, `I.ICPT`, `I.ICLM`, `I.SYMT`, `I.STBG`, `I.STSP`, `I.OPBG`, `I.OPSP` | Direct logical addresses couple code, symbols, temporary strings and operand storage. |
| Procedure header | D `P.MHDR`, `P.SIZE`, `P.PGMB`, `P.VARC`, `P.SYMB`, `P.DSCB`, `P.PRCS`, `P.DATA`, `P.EXEC`, `P.STAT`, `P.NAMS` | Mix of sizes, flags and module-relative offsets; not all fields are absolute pointers. B combines offsets with a module base to access sections. |
| Invocation frame | D `U.PROC`, `U.S`, `U.U`, `U.DATA`, `U.ICPT`, `U.STBG`, `U.PRLM`, `U.ERRA`, `U.SBSP`, mode/error flags | Suspended calls retain procedure, stack, parameter and trap references. Swapping an active procedure out cannot ignore these references. |
| Symbols | D `S.DEFM`, `S.SHPM`, `S.TYPM` | Definition kind, array shape and scalar/record type plus names/references. A source browser can index names externally; live variable inspection needs these version-specific structures. |
| Machine-language call packets | B `L3A8A` dispatch and subsequent parameter construction | Pointer/size arguments refer to this process; forwarding raw pointers to another process is invalid. |

**Major consumers:** mapped interpreter/runtime code and helpers; editable I-code; names, type descriptors and binding/reference data; loaded procedures; runtime variable/array/string storage; invocation/operand/string stacks; compilation and PACK scratch. PACK explicitly needs temporary space related to symbol count (`2 × count + $100` in its check). There is no trustworthy single “maximum source size” independent of workload.

RUNB's observed module is 11,205 bytes smaller than Basic09's, but that subtraction is **not** a measured increase in available user data: mapping granularity, helpers, linked procedures and requested workspace matter. Measure both on identical workloads before promising capacity.

## 3. PACK, RUN and RUNB

PACK (`L0B51`) rejects unsuitable/erroneous procedures, invokes binder passes, compacts symbol/reference information, preserves needed procedure names, changes type to `Sbrtn+ICode` (`$22`), updates flags/header parity and `F$CRC`, and writes the result. It changes workspace representation in place. It does not generate native 6309 machine code. Packed procedures are rejected by the ordinary edit path; preserve a text master before packing.

RUN's dispatch distinguishes editable procedures, packed I-code and machine-language subroutines (`$21`). Machine-language execution reaches an in-process `jsr`; BASIC09 procedure execution builds interpreter state in the same process. Loading modules by name can reduce the need to keep all editable source loaded, but does not demonstrate automatic eviction or unlimited call graphs.

R has the same initial pointer-directory/data layout and runtime architecture, without the full interactive editor/compiler. Its startup links/loads the requested procedure and executes it. This is strong evidence for a separate **run worker**, not proof that R is a standalone source compiler or that two processes can share a running BASIC09 activation.

SHELL (`L39BC`) uses `F$Fork`/`F$Wait`, checks the child's returned status and feeds errors into BASIC09's error machinery. CHAIN (`L39A0`) uses `F$Chain`; a successful replacement does not return to the previous language environment. These are useful models for a controller's process lifecycle, but should not be substituted silently for RUN semantics.

## 4. Errors, I/O and automation implications

B `L39FB` stores `I.ERR`; if `U.ERRS` enables a trap it restores the invocation's stack/trap address, otherwise it follows reporting/debug/unwind paths. Syntax-error tokens (`T.ERRI`) are detectable by `L1677`. The system-command error path and runtime ON ERROR path are not a uniform machine-readable diagnostics API.

Startup closes paths 3–15 and duplicates path 2 for command output; ordinary execution has its own I/O path fields. A controller cannot simply inherit an arbitrary extra pipe and assume it survives. A future worker must deliberately arrange standard paths and test which path receives prompts, listings, errors and program output. BASIC09 may wait in editor/debug/input modes; shell-marker completion from `os9_run` alone is insufficient to control those states.

An output window can display captured streams once transport is verified. A variables/debug window needs either a structured debugger addition or a version-specific stopped-process adapter. Scraping arbitrary live interpreter addresses while it runs is not a reliable debugger design.

## 5. 6309-specific changes and compatibility

`basic09defsfile` selects H6309 and Level II. B/R contain real conditional instruction changes: startup `LDW`/`TFM`, `LDQ`/`STQ` frame/data copies, and other 6309 instruction paths; arithmetic includes choose `.63.asm` versus `.68.asm`. These are implementation changes within the language engine, not evidence of wider BASIC09 pointers or an automatic new numeric language model.

The available `.63.asm` arithmetic files require numeric regression tests before reuse. Conditional references to `.68.asm` files do not establish that a complete alternative build is present. `syscall_6309.asm` even sets H6309=0 internally; filename inference is unsafe. Historical comments about signals or further optimization are proposals, not current language features.

Compatibility tests must cover source LOAD/SAVE normalization, procedure names and parameter passing, packed module formats/CRC, arrays and records, string sizes, numeric boundaries/rounding, errors/traps, RUN versus SHELL/CHAIN, machine-language helpers and GFX2. Preserve exact target identification; edition 25 alone is not an ABI guarantee.

## 6. Architectural options and evidence grades

“Supported boundary” means source shows a usable boundary; it does not mean the proposed application has been built or live-tested.

| Option | Evidence / feasibility | Capacity and semantics consequences |
|---|---|---|
| External editor + source files | **Supported boundary:** B LOAD reads text; SAVE reconstructs it. mvedit demonstrates OS-9 document/editor/event plumbing. | Best first approach. Keep complete original text as master; editor UI buffers leave BASIC09's process. Active compiler/runtime remains bounded. |
| Procedure-at-a-time editing/build | **Supported building blocks:** PROCEDURE recognition, per-procedure directory, EDIT/LOAD/PACK. | Larger stored projects need not all be editable in one workspace. Dependency ordering and unresolved cross-procedure binding need experiments before guaranteeing isolated compilation. |
| Separate source/document process | **Plausible composition:** files and OS-9 processes are present; no BASIC09 document-service protocol exists. | Keep the editor's own memory bounded using file-backed/chunked documents; moving every file into another 64K process alone is not sufficient. |
| Module-backed packed procedures | **Existing mechanism:** F$Link/F$Load and packed RUN. | Useful distribution/execution boundary. Mapped code, active variables and call frames still need logical space. No proven automatic overlay policy. |
| Temporary backing files | **Supported storage boundary:** ordinary RBF operations and source/module files. | Can hold inactive sources/build products outside RAM. Requires save/error/cleanup design, correct text encoding and source-map identity; does not page live frames automatically. |
| Separate compile worker | **Partial:** BASIC09 can LOAD/PACK, but shares interactive command state. | A controller can prototype scripted sessions. No verified headless compiler status/diagnostic protocol yet; compiler extraction would modify B. |
| Separate RUNB worker | **Strongest execution candidate:** R startup and module-loading paths. | Separates editor from runtime failure and reclaims compiler code in the run process. UI/controller memory remains in its own process. Must test parameter/standard-path handling. |
| Pipes | **OS support exists:** EOU PipeMan sources in [pipe index](../source-index/PIPES.md); no verified BASIC09 IDE protocol. | Good candidate for bounded requests/diagnostics. Avoid full-duplex deadlocks and distinguish EOF, process exit and language prompt; inherited-path issue above matters. |
| Shared/data modules | **Module mechanisms exist**, but no BASIC09 external-source-store integration found. | Could carry immutable assets or serialized metadata after mapping/lifetime verification. Raw compiler pointers cannot simply point into another task. Writable shared documents require a new protocol and synchronization. |
| Separate runtime per procedure | **Not a transparent replacement:** RUN passes in-process storage/parameters. | Explicit service-oriented applications may use messages/files; converting arbitrary calls to process messages would change behavior and require compiler/runtime work. |
| Overlays / banked active workspace | **Not established for unmodified BASIC09.** Kernel block mapping exists; the listed pointers/frames remain local. | Requires handle/relocation/lifetime design, safe suspension and extensive interpreter changes. Physical RAM availability is not evidence this is an appropriate first solution. |
| External symbolic debugger | **No existing stable service found.** Private symbols/frames and interactive debugging exist. | A later structured interface could expose stopped-state variables/stack/errors; do not promise transparent debugging of stripped PACK output. |

## 7. What can change without changing the language

External source views, procedure browser, file-backed project organization, menus/mouse, find/replace, undo, build manifests and a separate RUNB output window can preserve language semantics if they feed the original compiler/runtime and preserve source encoding. MVKit's document/event/menu concepts are useful, but its recovered library has missing pieces and its editor examples do not prove a BASIC09 IDE implementation. See [MVKit](../reference-projects/MVKIT_RECONSTRUCTION.md) and [application patterns](../reference-projects/MVEDIT_MVDRAW.md).

Changes to incremental compilation, compiler-service entry points, error records, symbolic debugger access, >64K active data addressing, automatic procedure eviction or pointer representation require modifying BASIC09 or providing a carefully versioned adaptation. Source-level project growth is achievable sooner than growth of one procedure's live arrays/stack.

Atari ST BASIC is a possible visual/workflow reference for panes and procedure navigation only. No Atari code, memory model, compiler service or OS abstraction is assumed to transfer to OS-9 in this proposal.

## 8. Staged roadmap and acceptance gates

1. **Establish a reproducible language baseline.** Resolve `nitros9-languages` reference availability; identify exact EOU build inputs/helpers; reproduce B/R or document byte differences. Run an unchanged small corpus, including LOAD → LIST/SAVE → PACK → RUNB and trapped/untrapped errors. Measure workspace under editing and execution. Do not execute existing absolute-path build scripts against canonical media.
2. **External single-procedure workbench.** Plain-text master, procedure browser, one edit view and explicit build/run actions. Use disposable source/output directories and a separate BASIC09/RUNB worker. First verify worker completion and diagnostics transport; do not use fixed sleeps. Preserve original files when SAVE formatting differs.
3. **Bounded multi-procedure projects.** Manifest/dependency inventory, per-procedure files and incremental rebuilds only where binding experiments justify them. Add output/error panes with source identity. Demonstrate a project larger on disk than any one compiler workspace without claiming larger active runtime data.
4. **Rich UI and controlled debugging.** Reuse verified window lifecycle/menu/mouse plumbing; add multiple views with bounded caches. Prototype structured stopped-state debugging only after the exact interpreter ABI is established.
5. **Internal modernization only for measured bottlenecks.** Consider compiler service hooks or memory-handle changes as separately versioned work, backed by format/semantic regression tests. Defer banking and automatic overlays until an actual workload proves files/modules/process separation inadequate.

The first decisive experiment is not a GUI mockup: it is an unchanged BASIC09 worker compiling a small text procedure, producing a checked packed module, and a separate RUNB worker executing it with reliable diagnostics and cleanup.
