# BASIC09 development

[Navigation](README.md) · [Provenance/dependencies](PROVENANCE_DEPENDENCIES.md) · [Runtime evidence](RUNTIME_MATCHES.md)

## What the source demonstrates

**Observed in the source paths below:** This collection contains both the language runtime in assembly and BASIC09 procedure examples. SysCall documents its parameter/register block, uses stack-local workspace, and explicitly forces H6309=0. GFX2 bridges BASIC09 calls to graphics operations.

## Best entry points

All references are guest paths, not repository source files. Line numbers use CR-normalized source. Labels below are navigation clues; reading a label is not evidence that the path executes in the current build.

| Source | Useful labels / modules | F$ / I$ references | Includes / definitions |
|---|---|---|---|
| `/dd/SOURCECODE/ASM/BASIC09/basic09_ver101.asm` | L00DC, L07B5, L07C9, ClrLp, L07FC, L082E, L0116, L0860, L086D, L0870, L088F, L0896, L0899, L08A6, L08B2, L08CC, L08D0, L08D3 | F$Chain, F$CRC, F$Exit, F$Fork, F$Icpt, F$Link, F$Load, F$Mem, F$PErr, F$Time, F$UnLink, F$Wait, I$ChgDir, I$Close, I$Create, I$Delete, I$Dup, I$GetStt, I$Open, I$Read, I$ReadLn, I$Seek, I$SetStt, I$W | L266: USE   basic09defsfile; L267: USE   basic09.d; L9906: use   basic09.real.add.63.asm; L9908: use   basic09.real.add.68.asm; L9913: use   basic09.real.mul.63.asm; L9915: use   basic09.real.mul.68.a |
| `/dd/SOURCECODE/ASM/BASIC09/runb_ver101.asm` | L00D9, L00FB, L00FE, L0101, L0104, L0107, L010A, L010D, L0110, L0189, L019D, L01D0, L01ED, L0202, L0222, L0244, L024E, L025B | F$Chain, F$Exit, F$Fork, F$Icpt, F$Link, F$Load, F$PErr, F$Time, F$UnLink, F$Wait, I$ChgDir, I$Close, I$Create, I$Delete, I$Dup, I$GetStt, I$Open, I$Read, I$ReadLn, I$Seek, I$Write, I$WritLn | L54: use   basic09defsfile; L55: use   basic09.d; L3337: use   basic09.real.add.63.asm; L3339: use   basic09.real.add.68.asm; L3344: use   basic09.real.mul.63.asm; L3346: use   basic09.real.mul.68.asm |
| `/dd/SOURCECODE/ASM/BASIC09/syscall_6309.asm` | start, L0034, L005C | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L23: use   /dd/defs/os9.d |
| `/dd/SOURCECODE/ASM/BASIC09/gfx2_ver1.asm` | start, L02A3, L02B0, L02B8, L02BE, L02C5, L02CF, L02D5, L02F0, L02F6, L02F8, L02FD, L0305, L030A, L0332, L0334, L033E, L0371 | F$ID, F$Sleep, I$GetStt, I$SetStt, I$Write | L28: use   defsfile |
| `/dd/SOURCECODE/BASIC09/GFX5/Gfx5.b09` | PROCEDURE, TYPE, PARAM, DIM, RUN, IF, FOR, NEXT, PRINT, ON | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/BASIC09/GUIB30/GUIBDemo.b09` | PROCEDURE, TYPE, DIM, REM, RUN, IF, PRINT | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | No direct include found; may be included by parent build |

## Hardware and runtime dependencies

Graphics examples require the appropriate GFX2/window modules and window mode; syscall numeric constants need mapping against the same os9.d revision. Sources: `/dd/SOURCECODE/ASM/BASIC09/basic09_ver101.asm`, `/dd/SOURCECODE/ASM/BASIC09/runb_ver101.asm`, `/dd/SOURCECODE/ASM/BASIC09/syscall_6309.asm`, `/dd/SOURCECODE/ASM/BASIC09/gfx2_ver1.asm`, `/dd/SOURCECODE/BASIC09/GFX5/Gfx5.b09`, `/dd/SOURCECODE/BASIC09/GUIB30/GUIBDemo.b09`.

## Reusable patterns — interpretation

Study typed parameter blocks and small wrapper procedures; document parameter order, size and register-frame layout alongside each syscall/GFX example. These are study patterns inferred from the cited implementations; adapt only after checking the relevant ABI, revision and manual.

## Dangerous, historical or version-specific patterns

A filename containing 6309 does not prove an extended-register ABI. basic09_ver101 retains a Microware/Motorola copyright and later modifications; packaged Basic09 and RunB each contain several modules, including renamed private helpers. A whole-file name is not a single-module identity. Evidence is in the entry-point files above; [the dependency audit](PROVENANCE_DEPENDENCIES.md) and [module comparisons](RUNTIME_MATCHES.md) record unresolved links.

## Verification boundary

Source inspection establishes the text and relationships described here. Only module identity was exercised live in Pass 2; these algorithms, driver operations and build recipes were not executed. No exact editable-source-to-running-binary claim is made. See [SYSTEM_CALL_USAGE](SYSTEM_CALL_USAGE.md) for per-file invocation versus reference distinctions, and [HARDWARE_USAGE](HARDWARE_USAGE.md) for address evidence.
