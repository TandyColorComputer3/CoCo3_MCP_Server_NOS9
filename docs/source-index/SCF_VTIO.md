# SCF, VTIO and local console

[Navigation](README.md) · [Provenance/dependencies](PROVENANCE_DEPENDENCIES.md) · [Runtime evidence](RUNTIME_MATCHES.md)

## What the source demonstrates

**Observed in the source paths below:** SCF provides ReadLn/WritLn and character-path handling; VTIO provides Init/Read and keyboard/window state; CoWin handles window functions and GrfDrv integration. term_win80 names SCF and VTIO. sc6551 is a separate serial-driver route, not evidence that the current Term is serial.

## Best entry points

All references are guest paths, not repository source files. Line numbers use CR-normalized source. Labels below are navigation clues; reading a label is not evidence that the path executes in the current build.

| Source | Useful labels / modules | F$ / I$ references | Includes / definitions |
|---|---|---|---|
| `/dd/SOURCECODE/ASM/NITROS9/SCF/scf_ver100.asm` | opbpnam, bpnam, oerr, open, open1, CopyMsg, CopyCR, L00CF, NoShare, Yespath, L00E6, CkCar, L00EF, L00F8, L010F, OpenErr, close, L0129 | F$Find64, F$GProcP, F$IOQu, F$Move, F$PrsNam, F$Send, F$SRqMem, F$SRtMem, I$Attach, I$Detach | L207: use   /dd/defs/deffile    EOU "current" deffile; L212: use   /dd/defs/cocovtio.d |
| `/dd/SOURCECODE/ASM/NITROS9/SCF/vtio_beta6.asm` | Term, noterm, TermSub, Init, PerWinInit, LinkSys, NotReady, Read, read1, bumpdon, ReadSlp, ReadErr, L0170, L017B, L017D, L017E, L01A2, L01B1 | F$Debug, F$Link, F$Load, F$Move, F$Send, F$Sleep | L90: use   /dd/defs/deffile |
| `/dd/SOURCECODE/ASM/NITROS9/SCF/cowin_beta6.asm` | Init, L0159, L0166, L0167, L0169, L0097, L00A9, L0101a, L0102, Nul0, ClrLp1, ClrLp2, L01DB, L01F4, L01FB, L021F, L022A | F$AllBit, F$CpyMem, F$DelBit, F$Find64, F$FModul, F$LDDDXY, F$Link, F$MapBlk, F$Move, F$NMLink, F$NMLoad, F$SchBit, F$Sleep, F$SRqMem, F$SRtMem, F$UnLink | L56: use   /dd/defs/deffile |
| `/dd/SOURCECODE/ASM/NITROS9/SCF/term_win80.asm` | See procedure/module declarations in file | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L16: use   defsfile |
| `/dd/SOURCECODE/ASM/NITROS9/SCF/sc6551.asm` | FIRQRtn, L003B, Init, NoSwap, DfltInfo, SetRxBuf, NoDTR, NoSelect, Term, KeepDTR, Read, ReadLoop, ReadChk, ReadLp2, ChkHWHS, RxFloClr, ReadChar, SetPckUp | F$IRQ, F$Send, F$Sleep, F$SRqMem, F$SRtMem | L26: use   defsfile; L27: use   scfdefs |

## Hardware and runtime dependencies

VTIO/CoWin rely on CoCo video/keyboard state, cocovtio.d and interrupt coordination. The SC6551 branch requires its own hardware and descriptor; it was not exercised. Sources: `/dd/SOURCECODE/ASM/NITROS9/SCF/scf_ver100.asm`, `/dd/SOURCECODE/ASM/NITROS9/SCF/vtio_beta6.asm`, `/dd/SOURCECODE/ASM/NITROS9/SCF/cowin_beta6.asm`, `/dd/SOURCECODE/ASM/NITROS9/SCF/term_win80.asm`, `/dd/SOURCECODE/ASM/NITROS9/SCF/sc6551.asm`.

## Reusable patterns — interpretation

Separate file manager, terminal driver, window module and renderer responsibilities. Trace descriptor-to-driver names before attributing observed console behavior to one layer. These are study patterns inferred from the cited implementations; adapt only after checking the relevant ABI, revision and manual.

## Dangerous, historical or version-specific patterns

VTIO’s header includes EOU edits and unresolved-code notes. SCF integrates fast text/GrfDrv paths; screen output is not automatically a replayable stdout transcript. The supplied source is not a general emulator console API. Evidence is in the entry-point files above; [the dependency audit](PROVENANCE_DEPENDENCIES.md) and [module comparisons](RUNTIME_MATCHES.md) record unresolved links.

## Verification boundary

Source inspection establishes the text and relationships described here. Only module identity was exercised live in Pass 2; these algorithms, driver operations and build recipes were not executed. No exact editable-source-to-running-binary claim is made. See [SYSTEM_CALL_USAGE](SYSTEM_CALL_USAGE.md) for per-file invocation versus reference distinctions, and [HARDWARE_USAGE](HARDWARE_USAGE.md) for address evidence.
