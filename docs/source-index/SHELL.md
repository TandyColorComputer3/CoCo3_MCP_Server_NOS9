# Shell+

[Navigation](README.md) · [Provenance/dependencies](PROVENANCE_DEPENDENCIES.md) · [Runtime evidence](RUNTIME_MATCHES.md)

## What the source demonstrates

**Observed in the source paths below:** shellplus2.2a.asm identifies changes from a 2.2 disassembly and includes process launch, interception and I/O. shell21.a and shell.orig represent other revisions; GShell is a graphical application with a different lineage.

## Best entry points

All references are guest paths, not repository source files. Line numbers use CR-normalized source. Labels below are navigation clues; reading a label is not evidence that the path executes in the current build.

| Source | Useful labels / modules | F$ / I$ references | Includes / definitions |
|---|---|---|---|
| `/dd/SOURCECODE/ASM/SHELL/shellplus2.2a.asm` | L006B, start, L009C, L00BF, L00CC, L00EA, L00FB, L010D, L0119, L0120, L0137, L0147, L014E, L015B, L016A, L0171, L0177, L017B | F$Chain, F$CmpNam, F$Exit, F$Fork, F$GPrDsc, F$Icpt, F$ID, F$Link, F$Load, F$NMLink, F$NMLoad, F$PErr, F$PrsNam, F$Send, F$Sleep, F$SPrior, F$SUser, F$Time, F$Unlink, F$UnLink, F$UnLoad, F$Wait, I$Chg | L33: use   /dd/defs/deffile |
| `/dd/SOURCECODE/ASM/SHELL/shell21.a` | L009C, L00BF, L00CC, L00EA, L00FB, L010D, L0119, L0120, L0137, L0147, L014E, L015B, L0171, L0177, L017F, L0191, L01A4, L01B9 | F$Chain, F$CmpNam, F$Exit, F$Fork, F$GPrDsc, F$Icpt, F$ID, F$Link, F$NMLink, F$NMLoad, F$PErr, F$PrsNam, F$Send, F$Sleep, F$SPrior, F$SUser, F$Time, F$UnLink, F$UnLoad, F$Wait, I$ChgDir, I$Close, I$Cr | L7: use   /dd/defs/os9defs |
| `/dd/SOURCECODE/ASM/SHELL/makeshell` | asm | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/ASM/GSHELL/gshell_ver1_0_1.asm` | CSTART, SAVESGNL, DoneSig, MAIN, GSHABORT, FIXWINDW, DoneFix, ONWINDOW, BILDDESC, SETWINDW, SETWIND1, SETFONTS, WINDPARM, WINDPAR1, WINDPAR3, WINDPAR4, LEAVEFAT, WINDPARX | F$Chain, F$CpyMem, F$Exit, F$Fork, F$GPrDsc, F$Icpt, F$ID, F$Mem, F$NMLink, F$NMLoad, F$Send, F$Sleep, F$UnLoad, F$Wait, I$ChgDir, I$Close, I$Delete, I$Dup, I$GetStt, I$MakDir, I$ModDsc, I$Open, I$Rea | L62: use   /dd/defs/deffile |

## Hardware and runtime dependencies

Shell+ depends on OS process/path and terminal behavior; GShell adds CoWin/GrfDrv/window dependencies. None of the shell build scripts were executed. Sources: `/dd/SOURCECODE/ASM/SHELL/shellplus2.2a.asm`, `/dd/SOURCECODE/ASM/SHELL/shell21.a`, `/dd/SOURCECODE/ASM/SHELL/makeshell`, `/dd/SOURCECODE/ASM/GSHELL/gshell_ver1_0_1.asm`.

## Reusable patterns — interpretation

Pair source reading with the measured shell fingerprint and the existing command-completion experiment. Track separate command submission, fresh prompt observation and status query as a protocol. These are study patterns inferred from the cited implementations; adapt only after checking the relevant ABI, revision and manual.

## Dangerous, historical or version-specific patterns

Do not assume Unix shell syntax or use a semicolon tail as a failure-proof completion sentinel. The previously verified Shell+ protocol sends a separate status query only after a fresh prompt; this pass identified the same resident Shell fingerprint but did not repeat sequencing experiments. Evidence is in the entry-point files above; [the dependency audit](PROVENANCE_DEPENDENCIES.md) and [module comparisons](RUNTIME_MATCHES.md) record unresolved links.

## Verification boundary

Source inspection establishes the text and relationships described here. Only module identity was exercised live in Pass 2; these algorithms, driver operations and build recipes were not executed. No exact editable-source-to-running-binary claim is made. See [SYSTEM_CALL_USAGE](SYSTEM_CALL_USAGE.md) for per-file invocation versus reference distinctions, and [HARDWARE_USAGE](HARDWARE_USAGE.md) for address evidence.

The prior behavioral evidence is [NITROS9_COMMAND_COMPLETION](../architecture/NITROS9_COMMAND_COMPLETION.md) and [NITROS9_RUN](../architecture/NITROS9_RUN.md); its scope is foreground completion/status, not complete output capture.
