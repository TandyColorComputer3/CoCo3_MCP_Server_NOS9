# Processes and signals

[Navigation](README.md) · [Provenance/dependencies](PROVENANCE_DEPENDENCIES.md) · [Runtime evidence](RUNTIME_MATCHES.md)

## What the source demonstrates

**Observed in the source paths below:** FFork builds a child process context and copies inherited information; FSend/ficpt are implementation routes for signaling and interception. C process.a exposes kill, wait, setpr, chain and os9fork. Shell+ installs an intercept routine and launches programs.

## Best entry points

All references are guest paths, not repository source files. Line numbers use CR-normalized source. Labels below are navigation clues; reading a label is not evidence that the path executes in the current build.

| Source | Useful labels / modules | F$ / I$ references | Includes / definitions |
|---|---|---|---|
| `/dd/SOURCECODE/ASM/NITROS9/KERNEL/ffork.asm` | FFork, GotNPrc, L0250, L0261, GetOPth, SveNPth, L02CF | F$AllTsk, F$AProc, F$DelTsk, F$Move, I$Dup | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fsend.asm` | FSend, L0647, L064D, L0652, L066A, L066D, L067B, L068F, L0697, L06CF, L06D3, L06D6, L06E0, L06F1, L06F4 | F$AProc | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/ASM/NITROS9/KERNEL/ficpt.asm` | FIcpt | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/C/LIB/process.a` | kill, wait, wait10, setpr, os9fork | F$CHAIN, F$EXIT, F$FORK, F$SEND, F$SPRIOR, F$WAIT | L4: use ..../defs/os9defs.a |
| `/dd/SOURCECODE/C/LIB/signal.a` | signal, sigerr, signal10, sigexit, signal20, lookup, loop, signal30, signal40, intrupt, intr10, intr20, intr30 | F$EXIT, F$ICPT | L1: use ..../defs/os9defs.a |
| `/dd/SOURCECODE/ASM/SHELL/shellplus2.2a.asm` | L006B, start, L009C, L00BF, L00CC, L00EA, L00FB, L010D, L0119, L0120, L0137, L0147, L014E, L015B, L016A, L0171, L0177, L017B | F$Chain, F$CmpNam, F$Exit, F$Fork, F$GPrDsc, F$Icpt, F$ID, F$Link, F$Load, F$NMLink, F$NMLoad, F$PErr, F$PrsNam, F$Send, F$Sleep, F$SPrior, F$SUser, F$Time, F$Unlink, F$UnLink, F$UnLoad, F$Wait, I$Chg | L33: use   /dd/defs/deffile |

## Hardware and runtime dependencies

No hardware device is required by the process examples themselves; kernel process/DAT layouts and 6809 versus H6309 saved frames are dependencies. Sources: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/ffork.asm`, `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fsend.asm`, `/dd/SOURCECODE/ASM/NITROS9/KERNEL/ficpt.asm`, `/dd/SOURCECODE/C/LIB/process.a`, `/dd/SOURCECODE/C/LIB/signal.a`, `/dd/SOURCECODE/ASM/SHELL/shellplus2.2a.asm`.

## Reusable patterns — interpretation

Use the wrapper’s explicit parent/error path and signal interception setup as study patterns; inspect resource inheritance/cleanup alongside creation. C wait converts returned status into the caller’s integer storage. These are study patterns inferred from the cited implementations; adapt only after checking the relevant ABI, revision and manual.

## Dangerous, historical or version-specific patterns

process.a explicitly warns that chain changes the stack into direct-page/global storage and cannot safely return on failure; its error path calls F$Exit. Signal wrappers and Shell+ are revision/ABI-specific, not a portable POSIX implementation. Evidence is in the entry-point files above; [the dependency audit](PROVENANCE_DEPENDENCIES.md) and [module comparisons](RUNTIME_MATCHES.md) record unresolved links.

## Verification boundary

Source inspection establishes the text and relationships described here. Only module identity was exercised live in Pass 2; these algorithms, driver operations and build recipes were not executed. No exact editable-source-to-running-binary claim is made. See [SYSTEM_CALL_USAGE](SYSTEM_CALL_USAGE.md) for per-file invocation versus reference distinctions, and [HARDWARE_USAGE](HARDWARE_USAGE.md) for address evidence.
