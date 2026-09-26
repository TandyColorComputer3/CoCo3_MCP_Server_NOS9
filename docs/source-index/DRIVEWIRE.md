# DriveWire and networking

[Navigation](README.md) · [Provenance/dependencies](PROVENANCE_DEPENDENCIES.md) · [Runtime evidence](RUNTIME_MATCHES.md)

## What the source demonstrates

**Observed in the source paths below:** DWIO includes transfer/init routines; scdwv and rbdw expose distinct character/block paths. DWRead documents serial framing and timeout behavior. dwnet.a uses /N and I$SetStt; FTP main.a identifies an RFC 959 client by Bill Nobel.

## Best entry points

All references are guest paths, not repository source files. Line numbers use CR-normalized source. Labels below are navigation clues; reading a label is not evidence that the path executes in the current build.

| Source | Useful labels / modules | F$ / I$ references | Includes / definitions |
|---|---|---|---|
| `/dd/SOURCECODE/ASM/NITROS9/DW/dwio.asm` | start, DWInit, loop@, IRQMt03, IRQM06, IRQM05, IRQM03, IRQM04, FRQdown, FRQd1, dostat, IRQsetFRQ, loop, out, statcont, dowaitq, dosleepq, send@ | F$AProc, F$Debug, F$Find64, F$IRQ, F$Send, F$SRqMem, F$STime, F$VIRQ | L22: use       /dd/defs/deffile; L23: use       /dd/defs/drivewire.d; L90: use       dwread.asm; L97: use       dwwrite.asm; L104: use		dwinit.asm |
| `/dd/SOURCECODE/ASM/NITROS9/DW/dwread.asm` | DWRead, dwrloop, rdlop, loop@, bkrd, bkto1, bkto, rdy1, rx0010, rx0020, rxByte, rx0030, rxExit | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/ASM/NITROS9/DW/scdwv.asm` | termbye, WriteOK, WriteExit, NotReady, ReadChr, ReadChr1, ReadSlp2, PrAbtErr, Sleep0, Sleep1, TimedSlp, GSExitOK, NRdyErr, UnSvcErr, GetScSiz, GetComSt, GetSSMntr | F$Link, F$Send, F$Sleep, F$UnLink | L56: use       defsfile; L57: use       drivewire.d |
| `/dd/SOURCECODE/ASM/NITROS9/DW/rbdw.asm` | start, no@, Init2, InitEx2, NextDrv, CopyLSN0, CpyLSNLp, CpyLSNEx, ReadSect, Read1, Read2, ReadEr1, ReadEr2, ReadEx, Write, Write1, WritEx0 | F$Link | L39: use     /dd/defs/deffile; L40: use     /dd/defs/drivewire.d; L335: use     dwcheck.asm |
| `/dd/SOURCECODE/ASM/DWNET/dwnet.a` | getopts, setopts, TCPEchoOn, TCPEchoOff, TCPLFOn, TCPLFOff, RawPath, rawloop, rawex, TCPOpen, openerr, TCPKill, TCPJoin, writeport, readresponse, connectex, TCPListen, TCPDisconnect | I$Close, I$Open, I$ReadLn, I$SetStt, I$Write, I$WritLn | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/ASM/FTP/main.a` | Main, optlp, pcmd1, gotcmd, prseext, noerr, ErrExit | F$Exit, F$Icpt, I$GetStt, I$SetStt | No direct include found; may be included by parent build |

## Hardware and runtime dependencies

drivewire.d names BBOUT/BBIN; deffile switches include BECKER, BECKERTO, ARDUINO, SY6551N and NOINTMASK. /N requires the corresponding installed descriptor/driver, not just a linked library. Sources: `/dd/SOURCECODE/ASM/NITROS9/DW/dwio.asm`, `/dd/SOURCECODE/ASM/NITROS9/DW/dwread.asm`, `/dd/SOURCECODE/ASM/NITROS9/DW/scdwv.asm`, `/dd/SOURCECODE/ASM/NITROS9/DW/rbdw.asm`, `/dd/SOURCECODE/ASM/DWNET/dwnet.a`, `/dd/SOURCECODE/ASM/FTP/main.a`.

## Reusable patterns — interpretation

Separate application protocol, character/block driver and physical transfer implementation. Inspect conditional transport selection and failure behavior before reusing a routine. These are study patterns inferred from the cited implementations; adapt only after checking the relevant ABI, revision and manual.

## Dangerous, historical or version-specific patterns

DWRead’s ARDUINO branch explicitly says it has no timeout fallback. Current deffile disables BECKER, ARDUINO and other alternatives. Bundled source and emulator capabilities do not prove a configured transport; no DriveWire server or descriptor was live-tested. Evidence is in the entry-point files above; [the dependency audit](PROVENANCE_DEPENDENCIES.md) and [module comparisons](RUNTIME_MATCHES.md) record unresolved links.

## Verification boundary

Source inspection establishes the text and relationships described here. Only module identity was exercised live in Pass 2; these algorithms, driver operations and build recipes were not executed. No exact editable-source-to-running-binary claim is made. See [SYSTEM_CALL_USAGE](SYSTEM_CALL_USAGE.md) for per-file invocation versus reference distinctions, and [HARDWARE_USAGE](HARDWARE_USAGE.md) for address evidence.
