# System calls

[Navigation](README.md) · [Provenance/dependencies](PROVENANCE_DEPENDENCIES.md) · [Runtime evidence](RUNTIME_MATCHES.md)

## What the source demonstrates

**Observed in the source paths below:** The F$ID leaf reads the current process descriptor and writes the saved return-register frame. The kernel has a SysCalls dispatch table; IOMan contains UsrIO, SysIO and IODsptch. C and BASIC09 wrappers demonstrate different caller ABIs, not interchangeable register blocks.

## Best entry points

All references are guest paths, not repository source files. Line numbers use CR-normalized source. Labels below are navigation clues; reading a label is not evidence that the path executes in the current build.

| Source | Useful labels / modules | F$ / I$ references | Includes / definitions |
|---|---|---|---|
| `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fid.asm` | FID | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/ASM/NITROS9/KERNEL/krn.asm` | Vectors, L001C, Loop2, L0065, L00EF, L0104, L0111, L0127, L0170, L01B0, L01B8, L01BF, ShowI, L01C1, L01CE, L01D0, L01D2 | F$Boot, F$Link | L29: use    defsfile; L654: use    fssvc.asm; L656: use    flink.asm; L658: use    fvmodul.asm; L660: use    ffmodul.asm; L662: use    fprsnam.asm |
| `/dd/SOURCECODE/ASM/NITROS9/MODS/ioman_beta5.asm` | start, ClrLoop, FIODel, L0086, L0097, L009D, L009E, UsrIO, SysIO, IODsptch, L00F9, SIModDsc, UIModDsc, imod001, imodexit, imod002, imod005, PtchCpLp | F$All64, F$AllPrc, F$AllTsk, F$CRC, F$DelPrc, F$ELink, F$Find64, F$FModul, F$GProcP, F$ID, F$LDABX, F$LDDDXY, F$Link, F$Move, F$PrsNam, F$RelTsk, F$ResTsk, F$Ret64, F$Send, F$SetTsk, F$Sleep, F$SLink, | L45: use   /dd/defs/deffile |
| `/dd/SOURCECODE/C/LIB/syscall.a` | _os9, os9.a | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/ASM/BASIC09/syscall_6309.asm` | start, L0034, L005C | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L23: use   /dd/defs/os9.d |

## Hardware and runtime dependencies

Kernel/IOMan are OS-version dependent. Definitions in /dd/DEFS/os9.d and H6309 selection affect frame offsets; application wrappers need the matching compiler/runtime ABI. Sources: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fid.asm`, `/dd/SOURCECODE/ASM/NITROS9/KERNEL/krn.asm`, `/dd/SOURCECODE/ASM/NITROS9/MODS/ioman_beta5.asm`, `/dd/SOURCECODE/C/LIB/syscall.a`, `/dd/SOURCECODE/ASM/BASIC09/syscall_6309.asm`.

## Reusable patterns — interpretation

Start from a caller and follow its named service to a dispatcher/implementation; preserve the wrapper’s register-saving and error-return conventions. Treat the syscall number, entry frame and language ABI as separate dependencies. These are study patterns inferred from the cited implementations; adapt only after checking the relevant ABI, revision and manual.

## Dangerous, historical or version-specific patterns

Internal kernel services and user-facing wrappers share F$ spelling. Do not assume all indexed services are permitted or safe from an ordinary process. Numeric BASIC09 RUN syscall values are not fully resolved by the lexical F$/I$ index. Evidence is in the entry-point files above; [the dependency audit](PROVENANCE_DEPENDENCIES.md) and [module comparisons](RUNTIME_MATCHES.md) record unresolved links.

## Verification boundary

Source inspection establishes the text and relationships described here. Only module identity was exercised live in Pass 2; these algorithms, driver operations and build recipes were not executed. No exact editable-source-to-running-binary claim is made. See [SYSTEM_CALL_USAGE](SYSTEM_CALL_USAGE.md) for per-file invocation versus reference distinctions, and [HARDWARE_USAGE](HARDWARE_USAGE.md) for address evidence.
