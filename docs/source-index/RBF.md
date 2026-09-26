# RBF and disk drivers

[Navigation](README.md) · [Provenance/dependencies](PROVENANCE_DEPENDENCIES.md) · [Runtime evidence](RUNTIME_MATCHES.md)

## What the source demonstrates

**Observed in the source paths below:** RBF handles path/directory/file operations; EmuDsk and rb1773 supply different block-device implementations. ddh0 names RBF and EmuDsk. llcocosdc is a distinct low-level SDC path; its presence in SOURCECODE does not make it active in the emulator.

## Best entry points

All references are guest paths, not repository source files. Line numbers use CR-normalized source. Labels below are navigation clues; reading a label is not evidence that the path executes in the current build.

| Source | Useful labels / modules | F$ / I$ references | Includes / definitions |
|---|---|---|---|
| `/dd/SOURCECODE/ASM/NITROS9/RBF/rbf_postbeta6.asm` | start, Create, Creat47, Creat6E, Creat75, Creat7C, Creat7E, Creat83, CreatAC, CreatB5, CreatC7, CreatD9, Creat131, Creat151, RtnMemry, Creat174, l1, Open | F$All64, F$AllBit, F$DelBit, F$GProcP, F$IOQu, F$LDABX, F$Move, F$PrsNam, F$Ret64, F$SchBit, F$Send, F$Sleep, F$SRqMem, F$SRtMem, F$Time | L124: use   /dd/defs/deffile |
| `/dd/SOURCECODE/ASM/NITROS9/RBF/emudsk_beta601.asm` | INIT, init2, GETSTA, start, READ, copy.0, copy.1, noerr, WRITE, reterr, GetSect, gs.1, DriveErr, FixErr, Seek, NotRdy, WP | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L71: use   /dd/DEFS/deffile |
| `/dd/SOURCECODE/ASM/NITROS9/RBF/rb1773.asm` | Init2, l1, GetStat, Return, Term, ex, Log2Phys, start, Read, L00F0, L00F0Lp, L0115, L011D, L0128, L012D, L014BLp, erbtyp, L0176 | F$AllRAM, F$DelRAM, F$IRQ, F$Move, F$RelTsk, F$ResTsk, F$Sleep, F$SRqMem, F$SRtMem, F$VIRQ | L101: use   defsfile |
| `/dd/SOURCECODE/ASM/NITROS9/RBF/ddh0.asm` | See procedure/module declarations in file | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L11: USE os9defs |
| `/dd/SOURCECODE/ASM/NITROS9/MODS/llcocosdc.asm` | start, rdWait, rdRdy, rdChnk, wrWait, wrRdy, wrChnk, wrComp | No literal OS9 invocation detected; wrappers/definitions or numeric calls may apply | L15: USE       defsfile; L16: USE       rbsuper.d |

## Hardware and runtime dependencies

EmuDsk, floppy controller and SDC interfaces are different dependencies. Runtime DD/H1 in the inspected boot use EmuDsk; a generic RBF name does not identify its backend. Sources: `/dd/SOURCECODE/ASM/NITROS9/RBF/rbf_postbeta6.asm`, `/dd/SOURCECODE/ASM/NITROS9/RBF/emudsk_beta601.asm`, `/dd/SOURCECODE/ASM/NITROS9/RBF/rb1773.asm`, `/dd/SOURCECODE/ASM/NITROS9/RBF/ddh0.asm`, `/dd/SOURCECODE/ASM/NITROS9/MODS/llcocosdc.asm`.

## Reusable patterns — interpretation

Keep filesystem operations separate from block transport. Use descriptors and installed boot-module evidence to select the relevant driver before studying request layouts. These are study patterns inferred from the cited implementations; adapt only after checking the relevant ABI, revision and manual.

## Dangerous, historical or version-specific patterns

rbf_postbeta6.asm opens with a warning about deletion of large ToolShed-created segments and allocation-table buffer sizing. It is a source warning, not a bug reproduced here. Do not run mutation examples on reference images or combine save-state memory with changed disks. Evidence is in the entry-point files above; [the dependency audit](PROVENANCE_DEPENDENCIES.md) and [module comparisons](RUNTIME_MATCHES.md) record unresolved links.

## Verification boundary

Source inspection establishes the text and relationships described here. Only module identity was exercised live in Pass 2; these algorithms, driver operations and build recipes were not executed. No exact editable-source-to-running-binary claim is made. See [SYSTEM_CALL_USAGE](SYSTEM_CALL_USAGE.md) for per-file invocation versus reference distinctions, and [HARDWARE_USAGE](HARDWARE_USAGE.md) for address evidence.
