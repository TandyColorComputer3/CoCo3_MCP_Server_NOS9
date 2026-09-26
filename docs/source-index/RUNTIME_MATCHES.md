# Runtime module evidence and source candidates

[Navigation](README.md) · [Modules](MODULES.md)

## Method and confidence

Live read-only survey at 2026-09-26 06:53–06:55 UTC, restored from `nos9_ready_v2` using the existing rebuilt MCP and canonical configuration. Original images were not attached: byte-identical temporary copies of the dev VHD and 63EMU.DSK were used. `os9_restore_ready` reported loadScheduled=true, loadCompleted=true, shellVerified=true, ready=true and postLoadEpoch=1. Fourteen `ident -m NAME` calls and `mdir` completed with status 000 and shellReady=true. MAME stopped cleanly.

A temporary host-only MCP wrapper acquired the existing begin_run/read_text_console/finish_run observation lease after each command and decoded the supported hardware text screen. These short ident results fit on screen; no claim of general stdout capture is made. An initial observer attempt without a lease was rejected, stopped cleanly, and was corrected before this complete survey. No repository implementation was changed.

Confidence levels: **live fingerprint** = module identified in restored RAM with good CRC; **stored binary equality** = bundled SOURCECODE binary equals the corresponding installed boot/CMDS module bytes; **source candidate** = nearby named source and compatible version, not rebuilt/verified exact source. A 24-bit CRC plus name/size/edition/revision is strong identity evidence but not a cryptographic full-RAM comparison. Edition and low-nibble revision are separate fields; Ty/La At/Rv is retained below.

## Live results and candidate mapping

| Module | Bytes | Edition | CRC (good) | Ty/La At/Rv | Candidate source | Confidence / stored evidence |
|---|---:|---:|---|---|---|---|
| Shell | 6999 | 23 | `F7A3C4` | `$11 $80` | `/dd/SOURCECODE/ASM/SHELL/shellplus2.2a.asm` | Stored binary equality: `/dd/SOURCECODE/ASM/SHELL/shell`. Source relationship probable, unbuilt. |
| Krn | 3805 | 19 | `9D89BE` | `$C0 $8A` | `/dd/SOURCECODE/ASM/NITROS9/KERNEL/krn_Beta5.asm` | Live fingerprint only; source candidate unproven. |
| KrnP2 | 3188 | 20 | `CA0AEB` | `$C0 $81` | `/dd/SOURCECODE/ASM/NITROS9/KERNEL/krnp2_ver101.asm` | Stored binary equality: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/krnp2_ver101`. Source relationship probable, unbuilt. |
| IOMan | 2593 | 13 | `40D032` | `$C1 $86` | `/dd/SOURCECODE/ASM/NITROS9/MODS/ioman_beta5.asm` | Stored binary equality: `/dd/SOURCECODE/ASM/NITROS9/MODS/ioman_beta5`. Source relationship probable, unbuilt. |
| SCF | 1918 | 18 | `3508DD` | `$D1 $80` | `/dd/SOURCECODE/ASM/NITROS9/SCF/scf_ver100.asm` | Stored binary equality: `/dd/SOURCECODE/ASM/NITROS9/SCF/scf_ver100`. Source relationship probable, unbuilt. |
| VTIO | 2952 | 4 | `156C70` | `$E1 $80` | `/dd/SOURCECODE/ASM/NITROS9/SCF/vtio_beta6.asm` | Stored binary equality: `/dd/SOURCECODE/ASM/NITROS9/SCF/vtio_beta6`. Source relationship probable, unbuilt. |
| CoWin | 7537 | 2 | `3C70C2` | `$C1 $80` | `/dd/SOURCECODE/ASM/NITROS9/SCF/cowin_beta6.asm` | Stored binary equality: `/dd/SOURCECODE/ASM/NITROS9/SCF/cowin_beta6`. Source relationship probable, unbuilt. |
| RBF | 4751 | 37 | `53B416` | `$D7 $83` | `/dd/SOURCECODE/ASM/NITROS9/RBF/rbf_postbeta6.asm` | Live fingerprint only; source candidate unproven. |
| PipeMan | 595 | 5 | `ECE938` | `$D1 $81` | `/dd/SOURCECODE/ASM/NITROS9/PIPE/pipeman_beta6.asm` | Stored binary equality: `/dd/SOURCECODE/ASM/NITROS9/PIPE/pipeman_beta6`. Source relationship probable, unbuilt. |
| Clock | 517 | 9 | `972B38` | `$C1 $85` | `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock.asm` | Stored binary equality: `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock_6309`. Source relationship probable, unbuilt. |
| Clock2 | 118 | 1 | `6CF198` | `$21 $80` | `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_messemu.asm` | Stored binary equality: `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_messemu_6309`, `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_messemu_6809`. Source relationship probable, unbuilt. |
| EmuDsk | 234 | 6 | `CF433F` | `$E1 $82` | `/dd/SOURCECODE/ASM/NITROS9/RBF/emudsk_beta601.asm` | Stored binary equality: `/dd/SOURCECODE/ASM/NITROS9/RBF/emudsk_beta601`. Source relationship probable, unbuilt. |
| GrfDrv | 8167 | 14 | `199209` | `$C1 $81` | `/dd/SOURCECODE/ASM/grfdrv0724_1998.lzh` | Live fingerprint only; archive member grf.a is a historical lead, not matched current source. |
| Term | 68 | 83 | `53A97B` | `$F1 $80` | `/dd/SOURCECODE/ASM/NITROS9/SCF/term_win80.asm` | Live fingerprint only; source candidate unproven. |

Ten of the fourteen resident module identities have corresponding byte-identical bundled binaries compared with installed boot/CMDS segments. Eleven bundled files are involved because both Clock2 CPU-named files are identical. No exact editable-source/runtime match is claimed.

## Important identity traps

- `/dd/CMDS/shell` is a multi-module pack (Shell plus Date, DeIniz, Echo, Iniz, Link, Load, Save, Unlink). Its Shell segment equals `/dd/SOURCECODE/ASM/SHELL/shell`. `shell.orig` instead identifies edition 22, 6,920 bytes, CRC 61243F; current Shell is edition 23, 6,999 bytes, CRC F7A3C4.
- `/dd/CMDS/basic09` identifies Basic09 edition 25, 22,774 bytes, CRC 69C082, plus Bnkey/BysCall/BFX helpers. `/dd/CMDS/runb` identifies RunB edition 25, 11,569 bytes, CRC F9C7B3, plus Inkey/gfx2/SysCall/GFX. These are disk identifications, not proof they were resident in the restored shell.
- `/dd/CMDS/gfx2`: edition 5, 2,381 bytes, CRC 2F7B08. `/dd/CMDS/gshell`: edition 4, 16,504 bytes, CRC 641943. No GUI or BASIC09 execution was performed.
- Live Clock2 matches both `clock2_messemu_6309` and `clock2_messemu_6809`; provider identity is established more strongly than a CPU requirement.
- RBF edition 37 agrees with the declaration in `rbf_postbeta6.asm`, but there was no matching bundled module/build proof. GrfDrv’s current 8,167-byte edition 14 binary was not matched to the historical archives.
- Host ToolShed identification of `media/63EMU.DSK,OS9Boot` names Term’s file manager SCF and driver VTIO; DD/H1 use RBF/EmuDsk. Names/relationships are verified for that installed boot, not every historical descriptor file.

## Exact observed ident blocks

These are decoded final-screen blocks from the successful run, trimmed to each command’s header output.

### `ident -m shell`

```text
Header for:  Shell
Module size: $1B57    #6999
Module CRC:  $F7A3C4 (Good)
Hdr parity:  $65
Exec. off:   $0074    #116
Data Size:   $1F00    #7936
Edition:     $17      #23
Ty/La At/Rv: $11 $80
Prog mod, 6809 obj, re-en, R/O
```

### `ident -m Krn`

```text
Header for:  Krn
Module size: $0EDD    #3805
Module CRC:  $9D89BE (Good)
Hdr parity:  $21
Edition:     $13      #19
Ty/La At/Rv: $C0 $8A
System mod, Data, re-en, R/O
```

### `ident -m KrnP2`

```text
Header for:  KrnP2
Module size: $0C74    #3188
Module CRC:  $CA0AEB (Good)
Hdr parity:  $81
Edition:     $14      #20
Ty/La At/Rv: $C0 $81
System mod, Data, re-en, R/O
```

### `ident -m IOMan`

```text
Header for:  IOMan
Module size: $0A21    #2593
Module CRC:  $40D032 (Good)
Hdr parity:  $D4
Edition:     $0D      #13
Ty/La At/Rv: $C1 $86
System mod, 6809 obj, re-en, R/O
```

### `ident -m SCF`

```text
Header for:  SCF
Module size: $077E    #1918
Module CRC:  $3508DD (Good)
Hdr parity:  $90
Edition:     $12      #18
Ty/La At/Rv: $D1 $80
File Man mod, 6809 obj, re-en, R/O
```

### `ident -m VTIO`

```text
Header for:  VTIO
Module size: $0B88    #2952
Module CRC:  $156C70 (Good)
Hdr parity:  $59
Exec. off:   $056B    #1387
Data Size:   $0100    #256
Edition:     $04      #4
Ty/La At/Rv: $E1 $80
Dev Dvr mod, 6809 obj, re-en, R/O
```

### `ident -m CoWin`

```text
Header for:  CoWin
Module size: $1D71    #7537
Module CRC:  $3C70C2 (Good)
Hdr parity:  $95
Edition:     $02      #2
Ty/La At/Rv: $C1 $80
System mod, 6809 obj, re-en, R/O
```

### `ident -m RBF`

```text
Header for:  RBF
Module size: $128F    #4751
Module CRC:  $53B416 (Good)
Hdr parity:  $71
Edition:     $25      #37
Ty/La At/Rv: $D7 $83
File Man mod, 6309 obj, re-en, R/O
```

### `ident -m PipeMan`

```text
Header for:  PipeMan
Module size: $0253    #595
Module CRC:  $ECE938 (Good)
Hdr parity:  $B9
Edition:     $05      #5
Ty/La At/Rv: $D1 $81
File Man mod, 6809 obj, re-en, R/O
```

### `ident -m Clock`

```text
Header for:  Clock
Module size: $0205    #517
Module CRC:  $972B38 (Good)
Hdr parity:  $FB
Edition:     $09      #9
Ty/La At/Rv: $C1 $85
System mod, 6809 obj, re-en, R/O
```

### `ident -m Clock2`

```text
Header for:  Clock2
Module size: $0076    #118
Module CRC:  $6CF198 (Good)
Hdr parity:  $6F
Edition:     $01      #1
Ty/La At/Rv: $21 $80
Subr mod, 6809 obj, re-en, R/O
```

### `ident -m EmuDsk`

```text
Header for:  EmuDsk
Module size: $00EA    #234
Module CRC:  $CF433F (Good)
Hdr parity:  $32
Exec. off:   $002F    #47
Data Size:   $00FF    #255
Edition:     $06      #6
Ty/La At/Rv: $E1 $82
Dev Dvr mod, 6809 obj, re-en, R/O
```

### `ident -m GrfDrv`

```text
Header for:  GrfDrv
Module size: $1FE7    #8167
Module CRC:  $199209 (Good)
Hdr parity:  $03
Edition:     $0E      #14
Ty/La At/Rv: $C1 $81
System mod, 6809 obj, re-en, R/O
```

### `ident -m Term`

```text
Header for:  Term
Module size: $0044    #68
Module CRC:  $53A97B (Good)
Hdr parity:  $B6
Edition:     $53      #83
Ty/La At/Rv: $F1 $80
Dev Dsc mod, 6809 obj, re-en, R/O
```

## Resident-name cross-check

`mdir` final screen (status 000) listed:

```text
   Module Directory at 23:55:18
REL         Boot        Krn         KrnP2       KrnP3       IOMan
Init        RBF         EmuDsk      DD          H1          rb1773
D0          D1          D2          Rammer      R0          MD
SCF         VTIO        JoyDrv      SndDrv      CoWin       CoVDG
Term        W           W1          W2          W3          W4
W5          W6          W7          W8          W9          W10
W11         W12         W13         W14         W15         Verm
scbbp       p           VRN         Nil         VI          FTDD
PipeMan     Piper       Pipe        Clock       Clock2      GrfDrv
Attr        Build       Copy        Del         Deldir      dir
Display     List        MakDir      MDir        Merge       Mfree
Rename      Sleep       Tmode       Dump        Error       Free
play        Proc        Shell       Date        DeIniz      Echo
Iniz        Link        Load        Save        Unlink

```

The listing contains local video/window modules and Clock2; it is a restored-session snapshot, not a statement that every bundled driver is loaded.
