# Pass 2 methods, boundaries and integrity

[Navigation](README.md)

## Reproducible selection

Pass 1 inventories every path, raw size and SHA-256. Pass 2 reused temporary raw files under `/private/tmp/sourcecode-audit/raw/SOURCECODE` after confirming the development image still had the Pass 1 hash. Selection was constrained to these priority prefixes:

```text
ASM/NITROS9/KERNEL  ASM/NITROS9/MODS  ASM/NITROS9/SCF
ASM/NITROS9/RBF     ASM/NITROS9/PIPE  ASM/NITROS9/DW
ASM/NITROS9/CLOCKS  ASM/NITROS9/GIMEX
ASM/DWNET          ASM/FTP          ASM/SHELL
ASM/BASIC09        ASM/GSHELL       ASM/GLIB
ASM/VEFIO-WINFO     ASM/SOUNDRV      ASM/PLAY
BASIC09/GFX5       BASIC09/GUIB30
C/LIB              C/CONTROL        C/COCOTHELLO
C/SDCCMDR          C/MAMOU          C/RMA
C/RLINK            C/RDUMP
```

These are the constituent prefixes of the twenty grouped priority rows, not twenty-seven newly selected projects. From the Pass 1 JSON, regular files whose `content_kind` was source, include/header, include/definitions, build script, documentation or text/unclassified were selected. Generated `.listing`/`.oldlisting` files were excluded. Result: **525 files**. Each selected raw-file hash was checked against Pass 1. No new indiscriminate traversal/read of all 2,284 files was performed.

Text was normalized CRLF→LF and CR→LF for line references. The bounded scan collected:

- F$/I$ literal tokens, separating non-comment `os9 NAME` invocations from references/dispatch/comments. Symbols were cross-checked against declarations (`RMB`, `EQU`, `SET`) in current `/dd/DEFS/os9.d`: 99 recognized, 30 unconfirmed/local/prose tokens. All 1,386 file/token pairs remain discoverable in SYSTEM_CALL_USAGE.
- `$FFxx` and `0xFFxx` literals, plus symbolic definitions in inspected dependency files. Hardware identity is not inferred from the number alone; indirect access and non-I/O constants remain caveats.
- Extended assembly mnemonics and register-transfer operands. Register-pair validation filters labels such as `ADDR lbsr ...` and prevents a V mention in a trailing comment from becoming a V-register use. Conditional branches remain included; static sites are not runtime traces. C instruction-name tables are not counted as executing guest opcodes.
- Include directives, labels, header provenance/version statements and build-file references. Semantic interpretation was limited to the curated entry points, rather than summarizing all selected source bodies.

Pass 2 does not fully parse assembler macros, BASIC09 numeric syscall wrappers or compiler/linker resolution. An absent lexical hit is not proof that an indirect capability is absent.

## Exact host-side utility methods

ToolShed executable:

```text
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9
```

From the repository root, the following read-only commands were used (substitute that full executable for `OS9`):

```sh
shasum -a 256 media/63SDC-MCP-DEV.VHD
OS9 dir -e media/63SDC-MCP-DEV.VHD,DEFS
OS9 dir -e media/63SDC-MCP-DEV.VHD,LIB
OS9 copy media/63SDC-MCP-DEV.VHD,DEFS/deffile /private/tmp/sourcecode-pass2/defs/deffile
OS9 ident media/63EMU.DSK,OS9Boot
OS9 copy media/63EMU.DSK,OS9Boot /private/tmp/sourcecode-pass2/binaries/OS9Boot
```

The same DEFS copy argument pattern was used for `defsfile`, `os9.d`, `coco.d`, `cocovtio.d`, `drivewire.d`, `pipedefs`, `rbsuper.d`, `cocosdc.d`, `rbf.d`, `scf.d`, `vdgdefs`. All were read successfully. Attempts for `os9defs`, `rbfdefs`, `scfdefs` produced `error 216 ... pathname not found`. ToolShed returned process exit code zero despite those diagnostics; success was checked using stderr and destination existence. Thus missing includes are not silently counted as successful reads.

Host module identification used exactly this pattern on **88 priority-scope bundled module files** already classified by their OS-9 header signature:

```text
OS9 ident /private/tmp/sourcecode-audit/raw/<guest-path-with-/dd/-removed>
```

It also used:

```text
OS9 ident media/63SDC-MCP-DEV.VHD,CMDS/shell
OS9 ident media/63SDC-MCP-DEV.VHD,CMDS/basic09
OS9 ident media/63SDC-MCP-DEV.VHD,CMDS/runb
OS9 ident media/63SDC-MCP-DEV.VHD,CMDS/gfx2
OS9 ident media/63SDC-MCP-DEV.VHD,CMDS/gshell
OS9 ident media/63SDC-MCP-DEV.VHD,CMDS/ident
```

The first five CMDS files were copied to `/private/tmp/sourcecode-pass2/binaries/<name>` with `OS9 copy image,CMDS/<name> <host-target>`, without translation. Multi-module packs were split in the order and lengths reported by ToolShed ident; summed lengths were checked against file size. Bundled artifacts were compared byte-for-byte with those segments. CRC/edition/type comparisons against live ident are recorded separately in RUNTIME_MATCHES. No source was compiled.

## Targeted archive work

Python's standard `zipfile.ZipFile` enumerated and read members of only:

```text
ASM/NITROS9/GIMEX/GIMEX_source.zip
ASM/NITROS9/GIMEX/gimex_src_frombill08-25.zip
```

For each member, SHA-256, raw-byte comparison and CRLF/CR-normalized comparison against the corresponding loose and NEW files were computed. Only the definitions member was read for dependency settings; this was not a wholesale semantic read of archive contents. Results are in PROVENANCE_DEPENDENCIES.

The exact archive-reader commands were:

```sh
tar -tf /private/tmp/sourcecode-audit/raw/SOURCECODE/ASM/grfdrv.lzh
tar -xOf /private/tmp/sourcecode-audit/raw/SOURCECODE/ASM/grfdrv.lzh readme.txt
tar -tf /private/tmp/sourcecode-audit/raw/SOURCECODE/ASM/grfdrv0724_1998.lzh
tar -xOf /private/tmp/sourcecode-audit/raw/SOURCECODE/ASM/grfdrv0724_1998.lzh read.me
```

Directory listing succeeded; both member reads failed with unsupported LH1 compression. No broad extraction/fallback download was attempted and no unread member is cited as inspected source.

## Live survey

Temporary media copies were created with host `shutil.copyfile` from the development VHD and 63EMU.DSK. Both still match originals after the survey. A temporary host wrapper imported the existing built MCP, supplied those copy paths, and exposed the existing bridge text reader for observation under its ownership lease. No MCP source, Lua script or tool implementation was edited.

Actual hardware arguments inherited from the frozen configuration:

```text
coco3h -ext multi
-ext:multi:slot1 ""
-ext:multi:slot2 ssc
-ext:multi:slot3 ""
-ext:multi:slot4 scii
-ext:multi:slot4:scii:meb rtime
-ramsize 2M
-flop1 /private/tmp/sourcecode-pass2/63EMU.DSK
-hard1 /private/tmp/sourcecode-pass2/63SDC-MCP-DEV.VHD
```

The existing RGB cfg, MCP bridge script and state directory were retained. This pass did not revalidate or redesign the hardware configuration. The local bridge listener required sandbox approval. The first observation attempt lacked a lease and failed; MAME stopped. The corrected complete run used:

1. `coco_start`
2. `os9_restore_ready({timeout_ms:30000})`
3. `os9_run({command:"ident -m <name>",timeout_ms:30000})` for Shell, Krn, KrnP2, IOMan, SCF, VTIO, CoWin, RBF, PipeMan, Clock, Clock2, EmuDsk, GrfDrv and Term, followed by `os9_run` with `mdir`.
4. After each completed command, temporary host observation used `begin_run` with the known post-load epoch and an ownership token, `read_text_console`, then `finish_run`; the existing decoder produced the rows retained in RUNTIME_MATCHES.
5. `coco_stop`, successful. No save, reset, disk mutation, clock change or guest build command was used.

Each successful ident block is preserved in RUNTIME_MATCHES. All fifteen guest commands returned status 000, completed=true and shellReady=true. This verifies module inspection and a responsive restored shell; it does not validate the indexed driver routines or recover complete command stdout.

## Integrity and validation

| Media | Before | After |
|---|---|---|
| Development VHD | `4c6bdc0cd2f68aa57a9c75f75f15fe429f30926aa522917dd8d14aa1b9ae622e` | `4c6bdc0cd2f68aa57a9c75f75f15fe429f30926aa522917dd8d14aa1b9ae622e` |
| Live-test temporary VHD | Copied from the above bytes | Same SHA-256 |
| Boot DSK / temporary copy | Original copied without translation | Both `9a51adb8656b9f003c7f822352c291487362f1b4c43aac1a16672d5d2bcb4c40` |

Final commands included:

```sh
shasum -a 256 media/63SDC-MCP-DEV.VHD /private/tmp/sourcecode-pass2/63SDC-MCP-DEV.VHD
shasum -a 256 media/63EMU.DSK /private/tmp/sourcecode-pass2/63EMU.DSK
git diff --check
```

Stock media was neither mounted nor modified. No source bodies were added to Git. New artifacts are documentation/indexes only; pre-existing MCP changes were preserved. JSON counts/hashes, index coverage, cited file/line existence and local Markdown links were checked. No commit was made.

## Remaining authority gaps

Applicable manuals and an identified upstream NitrOS-9 revision are still needed for normative syscall contracts, privileged-call restrictions, register/frame/CPU semantics, hardware register meaning, timing, and validation of source comments. Local `MCP/Documents` and `DOCS_INDEX.md` were unavailable. No historical copyright or “derived source” claim has been externally authenticated. Rebuilds, matching toolchains/defs, source-to-binary comparison and archive-compatible readers remain future work.
