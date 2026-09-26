# Modules and linkage

[Navigation](README.md) · [Provenance/dependencies](PROVENANCE_DEPENDENCIES.md) · [Runtime evidence](RUNTIME_MATCHES.md)

## What the source demonstrates

**Observed in the source paths below:** Link/unlink/validation routines show module-directory handling; llcocosdc has a MOD declaration and driver entry table. term_win80 declares descriptor relationships rather than implementing terminal I/O. C mod.a wraps module operations.

## Best entry points

All references are guest paths, not repository source files. Line numbers use CR-normalized source. Labels below are navigation clues; reading a label is not evidence that the path executes in the current build.

| Source | Useful labels / modules | F$ / I$ references | Includes / definitions |
|---|---|---|---|
| `/dd/SOURCECODE/ASM/NITROS9/KERNEL/flink.asm` | FSLink, FELink, L0398, L03AF, L03BB, L03E8, L03EB, L03FC, L0406, LinkErr, L0422, L0430, L0434, L0449 | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/ASM/NITROS9/KERNEL/funlink.asm` | FUnLink, L015D, L0161, L017C, L0185, L0198, L01B3, L01B5, L01CB, L01D0, L01D1, DelMod, L01DF, L01EA, L01FB, L0216, L021F | F$IODel, F$LDDDXY | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fvmodul.asm` | FVModul, L0463, L0491, L0493, L0495, L0497, L04BC, L04BE, L04CD, L04DE, L04F2, L04FA, L0503, L050C, L0519 | F$GCMDir, F$Sleep | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/ASM/NITROS9/MODS/llcocosdc.asm` | start, rdWait, rdRdy, rdChnk, wrWait, wrRdy, wrChnk, wrComp | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L15: USE       defsfile; L16: USE       rbsuper.d |
| `/dd/SOURCECODE/ASM/NITROS9/SCF/term_win80.asm` | See procedure/module declarations in file | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L16: use   defsfile |
| `/dd/SOURCECODE/C/LIB/mod.a` | shiftla, modlink, modload, munlink | F$LINK, F$LOAD, F$UNLINK | L11: use ..../defs/os9defs.a |

## Hardware and runtime dependencies

Descriptor addresses and device names depend on hardware/boot configuration. Module language tags alone do not establish absence of 6309 instructions. Sources: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/flink.asm`, `/dd/SOURCECODE/ASM/NITROS9/KERNEL/funlink.asm`, `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fvmodul.asm`, `/dd/SOURCECODE/ASM/NITROS9/MODS/llcocosdc.asm`, `/dd/SOURCECODE/ASM/NITROS9/SCF/term_win80.asm`, `/dd/SOURCECODE/C/LIB/mod.a`.

## Reusable patterns — interpretation

Keep module identity, edition, size, CRC and binary artifact path together. Pair link ownership with unlink cleanup. Read descriptor manager/driver names before tracing code. These are study patterns inferred from the cited implementations; adapt only after checking the relevant ABI, revision and manual.

## Dangerous, historical or version-specific patterns

Matching name or edition is weak evidence. A CRC/size match links binaries probabilistically; byte equality links stored artifacts exactly, but neither proves which editable source produced them. No source was rebuilt in this pass. Evidence is in the entry-point files above; [the dependency audit](PROVENANCE_DEPENDENCIES.md) and [module comparisons](RUNTIME_MATCHES.md) record unresolved links.

## Verification boundary

Source inspection establishes the text and relationships described here. Only module identity was exercised live in Pass 2; these algorithms, driver operations and build recipes were not executed. No exact editable-source-to-running-binary claim is made. See [SYSTEM_CALL_USAGE](SYSTEM_CALL_USAGE.md) for per-file invocation versus reference distinctions, and [HARDWARE_USAGE](HARDWARE_USAGE.md) for address evidence.
