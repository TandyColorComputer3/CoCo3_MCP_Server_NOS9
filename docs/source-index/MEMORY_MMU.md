# Memory and MMU

[Navigation](README.md) · [Provenance/dependencies](PROVENANCE_DEPENDENCIES.md) · [Runtime evidence](RUNTIME_MATCHES.md)

## What the source demonstrates

**Observed in the source paths below:** FMapBlk constructs a block-image buffer, calls F$FreeHB, returns a logical address in R$U, and calls F$SetImg. FDATLog searches mapping information to obtain an address in the process workspace. FMem handles requested process memory size; C mem.a offers runtime-facing wrappers.

## Best entry points

All references are guest paths, not repository source files. Line numbers use CR-normalized source. Labels below are navigation clues; reading a label is not evidence that the path executes in the current build.

| Source | Useful labels / modules | F$ / I$ references | Includes / definitions |
|---|---|---|---|
| `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fmapblk.asm` | FMapBlk, FMapBlk2, L0BA6, L0BAA | F$FreeHB, F$SetImg | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fdatlog.asm` | FDATLog, CmpLBlk | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fmem.asm` | FMem, L05EE, L0602, L0615, L0627, L0629, L0634, L0638 | F$AllImg, F$DelImg | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fcpymem.asm` | FCpyMem, L09FB, L0A01, L09C7, N09D6, L09E7, L09EC | F$LDAXY, F$LDDDXY, F$RelTsk, F$ResTsk, F$STABX | No direct include found; may be included by parent build |
| `/dd/SOURCECODE/C/LIB/mem.a` | sbrk, sbrk10, sbrk20, sbrk30, ibrk, sbloop, ibrk10, ibrk20 | F$MEM | L4: use ..../defs/os9defs.a |

## Hardware and runtime dependencies

coco.d supplies DAT.Task and DAT.Regs symbols; GIMEX variants and H6309 paths change assumptions. These source observations do not certify a general 2 MB memory mapping algorithm. Sources: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fmapblk.asm`, `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fdatlog.asm`, `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fmem.asm`, `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fcpymem.asm`, `/dd/SOURCECODE/C/LIB/mem.a`.

## Reusable patterns — interpretation

Distinguish block identifiers, process logical addresses and host physical RAM. Follow paired mapping/allocation cleanup and error paths, and keep temporary mapping state scoped to its owner. These are study patterns inferred from the cited implementations; adapt only after checking the relevant ABI, revision and manual.

## Dangerous, historical or version-specific patterns

A raw CPU address from one task is not a stable address for another task. The caller’s DAT image, internal descriptor offsets and kernel mappings must match the source revision. Directly transplanting kernel mapping code into an application is unsafe. Evidence is in the entry-point files above; [the dependency audit](PROVENANCE_DEPENDENCIES.md) and [module comparisons](RUNTIME_MATCHES.md) record unresolved links.

## Verification boundary

Source inspection establishes the text and relationships described here. Only module identity was exercised live in Pass 2; these algorithms, driver operations and build recipes were not executed. No exact editable-source-to-running-binary claim is made. See [SYSTEM_CALL_USAGE](SYSTEM_CALL_USAGE.md) for per-file invocation versus reference distinctions, and [HARDWARE_USAGE](HARDWARE_USAGE.md) for address evidence.
