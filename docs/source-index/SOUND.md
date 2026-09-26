# Sound and audio

[Navigation](README.md) · [Provenance/dependencies](PROVENANCE_DEPENDENCIES.md) · [Runtime evidence](RUNTIME_MATCHES.md)

## What the source demonstrates

**Observed in the source paths below:** SounDrv is a third-party SCF sound driver with Write/GetStat/SetStat entry points and a client. Its source masks a sample then writes $FF20 and changes audio control bits on open/close. PLAY is a different player with CPU/hardware conditionals. EOU SndDrv is a separate module.

## Best entry points

All references are guest paths, not repository source files. Line numbers use CR-normalized source. Labels below are navigation clues; reading a label is not evidence that the path executes in the current build.

| Source | Useful labels / modules | F$ / I$ references | Includes / definitions |
|---|---|---|---|
| `/dd/SOURCECODE/ASM/SOUNDRV/SounDrv.asm` | See procedure/module declarations in file | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/ASM/SOUNDRV/DrvPlay.asm` | exit, error, usage, nodriver | F$Exit, I$Close, I$Open, I$Read, I$Write | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/ASM/PLAY/play.a` | start, Is6309, Is6809, Continue, L0119, L011E, IllRiff, Error, NoFmt, YesWAV, PCMGood, BadChanl, Stereo, Mono, ToFast, GetBitSz, Is16, GetRte | F$AllRAM, F$ClrBlk, F$DelRAM, F$Exit, F$Icpt, F$MapBlk, I$GetStt, I$Open, I$Read, I$ReadLn, I$Seek, I$Write | L6: use   /dd/defs/os9defs; L7: use   /dd/defs/scfdefs; L1388: use    two_byte_bin2dec.a; L1389: use    mulaw_alaw.a |
| `/dd/SOURCECODE/ASM/NITROS9/SCF/snddrv_beta6.asm` | entry, init, okend, setstt, BadArgs, Bell, NormBell, BellTone, BellLoop, ToneLoop, Loop2, ToneExit, SendByte, SendDely | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L29: use   /dd/defs/deffile |

## Hardware and runtime dependencies

PIA/DAC selection and interrupt behavior are explicit dependencies; PLAY source refers to legacy missing defs names. No audio hardware or player was exercised. Sources: `/dd/SOURCECODE/ASM/SOUNDRV/SounDrv.asm`, `/dd/SOURCECODE/ASM/SOUNDRV/DrvPlay.asm`, `/dd/SOURCECODE/ASM/PLAY/play.a`, `/dd/SOURCECODE/ASM/NITROS9/SCF/snddrv_beta6.asm`.

## Reusable patterns — interpretation

The driver/client split demonstrates pushing byte streams through a device path, while keeping hardware operations in the driver. Trace both initialization and shutdown of audio selection. These are study patterns inferred from the cited implementations; adapt only after checking the relevant ABI, revision and manual.

## Dangerous, historical or version-specific patterns

SounDrv comments describe choppy/limited-rate output and are historical observations, not fresh benchmarks. Its instruction comment says “mask top 2 bits” beside ANDA #$FC; preserve the instruction as evidence and verify intended sample alignment rather than copying the comment as a hardware rule. SounDrv and SndDrv are not synonyms. Evidence is in the entry-point files above; [the dependency audit](PROVENANCE_DEPENDENCIES.md) and [module comparisons](RUNTIME_MATCHES.md) record unresolved links.

## Verification boundary

Source inspection establishes the text and relationships described here. Only module identity was exercised live in Pass 2; these algorithms, driver operations and build recipes were not executed. No exact editable-source-to-running-binary claim is made. See [SYSTEM_CALL_USAGE](SYSTEM_CALL_USAGE.md) for per-file invocation versus reference distinctions, and [HARDWARE_USAGE](HARDWARE_USAGE.md) for address evidence.
