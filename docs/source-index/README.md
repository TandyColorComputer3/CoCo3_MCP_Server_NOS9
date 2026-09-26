# NitrOS-9 source knowledge index

Start here when asking **“Where should I look for examples of X?”** This is a map of evidence from the EOU development VHD, not a substitute for an OS-9 API or hardware manual.

## Choose a topic

| I need examples of… | Start here | Best collections |
|---|---|---|
| Calling OS services; caller versus kernel implementation | [System calls](SYSTEM_CALLS.md), [complete usage index](SYSTEM_CALL_USAGE.md) | KERNEL, MODS/IOMan, C/LIB, BASIC09/SysCall |
| Creating processes, waiting, signals, interception | [Processes/signals](PROCESSES_SIGNALS.md) | ffork/fsend/ficpt, C/LIB/process.a, Shell+ |
| Logical memory, blocks, MMU, per-process mappings | [Memory/MMU](MEMORY_MMU.md) | fmapblk, fdatlog, fmem, fcpymem |
| Module headers, validation, linking, descriptors | [Modules](MODULES.md), [runtime evidence](RUNTIME_MATCHES.md) | flink/funlink/fvmodul, descriptors, installed boot |
| Keyboard, terminal, text input/output, local video | [SCF/VTIO](SCF_VTIO.md) | SCF, VTIO, CoWin, Term |
| Filesystems, disk drivers, emulator VHD | [RBF](RBF.md) | RBF, EmuDsk, rb1773, llcocosdc |
| Pipe streams and file-manager buffering | [Pipes](PIPES.md) | PipeMan and Pipe/Piper |
| DriveWire, serial/network transport layers | [DriveWire](DRIVEWIRE.md), [SCF/VTIO](SCF_VTIO.md) | DWIO, scdwv/rbdw, DWNET, FTP, sc6551 |
| Windows, GFX2, menus, mouse, sprites | [Graphics/windowing](GRAPHICS_WINDOWING.md) | CoWin, GSHELL, GLIB, VEFIO, GFX5, GUIB30 |
| Audio drivers and playback | [Sound](SOUND.md) | SOUNDRV, PLAY, EOU SndDrv |
| OS time, clock provider variants | [Clocks](CLOCKS.md) | Clock and Clock2 implementations |
| C wrappers, DCC, conditional CMOC code | [C development](C_DEVELOPMENT.md) | C/LIB, CONTROL, SDCCMDR |
| BASIC09 procedures, SysCall, runtime implementation | [BASIC09](BASIC09.md) | BASIC09 examples and ASM/BASIC09 |
| Shell+ launch/status behavior | [Shell](SHELL.md) | shellplus2.2a, verified completion investigation |
| Assembler/linker/disassembler internals | [Development tools](DEVELOPMENT_TOOLS.md) | MAMOU, RMA, RLINK, RDUMP |
| Actual 6309 instruction/register use | [6309](CPU_6309.md) | Conditional kernel paths, RunB, arithmetic, relocation |
| Direct I/O addresses and source symbols | [Hardware usage](HARDWARE_USAGE.md) | Source literals plus inspected DEFS |

All collection names above are navigation shorthand for `/dd/SOURCECODE/...`; each topic provides full paths. Guest paths are **not** host repository paths. Use ToolShed to read the image; do not execute a referenced build script merely to inspect it.

## Choose the reference layer

- **Frozen EOU behavior:** [runtime module observations](RUNTIME_MATCHES.md) and the live architecture investigations establish what our verified guest actually did.
- **EOU candidate implementation/examples:** the topic indexes above and [provenance/dependencies](PROVENANCE_DEPENDENCIES.md) lead to `/dd/SOURCECODE`; source candidates are not automatically exact runtime matches.
- **Modern implementation and builds:** [upstream NitrOS-9](UPSTREAM_NITROS9.md) records the pinned external checkout, CPU/level organization and recipe selection. [EOU ↔ upstream crosswalk](EOU_UPSTREAM_CROSSWALK.md) covers the twenty priority groups and runtime evidence. [Modernization notes](MODERNIZATION_NOTES.md) identify concrete newer changes, EOU differences and ABI hazards.
- **Normative contracts:** use the applicable manuals for ABI, syscall and hardware semantics. When source and runtime differ, retain both versions and identify the conflict; neither an old example nor a new commit silently overrides measured guest behavior.

The upstream checkout is an external read-only reference, not a replacement for the frozen EOU baseline. Its pinned snapshot integrates many EOU changes but does not contain every EOU enhancement. No upstream source tree is vendored here.

## Evidence and provenance first

1. Read [provenance, revisions and dependencies](PROVENANCE_DEPENDENCIES.md) before reusing a collection. Historical source, disassemblies, EOU edits, third-party code and examples coexist.
2. Consult [runtime matches](RUNTIME_MATCHES.md) when claiming behavior of the frozen `nos9_ready_v2` environment. Pass 2 identified **14 resident modules**; **10 identities** have matching bundled binaries that are byte-identical to installed boot/CMDS module segments. Editable source was **not rebuilt**.
3. Follow exact guest source paths/line references. “Observed” means present in inspected source; reusable patterns are explicitly interpretations. Conditional code may not be part of the installed binary.
4. Check a manual or identified upstream revision for ABI, hardware, instruction semantics or correctness claims not established here. The referenced `MCP/Documents` and `DOCS_INDEX.md` were unavailable in this checkout during Pass 2; no manual authority is implied.

## Scope and artifacts

Pass 2 selected **525 text/source/build/documentation files** within the **20 priority collection groups**. It reused the Pass 1 temporary byte-preserving extraction tied to the unchanged VHD hash. It did not reread all 2,284 source-tree files. Targeted DEFS/LIB inspection, module headers and two GIMEX ZIPs resolved particular dependencies. Historical GrfDrv LZH contents remain a gap because the installed archive reader cannot decode LH1.

- [Pass 1 inventory and priority list](SOURCECODE_INVENTORY.md)
- [Project catalog](PROJECT_CATALOG.md)
- [Language summary](LANGUAGE_SUMMARY.md)
- [Machine-readable inventory with compact Pass 2 annotations](sourcecode-inventory.json)
- [Pass 2 methods, commands, coverage and integrity](PASS2_METHODS.md)

## Strong reusable patterns and unresolved conflicts

- Follow caller → wrapper → dispatch → implementation, retaining language ABI and error paths: SYSTEM_CALLS and PROCESSES_SIGNALS.
- Separate descriptors, file managers, drivers and transport: MODULES, SCF_VTIO, RBF and DRIVEWIRE.
- Keep logical/process mapping separate from physical RAM and temporary mappings: MEMORY_MMU.
- Treat GUI setup/cleanup and driver initialization/termination as lifecycles: GRAPHICS_WINDOWING and SOUND.
- Compare conditional CPU variants rather than relying on filenames: CPU_6309.

Concrete conflicts include old missing defs names, non-self-contained library recipes, GIMEX flag/revision differences, older Shell artifacts, interchangeable-looking Clock2 names, and `syscall_6309.asm` deliberately selecting a 6809 frame. See the provenance audit for exact source references.

## Recommended AGENTS.md addition — proposal only

> Before implementing NitrOS-9 behavior, consult `docs/source-index/README.md` and its topic and upstream crosswalk. Use verified EOU runtime evidence for the frozen guest, provenance-qualified `/dd/SOURCECODE` for candidate implementations, and a pinned official upstream commit plus the correct CoCo 3 Level II/CPU recipe for modern implementations. Verify ABI and hardware contracts against applicable manuals. Record conflicts explicitly; never equate filenames, editions or source similarity with an exact runtime match. Do not update guest media, the reference checkout, or the frozen baseline merely to align them with upstream.

`AGENTS.md` was not changed. No MCP implementation or media changes and no commit were made for Pass 2.
