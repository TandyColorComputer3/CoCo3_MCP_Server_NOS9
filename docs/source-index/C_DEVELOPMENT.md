# C development

[Navigation](README.md) · [Provenance/dependencies](PROVENANCE_DEPENDENCIES.md) · [Runtime evidence](RUNTIME_MATCHES.md)

## What the source demonstrates

**Observed in the source paths below:** C/LIB implements runtime wrappers in assembly. SDCCMDR’s Makefile selects dcc with OS9=1; sdccmdr.c conditionally selects CMOC headers under DECB. commsdc.c contains OS9/DECB/FLEX assembly blocks and direct MPI access.

## Best entry points

All references are guest paths, not repository source files. Line numbers use CR-normalized source. Labels below are navigation clues; reading a label is not evidence that the path executes in the current build.

| Source | Useful labels / modules | F$ / I$ references | Includes / definitions |
|---|---|---|---|
| `/dd/SOURCECODE/C/LIB/process.a` | kill, wait, wait10, setpr, os9fork | F$CHAIN, F$EXIT, F$FORK, F$SEND, F$SPRIOR, F$WAIT | L4: use ..../defs/os9defs.a |
| `/dd/SOURCECODE/C/LIB/io.a` | read, read1, read10, rdexit, readln, write, write1, write10, writeln, lseek, lseek10, lserr, end, here, doseek | I$GETSTT, I$READ, I$READLN, I$SEEK, I$WRITE, I$WRITLN | L5: use ..../defs/os9defs.a |
| `/dd/SOURCECODE/C/LIB/intercept.a` | See procedure/module declarations in file | F$ICPT | L8: use ..../defs/os9defs.a |
| `/dd/SOURCECODE/C/SDCCMDR/Makefile` | all | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/C/SDCCMDR/commsdc.c` | char, int, void, unsigned | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L2: #include <cmoc.h>; L4: #include "commsdc.h" |
| `/dd/SOURCECODE/C/SDCCMDR/sdccmdr.c` | struct, char, int | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L2: #include <cmoc.h>; L3: #include <coco.h>; L6: #include "libsdc.h"; L7: #include "commsdc.h"; L8: #include "string.h"; L9: #include "sdccmdr.h" |
| `/dd/SOURCECODE/C/CONTROL/control.c` | MNDSCR, WNDSCR, MSRET, static, int, typedef, PSETS, CLK, AR, ENV, char | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L1: #include <stdio.h>; L2: #include <stdlib.h>; L3: #include <buffs.h>; L4: #include <string.h>; L5: #include <wind.h>; L6: #include <mouse.h> |

## Hardware and runtime dependencies

SDC/MPI hardware applies only to those branches. C/CONTROL depends on window/mouse headers and environment; ordinary C utility portability cannot be inferred from the C directory. Sources: `/dd/SOURCECODE/C/LIB/process.a`, `/dd/SOURCECODE/C/LIB/io.a`, `/dd/SOURCECODE/C/LIB/intercept.a`, `/dd/SOURCECODE/C/SDCCMDR/Makefile`, `/dd/SOURCECODE/C/SDCCMDR/commsdc.c`, `/dd/SOURCECODE/C/SDCCMDR/sdccmdr.c`, `/dd/SOURCECODE/C/CONTROL/control.c`.

## Reusable patterns — interpretation

Keep compiler ABI, runtime library, target macros and OS calls explicit. Reuse a build recipe only after resolving all headers/libraries and output paths. These are study patterns inferred from the cited implementations; adapt only after checking the relevant ABI, revision and manual.

## Dangerous, historical or version-specific patterns

C/LIB includes ..../defs/os9defs.a, an unresolved historical path. SDCCMDR interrupt helpers use ORCC/ANDCC constants rather than an obvious saved-CC restoration; assess the calling context before reuse. Its clean target removes files and supplied link commands write /h1 or /dd/cmds—none were executed. Evidence is in the entry-point files above; [the dependency audit](PROVENANCE_DEPENDENCIES.md) and [module comparisons](RUNTIME_MATCHES.md) record unresolved links.

## Verification boundary

Source inspection establishes the text and relationships described here. Only module identity was exercised live in Pass 2; these algorithms, driver operations and build recipes were not executed. No exact editable-source-to-running-binary claim is made. See [SYSTEM_CALL_USAGE](SYSTEM_CALL_USAGE.md) for per-file invocation versus reference distinctions, and [HARDWARE_USAGE](HARDWARE_USAGE.md) for address evidence.
