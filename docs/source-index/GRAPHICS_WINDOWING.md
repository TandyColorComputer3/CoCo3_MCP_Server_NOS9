# Graphics, windows and mouse

[Navigation](README.md) · [Provenance/dependencies](PROVENANCE_DEPENDENCIES.md) · [Runtime evidence](RUNTIME_MATCHES.md)

## What the source demonstrates

**Observed in the source paths below:** CoWin/GShell show window-manager/application integration. GLIB.links documents sprite helper dependencies. GFX5Demo calls GFX2 and setmouse; GuiB provides parameterized GUI procedures; CoCoThello uses _ss_mous.

## Best entry points

All references are guest paths, not repository source files. Line numbers use CR-normalized source. Labels below are navigation clues; reading a label is not evidence that the path executes in the current build.

| Source | Useful labels / modules | F$ / I$ references | Includes / definitions |
|---|---|---|---|
| `/dd/SOURCECODE/ASM/NITROS9/SCF/cowin_beta6.asm` | Init, L0159, L0166, L0167, L0169, L0097, L00A9, L0101a, L0102, Nul0, ClrLp1, ClrLp2, L01DB, L01F4, L01FB, L021F, L022A | F$AllBit, F$CpyMem, F$DelBit, F$Find64, F$FModul, F$LDDDXY, F$Link, F$MapBlk, F$Move, F$NMLink, F$NMLoad, F$SchBit, F$Sleep, F$SRqMem, F$SRtMem, F$UnLink | L56: use   /dd/defs/deffile |
| `/dd/SOURCECODE/ASM/GSHELL/gshell_ver1_0_1.asm` | CSTART, SAVESGNL, DoneSig, MAIN, GSHABORT, FIXWINDW, DoneFix, ONWINDOW, BILDDESC, SETWINDW, SETWIND1, SETFONTS, WINDPARM, WINDPAR1, WINDPAR3, WINDPAR4, LEAVEFAT, WINDPARX | F$Chain, F$CpyMem, F$Exit, F$Fork, F$GPrDsc, F$Icpt, F$ID, F$Mem, F$NMLink, F$NMLoad, F$Send, F$Sleep, F$UnLoad, F$Wait, I$ChgDir, I$Close, I$Delete, I$Dup, I$GetStt, I$MakDir, I$ModDsc, I$Open, I$Rea | L62: use   /dd/defs/deffile |
| `/dd/SOURCECODE/ASM/GLIB/GLIB.links` | Summary, If, G$4x4, G$8x8, G$10x10, G$12x8, G$14x8, G$16x16, G$NoSave, U$IPFLIP, U$AUTO, U$SAVE, U$RESTORE, U$PUT, U$SPFLIP, U$BOTH, U$SSWAP, U$DEBUG | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/ASM/VEFIO-WINFO/vefio.asm` | clrexit, exit, u$movexy, movexy10 | F$ClrBlk, F$Exit, F$Icpt, F$Link, F$Load, F$MapBlk, F$PrsNam, F$Sleep, F$UnLoad, I$Close, I$GetStt, I$Open, I$Read, I$SetStt, I$Write, I$WritLn | L16: use   /dd/defs/os9.d; L64: use   winfodefs |
| `/dd/SOURCECODE/BASIC09/GFX5/GFX5Demo.b09` | PROCEDURE, TYPE, DIM, REM, RUN | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/BASIC09/GUIB30/Guib.b09` | PROCEDURE, PARAM, TYPE, DIM, REM, IF, RUN | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/C/COCOTHELLO/cocothello.h` | See procedure/module declarations in file | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | No direct include found; may be included by parent build |

## Hardware and runtime dependencies

CoWin/GrfDrv, fonts/patterns and correct window modes are dependencies; GLIB uses mapping helpers. See GRAPHICS source paths and HARDWARE_USAGE before treating video addresses as portable. Sources: `/dd/SOURCECODE/ASM/NITROS9/SCF/cowin_beta6.asm`, `/dd/SOURCECODE/ASM/GSHELL/gshell_ver1_0_1.asm`, `/dd/SOURCECODE/ASM/GLIB/GLIB.links`, `/dd/SOURCECODE/ASM/VEFIO-WINFO/vefio.asm`, `/dd/SOURCECODE/BASIC09/GFX5/GFX5Demo.b09`, `/dd/SOURCECODE/BASIC09/GUIB30/Guib.b09`, `/dd/SOURCECODE/C/COCOTHELLO/cocothello.h`.

## Reusable patterns — interpretation

Prefer narrow application/library calls over copying direct rendering internals. Follow window setup, mouse enable/disable and cleanup paths as one lifecycle. Keep the expected window type with examples. These are study patterns inferred from the cited implementations; adapt only after checking the relevant ABI, revision and manual.

## Dangerous, historical or version-specific patterns

GShell is derived source with revisions; GLIB has internal I$-prefixed labels that are not all OS-9 syscalls. GuiB comments specify window-type constraints. GrfDrv archive candidates are historical and were not established as the live implementation. Evidence is in the entry-point files above; [the dependency audit](PROVENANCE_DEPENDENCIES.md) and [module comparisons](RUNTIME_MATCHES.md) record unresolved links.

## Verification boundary

Source inspection establishes the text and relationships described here. Only module identity was exercised live in Pass 2; these algorithms, driver operations and build recipes were not executed. No exact editable-source-to-running-binary claim is made. See [SYSTEM_CALL_USAGE](SYSTEM_CALL_USAGE.md) for per-file invocation versus reference distinctions, and [HARDWARE_USAGE](HARDWARE_USAGE.md) for address evidence.
