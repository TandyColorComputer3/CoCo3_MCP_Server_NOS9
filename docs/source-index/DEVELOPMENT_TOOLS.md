# Assemblers, linkers and development tools

[Navigation](README.md) · [Provenance/dependencies](PROVENANCE_DEPENDENCIES.md) · [Runtime evidence](RUNTIME_MATCHES.md)

## What the source demonstrates

**Observed in the source paths below:** MAMOU and RMA contain assembler processing/opcode tables, RLINK has separate output-format code including OS-9, and RDUMP includes a 6309 disassembler. An instruction mnemonic in a C table is data used by a tool, not guest execution.

## Best entry points

All references are guest paths, not repository source files. Line numbers use CR-normalized source. Labels below are navigation clues; reading a label is not evidence that the path executes in the current build.

| Source | Useful labels / modules | F$ / I$ references | Includes / definitions |
|---|---|---|---|
| `/dd/SOURCECODE/C/MAMOU/mamou.c` | static, char, int, void | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L11: #include "mamou.h"; L542: include = getenv("MAMOU_INCLUDE"); |
| `/dd/SOURCECODE/C/MAMOU/h6309.h` | struct | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/C/RMA/rtables.h` | GLOBAL, int | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/C/RLINK/rlink.c` | int, unsigned, extern | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L14: #include <stdio.h>; L16: #include <stdlib.h>; L17: #include <string.h>; L18: #include <libgen.h>; L20: #include "rlink.h" |
| `/dd/SOURCECODE/C/RLINK/os9out.c` | static, unsigned, int | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L1: #include <stdio.h>; L2: #include <string.h>; L3: #include "rlink.h" |
| `/dd/SOURCECODE/C/RDUMP/6309dasm.c` | typedef, static, int, const, extern | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L23: #include <stdio.h>; L24: #include <string.h>; L26: #include "hd6309.h"; L27: #include "rof.h" |

## Hardware and runtime dependencies

Tool target architecture and the machine running the compiler are separate. Object/module formats and compiler assumptions require revision-specific validation. Sources: `/dd/SOURCECODE/C/MAMOU/mamou.c`, `/dd/SOURCECODE/C/MAMOU/h6309.h`, `/dd/SOURCECODE/C/RMA/rtables.h`, `/dd/SOURCECODE/C/RLINK/rlink.c`, `/dd/SOURCECODE/C/RLINK/os9out.c`, `/dd/SOURCECODE/C/RDUMP/6309dasm.c`.

## Reusable patterns — interpretation

Trace parsing, symbol resolution and output emission separately. Use opcode tables to locate decoding logic, then validate opcode semantics against primary instruction documentation. These are study patterns inferred from the cited implementations; adapt only after checking the relevant ABI, revision and manual.

## Dangerous, historical or version-specific patterns

Source presence does not establish compiler compatibility, complete build dependencies, licensing permission or conformance to current toolchains. No tool was built or used to rebuild guest modules in this pass. Evidence is in the entry-point files above; [the dependency audit](PROVENANCE_DEPENDENCIES.md) and [module comparisons](RUNTIME_MATCHES.md) record unresolved links.

## Verification boundary

Source inspection establishes the text and relationships described here. Only module identity was exercised live in Pass 2; these algorithms, driver operations and build recipes were not executed. No exact editable-source-to-running-binary claim is made. See [SYSTEM_CALL_USAGE](SYSTEM_CALL_USAGE.md) for per-file invocation versus reference distinctions, and [HARDWARE_USAGE](HARDWARE_USAGE.md) for address evidence.
