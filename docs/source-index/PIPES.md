# Pipes

[Navigation](README.md) · [Provenance/dependencies](PROVENANCE_DEPENDENCIES.md) · [Runtime evidence](RUNTIME_MATCHES.md)

## What the source demonstrates

**Observed in the source paths below:** PipeMan supplies Open/Read/ReadLn/Write/Close paths and buffering. The beta6 header credits Alan DeKok, a Boisy Pitre Level-One backport, and 2020 Curtis Boyle optimizations. pipe/piper are associated descriptors; named-pipe source is a separate variant.

## Best entry points

All references are guest paths, not repository source files. Line numbers use CR-normalized source. Labels below are navigation clues; reading a label is not evidence that the path executes in the current build.

| Source | Useful labels / modules | F$ / I$ references | Includes / definitions |
|---|---|---|---|
| `/dd/SOURCECODE/ASM/NITROS9/PIPE/pipeman_beta6.asm` | start, Seek, GetStt, SetStt, Close, L008E, L009C, L00A9, Open, L0060, L007A, L007B, ReadLn, Read, L00DB, L00E0, L00ED, L00F1 | F$IOQu, F$LDABX, F$Move, F$PrsNam, F$Send, F$Sleep, F$SRqMem, F$SRtMem | L48: use   /dd/defs/deffile; L50: use   /dd/defs/pipedefs |
| `/dd/SOURCECODE/ASM/NITROS9/PIPE/pipeman_named.asm` | JmpTbl, Creat00, SetCnt, NotName, HasName, NameOK, GoCheck, BadXit2, BadNam2, BadName, TooBig, BadExit, Not1st, ChkLoop, NewPipe, NewP1, DoNew, ClrBuf | F$IOQU, F$LDABX, F$Move, F$PrsNam, F$Ret64, F$SEND, F$SLEEP, F$SrqMem, F$SRtMem, F$STABX, I$Detach | L117: use   defsfile; L118: use   pipedefs |
| `/dd/SOURCECODE/ASM/NITROS9/PIPE/pipe.asm` | See procedure/module declarations in file | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L16: use   defsfile; L17: use   pipedefs |
| `/dd/SOURCECODE/ASM/NITROS9/PIPE/piper.asm` | Init, Read, Write, GetStat, SetStat, Term | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L16: use   defsfile |

## Hardware and runtime dependencies

No physical peripheral is intrinsic to the pipe abstraction; scheduler, signal and descriptor definitions are OS dependencies. Sources: `/dd/SOURCECODE/ASM/NITROS9/PIPE/pipeman_beta6.asm`, `/dd/SOURCECODE/ASM/NITROS9/PIPE/pipeman_named.asm`, `/dd/SOURCECODE/ASM/NITROS9/PIPE/pipe.asm`, `/dd/SOURCECODE/ASM/NITROS9/PIPE/piper.asm`.

## Reusable patterns — interpretation

Study blocking/wakeup and close handling together with the data-transfer path. Use this implementation to trace how stream semantics are provided through a file manager. These are study patterns inferred from the cited implementations; adapt only after checking the relevant ABI, revision and manual.

## Dangerous, historical or version-specific patterns

Historical timing numbers in the header are not current performance measurements. Do not interchange pipeman_named and the live PipeMan solely because of the module name. Shell sequencing/status behavior must be verified independently. Evidence is in the entry-point files above; [the dependency audit](PROVENANCE_DEPENDENCIES.md) and [module comparisons](RUNTIME_MATCHES.md) record unresolved links.

## Verification boundary

Source inspection establishes the text and relationships described here. Only module identity was exercised live in Pass 2; these algorithms, driver operations and build recipes were not executed. No exact editable-source-to-running-binary claim is made. See [SYSTEM_CALL_USAGE](SYSTEM_CALL_USAGE.md) for per-file invocation versus reference distinctions, and [HARDWARE_USAGE](HARDWARE_USAGE.md) for address evidence.
