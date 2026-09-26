# EOU SOURCECODE inventory — Pass 1

## Scope and results

Read-only inventory of `/dd/SOURCECODE` in `media/63SDC-MCP-DEV.VHD`. The guest `/dd` prefix maps to ToolShed's image-root namespace: `media/63SDC-MCP-DEV.VHD,SOURCECODE`. ToolShed opened this filesystem directly; no mount, partition offset, emulator, guest commands, or image modification was necessary.

| Measure | Result |
|---|---:|
| Regular file entries | 2,284 |
| Subdirectories (excluding SOURCECODE itself) | 155 |
| Directories including SOURCECODE | 156 |
| Logical file bytes | 42,784,513 (40.80 MiB) |
| Directory-content bytes, including root | 93,152 |
| File + directory-content bytes | 42,877,665 |
| Immediate project/loose-file groups | 65 |
| Exact duplicate-content groups | 92 |
| Files participating in those groups | 202 |

Sizes are logical lengths from file descriptors, not allocated disk usage. Files inside archives are not counted separately. Generated listings, binaries, resources and duplicate files are included. `.` and `..` are excluded. Every directory header returned by recursive ToolShed traversal was reconciled with the directory entries (156 headers); every extracted file length matched ToolShed's byte count. This establishes inventory consistency, not a filesystem repair/check result.

| Top-level category | Files | File bytes | Principal contents |
|---|---:|---:|---|
| ASM | 1,758 | 39,604,731 | System source, libraries, applications, games, listings, modules and archives |
| BASIC09 | 324 | 1,802,473 | BASIC09 applications, games, GUI/graphics procedures and supporting files |
| C | 202 | 1,377,309 | C tools/applications, assembly runtime wrappers, headers and build artifacts |

The complete project list is in [PROJECT_CATALOG.md](PROJECT_CATALOG.md). [LANGUAGE_SUMMARY.md](LANGUAGE_SUMMARY.md) records content-based language classifications, all extension counts and caveats. [sourcecode-inventory.json](sourcecode-inventory.json) contains all 2,440 entries, including the root, sizes, raw SHA-256 hashes for files, project assignment, conservative provenance, content types, topic clues and source evidence line numbers. It does not embed source bodies.

## Twenty high-value collections for a development knowledge layer

These are research priorities based on inspected files and content searches, not endorsements of correctness or recommendations to run them. Paths are relative to `/dd/SOURCECODE`.

| Priority collection | Teaching value and concrete evidence |
|---|---|
| `ASM/NITROS9/KERNEL` | `fid.asm`, `ffork.asm`, `fchain.asm`, `fsend.asm`, `ficpt.asm`: F$ services, processes and signals. `fmem.asm`, `fmapblk.asm`, `fdatlog.asm`, `fcpymem.asm`: memory/MMU and logical/physical translation research. Header comments describe call contracts; verify against the applicable OS revision. |
| `ASM/NITROS9/MODS` | `ioman_beta5.asm`, `init.asm`, `sysgo.asm`: system I/O dispatch and initialization. `llcocosdc.asm` contains a module header and driver entry table. |
| `ASM/NITROS9/SCF` | `scf_ver100.asm`, `vtio_beta6.asm`, `cowin_beta6.asm`: SCF, terminal input, windows, IRQ interaction. Includes `sc6551.asm`, `scbbt.asm`, terminal descriptors and `s16550v16_lOS9.asm` for serial research. |
| `ASM/NITROS9/RBF` | `rbf_postbeta6.asm`, `rb1773.asm`, `emudsk_beta601.asm`, RAM drivers and descriptors: file manager/device boundaries and disk I/O. The RBF header itself documents unresolved issues—do not teach comments as resolved behavior. |
| `ASM/NITROS9/PIPE` | `pipeman.asm`, `pipeman_named.asm`, `pipeman_beta6.asm`, descriptors: pipe/file-manager implementation and revision comparisons. |
| `ASM/NITROS9/DW` | `dwio.asm`, `dwread.asm`, `dwwrite.asm`, `rbdw.asm`, `scdwv.asm`, `scdwp.asm`: DriveWire transport, serial transfer and SCF/RBF interfaces. |
| `ASM/DWNET` + `ASM/FTP` | `dwnet.a` exposes `/N` and I$SetStt usage; FTP `main.a` identifies Bill Nobel and RFC 959. Useful application-level networking complement to drivers. |
| `ASM/SHELL` | `shellplus2.2a.asm`: process launch, I/O, intercepts and shell behavior. Header explicitly identifies modified disassembly; match to runtime before relying on it. |
| `ASM/BASIC09` | BASIC09/RunB implementation; `syscall_6309.asm`, `gfx2_ver1.asm`, `inkey.asm`; `basic09.real.*.63.asm` and `runbcd.asm` for 6309 optimization leads. Historical Microware/Motorola core plus later modifications. |
| `ASM/GSHELL` | Derived graphical shell source, multiple revisions and listings: application/window integration, process and signal handling. |
| `ASM/GLIB` | Sprite/graphics library with documentation and Smash examples. `GLIB.tech` discusses save/restore buffers and IRQ timing. |
| `ASM/VEFIO-WINFO` | `vefio.asm`, window-info utilities, `windowinfo.b09`: graphics format and window-state access across assembly/BASIC09. |
| `BASIC09/GFX5` + `BASIC09/GUIB30` | Typed BASIC09 procedures, GFX2 calls, menus, mouse and syscall wrappers; concrete GUI demos. GUIB has explicit historical author credits. |
| `C/LIB` | `process.a`, `signal.a`, `intercept.a`, `mem.a`, `mod.a`, `io.a`, `syscall.a`: assembly wrappers connecting C runtime conventions to OS-9 services. |
| `C/CONTROL` + `C/COCOTHELLO` | EOU control panel and game examples using window/mouse interfaces. CONTROL has explicit EOU project attribution; COCOthello includes `_ss_mous` calls. |
| `C/SDCCMDR` | Hardware-oriented C/assembly, SDC communication, OS9/DCC build and CMOC/DECB conditional branches. Illustrates portability boundaries rather than one universal target. |
| `ASM/SOUNDRV` + `ASM/PLAY` | Documented SCF sound driver/client and audio player; interrupt and direct-hardware tradeoffs. SounDrv source credits Allen C. Huffman/Sub-Etha. |
| `ASM/NITROS9/CLOCKS` | Multiple Clock2 implementations (`disto2`, `disto4`, `messemu`, `dw`, `soft`, others) and clock core: historical RTC/time paths. Inventory does not revalidate their hardware behavior. |
| `ASM/NITROS9/GIMEX` | Boot, relocation, kernel and SDC variants plus archives: expanded-memory/hardware integration and 6309 build leads. Needs careful variant comparison. |
| `C/MAMOU`, `C/RMA`, `C/RLINK`, `C/RDUMP` | Assembler, linker/output formats and 6309 disassembler internals; useful for module/object tooling. These are host/tool algorithms in C, not all guest assembly examples. |

`ASM/ALIB`, `ASM/NLIB`, `ASM/BLANK`, `C/TEST` and loose BASIC09 demos are additional approachable library/example material. Older `NITROS9-L3`, `OLD_NITRO`, and `2_01_boot` should be isolated as historical material until lineage is established.

## Duplication and revision families

Raw SHA-256 comparison found 92 duplicate groups (202 file entries), representing 732,970 redundant logical bytes if one copy per group is retained. No files were removed. The JSON lists every group.

- `ASM/NITROS9115_PATCHES` and `ASM/NITROS9/NITROS9_115_PATCHES` contain the same eight names and byte-identical contents: an exact duplicate project tree.
- `ASM/BLANK` and `ASM/NANOGPU` share 12 distinct file hashes; related FTP/NMAIL frameworks also share files. Shared templates/relocatables do not make entire applications identical.
- DW implementations under `NITROS9/DW` and `NITROS9/MODS`, `VIEW44` versus `VIEW_4_5a`, `NITROS9/CMDS/SDC2` versus `ASM/SDC2`, BASIC09 versions, and `GIMEX/NEW` are apparent revision/near-duplicate families. Some share exact files, others differ. No semantic equivalence is claimed.
- `BASIC09/FONTMAN/FontMan.B09` and `FONTMAN/DCOM/FontMan.B09` have the same basename but did **not** appear in the exact-duplicate groups. Likewise, `ORIGINAL`, `ORG`, `NEW`, `beta`, and version suffixes are leads, not proof of lineage.

## Exact inspection method and commands

Commands ran from `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9`. The executable was:

```text
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9
```

Executed ToolShed commands (the executable above replaces `OS9` here):

```sh
OS9 dir '-?'
OS9 dir media/63SDC-MCP-DEV.VHD,SOURCECODE
OS9 dir -a -e -r media/63SDC-MCP-DEV.VHD,SOURCECODE > /private/tmp/sourcecode-dir.txt
OS9 copy '-?'
OS9 ident media/63SDC-MCP-DEV.VHD,SOURCECODE/ASM/2_01_boot/IOMan
```

An initial unquoted `-?` help attempt was rejected by zsh glob expansion before ToolShed ran. Quoting it corrected this. The inventory and extraction commands succeeded.

For every regular-file entry, a Python loop invoked this exact argument-vector pattern (2,284 invocations), with no shell interpolation and **no `-l` translation or `-r` rewrite flag**:

```python
subprocess.run([
    '/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9',
    'copy',
    'media/63SDC-MCP-DEV.VHD,' + entry['path'][4:],
    '/private/tmp/sourcecode-audit/raw/' + entry['path'][4:]
], capture_output=True)
```

`entry['path']` is the JSON guest path; `[4:]` removes `/dd/`. Host parent directories were created first. Thus a concrete invocation was:

```sh
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 copy media/63SDC-MCP-DEV.VHD,SOURCECODE/ASM/SHELL/shellplus2.2a.asm /private/tmp/sourcecode-audit/raw/SOURCECODE/ASM/SHELL/shellplus2.2a.asm
```

The recursive listing parser used `Directory of ...,...` headers for current paths and parsed owner, date, attributes, hexadecimal FD sector, decimal byte count and name. A leading `d` attribute identifies directories; `.`/`..` were excluded. It asserted directory-header reconciliation and every raw file length. SHA-256 was computed on raw file bytes, and matching hashes grouped duplicates. Temporary text copies normalized CR for `rg`/header inspection; no normalized copy was used for size or hash calculations. Text/type heuristics are documented in LANGUAGE_SUMMARY. No archives or embedded disk images were opened, no scripts compiled/executed, and no full source tree was added to the repository.

ToolShed `os9/os9dir.c` was inspected to verify `-a`, `-e`, `-r`, decimal byte counts, and read-only open flags. `os9/os9list.c` showed that `list` transforms CR-delimited lines, so raw `copy` was used for this inventory's content hashes. `os9/os9copy.c` and its help were inspected for copy options. Reproducibility does not depend on the temporary copies: all guest paths and hashes are in the JSON, and the copy loop can regenerate them from the same image.

## Media integrity

Before inspection:

```sh
shasum -a 256 media/63SDC-MCP-DEV.VHD
```

After inspection, both Python `hashlib.file_digest(..., 'sha256')` and `shasum -a 256` were used.

| Point | SHA-256 |
|---|---|
| Before | `4c6bdc0cd2f68aa57a9c75f75f15fe429f30926aa522917dd8d14aa1b9ae622e` |
| After | `4c6bdc0cd2f68aa57a9c75f75f15fe429f30926aa522917dd8d14aa1b9ae622e` |

**Development VHD unchanged.** Stock VHD, boot floppy and save states were not opened or modified by this inventory. MCP source and existing uncommitted work were preserved. No commit was made.

## Pass 2 priorities

1. Establish per-file provenance, licenses, release/revision lineage and whether each file is original, disassembled, patched or generated; compare selected system files with identified upstream revisions.
2. Choose canonical revisions without deleting historical alternatives. Perform normalized/structural comparisons of the near-duplicate families, excluding generated listings/object files from source equivalence.
3. Index F$/I$ call sites, module headers, entry points, descriptor/driver/file-manager relationships and build dependencies. Resolve external `/dd/defs` references in a separately scoped read-only pass.
4. Separate 6809, conditional 6309 and 6309-only routines; verify optimization claims and ABI/register assumptions from applicable manuals/source. No instruction-set certification was performed here.
5. Inspect archives in an isolated host directory, recording container-to-member provenance. They may contain significant additional source; current totals count archives as single files.
6. Validate DCC versus CMOC branches and toolchain versions; determine which supplied make/scripts have stale absolute paths. Do not run guest build scripts against development media.
7. Deep-index SCF/VTIO/CoWin/GrfDrv and Shell+ for console, process and signal work; match revisions to actual EOU modules before using them as runtime evidence.
8. Index graphics/mouse/GFX2 and serial/DriveWire/audio examples with explicit platform, required modules, side effects and hardware assumptions.
9. Review unclassified/binary/high-bit files and ambiguous `.bas`, `.a`, `.r`, `.d` contents; distinguish tokenized procedures, relocatables, resources and readable source.

This pass inventories evidence; it does not establish a universal authoritative OS-9 knowledge base or summarize each source file.
