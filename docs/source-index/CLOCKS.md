# Clock and Clock2

[Navigation](README.md) · [Provenance/dependencies](PROVENANCE_DEPENDENCIES.md) · [Runtime evidence](RUNTIME_MATCHES.md)

## What the source demonstrates

**Observed in the source paths below:** Clock’s IRQ/time path dispatches through D.Clock2. Several different implementations all declare Clock2. messemu uses RTC.Base=$FF50 and comments that its MESS/Disto update ignores MPI and assumes AM/PM host-clock mode.

## Best entry points

All references are guest paths, not repository source files. Line numbers use CR-normalized source. Labels below are navigation clues; reading a label is not evidence that the path executes in the current build.

| Source | Useful labels / modules | F$ / I$ references | Includes / definitions |
|---|---|---|---|
| `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock.asm` | SvcIRQ, NoClock, ContIRQ, SvcVIRQ, doreset, notzero, virqent, NoGet, checkbel, endalarm, dobell, VIRQend, DoToggle, F.VIRQ, v.loop, FindVIRQ, v.chk, RemVIRQ | F$Link, F$Move, F$Send, F$SSvc | L35: use   /dd/defs/deffile |
| `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_messemu.asm` | GetTime, not20, AM, getval, getval1 | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L23: use   /dd/defs/deffile |
| `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_disto2.asm` | GetTime, RTCPost, RTCPre, GetVal, GetVal1, SetTime, SetVal, SetVal1, DvLoop, DvDone | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L16: use   /dd/defs/deffile |
| `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_disto4.asm` | GetVal, GetVal1, SetTime, SetVal, DvLoop, DvDone | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L16: use   /dd/defs/deffile |
| `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_soft.asm` | GetTime, NoLeap, UpdMonth, UpdDay, UpdHour, UpdMin, UpdTExit | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L16: use   /dd/defs/deffile |

## Hardware and runtime dependencies

RTC.Base, MPI handling, emulator assumptions and host time formats are provider-specific. See runtime match table; no new hardware behavior is inferred. Sources: `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock.asm`, `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_messemu.asm`, `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_disto2.asm`, `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_disto4.asm`, `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_soft.asm`.

## Reusable patterns — interpretation

Keep the clock service and hardware time provider separate. Identify providers by full module fingerprint, not their common Clock2 name or edition 1. Preserve boot selection and source revision in examples. These are study patterns inferred from the cited implementations; adapt only after checking the relevant ABI, revision and manual.

## Dangerous, historical or version-specific patterns

disto2/disto4/messemu/software are different implementations. The live Clock2 matches both bundled messemu CPU-named artifacts, not proof of a hardware RTC solution in another MPI slot. This pass neither changes RTC configuration nor validates wall-clock accuracy. Evidence is in the entry-point files above; [the dependency audit](PROVENANCE_DEPENDENCIES.md) and [module comparisons](RUNTIME_MATCHES.md) record unresolved links.

## Verification boundary

Source inspection establishes the text and relationships described here. Only module identity was exercised live in Pass 2; these algorithms, driver operations and build recipes were not executed. No exact editable-source-to-running-binary claim is made. See [SYSTEM_CALL_USAGE](SYSTEM_CALL_USAGE.md) for per-file invocation versus reference distinctions, and [HARDWARE_USAGE](HARDWARE_USAGE.md) for address evidence.
