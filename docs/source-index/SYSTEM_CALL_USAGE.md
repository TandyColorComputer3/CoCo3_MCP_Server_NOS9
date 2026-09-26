# System-call usage in the priority source

[Navigation](README.md) · [Conceptual route](SYSTEM_CALLS.md)

Scope: 525 selected text/source/build/documentation files in the 20 priority collections. Generated listings, binaries and opaque archives excluded. Case-insensitive F$/I$ tokens were indexed; each distinct file/call pair appears below. There are 129 token spellings: 99 also declared in inspected `/dd/DEFS/os9.d`; 30 are unconfirmed tokens, prose/plurals or local labels, listed separately. This is a literal usage index, not a claim of 129 OS calls. Numeric BASIC09 syscall invocations and indirect calls are not fully decoded.

`call` means a non-comment `os9 F$...` or `os9 I$...` line; `ref` includes dispatch tables, comments and symbolic references. Conditional code remains included. Purpose is the surrounding source label/short line, not an independently verified API definition. All paths below are relative to `/dd/SOURCECODE`; lines are CR-normalized.

## Declared syscall symbols

### F$Alarm

Definition: `/dd/DEFS/os9.d:114`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock.asm` — 6809-family assembly (6309 indicators) | ref 58,350,355 | `header / procedure / data`; L58: fcb   F$Alarm |
| `ASM/NITROS9/SCF/snddrv_beta6.asm` — 6809-family assembly (6309 indicators) | ref 55 | `entry`; L55: * INIT: set bell vector for F$Alarm |

### F$AlHRAM

Definition: `/dd/DEFS/os9.d:179`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fallram.asm:44`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/fallram.asm` — 6809-family assembly (CPU not certified) | ref 44 | `L0995`; L44: * System Call: F$AlHRAM |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 258 | `L009B`; L258: fcb    F$AlHRAM+SysState |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 257 | `L009B`; L257: fcb    F$AlHRAM+SysState |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 258 | `L009B`; L258: fcb    F$AlHRAM+SysState |
| `ASM/NITROS9/SCF/covdg_beta6.asm` — 6809-family assembly (6309 indicators) | call 1875,2098 | `BA010`; L1875: os9   F$AlHRAM   allocate a screen |
| `ASM/NITROS9/SCF/covdg_beta61.asm` — 6809-family assembly (6309 indicators) | call 1909,2132 | `BA010`; L1909: os9   F$AlHRAM   allocate a screen |
| `ASM/NITROS9/SCF/covdg_ver100.asm` — 6809-family assembly (6309 indicators) | call 1906,2129 | `BA010`; L1906: os9   F$AlHRAM   allocate a screen |

### F$All64

Definition: `/dd/DEFS/os9.d:142`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/ffind64.asm:39`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ffind64.asm` — 6809-family assembly (6309 indicators) | ref 39 | `L0A71`; L39: * System Call: F$All64 |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 20,314 | `header / procedure / data`; L20: *                 - Optimized F$All64 to use tfm (BN) |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 20,311 | `header / procedure / data`; L20: *                 - Optimized F$All64 to use tfm (BN) |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 20,312 | `header / procedure / data`; L20: *                 - Optimized F$All64 to use tfm (BN) |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 108,1043 | `ClrLoop`; L108: os9   F$All64        split it into 64 byte chunks |
| `ASM/NITROS9/RBF/msf.asm` — 6809-family assembly (6309 indicators) | call 414,2227 | `open1`; L414: OS9   F$All64        get 64 bytes of storage |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | call 1560 | `FindFile`; L1560: os9   F$All64      allocate path descriptor |

### F$AllBit

Definition: `/dd/DEFS/os9.d:100`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fallbit.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/fallbit.asm` — 6809-family assembly (6309 indicators) | ref 2,22 | `header / procedure / data`; L2: * System Call: F$AllBit |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 288,290 | `L009B`; L288: fcb    F$AllBit |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 287,289 | `L009B`; L287: fcb    F$AllBit |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 288,290 | `L009B`; L288: fcb    F$AllBit |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | call 2627 | `L0E9D`; L2627: os9   F$AllBit     go allocate the bits |
| `ASM/NITROS9/SCF/cowin_beta6.asm` — 6809-family assembly (6309 indicators) | call 465 | `L024A`; L465: os9   F$AllBit       Allocate it |

### F$AllImg

Definition: `/dd/DEFS/os9.d:154`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fallimg.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 511 | `link`; L511: fcb	F$AllImg+SysState |
| `ASM/NITROS9/KERNEL/fallimg.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$AllImg |
| `ASM/NITROS9/KERNEL/fmem.asm` — 6809-family assembly (6309 indicators) | call 58 | `L0615`; L58: os9   F$AllImg     allocate the image in DAT |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 405 | `link`; L405: fcb    F$AllImg+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 395 | `link`; L395: fcb   F$AllImg+SysState |

### F$AllPrc

Definition: `/dd/DEFS/os9.d:171`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fallprc.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/fallprc.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$AllPrc |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 322 | `L009B`; L322: fcb    F$AllPrc+$80 |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 72,319 | `header / procedure / data`; L72: * 6809: Optimized F$AllPrc- speed up clearing Process descriptor data (saves |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 72,320 | `header / procedure / data`; L72: * 6809: Optimized F$AllPrc- speed up clearing Process descriptor data (saves |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 1654 | `L0729`; L1654: os9   F$AllPrc       allocate proc desc |

### F$AllRAM

Definition: `/dd/DEFS/os9.d:153`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fallram.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/fallram.asm` — 6809-family assembly (CPU not certified) | ref 2 | `header / procedure / data`; L2: * System Call: F$AllRAM |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 256 | `L009B`; L256: fcb    F$AllRAM |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 255 | `L009B`; L255: fcb    F$AllRAM |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 256 | `L009B`; L256: fcb    F$AllRAM |
| `ASM/NITROS9/RBF/myram.asm` — 6809-family assembly (6309 indicators) | call 237; ref 675 | `L0094`; L237: os9   F$AllRAM os9 manual doesn't say but |
| `ASM/NITROS9/RBF/ram.asm` — 6809-family assembly (6309 indicators) | call 238; ref 676 | `L0094`; L238: os9   F$AllRAM os9 manual doesn't say but |
| `ASM/NITROS9/RBF/rammer_2mb_beta6.asm` — 6809-family assembly (6309 indicators) | call 142 | `L0078`; L142: os9   F$AllRAM |
| `ASM/NITROS9/RBF/rb1773.asm` — 6809-family assembly (6309 indicators) | call 1131 | `SSWTrk`; L1131: os9   F$AllRAM   allocate some RAM |
| `ASM/NITROS9/SCF/vrn.asm` — 6809-family assembly (CPU not certified) | call 250 | `Chk.SSCA`; L250: os9   F$AllRAM |
| `ASM/PLAY/play.a` — 6809-family assembly (6309 indicators) | call 621 | `L019C`; L621: os9   F$AllRAM |

### F$AllTsk

Definition: `/dd/DEFS/os9.d:159`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/falltsk.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 519 | `link`; L519: fcb	F$AllTsk+SysState |
| `ASM/NITROS9/KERNEL/falltsk.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$AllTsk |
| `ASM/NITROS9/KERNEL/fchain.asm` — 6809-family assembly (6309 indicators) | call 97; ref 49 | `L040C`; L97: os9   F$AllTsk     allocate a new task number |
| `ASM/NITROS9/KERNEL/ffork.asm` — 6809-family assembly (6309 indicators) | call 90 | `SveNPth`; L90: os9    F$AllTsk    allocate the task & setup MMU |
| `ASM/NITROS9/KERNEL/fsleep.asm` — 6809-family assembly (6309 indicators) | ref 109 | `L0782`; L109: cmpb  <D.SysTsk   that stops OS9p1 from doing an F$AllTsk on |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 413 | `link`; L413: fcb    F$AllTsk+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 403 | `link`; L403: fcb   F$AllTsk+SysState |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 1687 | `L0731`; L1687: os9   F$AllTsk       allocate task |
| `ASM/NITROS9/RBF/myram.asm` — 6809-family assembly (6309 indicators) | call 256 | `L0094`; L256: os9   F$AllTsk |
| `ASM/NITROS9/RBF/ram.asm` — 6809-family assembly (6309 indicators) | call 257 | `L0094`; L257: os9   F$AllTsk |

### F$AProc

Definition: `/dd/DEFS/os9.d:138`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/faproc.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/DW/dwio.asm` — 6809-family assembly (CPU not certified) | call 501 | `loop`; L501: os9       F$AProc |
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 491 | `link`; L491: fcb	F$AProc+SysState |
| `ASM/NITROS9/KERNEL/falltsk.asm` — 6809-family assembly (6309 indicators) | ref 138 | `L0CD0`; L138: * Could move this code into Clock, but what about the call to F$AProc (L0D11)? |
| `ASM/NITROS9/KERNEL/faproc.asm` — 6809-family assembly (CPU not certified) | ref 2 | `header / procedure / data`; L2: * System Call: F$AProc |
| `ASM/NITROS9/KERNEL/fchain.asm` — 6809-family assembly (6309 indicators) | call 179 | `L0474`; L179: os9   F$AProc      activate the process |
| `ASM/NITROS9/KERNEL/fexit.asm` — 6809-family assembly (6309 indicators) | call 63 | `L0592`; L63: os9   F$AProc      move parent to active queue |
| `ASM/NITROS9/KERNEL/ffork.asm` — 6809-family assembly (6309 indicators) | call 133 | `SveNPth`; L133: os9    F$AProc     and start the process |
| `ASM/NITROS9/KERNEL/fsend.asm` — 6809-family assembly (6309 indicators) | call 133 | `L06F1`; L133: L06F1    os9   F$AProc      activate the process |
| `ASM/NITROS9/KERNEL/fsleep.asm` — 6809-family assembly (6309 indicators) | call 33 | `L071B`; L33: os9   F$AProc      activate the process |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 385 | `link`; L385: fcb    F$AProc+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 377 | `link`; L377: fcb   F$AProc+SysState |
| `ASM/NITROS9/MODS/dwio.asm` — 6809-family assembly (CPU not certified) | call 514 | `loop`; L514: os9       F$AProc |

### F$Boot

Definition: `/dd/DEFS/os9.d:149`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/ccbfsrqmem.asm:233`, `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fsrqmem.asm:226`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbfsrqmem.asm` — 6809-family assembly (6309 indicators) | ref 233 | `L08F3`; L233: * System Call: F$Boot |
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | call 390,404; ref 420,428,501 | `L01B8`; L390: L01B8	os9	F$Boot		error linking init, try & load boot file |
| `ASM/NITROS9/KERNEL/ccbkrn.txt` — prose | ref 60,61 | `And`; L60: * I changed f$boot to just issue a kernel panic.  I'm not sure if this |
| `ASM/NITROS9/KERNEL/fsrqmem.asm` — 6809-family assembly (6309 indicators) | ref 226 | `L08F3`; L226: * System Call: F$Boot |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | call 328,343; ref 395 | `L01B8`; L328: L01B8    os9    F$Boot        error linking init, try & load boot file |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | call 319,334; ref 387 | `L01B8`; L319: L01B8     os9   F$Boot      error linking init, try & load boot file |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | call 199,348; ref 212 | `L003A`; L199: os9    F$Boot      try & load boot file |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | call 200,345; ref 212 | `L003A`; L200: os9    F$Boot      try & load boot file |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | call 201,346; ref 213 | `L003A`; L201: os9    F$Boot      try & load boot file |

### F$BtMem

Definition: `/dd/DEFS/os9.d:150`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/GIMEX/NEW/boot_common.asm` — 6809-family assembly (CPU not certified) | call 142 | `FragBoot`; L142: os9   F$BtMem |
| `ASM/NITROS9/GIMEX/boot_common.asm` — 6809-family assembly (CPU not certified) | call 139 | `FragBoot`; L139: os9   F$BtMem |
| `ASM/NITROS9/KERNEL/boot_burke.asm` — 6809-family assembly (CPU not certified) | call 98 | `start`; L98: os9   F$BtMem           Allocate memory for boot file |
| `ASM/NITROS9/KERNEL/boot_common.asm` — 6809-family assembly (CPU not certified) | call 142 | `FragBoot`; L142: os9   F$BtMem |
| `ASM/NITROS9/KERNEL/boot_vhd.asm` — 6809-family assembly (CPU not certified) | call 83 | `pause`; L83: os9   F$BtMem |
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 503 | `link`; L503: fcb	F$BtMem+SysState |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 397 | `link`; L397: fcb    F$BtMem+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 389 | `link`; L389: fcb   F$BtMem+SysState |

### F$Chain

Definition: `/dd/DEFS/os9.d:86`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fchain.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 728 | `header / procedure / data`; L728: F$Chain        EQU       5                   Chain process to New Module |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 9660 | `L39A0`; L9660: os9   F$Chain    Chain to other program |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 8824 | `L39A0`; L8824: os9   F$Chain    Chain to other program |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 2250 | `CHAIN`; L2250: os9   F$Chain        Chain to other program |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 2291 | `CHAIN`; L2291: os9   F$Chain        Chain to other program |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 2307 | `CHAIN`; L2307: os9   F$Chain        Chain to other program |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 1712 | `CHAIN`; L1712: os9   F$Chain |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 4535 | `SUREQUI4`; L4535: os9   F$Chain |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 4502 | `SUREQUI4`; L4502: os9   F$Chain |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 4504 | `SUREQUI4`; L4504: os9   F$Chain |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 4544 | `SUREQUI4`; L4544: os9   F$Chain |
| `ASM/NITROS9/KERNEL/fchain.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$Chain |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 17,52,264 | `header / procedure / data`; L17: *                 - Optimized F$Chain (BN) |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 17,52,70,263 | `header / procedure / data`; L17: *                 - Optimized F$Chain (BN) |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 17,52,70,90,264 | `header / procedure / data`; L17: *                 - Optimized F$Chain (BN) |
| `ASM/NITROS9/MODS/sysgo.asm` — 6809-family assembly (CPU not certified) | call 268 | `L0190`; L268: os9   F$Chain |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 1309 | `L0983`; L1309: os9   F$Chain |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 1473 | `CmdEX`; L1473: os9   F$Chain        Chain to the new program |
| `C/LIB/process.a` — 6809-family assembly (CPU not certified) | call 58 | `setpr`; L58: os9 F$CHAIN go do it |

### F$ClrBlk

Definition: `/dd/DEFS/os9.d:176`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fclrblk.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB_p2.doc` — prose | ref 152 | `ERROR`; L152: Use F$MapBlk and F$ClrBlk to map and unmap the window from user memory, if you want.  Each window is four blocks long, taking up 32K of memory.  Screen types 5/ |
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 624 | `Loop3`; L624: * Check for image change now, which lets stuff like F$MapBlk and F$ClrBlk |
| `ASM/NITROS9/KERNEL/fclrblk.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$ClrBlk |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 518 | `Loop3`; L518: * Check for image change now, which lets stuff like F$MapBlk and F$ClrBlk |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 504 | `Loop3`; L504: * Check for image change now, which lets stuff like F$MapBlk and F$ClrBlk |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 328 | `L009B`; L328: fcb    F$ClrBlk |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 325 | `L009B`; L325: fcb    F$ClrBlk |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 326 | `L009B`; L326: fcb    F$ClrBlk |
| `ASM/PLAY/play.a` — 6809-family assembly (6309 indicators) | call 643 | `Fine2`; L643: os9   F$ClrBlk |
| `ASM/VEFIO-WINFO/vefio.asm` — 6809-family assembly (CPU not certified) | call 469 | `header / procedure / data`; L469: os9   F$ClrBlk clear the block |

### F$CmpNam

Definition: `/dd/DEFS/os9.d:98`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fcmpnam.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 481,483 | `link`; L481: fcb	F$CmpNam |
| `ASM/NITROS9/KERNEL/fcmpnam.asm` — 6809-family assembly (CPU not certified) | ref 2,23 | `header / procedure / data`; L2: * System Call: F$CmpNam |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 375,377 | `link`; L375: fcb    F$CmpNam |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 367,369 | `link`; L367: fcb   F$CmpNam |
| `ASM/NITROS9/PIPE/pipeman_named.asm` — 6809-family assembly (CPU not certified) | ref 706 | `OpnXt2`; L706: *   Can't use F$CmpNam here because the strings |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 1983,2007,2564 | `L0F6D`; L1983: os9   F$CmpNam |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 2255,2282,3016; ref 2267 | `L0F6D`; L2255: os9   F$CmpNam       Compare with string pointed to by X |
| `ASM/VEFIO-WINFO/winfo.asm` — 6809-family assembly (6309 indicators) | call 230; ref 8 | `not.l3`; L230: os9   F$CmpNam     see if they're the same |

### F$CpyMem

Definition: `/dd/DEFS/os9.d:111`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fcpymem.asm:2`, `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fcpymem_330.asm:2`, `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fcpymem_beta5.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 502 | `MAIN`; L502: os9   F$CpyMem |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 493 | `MAIN`; L493: os9   F$CpyMem |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 495 | `MAIN`; L495: os9   F$CpyMem |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 507 | `MAIN`; L507: os9   F$CpyMem |
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 506 | `link`; L506: fcb	F$CpyMem |
| `ASM/NITROS9/KERNEL/fcpymem.asm` — 6809-family assembly (6309 indicators) | ref 2,16,79 | `header / procedure / data`; L2: * System Call: F$CpyMem |
| `ASM/NITROS9/KERNEL/fcpymem_330.asm` — 6809-family assembly (6309 indicators) | ref 2,16,79 | `header / procedure / data`; L2: * System Call: F$CpyMem |
| `ASM/NITROS9/KERNEL/fcpymem_beta5.asm` — 6809-family assembly (6309 indicators) | ref 2,16,71 | `header / procedure / data`; L2: * System Call: F$CpyMem |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 400 | `link`; L400: fcb    F$CpyMem |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 38,391 | `header / procedure / data`; L38: * Moved F$CpyMem from KrnP2, which allows shortcut bsr calls to F$Move,etc. *   Much, much faster. (6809) |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 21,303 | `header / procedure / data`; L21: * NitrOS9 V1.09 - Move & optimized F$CpyMem to OS9P1 |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 21,68 | `header / procedure / data`; L21: * NitrOS9 V1.09 - Move & optimized F$CpyMem to OS9P1 |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 21,68 | `header / procedure / data`; L21: * NitrOS9 V1.09 - Move & optimized F$CpyMem to OS9P1 |
| `ASM/NITROS9/SCF/cowin_beta6.asm` — 6809-family assembly (6309 indicators) | call 3703 | `L1381`; L3703: os9   F$CpyMem       copy the window descriptor from process space |
| `ASM/VEFIO-WINFO/winfo.asm` — 6809-family assembly (6309 indicators) | call 147,158,173,196,211,223,241,266,279,344,357,610 | `start`; L147: os9   F$CpyMem     get it |

### F$CRC

Definition: `/dd/DEFS/os9.d:104`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fvmodul.asm:399`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 734 | `header / procedure / data`; L734: F$CRC          EQU       $17                 Generate CRC |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 2156 | `L0C52`; L2156: os9   F$CRC      Calculate module CRC |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 2181 | `L0C52`; L2181: os9   F$CRC      Calculate module CRC |
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 485 | `link`; L485: fcb	F$CRC |
| `ASM/NITROS9/KERNEL/fcrc.asm` — 6809-family assembly (CPU not certified) | ref 1 | `header / procedure / data`; L1: * F$CRC |
| `ASM/NITROS9/KERNEL/fmove.asm` — 6809-family assembly (6309 indicators) | ref 36 | `FMove`; L36: * Entry point from F$CRC |
| `ASM/NITROS9/KERNEL/fmove_old.asm` — 6809-family assembly (6309 indicators) | ref 32 | `FMove`; L32: * Entry point from F$CRC |
| `ASM/NITROS9/KERNEL/fvmodul.asm` — 6809-family assembly (6309 indicators) | ref 399 | `L0635`; L399: * System Call: F$CRC |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 379 | `link`; L379: fcb    F$CRC |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 371 | `link`; L371: fcb   F$CRC |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 435; ref 417,420 | `CRCLp`; L435: os9   F$CRC          Update CRC with new byte |
| `C/LIB/misc.a` — 6809-family assembly (CPU not certified) | call 25 | `crc`; L25: os9 F$CRC call os9 |

### F$CRCMod

Definition: `/dd/DEFS/os9.d:183`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fcrcmod.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/fcrcmod.asm` — 6809-family assembly (CPU not certified) | ref 2 | `header / procedure / data`; L2: * System Call: F$CRCMod |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 39,334 | `header / procedure / data`; L39: *        93/12/17 - Moved F$CRCMod code here to give some room in OS9P1 |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 39,331 | `header / procedure / data`; L39: *        93/12/17 - Moved F$CRCMod code here to give some room in OS9P1 |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 39,332 | `header / procedure / data`; L39: *        93/12/17 - Moved F$CRCMod code here to give some room in OS9P1 |

### F$DATLog

Definition: `/dd/DEFS/os9.d:164`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fdatlog.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 529 | `link`; L529: fcb	F$DATLog+SysState |
| `ASM/NITROS9/KERNEL/fdatlog.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$DATLog |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 423 | `link`; L423: fcb    F$DATLog+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 413 | `link`; L413: fcb   F$DATLog+SysState |

### F$Debug

Definition: `/dd/DEFS/os9.d:123`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/DW/dwio.asm` — 6809-family assembly (CPU not certified) | call 403 | `FRQd1`; L403: os9       F$Debug |
| `ASM/NITROS9/KERNEL/fdebug.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * F$Debug entry point |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 65,332 | `header / procedure / data`; L65: * F$Debug now incorporated, allows for reboot. |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 65,329 | `header / procedure / data`; L65: * F$Debug now incorporated, allows for reboot. |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 65,330 | `header / procedure / data`; L65: * F$Debug now incorporated, allows for reboot. |
| `ASM/NITROS9/MODS/dwio.asm` — 6809-family assembly (CPU not certified) | call 406 | `FRQd1`; L406: os9       F$Debug |
| `ASM/NITROS9/SCF/snddrv_beta6.asm` — 6809-family assembly (6309 indicators) | ref 94 | `BadArgs`; L94: *   $9B if F$Debug call |
| `ASM/NITROS9/SCF/vtio_beta6.asm` — 6809-family assembly (6309 indicators) | call 777 | `L03C8`; L777: os9   F$Debug      And call debugger routine |

### F$DelBit

Definition: `/dd/DEFS/os9.d:101`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fallbit.asm:138`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/fallbit.asm` — 6809-family assembly (6309 indicators) | ref 138,158 | `CalcBit`; L138: * System Call: F$DelBit |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 292,294 | `L009B`; L292: fcb    F$DelBit |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 291,293 | `L009B`; L291: fcb    F$DelBit |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 292,294 | `L009B`; L292: fcb    F$DelBit |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | call 2894 | `L1012`; L2894: os9   F$DelBit   go delete them (no error possible) |
| `ASM/NITROS9/SCF/cowin_beta6.asm` — 6809-family assembly (6309 indicators) | call 485 | `L026A`; L485: os9   F$DelBit       Delete it & return |

### F$DelImg

Definition: `/dd/DEFS/os9.d:155`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fdelimg.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/fdelimg.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$DelImg |
| `ASM/NITROS9/KERNEL/fexit.asm` — 6809-family assembly (6309 indicators) | call 89 | `L05B9`; L89: os9   F$DelImg     delete the ram & DAT image |
| `ASM/NITROS9/KERNEL/fmem.asm` — 6809-family assembly (6309 indicators) | call 72 | `L0629`; L72: os9   F$DelImg |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 320 | `L009B`; L320: fcb    F$DelImg+$80 |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 317 | `L009B`; L317: fcb    F$DelImg+SysState |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 318 | `L009B`; L318: fcb    F$DelImg+SysState |

### F$DelPrc

Definition: `/dd/DEFS/os9.d:172`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fallprc.asm:85`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/fallprc.asm` — 6809-family assembly (6309 indicators) | ref 85 | `L032F`; L85: * System Call: F$DelPrc |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 324 | `L009B`; L324: fcb    F$DelPrc+$80 |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 321 | `L009B`; L321: fcb    F$DelPrc+SysState |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 322 | `L009B`; L322: fcb    F$DelPrc+SysState |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 1787 | `L081D`; L1787: os9   F$DelPrc       Delete the temporary process we used to Load |

### F$DelRAM

Definition: `/dd/DEFS/os9.d:177`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fdelram.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 546 | `link`; L546: fcb	F$DelRAM |
| `ASM/NITROS9/KERNEL/fdelram.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$DelRAM |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 440 | `link`; L440: fcb    F$DelRAM |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 430 | `link`; L430: fcb   F$DelRAM |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 305 | `L009B`; L305: fcb    F$DelRAM |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 302 | `L009B`; L302: fcb    F$DelRAM |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 303 | `L009B`; L303: fcb    F$DelRAM |
| `ASM/NITROS9/RBF/myram.asm` — 6809-family assembly (6309 indicators) | call 678 | `L020A`; L678: os9   F$DelRAM ends mmap showed |
| `ASM/NITROS9/RBF/ram.asm` — 6809-family assembly (6309 indicators) | call 679 | `L020A`; L679: os9   F$DelRAM ends mmap showed |
| `ASM/NITROS9/RBF/rammer_2mb_beta6.asm` — 6809-family assembly (6309 indicators) | call 84 | `L002E`; L84: os9   F$DelRAM      Deallocate the block |
| `ASM/NITROS9/RBF/rb1773.asm` — 6809-family assembly (6309 indicators) | call 1214 | `L0479`; L1214: os9   F$DelRAM   de-allocate image RAM blocks |
| `ASM/NITROS9/SCF/covdg_beta6.asm` — 6809-family assembly (6309 indicators) | call 903,1922,2034 | `L0522`; L903: os9   F$DelRAM     deallocate it from main RAM |
| `ASM/NITROS9/SCF/covdg_beta61.asm` — 6809-family assembly (6309 indicators) | call 936,1956,2067 | `L0522`; L936: os9   F$DelRAM     deallocate it from main RAM |
| `ASM/NITROS9/SCF/covdg_ver100.asm` — 6809-family assembly (6309 indicators) | call 924,1953,2064 | `L0522`; L924: os9   F$DelRAM     deallocate it from main RAM |
| `ASM/NITROS9/SCF/vrn.asm` — 6809-family assembly (CPU not certified) | call 210 | `SS2A.RAM`; L210: os9   F$DelRAM |
| `ASM/PLAY/play.a` — 6809-family assembly (6309 indicators) | call 683 | `L0221`; L683: os9   F$DelRAM |

### F$DelTsk

Definition: `/dd/DEFS/os9.d:160`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/falltsk.asm:23`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 521 | `link`; L521: fcb	F$DelTsk+SysState |
| `ASM/NITROS9/KERNEL/fallprc.asm` — 6809-family assembly (6309 indicators) | call 187 | `L0393`; L187: os9   F$DelTsk     Remove task # for this process |
| `ASM/NITROS9/KERNEL/falltsk.asm` — 6809-family assembly (6309 indicators) | ref 23 | `L0C65`; L23: * System Call: F$DelTsk |
| `ASM/NITROS9/KERNEL/fchain.asm` — 6809-family assembly (6309 indicators) | call 168 | `L0474`; L168: os9   F$DelTsk     delete the old task |
| `ASM/NITROS9/KERNEL/fexit.asm` — 6809-family assembly (6309 indicators) | call 98 | `L05CB`; L98: os9   F$DelTsk     release X's task # |
| `ASM/NITROS9/KERNEL/ffork.asm` — 6809-family assembly (6309 indicators) | call 113 | `SveNPth`; L113: os9    F$DelTsk |
| `ASM/NITROS9/KERNEL/fsleep.asm` — 6809-family assembly (6309 indicators) | call 111 | `L0782`; L111: os9   F$DelTsk |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 415 | `link`; L415: fcb    F$DelTsk+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 405 | `link`; L405: fcb   F$DelTsk+SysState |

### F$ELink

Definition: `/dd/DEFS/os9.d:173`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/flink.asm:24`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 539 | `link`; L539: fcb	F$ELink+SysState |
| `ASM/NITROS9/KERNEL/flink.asm` — 6809-family assembly (6309 indicators) | ref 24 | `FSLink`; L24: * System Call: F$ELink |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 433 | `link`; L433: fcb    F$ELink+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 423 | `link`; L423: fcb   F$ELink+SysState |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 1605 | `L06EE`; L1605: os9   F$ELink |

### F$Exit

Definition: `/dd/DEFS/os9.d:87`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fexit.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 729 | `header / procedure / data`; L729: F$Exit         EQU       6                   Terminate Process |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 2438 | `L0E6D`; L2438: os9   F$Exit |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 2465 | `L0E6D`; L2465: os9   F$Exit |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 398,2320; ref 225 | `BYE`; L398: os9   F$Exit |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 405,2361; ref 232 | `BYE`; L405: os9   F$Exit |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 412,2377; ref 236 | `BYE`; L412: os9   F$Exit |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 85,352,1768 | `L6809`; L85: os9   F$Exit |
| `ASM/FTP/main.a` — 6809-family assembly (CPU not certified) | call 97,104 | `prseext`; L97: os9     F$Exit |
| `ASM/GLIB/GLIB/IPRINT.a` — 6809-family assembly (CPU not certified) | call 131,135,138 | `I$HXA`; L131: OS9   F$Exit |
| `ASM/GLIB/SMASH/Help.a` — 6809-family assembly (CPU not certified) | call 7 | `Help`; L7: OS9   F$Exit     and quit |
| `ASM/GLIB/SMASH/S_Iniz.a` — 6809-family assembly (6309 indicators) | call 200 | `TRAP`; L200: OS9   F$Exit     and exit |
| `ASM/GLIB/SMASH/Smash.a` — 6809-family assembly (CPU not certified) | call 146 | `Exit`; L146: OS9   F$Exit |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 757,9447; ref 107,3286,3326,3359,8647 | `CHEKMENU`; L757: os9   F$Exit |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 747,9231; ref 100,3253,3293,3326,8431 | `CHEKMENU`; L747: os9   F$Exit |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 749,8402; ref 102,3255,3295,3328,7602 | `CHEKMENU`; L749: os9   F$Exit |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 768,8469; ref 108,3295,3335,3368,7669 | `CHEKMENU`; L768: os9   F$Exit |
| `ASM/NITROS9/GIMEX/gimexcheck.a` — 6809-family assembly (CPU not certified) | call 96 | `NoError`; L96: os9   F$Exit |
| `ASM/NITROS9/KERNEL/fchain.asm` — 6809-family assembly (6309 indicators) | call 188 | `L04A1`; L188: os9   F$Exit       exit from the process with error condition |
| `ASM/NITROS9/KERNEL/fexit.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$Exit |
| `ASM/NITROS9/KERNEL/fnproc.asm` — 6809-family assembly (6309 indicators) | call 100; ref 56 | `L0DF2`; L100: os9   F$Exit       Exit with signal # being error code |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 103,111,116,117,266 | `BadIns`; L103: ldb    #18         get error code for F$Exit |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 107,112,116,117,265 | `BadIns`; L107: ldb    #18         get error code for F$Exit |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 108,113,117,118,266 | `BadIns`; L108: ldb    #18         get error code for F$Exit |
| `ASM/NITROS9/RBF/myram.asm` — 6809-family assembly (6309 indicators) | ref 599 | `Linkus`; L599: *        os9   F$Exit take error with us |
| `ASM/NITROS9/RBF/ram.asm` — 6809-family assembly (6309 indicators) | ref 600 | `Linkus`; L600: *        os9   F$Exit take error with us |
| `ASM/PLAY/play.a` — 6809-family assembly (6309 indicators) | call 256,348,687,700 | `Error`; L256: os9   F$Exit |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 415 | `L020D`; L415: os9   F$Exit |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 417 | `exit`; L417: exit     os9   F$Exit         Terminate shellplus |
| `ASM/SOUNDRV/DrvPlay.asm` — 6809-family assembly (CPU not certified) | call 71 | `error`; L71: error os9 F$Exit |
| `ASM/VEFIO-WINFO/vefio.asm` — 6809-family assembly (CPU not certified) | call 552 | `exit`; L552: exit     OS9   F$Exit exit |
| `ASM/VEFIO-WINFO/witesta.asm` — 6809-family assembly (CPU not certified) | call 280 | `exit`; L280: exit     os9   F$exit |
| `C/LIB/abort.a` — 6809-family assembly (CPU not certified) | call 15,39 | `abort`; L15: os9 F$EXIT |
| `C/LIB/cfinish.a` — 6809-family assembly (CPU not certified) | call 12; ref 10 | `_exit`; L12: os9 F$EXIT and bye-bye! |
| `C/LIB/process.a` — 6809-family assembly (CPU not certified) | call 61 | `setpr`; L61: os9 F$EXIT error code already in b reg. |
| `C/LIB/signal.a` — 6809-family assembly (CPU not certified) | call 137 | `intr10`; L137: intr10 os9 F$EXIT status still in B reg. |

### F$Find64

Definition: `/dd/DEFS/os9.d:141`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/ffind64.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/DW/dwio.asm` — 6809-family assembly (CPU not certified) | call 491; ref 495 | `loop`; L491: os9       F$Find64 |
| `ASM/NITROS9/KERNEL/ffind64.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$Find64 |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 29,312 | `header / procedure / data`; L29: *        93/09/10 - F$Find64 (L0A50) - Took out BSR to L0A5C, merged routine |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 29,309 | `header / procedure / data`; L29: *        93/09/10 - F$Find64 (L0A50) - Took out BSR to L0A5C, merged routine |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 29,310 | `header / procedure / data`; L29: *        93/09/10 - F$Find64 (L0A50) - Took out BSR to L0A5C, merged routine |
| `ASM/NITROS9/MODS/dwio.asm` — 6809-family assembly (CPU not certified) | call 501; ref 505 | `loop`; L501: os9       F$Find64 |
| `ASM/NITROS9/MODS/dwiomess.asm` — 6809-family assembly (CPU not certified) | call 469 | `loop`; L469: os9       F$Find64 |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 1183,1359,2058,2075,2089,2099,2130,2147 | `GetPDesc`; L1183: os9   F$Find64       Get address of path descriptor |
| `ASM/NITROS9/SCF/covdg_beta6.asm` — 6809-family assembly (6309 indicators) | call 447; ref 444 | `L024A`; L447: os9   F$Find64     Get ptr to path descriptor for path # in A |
| `ASM/NITROS9/SCF/covdg_beta61.asm` — 6809-family assembly (6309 indicators) | call 477; ref 474 | `L024A`; L477: os9   F$Find64     Get ptr to path descriptor for path # in A |
| `ASM/NITROS9/SCF/covdg_ver100.asm` — 6809-family assembly (6309 indicators) | call 468; ref 465 | `L024A`; L468: os9   F$Find64     Get ptr to path descriptor for path # in A |
| `ASM/NITROS9/SCF/cowin_beta6.asm` — 6809-family assembly (6309 indicators) | call 1124; ref 1120 | `L0592`; L1124: os9   F$Find64       get pointer to path descriptor |
| `ASM/NITROS9/SCF/scf_beta6.asm` — 6809-family assembly (6309 indicators) | call 489 | `L0198`; L489: os9   F$Find64 |
| `ASM/NITROS9/SCF/scf_ver100.asm` — 6809-family assembly (6309 indicators) | call 495 | `L0198`; L495: os9   F$Find64 |

### F$FModul

Definition: `/dd/DEFS/os9.d:174`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/ffmodul.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 541 | `link`; L541: fcb	F$FModul+SysState |
| `ASM/NITROS9/KERNEL/ffmodul.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$FModul |
| `ASM/NITROS9/KERNEL/funload.asm` — 6809-family assembly (6309 indicators) | call 18 | `FUnLoad`; L18: os9   F$FModul     find it in module directory |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 435 | `link`; L435: fcb    F$FModul+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 425 | `link`; L425: fcb   F$FModul+SysState |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 340,1563 | `imod001`; L340: os9   F$FModul       Go find the descriptor in the module directory (into U) |
| `ASM/NITROS9/SCF/cowin_beta6.asm` — 6809-family assembly (6309 indicators) | call 309 | `L00A9`; L309: os9   F$FModul       Get module directory pointer to grfdrv |

### F$Fork

Definition: `/dd/DEFS/os9.d:84`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/ffork.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 726 | `header / procedure / data`; L726: F$Fork         EQU       3                   Start new process |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 1831,9672 | `L09FF`; L1831: os9   F$Fork     Fork shell out |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 1853,8836 | `L09FF`; L1853: os9   F$Fork     Fork shell out |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 2258 | `SHELL`; L2258: os9   F$Fork         Fork a shell |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 2299 | `SHELL`; L2299: os9   F$Fork         Fork a shell |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 2315 | `SHELL`; L2315: os9   F$Fork         Fork a shell |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 1720 | `SHELL`; L1720: os9   F$Fork |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 8664; ref 111,409,854,2521,3144,3149,3154,3238,3266,4721,8552,8655,8656 | `F.FORK`; L8664: os9   F$Fork     Fork the program |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 8448; ref 104,400,838,2488,3111,3116,3121,3205,3233,4672,8336,8439,8440 | `F.FORK`; L8448: os9   F$Fork     Fork the program |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 7619; ref 106,402,840,2490,3113,3118,3123,3207,3235,4678,7507,7610,7611 | `F.FORK`; L7619: os9   F$Fork     Fork the program |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 7686; ref 112,410,864,2530,3153,3158,3163,3247,3275,4728,7574,7677,7678 | `F.FORK`; L7686: os9   F$Fork     Fork the program |
| `ASM/NITROS9/KERNEL/ffork.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$Fork |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | call 246; ref 16,33,34,52,53,70,260 | `L0083`; L246: os9    F$Fork      fork it |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | call 245; ref 16,33,34,52,53,80,259 | `L0083`; L245: os9    F$Fork      fork it |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | call 246; ref 16,33,34,52,53,81,260 | `L0083`; L246: os9    F$Fork      fork it |
| `ASM/NITROS9/MODS/sysgo.asm` — 6809-family assembly (CPU not certified) | call 242,251,275 | `DoStartup`; L242: os9   F$Fork |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 1290,2720 | `L0953`; L1290: os9   F$Fork |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 1450,3219; ref 3257 | `L0953`; L1450: os9   F$Fork         Fork a shell to run the startup file |
| `C/LIB/process.a` — 6809-family assembly (CPU not certified) | call 73 | `os9fork`; L73: os9 F$FORK call os9 |

### F$FreeHB

Definition: `/dd/DEFS/os9.d:158`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/ffreehb.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 517 | `link`; L517: fcb	F$FreeHB+SysState |
| `ASM/NITROS9/KERNEL/ffreehb.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$FreeHB |
| `ASM/NITROS9/KERNEL/fmapblk.asm` — 6809-family assembly (6309 indicators) | call 34 | `FMapBlk2`; L34: os9   F$FreeHB     find the highest free block offset |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 411 | `link`; L411: fcb    F$FreeHB+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 401 | `link`; L401: fcb   F$FreeHB+SysState |

### F$FreeLB

Definition: `/dd/DEFS/os9.d:157`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/ffreehb.asm:76`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 515 | `link`; L515: fcb	F$FreeLB+SysState |
| `ASM/NITROS9/KERNEL/ffreehb.asm` — 6809-family assembly (6309 indicators) | ref 76 | `L0A4B`; L76: * System Call: F$FreeLB |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 409 | `link`; L409: fcb    F$FreeLB+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 399 | `link`; L399: fcb   F$FreeLB+SysState |

### F$GBlkMp

Definition: `/dd/DEFS/os9.d:109`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fgblkmp.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB/UGWBLK.a` — 6809-family assembly (CPU not certified) | call 64,77 | `A000`; L64: OS9   F$GBlkMp   get the system block map |
| `ASM/NITROS9/KERNEL/fgblkmp.asm` — 6809-family assembly (CPU not certified) | ref 2 | `header / procedure / data`; L2: * System Call: F$GBlkMp |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 56,298 | `header / procedure / data`; L56: *                 - Changed F$GModDr to BRA to similar code in F$GBlkMp |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 56,297 | `header / procedure / data`; L56: *                 - Changed F$GModDr to BRA to similar code in F$GBlkMp |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 56,298 | `header / procedure / data`; L56: *                 - Changed F$GModDr to BRA to similar code in F$GBlkMp |

### F$GCMDir

Definition: `/dd/DEFS/os9.d:178`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fgcmdir.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/fgcmdir.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$GCMDir |
| `ASM/NITROS9/KERNEL/fvmodul.asm` — 6809-family assembly (6309 indicators) | call 177 | `L0524`; L177: os9    F$GCMDir    get rid of empty slots in module directory |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 49,330 | `header / procedure / data`; L49: *                 - Changed L0C53 & L0C81 BRA L0C93 to CLRB/RTS (F$GCMDir) |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 49,327 | `header / procedure / data`; L49: *                 - Changed L0C53 & L0C81 BRA L0C93 to CLRB/RTS (F$GCMDir) |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 49,328 | `header / procedure / data`; L49: *                 - Changed L0C53 & L0C81 BRA L0C93 to CLRB/RTS (F$GCMDir) |

### F$GModDr

Definition: `/dd/DEFS/os9.d:110`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fgmoddr.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/fgmoddr.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$GModDr |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 56,300 | `header / procedure / data`; L56: *                 - Changed F$GModDr to BRA to similar code in F$GBlkMp |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 56,299 | `header / procedure / data`; L56: *                 - Changed F$GModDr to BRA to similar code in F$GBlkMp |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 56,300 | `header / procedure / data`; L56: *                 - Changed F$GModDr to BRA to similar code in F$GBlkMp |

### F$GPrDsc

Definition: `/dd/DEFS/os9.d:108`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fgprdsc.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB/USPFLIP.a` — 6809-family assembly (CPU not certified) | call 62; ref 58 | `U$SPFLIP`; L62: OS9   F$GPrDsc  and process descriptor |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 4525 | `SUREQUI4`; L4525: os9   F$GPrDsc |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 4492 | `SUREQUI4`; L4492: os9   F$GPrDsc |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 4494 | `SUREQUI4`; L4494: os9   F$GPrDsc |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 4534 | `SUREQUI4`; L4534: os9   F$GPrDsc |
| `ASM/NITROS9/GIMEX/gimexcheck.a` — 6809-family assembly (CPU not certified) | call 32 | `start`; L32: os9   F$GPrDsc     Get our process descriptor |
| `ASM/NITROS9/KERNEL/fgprdsc.asm` — 6809-family assembly (CPU not certified) | ref 2 | `header / procedure / data`; L2: * System Call: F$GPrDsc |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 296 | `L009B`; L296: fcb    F$GPrDsc |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 295 | `L009B`; L295: fcb    F$GPrDsc |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 296 | `L009B`; L296: fcb    F$GPrDsc |
| `ASM/NITROS9/MODS/sysgo.asm` — 6809-family assembly (CPU not certified) | call 207 | `CopyLoop`; L207: os9   F$GPrDsc |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 504 | `L02D4`; L504: os9   F$GPrDsc |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 522,3212 | `L02D4`; L522: os9   F$GPrDsc       Get our process descriptor |

### F$GProcP

Definition: `/dd/DEFS/os9.d:151`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fgprocp.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/fgprdsc.asm` — 6809-family assembly (CPU not certified) | call 16 | `FGPrDsc`; L16: os9   F$GProcP    Get ptr to process to descriptor |
| `ASM/NITROS9/KERNEL/fgprocp.asm` — 6809-family assembly (CPU not certified) | ref 2 | `header / procedure / data`; L2: * System Call: F$GProcP |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 318 | `L009B`; L318: fcb    F$GProcP+$80 |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 315 | `L009B`; L315: fcb    F$GProcP+SysState |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 316 | `L009B`; L316: fcb    F$GProcP+SysState |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 1356,2055,2072,2086,2096,2127,2144 | `L0595`; L1356: os9   F$GProcP       Get copy of process descriptor |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | call 2096,2273 | `L0B1D`; L2096: os9   F$GProcP     get a pointer to it |
| `ASM/NITROS9/SCF/scf_beta6.asm` — 6809-family assembly (6309 indicators) | call 486 | `L0198`; L486: os9   F$GProcP     Get pointer to parent process descriptor |
| `ASM/NITROS9/SCF/scf_ver100.asm` — 6809-family assembly (6309 indicators) | call 492 | `L0198`; L492: os9   F$GProcP     Get pointer to parent process descriptor |

### F$Icpt

Definition: `/dd/DEFS/os9.d:90`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/ficpt.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 731 | `header / procedure / data`; L731: F$Icpt         EQU       9                   Set signal Intercept |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 1542; ref 1463 | `L07FC`; L1542: os9   F$Icpt     it's memory area @ start of param area |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 1563; ref 1482 | `L07FC`; L1563: os9   F$Icpt     it's memory area @ start of param area |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 197 | `L01D0`; L197: os9   F$Icpt |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 203 | `L01D0`; L203: os9   F$Icpt |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 207 | `L01D0`; L207: os9   F$Icpt |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 224 | `L96`; L224: os9   F$Icpt |
| `ASM/FTP/main.a` — 6809-family assembly (CPU not certified) | call 28 | `Main`; L28: os9     F$Icpt |
| `ASM/GLIB/SMASH/S_Iniz.a` — 6809-family assembly (6309 indicators) | call 115 | `V010`; L115: OS9   F$Icpt     intercept VRN signals |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 517 | `MAIN`; L517: os9   F$Icpt |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 508 | `MAIN`; L508: os9   F$Icpt |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 510 | `MAIN`; L510: os9   F$Icpt |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 522 | `MAIN`; L522: os9   F$Icpt |
| `ASM/NITROS9/KERNEL/ficpt.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$Icpt |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 23,272 | `header / procedure / data`; L23: * V1.11  93/07/26 - Slight opt in F$Icpt |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 23,271 | `header / procedure / data`; L23: * V1.11  93/07/26 - Slight opt in F$Icpt |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 23,272 | `header / procedure / data`; L23: * V1.11  93/07/26 - Slight opt in F$Icpt |
| `ASM/NITROS9/MODS/sysgo.asm` — 6809-family assembly (CPU not certified) | call 139 | `start`; L139: os9   F$Icpt |
| `ASM/PLAY/play.a` — 6809-family assembly (6309 indicators) | call 175 | `start`; L175: os9   F$Icpt |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 257 | `header / procedure / data`; L257: os9   F$Icpt |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 236 | `start`; L236: os9   F$Icpt         Setup the intercept routine |
| `ASM/VEFIO-WINFO/vefio.asm` — 6809-family assembly (CPU not certified) | call 232 | `header / procedure / data`; L232: os9   F$Icpt |
| `C/LIB/intercept.a` — 6809-family assembly (CPU not certified) | call 26 | `header / procedure / data`; L26: os9 F$ICPT call os9 |
| `C/LIB/signal.a` — 6809-family assembly (CPU not certified) | call 72 | `signal20`; L72: os9 F$ICPT |

### F$ID

Definition: `/dd/DEFS/os9.d:93`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fid.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/gfx2_ver1.asm` — 6809-family assembly (6309 indicators) | call 466 | `L02FD`; L466: L02FD    os9   F$ID           Get process ID # into D |
| `ASM/GLIB/GLIB/USPFLIP.a` — 6809-family assembly (CPU not certified) | call 61 | `U$SPFLIP`; L61: OS9   F$ID       get my ID |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 565,4523,9425 | `BILDDESC`; L565: os9   F$ID       Get process # |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 556,4490,9209 | `BILDDESC`; L556: os9   F$ID       Get process # |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 558,4492,8380 | `BILDDESC`; L558: os9   F$ID       Get process # |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 570,4532,8447 | `BILDDESC`; L570: os9   F$ID       Get process # |
| `ASM/NITROS9/GIMEX/gimexcheck.a` — 6809-family assembly (CPU not certified) | call 30 | `start`; L30: os9   F$ID         Get our process ID |
| `ASM/NITROS9/KERNEL/fid.asm` — 6809-family assembly (CPU not certified) | ref 2 | `header / procedure / data`; L2: * System Call: F$ID |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 278 | `L009B`; L278: fcb    F$ID |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 277 | `L009B`; L277: fcb    F$ID |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 278 | `L009B`; L278: fcb    F$ID |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 691 | `L021D`; L691: os9   F$ID |
| `ASM/NITROS9/MODS/sysgo.asm` — 6809-family assembly (CPU not certified) | call 141,204 | `start`; L141: os9   F$ID |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 501,723,2801 | `L02D4`; L501: os9   F$ID |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 519,730,3211,3223,3321; ref 112 | `L02D4`; L519: os9   F$ID           Get our process ID # into A |
| `C/LIB/change.a` — 6809-family assembly (CPU not certified) | call 17,69 | `header / procedure / data`; L17: os9 F$ID get user ID |
| `C/LIB/id.a` — 6809-family assembly (CPU not certified) | call 16,27 | `getpid`; L16: os9 F$ID |

### F$IODel

Definition: `/dd/DEFS/os9.d:145`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/funlink.asm` — 6809-family assembly (6309 indicators) | call 91 | `L0198`; L91: os9    F$IODel     device still being used by somebody else? |
| `ASM/NITROS9/KERNEL/funload.asm` — 6809-family assembly (6309 indicators) | call 53 | `L0A2B`; L53: os9   F$IODel      delete the device from memory |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | ref 149 | `ClrLoop`; L149: fcb   F$IODel+$80    System ONLY |

### F$IOQu

Definition: `/dd/DEFS/os9.d:137`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | ref 143,569,2027,2036 | `ClrLoop`; L143: fcb   F$IOQu+$80     System ONLY |
| `ASM/NITROS9/PIPE/pipeman.asm` — 6809-family assembly (CPU not certified) | call 293 | `L0173`; L293: L0173    os9   F$IOQu       and insert it in the others IO queue |
| `ASM/NITROS9/PIPE/pipeman_beta6.asm` — 6809-family assembly (CPU not certified) | call 302 | `L0173`; L302: L0173    os9   F$IOQu       and insert it in the others IO queue |
| `ASM/NITROS9/PIPE/pipeman_named.asm` — 6809-family assembly (CPU not certified) | call 1507 | `SETQUEUE`; L1507: SETQUEUE os9   F$IOQU |
| `ASM/NITROS9/RBF/msf.asm` — 6809-family assembly (6309 indicators) | call 2941 | `CallDr0`; L2941: CallDr0    OS9   F$IOQu |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | call 2906,3098,3266 | `L103B`; L2906: os9   F$IOQu   queue the process |
| `ASM/NITROS9/SCF/scf_beta6.asm` — 6809-family assembly (6309 indicators) | call 894 | `L046A`; L894: os9   F$IOQu         Put our process into the IO Queue |
| `ASM/NITROS9/SCF/scf_ver100.asm` — 6809-family assembly (6309 indicators) | call 900 | `L046A`; L900: os9   F$IOQu         Put our process into the IO Queue |

### F$IRQ

Definition: `/dd/DEFS/os9.d:136`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/DW/dwio.asm` — 6809-family assembly (CPU not certified) | call 194 | `loop@`; L194: os9       F$IRQ               ;install |
| `ASM/NITROS9/MODS/dwio.asm` — 6809-family assembly (CPU not certified) | call 166 | `loop@`; L166: os9       F$IRQ               ;install |
| `ASM/NITROS9/MODS/dwiomess.asm` — 6809-family assembly (CPU not certified) | call 178 | `loop@`; L178: os9       F$IRQ               ;install |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | ref 147,1413 | `ClrLoop`; L147: fcb   F$IRQ+$80      System ONLY |
| `ASM/NITROS9/RBF/cc3disk_disto.asm` — 6809-family assembly (CPU not certified) | call 87,89,126,134; ref 24,95 | `TERM`; L87: os9   F$IRQ |
| `ASM/NITROS9/RBF/cc3disk_sc2_irq.asm` — 6809-family assembly (CPU not certified) | call 73,75,110,118 | `header / procedure / data`; L73: os9   F$IRQ |
| `ASM/NITROS9/RBF/cc3disk_sc2_slp.asm` — 6809-family assembly (CPU not certified) | call 69,101 | `header / procedure / data`; L69: os9   F$IRQ |
| `ASM/NITROS9/RBF/rb1773.asm` — 6809-family assembly (6309 indicators) | call 253,300 | `l1`; L253: os9   F$IRQ          install IRQ |
| `ASM/NITROS9/RBF/sdisk3_dmc.asm` — 6809-family assembly (CPU not certified) | call 139,141,187,195 | `start`; L139: os9   F$IRQ |
| `ASM/NITROS9/RBF/sdisk3_dpj.asm` — 6809-family assembly (CPU not certified) | call 72,113 | `start`; L72: os9   F$IRQ |
| `ASM/NITROS9/SCF/joydrv_6551l.asm` — 6809-family assembly (6309 indicators) | call 196,303 | `InstIRQ`; L196: os9   F$IRQ install the IRQSvs routine |
| `ASM/NITROS9/SCF/joydrv_6551m.asm` — 6809-family assembly (CPU not certified) | call 121,168 | `L008F`; L121: os9   F$IRQ |
| `ASM/NITROS9/SCF/joydrv_6552l.asm` — 6809-family assembly (CPU not certified) | call 107,145 | `Init`; L107: os9   F$IRQ |
| `ASM/NITROS9/SCF/joydrv_6552m.asm` — 6809-family assembly (CPU not certified) | call 110,151 | `L008F`; L110: os9   F$IRQ |
| `ASM/NITROS9/SCF/s16550v16_lOS9.asm` — 6809-family assembly (CPU not certified) | call 108,118,142 | `L005E`; L108: os9   F$IRQ |
| `ASM/NITROS9/SCF/sc6551.asm` — 6809-family assembly (6309 indicators) | call 246,400 | `Init`; L246: os9   F$IRQ |
| `ASM/NITROS9/SCF/sc6551dragon.asm` — 6809-family assembly (CPU not certified) | call 218,469; ref 473 | `Init`; L218: os9     F$IRQ           ; Install it ! |
| `ASM/NITROS9/SCF/vrn.asm` — 6809-family assembly (CPU not certified) | call 92,123 | `VEntry`; L92: os9   F$IRQ |

### F$LDABX

Definition: `/dd/DEFS/os9.d:169`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fldabx.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/GIMEX/llcocosdc.asm` — 6809-family assembly (6309 indicators) | call 429,432 | `txWord`; L429: txWord         os9       F$LDABX            get byte from user space buffer |
| `ASM/NITROS9/GIMEX/llcocosdc_beta6.asm` — 6809-family assembly (6309 indicators) | call 414,417 | `txWord`; L414: txWord   os9   F$LDABX      get byte from user space buffer |
| `ASM/NITROS9/GIMEX/llcocosdc_ver101.asm` — 6809-family assembly (6309 indicators) | call 446,449; ref 441 | `txWord`; L446: txWord         os9       F$LDABX            get byte from user space buffer |
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 535 | `link`; L535: fcb	F$LDABX+SysState |
| `ASM/NITROS9/KERNEL/fallbit.asm` — 6809-family assembly (6309 indicators) | call 40,90,176,224,311 | `FSAllBit`; L40: os9   F$LDABX      go get original value from bit map |
| `ASM/NITROS9/KERNEL/fchain.asm` — 6809-family assembly (6309 indicators) | call 147 | `L0457`; L147: os9   F$LDABX |
| `ASM/NITROS9/KERNEL/fldabx.asm` — 6809-family assembly (CPU not certified) | ref 2 | `header / procedure / data`; L2: * System Call: F$LDABX |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 429 | `link`; L429: fcb    F$LDABX+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 419 | `link`; L419: fcb   F$LDABX+SysState |
| `ASM/NITROS9/KERNEL/krnp3_ed3_perr.asm` — 6809-family assembly (CPU not certified) | call 242 | `NextDig`; L242: os9   F$LDABX    ;Get digit from user space |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 373,376,384,399,431,1054 | `imod005`; L373: os9   F$LDABX        Get high byte of size |
| `ASM/NITROS9/PIPE/pipeman.asm` — 6809-family assembly (CPU not certified) | call 143 | `Open`; L143: os9   F$LDABX      get last character of the filename |
| `ASM/NITROS9/PIPE/pipeman_beta6.asm` — 6809-family assembly (CPU not certified) | call 152 | `Open`; L152: os9   F$LDABX      get last character of the filename |
| `ASM/NITROS9/PIPE/pipeman_named.asm` — 6809-family assembly (CPU not certified) | call 775,1421 | `GtNext`; L775: os9   F$LDABX |
| `ASM/NITROS9/RBF/msf.asm` — 6809-family assembly (6309 indicators) | call 430,517,529,810,1469,2643 | `open1`; L430: OS9   F$LDABX        get first character of path list |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | call 1095,1825; ref 898,1089 | `WtLn547`; L1095: os9   F$LDABX    grab one byte from the user |

### F$LDAXY

Definition: `/dd/DEFS/os9.d:166`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fld.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 531 | `link`; L531: fcb	F$LDAXY+SysState |
| `ASM/NITROS9/KERNEL/fcpymem.asm` — 6809-family assembly (6309 indicators) | call 121 | `L09EC`; L121: L09EC    os9   F$LDAXY    get byte |
| `ASM/NITROS9/KERNEL/fcpymem_330.asm` — 6809-family assembly (6309 indicators) | call 121 | `L09EC`; L121: L09EC    os9   F$LDAXY    get byte |
| `ASM/NITROS9/KERNEL/fld.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$LDAXY |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 425 | `link`; L425: fcb    F$LDAXY+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 35,415 | `header / procedure / data`; L35: * 6309-Removed 2 BRN's from F$LDAXY, as they were not 2.01 source, and |

### F$LDDDXY

Definition: `/dd/DEFS/os9.d:168`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 533 | `link`; L533: fcb	F$LDDDXY+SysState |
| `ASM/NITROS9/KERNEL/fchain.asm` — 6809-family assembly (6309 indicators) | call 240 | `L04FB`; L240: os9    F$LDDDXY    get module memory size |
| `ASM/NITROS9/KERNEL/fcpymem.asm` — 6809-family assembly (6309 indicators) | call 43,98 | `FCpyMem`; L43: bsr   L0B02         Short cut OS9 F$LDDDXY |
| `ASM/NITROS9/KERNEL/fcpymem_330.asm` — 6809-family assembly (6309 indicators) | call 43,98 | `FCpyMem`; L43: bsr   L0B02         Short cut OS9 F$LDDDXY |
| `ASM/NITROS9/KERNEL/fcpymem_beta5.asm` — 6809-family assembly (6309 indicators) | call 37,91 | `L09C7`; L37: bsr   L0B02        Short cut OS9 F$LDDDXY |
| `ASM/NITROS9/KERNEL/funlink.asm` — 6809-family assembly (6309 indicators) | call 88 | `L0198`; L88: os9    F$LDDDXY    get module type |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 427 | `link`; L427: fcb    F$LDDDXY+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 36,417 | `header / procedure / data`; L36: *   don't appear to useful since F$LDDDXY does the same type of MMU mapping |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 1553,1579,1601 | `FNMLoad`; L1553: os9   F$LDDDXY |
| `ASM/NITROS9/SCF/cowin_beta6.asm` — 6809-family assembly (6309 indicators) | call 348 | `L0102`; L348: os9   F$LDDDXY |

### F$Link

Definition: `/dd/DEFS/os9.d:81`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/flink.asm:43`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 723 | `header / procedure / data`; L723: F$Link         EQU       0                   Link Module |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 2637; ref 68,2622 | `L0F96`; L2637: os9   F$Link     See if it's already in memory & map it in |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 2663; ref 68,2648 | `L0F96`; L2663: os9   F$Link     See if it's already in memory & map it in |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 517; ref 502 | `L03EE`; L517: os9   F$Link         See if it's already in memory & map it in |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 524; ref 509 | `L03EE`; L524: os9   F$Link         See if it's already in memory & map it in |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 531; ref 516 | `L03EE`; L531: os9   F$Link         See if it's already in memory & map it in |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 437 | `L190`; L437: os9   F$Link |
| `ASM/NITROS9/CLOCKS/clock.asm` — 6809-family assembly (6309 indicators) | call 437 | `Init`; L437: os9   F$Link |
| `ASM/NITROS9/CLOCKS/clock2_dw.asm` — 6809-family assembly (CPU not certified) | call 108 | `UpdLeave`; L108: os9       F$Link |
| `ASM/NITROS9/DW/rbdw.asm` — 6809-family assembly (CPU not certified) | call 141; ref 111 | `Init2`; L141: os9     F$Link |
| `ASM/NITROS9/DW/scdwp.asm` — 6809-family assembly (CPU not certified) | call 94 | `header / procedure / data`; L94: os9   F$Link |
| `ASM/NITROS9/DW/scdwv.asm` — 6809-family assembly (6309 indicators) | call 166,605; ref 41 | `termbye`; L166: os9       F$Link |
| `ASM/NITROS9/KERNEL/ccbfsrqmem.asm` — 6809-family assembly (6309 indicators) | call 267 | `L090C`; L267: os9   F$Link |
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | call 470; ref 477 | `link`; L470: os9	F$Link |
| `ASM/NITROS9/KERNEL/ffreehb.asm` — 6809-family assembly (6309 indicators) | ref 24 | `L0A31`; L24: * This gets called directly from within F$Link |
| `ASM/NITROS9/KERNEL/flink.asm` — 6809-family assembly (6309 indicators) | ref 43 | `FELink`; L43: * System Call: F$Link |
| `ASM/NITROS9/KERNEL/fsrqmem.asm` — 6809-family assembly (6309 indicators) | call 256 | `L090C`; L256: os9   F$Link |
| `ASM/NITROS9/KERNEL/fstime.asm` — 6809-family assembly (CPU not certified) | call 35 | `FSTime`; L35: os9   F$Link |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | call 364; ref 371 | `link`; L364: os9    F$Link |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | call 356; ref 363 | `link`; L356: os9   F$Link |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | call 228,365 | `L0077`; L228: os9    F$Link      try & link |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | call 229,362 | `L0077`; L229: os9    F$Link      try & link |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | call 230,363 | `L0077`; L230: os9    F$Link      try & link |
| `ASM/NITROS9/KERNEL/krnp3_ed3_perr.asm` — 6809-family assembly (CPU not certified) | call 93 | `Entry`; L93: os9   F$Link     ;Try to link to it |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 498,516,529 | `IALoop`; L498: os9   F$Link         link to it |
| `ASM/NITROS9/MODS/krnp4_regdump.asm` — 6809-family assembly (6309 indicators) | call 51 | `start`; L51: os9   F$Link     attempt to link to it |
| `ASM/NITROS9/MODS/sysgo.asm` — 6809-family assembly (CPU not certified) | call 149 | `start`; L149: os9   F$Link |
| `ASM/NITROS9/SCF/cowin_beta6.asm` — 6809-family assembly (6309 indicators) | call 2392 | `L0B92`; L2392: os9   F$Link         try & link it |
| `ASM/NITROS9/SCF/krnp4_regdump.asm` — 6809-family assembly (6309 indicators) | call 53 | `start`; L53: os9   F$Link     attempt to link to it |
| `ASM/NITROS9/SCF/vtio_beta6.asm` — 6809-family assembly (6309 indicators) | call 254,1726 | `LinkSys`; L254: os9   F$Link       link to it |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 2077,2522 | `L1022`; L2077: os9   F$Link |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 2362,2905,2942,2950,2969,2996,3007,3180,3234,3252 | `CmdMEq`; L2362: os9   F$Link |
| `ASM/VEFIO-WINFO/vefio.asm` — 6809-family assembly (CPU not certified) | call 699 | `movexy10`; L699: os9   F$Link link to the module |
| `ASM/VEFIO-WINFO/witesta.asm` — 6809-family assembly (CPU not certified) | call 135 | `start`; L135: os9   F$Link       link to the module |
| `C/LIB/mod.a` — 6809-family assembly (CPU not certified) | call 21 | `modlink`; L21: os9 F$LINK call os9 |

### F$Load

Definition: `/dd/DEFS/os9.d:82`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 724 | `header / procedure / data`; L724: F$Load         EQU       1                   Load Module from File |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 2641; ref 68,2622 | `L0F96`; L2641: os9   F$Load     Try loading it & linking it in |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 2667; ref 68,2648 | `L0F96`; L2667: os9   F$Load     Try loading it & linking it in |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 521; ref 502 | `L03EE`; L521: os9   F$Load         Try loading and linking it in |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 528; ref 509 | `L03EE`; L528: os9   F$Load         Try loading and linking it in |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 535; ref 516 | `L03EE`; L535: os9   F$Load         Try loading and linking it in |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 441 | `L190`; L441: os9   F$Load |
| `ASM/NITROS9/KERNEL/fchain.asm` — 6809-family assembly (6309 indicators) | call 204 | `L04B1`; L204: os9    F$Load      try & load it |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | ref 135 | `ClrLoop`; L135: fcb   F$Load         User & System |
| `ASM/NITROS9/SCF/vtio_beta6.asm` — 6809-family assembly (6309 indicators) | call 1730 | `L0905`; L1730: os9   F$Load         load it |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 2914,3189 | `L13EF`; L2914: os9   F$Load |
| `ASM/VEFIO-WINFO/vefio.asm` — 6809-family assembly (CPU not certified) | call 708 | `movexy10`; L708: os9   F$Load load/link to the module |
| `ASM/VEFIO-WINFO/witesta.asm` — 6809-family assembly (CPU not certified) | call 142 | `start`; L142: os9   F$Load       load/link to the module |
| `C/LIB/mod.a` — 6809-family assembly (CPU not certified) | call 38 | `modload`; L38: os9 F$LOAD call os9 |

### F$MapBlk

Definition: `/dd/DEFS/os9.d:175`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fmapblk.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB/UMAPSCB.a` — 6809-family assembly (CPU not certified) | call 18 | `U$MAPSCB`; L18: OS9   F$MapBlk |
| `ASM/GLIB/GLIB_p2.doc` — prose | ref 152,281,308 | `ERROR`; L152: Use F$MapBlk and F$ClrBlk to map and unmap the window from user memory, if you want.  Each window is four blocks long, taking up 32K of memory.  Screen types 5/ |
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 624 | `Loop3`; L624: * Check for image change now, which lets stuff like F$MapBlk and F$ClrBlk |
| `ASM/NITROS9/KERNEL/ffreehb.asm` — 6809-family assembly (6309 indicators) | ref 7 | `header / procedure / data`; L7: * Called from F$MapBlk and from SS.MpGPB) |
| `ASM/NITROS9/KERNEL/fmapblk.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$MapBlk |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 518 | `Loop3`; L518: * Check for image change now, which lets stuff like F$MapBlk and F$ClrBlk |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 504 | `Loop3`; L504: * Check for image change now, which lets stuff like F$MapBlk and F$ClrBlk |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 326 | `L009B`; L326: fcb    F$MapBlk |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 323 | `L009B`; L323: fcb    F$MapBlk |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 324 | `L009B`; L324: fcb    F$MapBlk |
| `ASM/NITROS9/MODS/sysgo.asm` — 6809-family assembly (CPU not certified) | call 212 | `CopyLoop`; L212: os9   F$MapBlk |
| `ASM/NITROS9/SCF/covdg_beta6.asm` — 6809-family assembly (6309 indicators) | call 785,2109 | `DispGfx`; L785: os9   F$MapBlk     map it in to our space |
| `ASM/NITROS9/SCF/covdg_beta61.asm` — 6809-family assembly (6309 indicators) | call 818,2143 | `DispGfx`; L818: os9   F$MapBlk     map it in to our space |
| `ASM/NITROS9/SCF/covdg_ver100.asm` — 6809-family assembly (6309 indicators) | call 806,2140 | `DispGfx`; L806: os9   F$MapBlk     map it in to our space |
| `ASM/NITROS9/SCF/cowin_beta6.asm` — 6809-family assembly (6309 indicators) | call 2444 | `L0BD1`; L2444: os9   F$MapBlk       map blocks into process space |
| `ASM/PLAY/play.a` — 6809-family assembly (6309 indicators) | call 627 | `L019C`; L627: os9   F$MapBlk     Map block of RAM in |
| `ASM/VEFIO-WINFO/vefio.asm` — 6809-family assembly (CPU not certified) | call 458,684 | `header / procedure / data`; L458: os9   F$MapBlk map the block in |

### F$Mem

Definition: `/dd/DEFS/os9.d:88`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fmem.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 730 | `header / procedure / data`; L730: F$Mem          EQU       7                   Set Memory size |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 1711 | `L0918`; L1711: os9   F$Mem      Won't fit, request the required data mem size |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 1732 | `L0918`; L1732: os9   F$Mem      Won't fit, request the required data mem size |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 8884; ref 8887 | `SBRK`; L8884: os9   F$Mem      Attempt to change data area size to D bytes |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 8668; ref 8671 | `SBRK`; L8668: os9   F$Mem      Attempt to change data area size to D bytes |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 7839; ref 7842 | `SBRK`; L7839: os9   F$Mem      Attempt to change data area size to D bytes |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 7906; ref 7909 | `SBRK`; L7906: os9   F$Mem      Attempt to change data area size to D bytes |
| `ASM/NITROS9/KERNEL/fchain.asm` — 6809-family assembly (6309 indicators) | call 245 | `L050E`; L245: L050E    os9    F$Mem       try & get the data memory |
| `ASM/NITROS9/KERNEL/fmem.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$Mem |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 268 | `L009B`; L268: fcb    F$Mem |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 267 | `L009B`; L267: fcb    F$Mem |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 268 | `L009B`; L268: fcb    F$Mem |
| `C/LIB/mem.a` — 6809-family assembly (CPU not certified) | call 22 | `sbrk`; L22: os9 F$MEM re-size memory |

### F$Move

Definition: `/dd/DEFS/os9.d:152`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fmove.asm:2`, `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fmove_old.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/FTP/regdump.a` — 6809-family assembly (CPU not certified) | ref 147 | `reg070`; L147: *        os9   F$Move     move from system to user space |
| `ASM/NITROS9/CLOCKS/clock.asm` — 6809-family assembly (6309 indicators) | call 391 | `FMove`; L391: FMove    os9   F$Move |
| `ASM/NITROS9/CLOCKS/clock2_elim.asm` — 6809-family assembly (CPU not certified) | call 150,195 | `F.NVRAM`; L150: os9   F$Move     go MOVE data |
| `ASM/NITROS9/GIMEX/llcocosdc_ver101.asm` — 6809-family assembly (6309 indicators) | ref 440 | `waitRet`; L440: * Since we are transferring 256 bytes at a time, F$Move might be a better |
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 509 | `link`; L509: fcb	F$Move+SysState |
| `ASM/NITROS9/KERNEL/fchain.asm` — 6809-family assembly (6309 indicators) | call 158,164; ref 114,115,132,156 | `L0471`; L158: L0471    os9   F$Move       move data over? |
| `ASM/NITROS9/KERNEL/fcpymem.asm` — 6809-family assembly (6309 indicators) | ref 56,63,64 | `FCpyMem`; L56: *   from the caller, to help do a 1 shot F$Move command, because in general |
| `ASM/NITROS9/KERNEL/fcpymem_330.asm` — 6809-family assembly (6309 indicators) | ref 56,63,64 | `FCpyMem`; L56: *   from the caller, to help do a 1 shot F$Move command, because in general |
| `ASM/NITROS9/KERNEL/fcpymem_beta5.asm` — 6809-family assembly (6309 indicators) | ref 49,56,57,107,117 | `L09C7`; L49: *   from the caller, to help do a 1 shot F$Move command, because in general |
| `ASM/NITROS9/KERNEL/ffork.asm` — 6809-family assembly (6309 indicators) | call 103,111 | `SveNPth`; L103: os9    F$Move      move parameters to new process |
| `ASM/NITROS9/KERNEL/fgblkmp.asm` — 6809-family assembly (CPU not certified) | call 25 | `L0978`; L25: os9   F$Move      Move it into caller's space |
| `ASM/NITROS9/KERNEL/fgmoddr.asm` — 6809-family assembly (6309 indicators) | ref 32 | `FGModDr`; L32: ***         os9   F$Move      Copy module directory in caller's buffer |
| `ASM/NITROS9/KERNEL/fgprdsc.asm` — 6809-family assembly (CPU not certified) | call 22 | `FGPrDsc`; L22: os9   F$Move      Move it into caller's space |
| `ASM/NITROS9/KERNEL/fmove.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$Move |
| `ASM/NITROS9/KERNEL/fmove_old.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$Move |
| `ASM/NITROS9/KERNEL/fstime.asm` — 6809-family assembly (CPU not certified) | call 28 | `FSTime`; L28: os9   F$Move          Go move it |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 403 | `link`; L403: fcb    F$Move+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 30,38,393 | `header / procedure / data`; L30: * Changed F$Move to use D.IRQTmp to eliminate some TFR's, and changed it so |
| `ASM/NITROS9/KERNEL/krnp3_ed3_perr.asm` — 6809-family assembly (CPU not certified) | call 185 | `MoveBuf`; L185: os9   F$Move     ;Move string to user space |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 1334,1707,2009 | `SSCopy`; L1334: os9   F$Move         Move data to caller |
| `ASM/NITROS9/MODS/krnp4_regdump.asm` — 6809-family assembly (6309 indicators) | call 198 | `reg070`; L198: os9   F$Move     move from system to user space |
| `ASM/NITROS9/PIPE/pipeman.asm` — 6809-family assembly (CPU not certified) | call 242,334 | `read.out`; L242: os9   F$Move       move the data over |
| `ASM/NITROS9/PIPE/pipeman_beta6.asm` — 6809-family assembly (CPU not certified) | call 251,344 | `read.out`; L251: os9   F$Move       move the data over |
| `ASM/NITROS9/PIPE/pipeman_named.asm` — 6809-family assembly (CPU not certified) | call 1685,1850; ref 1281 | `MovSet`; L1685: MovSet   os9   F$Move     ;Do inter-process block move |
| `ASM/NITROS9/RBF/cc3disk_disto.asm` — 6809-family assembly (CPU not certified) | call 529 | `wtrak`; L529: os9   F$Move |
| `ASM/NITROS9/RBF/cc3disk_sc2_irq.asm` — 6809-family assembly (CPU not certified) | call 476 | `L0384`; L476: os9   F$Move |
| `ASM/NITROS9/RBF/cc3disk_sc2_slp.asm` — 6809-family assembly (CPU not certified) | call 433 | `L0323`; L433: os9   F$Move |
| `ASM/NITROS9/RBF/myram.asm` — 6809-family assembly (6309 indicators) | call 277,282,295,386,454,514,620 | `L0094`; L277: os9   F$Move our dummy sector 0 |
| `ASM/NITROS9/RBF/ram.asm` — 6809-family assembly (6309 indicators) | call 278,283,296,387,455,515,621 | `L0094`; L278: os9   F$Move our dummy sector 0 |
| `ASM/NITROS9/RBF/rb1773.asm` — 6809-family assembly (6309 indicators) | call 1158 | `SSWTrk`; L1158: os9   F$Move         Copy from caller to temporary task |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | call 967,1199; ref 1088 | `RdLn49B`; L967: os9   F$Move     move 'em |
| `ASM/NITROS9/RBF/sdisk3_dpj.asm` — 6809-family assembly (CPU not certified) | call 581,674,707 | `L0488`; L581: os9   F$Move |
| `ASM/NITROS9/SCF/covdg_beta6.asm` — 6809-family assembly (6309 indicators) | call 1586 | `Rt.Palet`; L1586: os9   F$Move |
| `ASM/NITROS9/SCF/covdg_beta61.asm` — 6809-family assembly (6309 indicators) | call 1619 | `Rt.Palet`; L1619: os9   F$Move |
| `ASM/NITROS9/SCF/covdg_ver100.asm` — 6809-family assembly (6309 indicators) | call 1616 | `Rt.Palet`; L1616: os9   F$Move |
| `ASM/NITROS9/SCF/cowin_beta6.asm` — 6809-family assembly (6309 indicators) | call 2247,4402 | `L0ABB`; L2247: os9   F$Move         move 'em |
| `ASM/NITROS9/SCF/krnp4_regdump.asm` — 6809-family assembly (6309 indicators) | call 198 | `reg070`; L198: os9   F$Move     move from system to user space |
| `ASM/NITROS9/SCF/s16550v16_lOS9.asm` — 6809-family assembly (CPU not certified) | call 374,489 | `L0284`; L374: os9   F$Move |
| `ASM/NITROS9/SCF/scf_beta6.asm` — 6809-family assembly (6309 indicators) | call 601,633,831,1015; ref 590 | `putkey`; L601: os9   F$Move       Move it |
| `ASM/NITROS9/SCF/scf_ver100.asm` — 6809-family assembly (6309 indicators) | call 607,639,837,1022; ref 596 | `putkey`; L607: os9   F$Move       Move it |
| `ASM/NITROS9/SCF/vtio_beta6.asm` — 6809-family assembly (6309 indicators) | call 1352 | `MovMsPkt`; L1352: os9   F$Move         move it to the process & return |

### F$NMLink

Definition: `/dd/DEFS/os9.d:116`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 8582; ref 8594 | `F.NMLINK`; L8582: os9   F$NMLink     Attempt to link to module (w/o mapping it into our space) |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 8366; ref 8378 | `F.NMLINK`; L8366: os9   F$NMLink     Attempt to link to module (w/o mapping it into our space) |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 7537; ref 7549 | `F.NMLINK`; L7537: os9   F$NMLink     Attempt to link to module (w/o mapping it into our space) |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 7604; ref 7616 | `F.NMLINK`; L7604: os9   F$NMLink     Attempt to link to module (w/o mapping it into our space) |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | ref 152 | `ClrLoop`; L152: fcb   F$NMLink       User & System |
| `ASM/NITROS9/SCF/cowin_beta6.asm` — 6809-family assembly (6309 indicators) | call 432 | `L01FB`; L432: os9   F$NMLink |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 2498,2554,2709 | `L13EF`; L2498: os9   F$NMLink |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 2902,2993,3177 | `L13EF`; L2902: os9   F$NMLink |

### F$NMLoad

Definition: `/dd/DEFS/os9.d:117`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 8570; ref 102,8566,8594 | `F.NMLOAD`; L8570: os9   F$NMLoad     Attempt to load it |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 8354; ref 95,8350,8378 | `F.NMLOAD`; L8354: os9   F$NMLoad     Attempt to load it |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 7525; ref 97,7521,7549 | `F.NMLOAD`; L7525: os9   F$NMLoad     Attempt to load it |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 7592; ref 103,7588,7616 | `F.NMLOAD`; L7592: os9   F$NMLoad     Attempt to load it |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | ref 154 | `ClrLoop`; L154: fcb   F$NMLoad       User & System |
| `ASM/NITROS9/SCF/cowin_beta6.asm` — 6809-family assembly (6309 indicators) | call 441 | `L021F`; L441: os9   F$NMLoad |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 2501,2711 | `L13EF`; L2501: os9   F$NMLoad |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 2911,3186 | `L13EF`; L2911: os9   F$NMLoad |

### F$NProc

Definition: `/dd/DEFS/os9.d:139`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fnproc.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 493 | `link`; L493: fcb	F$NProc+SysState |
| `ASM/NITROS9/KERNEL/fchain.asm` — 6809-family assembly (6309 indicators) | call 180 | `L0474`; L180: os9   F$NProc      go to it |
| `ASM/NITROS9/KERNEL/fexit.asm` — 6809-family assembly (6309 indicators) | call 64 | `L05A2`; L64: L05A2    os9   F$NProc      start next proc in active queue |
| `ASM/NITROS9/KERNEL/fnproc.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$NProc |
| `ASM/NITROS9/KERNEL/fsleep.asm` — 6809-family assembly (6309 indicators) | call 118 | `L0792`; L118: os9   F$NProc |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 387 | `link`; L387: fcb    F$NProc+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 379 | `link`; L379: fcb   F$NProc+SysState |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | call 248 | `L0093`; L248: L0093    os9    F$NProc     let it take over |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | call 247 | `L0093`; L247: L0093    os9    F$NProc     let it take over |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | call 248 | `L0093`; L248: L0093    os9    F$NProc     let it take over |

### F$NVRAM

Definition: `/dd/DEFS/os9.d:197`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_elim.asm` — 6809-family assembly (CPU not certified) | ref 109 | `UpdatCk0`; L109: NewSvc   fcb   F$NVRAM    Eliminator adds one new service call |

### F$PErr

Definition: `/dd/DEFS/os9.d:96`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 732 | `header / procedure / data`; L732: F$PErr         EQU       $0F                 Print Error |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 3075 | `L1287`; L3075: L1287    os9   F$PErr     Print error message |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 3142 | `L1287`; L3142: L1287    os9   F$PErr     Print error message |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 529 | `L040E`; L529: L040E    os9   F$PErr         Print error to screen |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 536 | `L040E`; L536: L040E    os9   F$PErr         Print error to screen |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 543 | `L040E`; L543: L040E    os9   F$PErr         Print error to screen |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 447 | `PRerror`; L447: PRerror    os9   F$PErr |
| `ASM/NITROS9/KERNEL/krnp3_ed3_perr.asm` — 6809-family assembly (CPU not certified) | ref 79,98 | `header / procedure / data`; L79: SvcTbl   fcb   F$PErr     ;System call number |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | ref 141,1968 | `ClrLoop`; L141: fcb   F$PErr         User & System |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 370 | `L01A4`; L370: L01A4    os9   F$PErr |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 362 | `L01A4`; L362: L01A4    os9   F$PErr         Print the error message |
| `C/LIB/misc.a` — 6809-family assembly (CPU not certified) | call 31 | `prerr`; L31: os9 F$PERR call os9 |

### F$PrsNam

Definition: `/dd/DEFS/os9.d:97`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fprsnam.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 479 | `link`; L479: fcb	F$PrsNam |
| `ASM/NITROS9/KERNEL/fprsnam.asm` — 6809-family assembly (CPU not certified) | ref 2 | `header / procedure / data`; L2: * System Call: F$PrsNam |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 373 | `link`; L373: fcb    F$PrsNam |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 365 | `link`; L365: fcb   F$PrsNam |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 1090 | `L0459`; L1090: os9   F$PrsNam       parse it |
| `ASM/NITROS9/PIPE/pipeman.asm` — 6809-family assembly (CPU not certified) | call 137,150 | `Open`; L137: os9   F$PrsNam     parse /pipe |
| `ASM/NITROS9/PIPE/pipeman_beta6.asm` — 6809-family assembly (CPU not certified) | call 146,159 | `Open`; L146: os9   F$PrsNam     parse /pipe |
| `ASM/NITROS9/PIPE/pipeman_named.asm` — 6809-family assembly (CPU not certified) | call 287,302,316,331 | `SetCnt`; L287: os9   F$PrsNam   ;Error if driver name (e.g. /pipe) invalid |
| `ASM/NITROS9/RBF/msf.asm` — 6809-family assembly (6309 indicators) | call 502,551,993,3154 | `findf1`; L502: OS9   F$PrsNam       skip device name |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | call 275,1742,1832 | `CreatC7`; L275: os9   F$PrsNam     parse it to get filename |
| `ASM/NITROS9/SCF/scf_beta6.asm` — 6809-family assembly (6309 indicators) | call 241,246 | `open`; L241: os9   F$PrsNam     Parse it |
| `ASM/NITROS9/SCF/scf_ver100.asm` — 6809-family assembly (6309 indicators) | call 247,252 | `open`; L247: os9   F$PrsNam     Parse it |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 1192,2479,2579,2994 | `L08B6`; L1192: L08B6    os9   F$PrsNam |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 1333,2881,3032,3521 | `L08B6`; L1333: L08B6    os9   F$PrsNam       Valid OS9 device name? |
| `ASM/VEFIO-WINFO/vefio.asm` — 6809-family assembly (CPU not certified) | call 720 | `movexy10`; L720: os9   F$PrsNam get length of window name |
| `ASM/VEFIO-WINFO/winfo.asm` — 6809-family assembly (6309 indicators) | call 122 | `start`; L122: os9   F$PrsNam     get end of name |

### F$ReBoot

Definition: `/dd/DEFS/os9.d:182`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/freboot.asm` — unknown | ref 2 | `header / procedure / data`; L2: * F$ReBoot entry point |

### F$RegDmp

Definition: `/dd/DEFS/os9.d:196`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/MODS/krnp4_regdump.asm` — 6809-family assembly (6309 indicators) | ref 15,43,43 | `header / procedure / data`; L15: * be changed by resetting the equate at "F$RegDmp" |
| `ASM/NITROS9/RBF/myram.asm` — 6809-family assembly (6309 indicators) | ref 714 | `L022A`; L714: *        os9   F$RegDmp lets take a look |
| `ASM/NITROS9/RBF/ram.asm` — 6809-family assembly (6309 indicators) | ref 715 | `L022A`; L715: *        os9   F$RegDmp lets take a look |
| `ASM/NITROS9/SCF/krnp4_regdump.asm` — 6809-family assembly (6309 indicators) | ref 15,45,45 | `header / procedure / data`; L15: * be changed by resetting the equate at "F$RegDmp" |

### F$RelTsk

Definition: `/dd/DEFS/os9.d:163`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/falltsk.asm:119`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 527 | `link`; L527: fcb	F$RelTsk+SysState |
| `ASM/NITROS9/KERNEL/falltsk.asm` — 6809-family assembly (6309 indicators) | ref 37,119 | `L0C68`; L37: bra   L0CC3      do a F$RelTsk |
| `ASM/NITROS9/KERNEL/fcpymem.asm` — 6809-family assembly (6309 indicators) | call 69 | `FCpyMem`; L69: lbsr  L0CC3        Short cut OS9 F$RelTsk |
| `ASM/NITROS9/KERNEL/fcpymem_330.asm` — 6809-family assembly (6309 indicators) | call 69 | `FCpyMem`; L69: lbsr  L0CC3        Short cut OS9 F$RelTsk |
| `ASM/NITROS9/KERNEL/fcpymem_beta5.asm` — 6809-family assembly (6309 indicators) | call 62,119 | `L09C7`; L62: lbsr  L0CC3        Short cut OS9 F$RelTsk |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 421 | `link`; L421: fcb    F$RelTsk+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 411 | `link`; L411: fcb   F$RelTsk+SysState |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 459; ref 457 | `CRCSavLp`; L459: os9   F$RelTsk       Release it |
| `ASM/NITROS9/RBF/rb1773.asm` — 6809-family assembly (6309 indicators) | call 1205 | `L0479`; L1205: os9   F$RelTsk   release the task |

### F$ResTsk

Definition: `/dd/DEFS/os9.d:162`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/falltsk.asm:83`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 525 | `link`; L525: fcb	F$ResTsk+SysState |
| `ASM/NITROS9/KERNEL/falltsk.asm` — 6809-family assembly (6309 indicators) | ref 83 | `L0C9F`; L83: * System Call: F$ResTsk |
| `ASM/NITROS9/KERNEL/fcpymem.asm` — 6809-family assembly (6309 indicators) | call 50 | `FCpyMem`; L50: lbsr  L0CA6        Short cut OS9 F$ResTsk |
| `ASM/NITROS9/KERNEL/fcpymem_330.asm` — 6809-family assembly (6309 indicators) | call 50 | `FCpyMem`; L50: lbsr  L0CA6        Short cut OS9 F$ResTsk |
| `ASM/NITROS9/KERNEL/fcpymem_beta5.asm` — 6809-family assembly (6309 indicators) | call 43,97 | `L09C7`; L43: lbsr  L0CA6        Short cut OS9 F$ResTsk |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 419 | `link`; L419: fcb    F$ResTsk+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 409 | `link`; L409: fcb   F$ResTsk+SysState |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 359 | `imod002`; L359: os9   F$ResTsk       Reserve a temp task for the module's DAT image (in B) |
| `ASM/NITROS9/RBF/rb1773.asm` — 6809-family assembly (6309 indicators) | call 1135 | `SSWTrk`; L1135: os9   F$ResTsk   reserve a task number for the copy |

### F$Ret64

Definition: `/dd/DEFS/os9.d:143`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/ffind64.asm:165`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ffind64.asm` — 6809-family assembly (6309 indicators) | ref 165 | `L0AF0`; L165: * System Call: F$Ret64 |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 316 | `L009B`; L316: fcb    F$Ret64+$80 |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 313 | `L009B`; L313: fcb    F$Ret64+SysState |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 314 | `L009B`; L314: fcb    F$Ret64+SysState |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 111,988,1133 | `ClrLoop`; L111: os9   F$Ret64 |
| `ASM/NITROS9/PIPE/pipeman_named.asm` — 6809-family assembly (CPU not certified) | call 626; ref 635 | `IsAsOld`; L626: os9   F$Ret64    ; and give back descriptor |
| `ASM/NITROS9/RBF/msf.asm` — 6809-family assembly (6309 indicators) | call 1121,1842,1856,2358 | `open11`; L1121: os9   F$Ret64        return the directory entry to the system |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | call 604 | `Rt100Mem`; L604: os9   F$Ret64 |

### F$SchBit

Definition: `/dd/DEFS/os9.d:99`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fallbit.asm:241`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/fallbit.asm` — 6809-family assembly (6309 indicators) | ref 241,263 | `L08E0`; L241: * System Call: F$SchBit |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 284,286 | `L009B`; L284: fcb    F$SchBit |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 283,285 | `L009B`; L283: fcb    F$SchBit |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 284,286 | `L009B`; L284: fcb    F$SchBit |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | call 2597 | `L0E54`; L2597: os9   F$SchBit |
| `ASM/NITROS9/SCF/cowin_beta6.asm` — 6809-family assembly (6309 indicators) | call 476 | `L025B`; L476: os9   F$SchBit       Find it |

### F$Send

Definition: `/dd/DEFS/os9.d:89`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fsend.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 9427 | `RPTERR`; L9427: os9   F$Send |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 9211 | `RPTERR`; L9211: os9   F$Send |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 8382 | `RPTERR`; L8382: os9   F$Send |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 8449 | `RPTERR`; L8449: os9   F$Send |
| `ASM/NITROS9/CLOCKS/clock.asm` — 6809-family assembly (6309 indicators) | call 221 | `NoGet`; L221: os9   F$Send |
| `ASM/NITROS9/DW/dwio.asm` — 6809-family assembly (CPU not certified) | call 526,545,561,573 | `dosleepq`; L526: os9       F$Send              ; send signal, don't think we can do anything about an error result anyway.. so |
| `ASM/NITROS9/DW/scdwv.asm` — 6809-family assembly (6309 indicators) | call 523 | `ssig`; L523: os9       F$Send |
| `ASM/NITROS9/KERNEL/fsend.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$Send |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 47,270 | `header / procedure / data`; L47: *                   (F$Send errors) |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 47,269 | `header / procedure / data`; L47: *                   (F$Send errors) |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 47,270 | `header / procedure / data`; L47: *                   (F$Send errors) |
| `ASM/NITROS9/MODS/dwio.asm` — 6809-family assembly (CPU not certified) | call 543,564,584,597 | `dosleepq`; L543: os9       F$Send              ; send signal, don't think we can do anything about an error result anyway.. so |
| `ASM/NITROS9/MODS/dwiomess.asm` — 6809-family assembly (CPU not certified) | call 481,506,527,547,560 | `loop`; L481: os9       F$Send |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 1354,2067 | `L0595`; L1354: os9   F$Send         wake up the process that was next in the IO Queue |
| `ASM/NITROS9/PIPE/pipeman.asm` — 6809-family assembly (CPU not certified) | call 111 | `L009C`; L111: os9   F$Send       send a wake-up signal to the process |
| `ASM/NITROS9/PIPE/pipeman_beta6.asm` — 6809-family assembly (CPU not certified) | call 120 | `L009C`; L120: os9   F$Send       send a wake-up signal to the process |
| `ASM/NITROS9/PIPE/pipeman_named.asm` — 6809-family assembly (CPU not certified) | call 869 | `SENDSIG`; L869: os9   F$SEND |
| `ASM/NITROS9/RBF/msf.asm` — 6809-family assembly (6309 indicators) | call 1544 | `unlock2`; L1544: OS9   F$SEND         wake up the waiting process |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | call 2046,2271,3149 | `L0AE2`; L2046: os9   F$Send |
| `ASM/NITROS9/SCF/s16550v16_lOS9.asm` — 6809-family assembly (CPU not certified) | call 596,750,773,927 | `L0476`; L596: os9   F$Send |
| `ASM/NITROS9/SCF/sc6551.asm` — 6809-family assembly (6309 indicators) | call 733,877,1048,1075,1083 | `RSendSig`; L733: os9   F$Send |
| `ASM/NITROS9/SCF/sc6551dragon.asm` — 6809-family assembly (CPU not certified) | call 542 | `L01F8`; L542: L01F8   os9     F$Send          ; send wakeup signal |
| `ASM/NITROS9/SCF/scbbp.asm` — 6809-family assembly (6309 indicators) | call 195,399 | `W020`; L195: os9   F$Send       send a signal to wake it |
| `ASM/NITROS9/SCF/scf_beta6.asm` — 6809-family assembly (6309 indicators) | call 951 | `L04C6`; L951: os9   F$Send         Send it to process |
| `ASM/NITROS9/SCF/scf_ver100.asm` — 6809-family assembly (6309 indicators) | call 957 | `L04C6`; L957: os9   F$Send         Send it to process |
| `ASM/NITROS9/SCF/vrn.asm` — 6809-family assembly (CPU not certified) | call 324,344 | `IRQLoop`; L324: os9   F$Send     send signal, ignore error (if any) |
| `ASM/NITROS9/SCF/vtio_beta6.asm` — 6809-family assembly (6309 indicators) | call 572,846,855,1464 | `L0276`; L572: os9   F$Send       and send it to process |
| `ASM/NITROS9/SCF/wordpakii.asm` — 6809-family assembly (CPU not certified) | call 320 | `L020E`; L320: os9   F$Send |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 509,2384,2831 | `L02D4`; L509: os9   F$Send |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 530,2705,3359 | `L02D4`; L530: os9   F$Send         Send it to parent |
| `C/LIB/process.a` — 6809-family assembly (CPU not certified) | call 10 | `kill`; L10: os9 F$SEND |

### F$SetImg

Definition: `/dd/DEFS/os9.d:156`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 513 | `link`; L513: fcb	F$SetImg+SysState |
| `ASM/NITROS9/KERNEL/fmapblk.asm` — 6809-family assembly (6309 indicators) | call 54 | `FMapBlk2`; L54: os9   F$SetImg     Change process dsc to reflect new blocks |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 407 | `link`; L407: fcb    F$SetImg+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 397 | `link`; L397: fcb   F$SetImg+SysState |

### F$SetTsk

Definition: `/dd/DEFS/os9.d:161`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/falltsk.asm:49`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 523,887 | `link`; L523: fcb	F$SetTsk+SysState |
| `ASM/NITROS9/KERNEL/falltsk.asm` — 6809-family assembly (6309 indicators) | ref 49 | `L0C68`; L49: * System Call: F$SetTsk |
| `ASM/NITROS9/KERNEL/fnproc.asm` — 6809-family assembly (6309 indicators) | ref 45 | `L0DB9`; L45: lbsr  TstImg      do a F$SetTsk if the ImgChg flag is set |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 777 | `L0E2B`; L777: lbsr    TstImg        check image, and F$SetTsk (PRESERVES A) |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 407,736 | `link`; L407: fcb   F$SetTsk+SysState |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 1892 | `L08A9`; L1892: os9   F$SetTsk       Set the DAT registers to what the process DAT image says |
| `ASM/NITROS9/RBF/myram.asm` — 6809-family assembly (6309 indicators) | call 652 | `L01D8`; L652: os9   F$SetTsk |
| `ASM/NITROS9/RBF/ram.asm` — 6809-family assembly (6309 indicators) | call 653 | `L01D8`; L653: os9   F$SetTsk |
| `ASM/NITROS9/SCF/covdg_beta6.asm` — 6809-family assembly (6309 indicators) | call 1830 | `Rt.SLGBf`; L1830: os9   F$SetTsk |
| `ASM/NITROS9/SCF/covdg_beta61.asm` — 6809-family assembly (6309 indicators) | call 1862 | `Rt.SLGBf`; L1862: os9   F$SetTsk |
| `ASM/NITROS9/SCF/covdg_ver100.asm` — 6809-family assembly (6309 indicators) | call 1859 | `Rt.SLGBf`; L1859: os9   F$SetTsk |

### F$Sleep

Definition: `/dd/DEFS/os9.d:91`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fsleep.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/gfx2_ver1.asm` — 6809-family assembly (6309 indicators) | call 612; ref 596 | `L0409`; L612: os9   F$Sleep        Yes, sleep until signal received |
| `ASM/GLIB/GLIB/USLPNSEL.a` — 6809-family assembly (CPU not certified) | call 26 | `U015`; L26: U015     OS9   F$Sleep |
| `ASM/GLIB/GLIB_p3.doc` — prose | ref 145,145 | `U$SAVE`; L145: Since OS-9 Level II (6809) is not a real-time operating system, events are not guaranteed to occur at regular times.  This can easily be seen by changing ONE BY |
| `ASM/GLIB/SMASH/KeyPress.a` — 6809-family assembly (CPU not certified) | call 76 | `P012`; L76: OS9   F$Sleep |
| `ASM/GLIB/SMASH/Playball.a` — 6809-family assembly (CPU not certified) | call 109,158 | `S030`; L109: OS9   F$Sleep    or until some time has passed... |
| `ASM/GLIB/SMASH/S_Title.a` — 6809-family assembly (CPU not certified) | call 330 | `Tights`; L330: OS9   F$Sleep |
| `ASM/GLIB/SMASH/Smash.a` — 6809-family assembly (CPU not certified) | call 154 | `Z200`; L154: OS9   F$Sleep |
| `ASM/GSHELL/debug.a` — 6809-family assembly (6309 indicators) | ref 156 | `dmpregs2`; L156: *   os9  F$Sleep |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 728,4407,4579,5010,5076,5147,5772 | `WAITLOOP`; L728: os9   F$Sleep    Sleep until signal received |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 718,4374,4546,4961,5027,5098,5421 | `WAITLOOP`; L718: os9   F$Sleep    Sleep until signal received |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 720,4376,4548,4967,5033,5104,5427 | `WAITLOOP`; L720: os9   F$Sleep    Sleep until signal received |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 739,4416,4588,5025,5091,5162,5485 | `WAITLOOP`; L739: os9   F$Sleep    Sleep until signal received |
| `ASM/NITROS9/DW/scdwv.asm` — 6809-family assembly (6309 indicators) | call 367 | `TimedSlp`; L367: os9       F$Sleep |
| `ASM/NITROS9/KERNEL/fsleep.asm` — 6809-family assembly (6309 indicators) | ref 2,14 | `header / procedure / data`; L2: * System Call: F$Sleep |
| `ASM/NITROS9/KERNEL/fvmodul.asm` — 6809-family assembly (6309 indicators) | call 312 | `L05BA`; L312: os9   F$Sleep |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 19,274 | `header / procedure / data`; L19: * 18.5   93/01/18 - Fixed bug in F$Sleep (LCB) |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 19,273 | `header / procedure / data`; L19: * 18.5   93/01/18 - Fixed bug in F$Sleep (LCB) |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 19,274 | `header / procedure / data`; L19: * 18.5   93/01/18 - Fixed bug in F$Sleep (LCB) |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 2122 | `L097A`; L2122: os9   F$Sleep |
| `ASM/NITROS9/PIPE/pipeman.asm` — 6809-family assembly (CPU not certified) | call 436 | `L018B`; L436: os9   F$Sleep      sleep forever |
| `ASM/NITROS9/PIPE/pipeman_beta6.asm` — 6809-family assembly (CPU not certified) | call 447 | `L018B`; L447: os9   F$Sleep      sleep forever |
| `ASM/NITROS9/PIPE/pipeman_named.asm` — 6809-family assembly (CPU not certified) | call 1560 | `SgSlp01`; L1560: os9   F$SLEEP    ;Caller sleeps until signaled |
| `ASM/NITROS9/RBF/cc3disk_disto.asm` — 6809-family assembly (CPU not certified) | call 593 | `l0424`; L593: os9   F$Sleep |
| `ASM/NITROS9/RBF/cc3disk_sc2_irq.asm` — 6809-family assembly (CPU not certified) | call 536 | `L0424`; L536: os9   F$Sleep |
| `ASM/NITROS9/RBF/cc3disk_sc2_slp.asm` — 6809-family assembly (CPU not certified) | call 493 | `L03C3`; L493: os9   F$Sleep |
| `ASM/NITROS9/RBF/msf.asm` — 6809-family assembly (6309 indicators) | call 455,1375 | `getFAT0`; L455: OS9   F$SLEEP        sleep until ready |
| `ASM/NITROS9/RBF/parallel.asm` — 6809-family assembly (CPU not certified) | call 63 | `Nap`; L63: os9   F$Sleep |
| `ASM/NITROS9/RBF/rb1773.asm` — 6809-family assembly (6309 indicators) | call 627; ref 54,1254 | `tmout`; L627: os9   F$Sleep       if not system then sleep |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | call 2124,2951,3164 | `L0B42`; L2124: os9   F$Sleep      sleep that amount |
| `ASM/NITROS9/RBF/sdisk3_dmc.asm` — 6809-family assembly (CPU not certified) | call 844,924 | `L06B7`; L844: L06B7    os9   F$Sleep |
| `ASM/NITROS9/SCF/covdg_beta6.asm` — 6809-family assembly (6309 indicators) | call 433 | `L0209`; L433: os9   F$Sleep |
| `ASM/NITROS9/SCF/covdg_beta61.asm` — 6809-family assembly (6309 indicators) | call 463 | `L0209`; L463: os9   F$Sleep |
| `ASM/NITROS9/SCF/covdg_ver100.asm` — 6809-family assembly (6309 indicators) | call 454 | `L0209`; L454: os9   F$Sleep |
| `ASM/NITROS9/SCF/cowin_beta6.asm` — 6809-family assembly (6309 indicators) | call 1111,4472 | `L0582`; L1111: os9   F$Sleep |
| `ASM/NITROS9/SCF/s16550v16_lOS9.asm` — 6809-family assembly (CPU not certified) | call 549,569,575,681 | `L0402`; L549: os9   F$Sleep |
| `ASM/NITROS9/SCF/sc6551.asm` — 6809-family assembly (6309 indicators) | call 668 | `TimedSlp`; L668: os9   F$Sleep |
| `ASM/NITROS9/SCF/sc6551dragon.asm` — 6809-family assembly (CPU not certified) | call 318 | `L00CF`; L318: os9     F$Sleep         ; Put caller to sleep |
| `ASM/NITROS9/SCF/scbbp.asm` — 6809-family assembly (6309 indicators) | call 201,283 | `W020`; L201: os9   F$Sleep      and go to sleep forever |
| `ASM/NITROS9/SCF/scbbt.asm` — 6809-family assembly (6309 indicators) | call 143,398 | `L0068`; L143: os9   F$Sleep |
| `ASM/NITROS9/SCF/scdpp.asm` — 6809-family assembly (CPU not certified) | call 108 | `L0055`; L108: os9     F$Sleep |
| `ASM/NITROS9/SCF/sspak.asm` — 6809-family assembly (CPU not certified) | call 144 | `SSWait0`; L144: os9   F$Sleep |
| `ASM/NITROS9/SCF/vtio_beta6.asm` — 6809-family assembly (6309 indicators) | call 310 | `ReadSlp`; L310: os9   F$Sleep |
| `ASM/NITROS9/SCF/wordpakii.asm` — 6809-family assembly (CPU not certified) | call 491 | `L03A8`; L491: os9   F$Sleep |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 349,2281,2724 | `L0171`; L349: os9   F$Sleep |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 335,2579; ref 3592 | `L0171`; L335: os9   F$Sleep |
| `ASM/VEFIO-WINFO/vefio.asm` — 6809-family assembly (CPU not certified) | call 256 | `header / procedure / data`; L256: os9   F$Sleep sleep till mouse click/ key press |
| `C/LIB/misc.a` — 6809-family assembly (CPU not certified) | call 14,38 | `pause`; L14: os9 F$SLEEP |

### F$SLink

Definition: `/dd/DEFS/os9.d:148`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/flink.asm:4`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 499 | `link`; L499: fcb	F$SLink+SysState |
| `ASM/NITROS9/KERNEL/fchain.asm` — 6809-family assembly (6309 indicators) | call 199 | `L04B1`; L199: os9    F$SLink     map it into new process DAT image |
| `ASM/NITROS9/KERNEL/flink.asm` — 6809-family assembly (6309 indicators) | ref 4 | `header / procedure / data`; L4: * System Call: F$SLink |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 393 | `link`; L393: fcb    F$SLink+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 385 | `link`; L385: fcb   F$SLink+SysState |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 496 | `IALoop`; L496: os9   F$SLink        link to it |

### F$SPrior

Definition: `/dd/DEFS/os9.d:94`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fsprior.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/fsprior.asm` — 6809-family assembly (CPU not certified) | ref 2 | `header / procedure / data`; L2: * System Call: F$SPrior |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 276 | `L009B`; L276: fcb    F$SPrior |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 275 | `L009B`; L275: fcb    F$SPrior |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 276 | `L009B`; L276: fcb    F$SPrior |
| `ASM/NITROS9/MODS/sysgo.asm` — 6809-family assembly (CPU not certified) | call 143 | `start`; L143: os9   F$SPrior |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 2727,2856 | `L15F2`; L2727: os9   F$SPrior |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 3217,3224,3400 | `L15F2`; L3217: os9   F$SPrior       Set our priority so child will inherit it |
| `C/LIB/process.a` — 6809-family assembly (CPU not certified) | call 31 | `setpr`; L31: os9 F$SPRIOR call os9 |

### F$SRqMem

Definition: `/dd/DEFS/os9.d:134`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/ccbfsrqmem.asm:2`, `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fsrqmem.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_elim.asm` — 6809-family assembly (CPU not certified) | call 129 | `F.NVRAM`; L129: os9   F$SRqMem |
| `ASM/NITROS9/DW/dwio.asm` — 6809-family assembly (CPU not certified) | call 126 | `DWInit`; L126: os9       F$SRqMem |
| `ASM/NITROS9/GIMEX/NEW/boot_common.asm` — 6809-family assembly (CPU not certified) | call 144; ref 43,46,147 | `FragBoot`; L144: os9   F$SRqMem |
| `ASM/NITROS9/GIMEX/boot_common.asm` — 6809-family assembly (CPU not certified) | call 141; ref 43,46,144 | `FragBoot`; L141: os9   F$SRqMem |
| `ASM/NITROS9/KERNEL/boot_burke.asm` — 6809-family assembly (CPU not certified) | call 62,96 | `start`; L62: os9   F$SRqMem     Request 512 byte buffer from OS9P1 |
| `ASM/NITROS9/KERNEL/boot_common.asm` — 6809-family assembly (CPU not certified) | call 144; ref 43,46,147 | `FragBoot`; L144: os9   F$SRqMem |
| `ASM/NITROS9/KERNEL/boot_rom.asm` — 6809-family assembly (6309 indicators) | call 42 | `start`; L42: os9   F$SRqMem |
| `ASM/NITROS9/KERNEL/boot_vhd.asm` — 6809-family assembly (CPU not certified) | call 63 | `pause`; L63: os9   F$SRqMem   request one page of RAM |
| `ASM/NITROS9/KERNEL/boot_wd1002.asm` — 6809-family assembly (CPU not certified) | call 242; ref 245 | `GotSide`; L242: os9   F$SRqMem |
| `ASM/NITROS9/KERNEL/ccbfsrqmem.asm` — 6809-family assembly (6309 indicators) | ref 2,6,15,40 | `header / procedure / data`; L2: * System Call: F$SRqMem |
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | call 415; ref 22,422,487 | `L01D0`; L415: os9	F$SRqMem	get memory - U is our starting address |
| `ASM/NITROS9/KERNEL/ccbkrn.txt` — prose | ref 27,53 | `header / procedure / data`; L27: either, it now simply calls f$srqmem to do this for us and the |
| `ASM/NITROS9/KERNEL/fallimg.asm` — 6809-family assembly (6309 indicators) | ref 16,18 | `FAllImg`; L16: * 6309 NOTE: IF W IS USED HERE, TRY TO PRESERVE IT AS F$SRQMEM WILL |
| `ASM/NITROS9/KERNEL/fallprc.asm` — 6809-family assembly (6309 indicators) | call 30 | `L02FE`; L30: os9    F$SRqMem    request the memory for it |
| `ASM/NITROS9/KERNEL/ffind64.asm` — 6809-family assembly (6309 indicators) | call 72 | `L0A89`; L72: os9   F$SRqMem     request mem for it |
| `ASM/NITROS9/KERNEL/fsrqmem.asm` — 6809-family assembly (6309 indicators) | ref 2,6,15,40 | `header / procedure / data`; L2: * System Call: F$SRqMem |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 22,381 | `header / procedure / data`; L22: * F$SRqMem now properly scans the DAT images of the system to update |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 22,373 | `header / procedure / data`; L22: * F$SRqMem now properly scans the DAT images of the system to update |
| `ASM/NITROS9/MODS/dwio.asm` — 6809-family assembly (CPU not certified) | call 109 | `DWInit`; L109: os9       F$SRqMem |
| `ASM/NITROS9/MODS/dwiomess.asm` — 6809-family assembly (CPU not certified) | call 121 | `start`; L121: os9       F$SRqMem |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 77,644,1927 | `start`; L77: os9   F$SRqMem       allocate memory |
| `ASM/NITROS9/PIPE/pipeman.asm` — 6809-family assembly (CPU not certified) | call 155 | `L0060`; L155: os9   F$SRqMem     request one page (256 bytes) for the pipe |
| `ASM/NITROS9/PIPE/pipeman_beta6.asm` — 6809-family assembly (CPU not certified) | call 164 | `L0060`; L164: os9   F$SRqMem     request one page (256 bytes) for the pipe |
| `ASM/NITROS9/PIPE/pipeman_named.asm` — 6809-family assembly (CPU not certified) | call 484 | `DoNew`; L484: os9   F$SrqMem   ;Attempt to allocate buffer |
| `ASM/NITROS9/RBF/cc3disk_disto.asm` — 6809-family assembly (CPU not certified) | call 518 | `wtrak`; L518: os9   F$SRqMem |
| `ASM/NITROS9/RBF/cc3disk_sc2_irq.asm` — 6809-family assembly (CPU not certified) | call 465 | `L0384`; L465: os9   F$SRqMem |
| `ASM/NITROS9/RBF/cc3disk_sc2_slp.asm` — 6809-family assembly (CPU not certified) | call 422 | `L0323`; L422: os9   F$SRqMem |
| `ASM/NITROS9/RBF/msf.asm` — 6809-family assembly (6309 indicators) | call 468,2189,2322 | `open012`; L468: os9   F$SRQMEM       request memory |
| `ASM/NITROS9/RBF/myram.asm` — 6809-family assembly (6309 indicators) | call 177,216 | `GTime`; L177: os9   F$SRqMem |
| `ASM/NITROS9/RBF/ram.asm` — 6809-family assembly (6309 indicators) | call 178,217 | `GTime`; L178: os9   F$SRqMem |
| `ASM/NITROS9/RBF/rb1773.asm` — 6809-family assembly (6309 indicators) | call 258 | `l1`; L258: os9   F$SRqMem       Request sector buffer |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | call 719,1554 | `Del358`; L719: os9   F$SRqMem |
| `ASM/NITROS9/RBF/sdisk3_dpj.asm` — 6809-family assembly (CPU not certified) | call 118,599,699 | `L0087`; L118: os9   F$SRqMem |
| `ASM/NITROS9/SCF/covdg_beta6.asm` — 6809-family assembly (6309 indicators) | call 87,1492 | `Read`; L87: os9   F$SRqMem       request from top of sys ram |
| `ASM/NITROS9/SCF/covdg_beta61.asm` — 6809-family assembly (6309 indicators) | call 88,1525 | `Init`; L88: os9   F$SRqMem       request from top of sys ram |
| `ASM/NITROS9/SCF/covdg_ver100.asm` — 6809-family assembly (6309 indicators) | call 107,1520 | `Init`; L107: os9   F$SRqMem       request from top of sys ram |
| `ASM/NITROS9/SCF/cowin_beta6.asm` — 6809-family assembly (6309 indicators) | call 361 | `L0102`; L361: os9   F$SRqMem       Reserve it (note: only $2cf is used so far) |
| `ASM/NITROS9/SCF/s16550v16_lOS9.asm` — 6809-family assembly (CPU not certified) | call 136 | `L00AC`; L136: os9   F$SRqMem |
| `ASM/NITROS9/SCF/sc6551.asm` — 6809-family assembly (6309 indicators) | call 277 | `NoSwap`; L277: os9   F$SRqMem       get extended buffer |
| `ASM/NITROS9/SCF/scf_beta6.asm` — 6809-family assembly (6309 indicators) | call 251; ref 373 | `open1`; L251: os9   F$SRqMem     Allocate it |
| `ASM/NITROS9/SCF/scf_ver100.asm` — 6809-family assembly (6309 indicators) | call 257; ref 379 | `open1`; L257: os9   F$SRqMem     Allocate it |

### F$SRtMem

Definition: `/dd/DEFS/os9.d:135`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/ccbfsrqmem.asm:140`, `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fsrqmem.asm:133`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_elim.asm` — 6809-family assembly (CPU not certified) | call 167,200 | `NVR.RtM`; L167: os9   F$SRtMem |
| `ASM/NITROS9/KERNEL/boot_burke.asm` — 6809-family assembly (CPU not certified) | call 92 | `start`; L92: os9   F$SRtMem          Deallocate our old sector buffer |
| `ASM/NITROS9/KERNEL/boot_vhd.asm` — 6809-family assembly (CPU not certified) | call 81 | `pause`; L81: os9   F$SRtMem |
| `ASM/NITROS9/KERNEL/ccbfsrqmem.asm` — 6809-family assembly (6309 indicators) | ref 140 | `L0894`; L140: * System Call: F$SRtMem |
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 489 | `link`; L489: fcb	F$SRtMem+SysState |
| `ASM/NITROS9/KERNEL/fallprc.asm` — 6809-family assembly (6309 indicators) | call 190 | `L0393`; L190: os9   F$SRtMem     Deallocate process dsc. from system memory pool |
| `ASM/NITROS9/KERNEL/ffind64.asm` — 6809-family assembly (6309 indicators) | call 204 | `L0B10`; L204: os9   F$SRtMem |
| `ASM/NITROS9/KERNEL/fsrqmem.asm` — 6809-family assembly (6309 indicators) | ref 133 | `L0894`; L133: * System Call: F$SRtMem |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 383 | `link`; L383: fcb    F$SRtMem+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 375 | `link`; L375: fcb   F$SRtMem+SysState |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 797,1946 | `L02B4`; L797: os9   F$SRtMem       return mem |
| `ASM/NITROS9/PIPE/pipeman.asm` — 6809-family assembly (CPU not certified) | call 96 | `Close`; L96: os9   F$SRtMem     return the memory |
| `ASM/NITROS9/PIPE/pipeman_beta6.asm` — 6809-family assembly (CPU not certified) | call 105 | `Close`; L105: os9   F$SRtMem     return the memory |
| `ASM/NITROS9/PIPE/pipeman_named.asm` — 6809-family assembly (CPU not certified) | call 971; ref 973 | `ZapIt`; L971: os9   F$SRtMem |
| `ASM/NITROS9/RBF/cc3disk_disto.asm` — 6809-family assembly (CPU not certified) | call 553 | `l03d3`; L553: os9   F$SRtMem |
| `ASM/NITROS9/RBF/cc3disk_sc2_irq.asm` — 6809-family assembly (CPU not certified) | call 500 | `L03D3`; L500: os9   F$SRtMem |
| `ASM/NITROS9/RBF/cc3disk_sc2_slp.asm` — 6809-family assembly (CPU not certified) | call 457 | `L0372`; L457: os9   F$SRtMem |
| `ASM/NITROS9/RBF/msf.asm` — 6809-family assembly (6309 indicators) | call 1815,1853,2310,2352 | `closef`; L1815: os9   F$SRTMem       return system memory |
| `ASM/NITROS9/RBF/myram.asm` — 6809-family assembly (6309 indicators) | call 528,701 | `DClr`; L528: os9   F$SRtMem give back FatScra |
| `ASM/NITROS9/RBF/ram.asm` — 6809-family assembly (6309 indicators) | call 529,702 | `DClr`; L529: os9   F$SRtMem give back FatScra |
| `ASM/NITROS9/RBF/rb1773.asm` — 6809-family assembly (6309 indicators) | call 316 | `Term`; L316: os9   F$SRtMem |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | call 597,823 | `Rt100Mem`; L597: os9   F$SRtMem   return the memory to system |
| `ASM/NITROS9/RBF/sdisk3_dpj.asm` — 6809-family assembly (CPU not certified) | call 80,83,724 | `start`; L80: os9   F$SRtMem |
| `ASM/NITROS9/SCF/covdg_beta6.asm` — 6809-family assembly (6309 indicators) | call 98,220,909; ref 16 | `IsOdd`; L98: os9   F$SRtMem       return system memory |
| `ASM/NITROS9/SCF/covdg_beta61.asm` — 6809-family assembly (6309 indicators) | call 99,249,942; ref 16 | `IsOdd`; L99: os9   F$SRtMem       return system memory |
| `ASM/NITROS9/SCF/covdg_ver100.asm` — 6809-family assembly (6309 indicators) | call 118,240,930; ref 16 | `IsOdd`; L118: os9   F$SRtMem       return system memory |
| `ASM/NITROS9/SCF/cowin_beta6.asm` — 6809-family assembly (6309 indicators) | call 581 | `Lp4`; L581: os9   F$SRtMem       Return graphics table memory to system |
| `ASM/NITROS9/SCF/s16550v16_lOS9.asm` — 6809-family assembly (CPU not certified) | call 106 | `L005E`; L106: os9   F$SRtMem |
| `ASM/NITROS9/SCF/sc6551.asm` — 6809-family assembly (6309 indicators) | call 384 | `KeepDTR`; L384: os9   F$SRtMem |
| `ASM/NITROS9/SCF/scf_beta6.asm` — 6809-family assembly (6309 indicators) | call 408 | `L013D`; L408: os9   F$SRtMem     Return buffer memory to system |
| `ASM/NITROS9/SCF/scf_ver100.asm` — 6809-family assembly (6309 indicators) | call 414 | `L013D`; L414: os9   F$SRtMem     Return buffer memory to system |

### F$SSvc

Definition: `/dd/DEFS/os9.d:144`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fssvc.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock.asm` — 6809-family assembly (6309 indicators) | call 483 | `InitCont`; L483: os9   F$SSvc |
| `ASM/NITROS9/CLOCKS/clock2_elim.asm` — 6809-family assembly (CPU not certified) | call 210 | `NVR.Err`; L210: os9   F$SSvc |
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 497 | `link`; L497: fcb	F$SSvc+SysState |
| `ASM/NITROS9/KERNEL/fssvc.asm` — 6809-family assembly (CPU not certified) | ref 2 | `header / procedure / data`; L2: * System Call: F$SSVC |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 391 | `link`; L391: fcb    F$SSvc+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 383 | `link`; L383: fcb   F$SSvc+SysState |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | call 182 | `krnp2`; L182: os9    F$SSvc |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | call 185 | `krnp2`; L185: os9    F$SSvc |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | call 186 | `krnp2`; L186: os9    F$SSvc |
| `ASM/NITROS9/KERNEL/krnp3_ed3_perr.asm` — 6809-family assembly (CPU not certified) | call 90 | `Entry`; L90: os9   F$SSvc     ;Install services in table |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 115; ref 157 | `ClrLoop`; L115: os9   F$SSvc         set up calls |
| `ASM/NITROS9/MODS/krnp4_regdump.asm` — 6809-family assembly (6309 indicators) | call 48 | `start`; L48: os9   F$SSvc     insert the new op code in the table |
| `ASM/NITROS9/SCF/krnp4_regdump.asm` — 6809-family assembly (6309 indicators) | call 50 | `start`; L50: os9   F$SSvc     insert the new op code in the table |

### F$SSWI

Definition: `/dd/DEFS/os9.d:95`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fsswi.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/fsswi.asm` — 6809-family assembly (CPU not certified) | ref 2 | `header / procedure / data`; L2: * System Call: F$SSWI |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 54,280 | `header / procedure / data`; L54: *                 - Minor mods to F$SSWI call |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 54,279 | `header / procedure / data`; L54: *                 - Minor mods to F$SSWI call |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 54,280 | `header / procedure / data`; L54: *                 - Minor mods to F$SSWI call |

### F$STABX

Definition: `/dd/DEFS/os9.d:170`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fldabx.asm:32`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/GIMEX/llcocosdc.asm` — 6809-family assembly (6309 indicators) | call 450,453 | `rxWord`; L450: os9       F$STABX            store in user space buffer |
| `ASM/NITROS9/GIMEX/llcocosdc_beta6.asm` — 6809-family assembly (6309 indicators) | call 435,438 | `rxWord`; L435: os9   F$STABX      store in user space buffer |
| `ASM/NITROS9/GIMEX/llcocosdc_ver101.asm` — 6809-family assembly (6309 indicators) | call 467,470; ref 441 | `rxWord`; L467: os9       F$STABX            store in user space buffer |
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 537 | `link`; L537: fcb	F$STABX+SysState |
| `ASM/NITROS9/KERNEL/fallbit.asm` — 6809-family assembly (6309 indicators) | call 54,64,97,190,200,231 | `NxtBitLp`; L54: os9   F$STABX      if it was a 1 (which means whole byte done), |
| `ASM/NITROS9/KERNEL/fchain.asm` — 6809-family assembly (6309 indicators) | call 151 | `L0457`; L151: os9   F$STABX |
| `ASM/NITROS9/KERNEL/fcpymem.asm` — 6809-family assembly (6309 indicators) | call 125 | `L09EC`; L125: os9   F$STABX    store byte |
| `ASM/NITROS9/KERNEL/fcpymem_330.asm` — 6809-family assembly (6309 indicators) | call 125 | `L09EC`; L125: os9   F$STABX    store byte |
| `ASM/NITROS9/KERNEL/fldabx.asm` — 6809-family assembly (CPU not certified) | ref 32 | `L0C40`; L32: * System Call: F$STABX |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 431 | `link`; L431: fcb    F$STABX+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 421 | `link`; L421: fcb   F$STABX+SysState |
| `ASM/NITROS9/KERNEL/krnp3_ed3_perr.asm` — 6809-family assembly (CPU not certified) | call 168,227 | `Error`; L168: os9   F$STABX    ;Move the CR to the buffer |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 413,453 | `WrPtchLp`; L413: os9   F$STABX        Save byte A into module |
| `ASM/NITROS9/PIPE/pipeman_named.asm` — 6809-family assembly (CPU not certified) | call 1288 | `RddMore`; L1288: os9   F$STABX    ;Put byte in caller's buffer |
| `ASM/NITROS9/RBF/msf.asm` — 6809-family assembly (6309 indicators) | call 1285,2904 | `readln12`; L1285: OS9   F$STABX |

### F$STime

Definition: `/dd/DEFS/os9.d:103`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fstime.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock.asm` — 6809-family assembly (6309 indicators) | ref 60,396,424,425,426,427 | `header / procedure / data`; L60: fcb   F$STime |
| `ASM/NITROS9/DW/dwio.asm` — 6809-family assembly (CPU not certified) | call 215 | `loop@`; L215: os9       F$STime |
| `ASM/NITROS9/DW/scdwv.asm` — 6809-family assembly (6309 indicators) | ref 37 | `header / procedure / data`; L37: * Modified so that F$STime is called if we get an error on calling |
| `ASM/NITROS9/KERNEL/fstime.asm` — 6809-family assembly (CPU not certified) | ref 2 | `header / procedure / data`; L2: * System Call: F$STime |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 55,282 | `header / procedure / data`; L55: *                 - Minor mods to F$STime |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 55,281 | `header / procedure / data`; L55: *                 - Minor mods to F$STime |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 55,282 | `header / procedure / data`; L55: *                 - Minor mods to F$STime |
| `ASM/NITROS9/MODS/dwio.asm` — 6809-family assembly (CPU not certified) | call 187 | `loop@`; L187: os9       F$STime |
| `ASM/NITROS9/MODS/dwiomess.asm` — 6809-family assembly (CPU not certified) | call 199 | `loop@`; L199: os9       F$STime |
| `ASM/NITROS9/MODS/sysgo.asm` — 6809-family assembly (CPU not certified) | call 177 | `start`; L177: os9   F$STime                 set time to default |
| `C/LIB/time.a` — 6809-family assembly (CPU not certified) | call 11 | `setime`; L11: os9 F$STIME call os9 |

### F$SUser

Definition: `/dd/DEFS/os9.d:112`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fsuser.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/fsuser.asm` — 6809-family assembly (CPU not certified) | ref 2 | `header / procedure / data`; L2: * System Call: F$SUser |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 27,28,308 | `header / procedure / data`; L27: * V1.16  93/09/03 - Moved F$SUser to OS9P1 (WG) |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 27,28,305 | `header / procedure / data`; L27: * V1.16  93/09/03 - Moved F$SUser to OS9P1 (WG) |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 27,28,306 | `header / procedure / data`; L27: * V1.16  93/09/03 - Moved F$SUser to OS9P1 (WG) |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 1117,1137 | `L07F5`; L1117: os9   F$SUser |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 1242,1264 | `L07F5`; L1242: os9   F$SUser |
| `C/LIB/id.a` — 6809-family assembly (CPU not certified) | call 46 | `setu10`; L46: os9 F$SUSER set user id |

### F$Time

Definition: `/dd/DEFS/os9.d:102`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 733 | `header / procedure / data`; L733: F$Time         EQU       $15                 Get Current Time |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 12933; ref 242 | `L4FF8`; L12933: os9   F$Time     Get 6 byte time packet to ,X |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 12129; ref 242 | `L4FF8`; L12129: os9   F$Time     Get 6 byte time packet to ,X |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 6918; ref 28 | `DATE$`; L6918: os9   F$Time         Get system date/time into string stack |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 5424; ref 28 | `DATE$`; L5424: os9   F$Time         Get system date/time into string stack |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 5456; ref 28 | `DATE$`; L5456: os9   F$Time         Get system date/time into string stack |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 4595 | `DATE$`; L4595: os9   F$Time |
| `ASM/GLIB/SMASH/S_Iniz.a` — 6809-family assembly (6309 indicators) | call 120 | `V010`; L120: OS9   F$Time     use current time to seed random number generator |
| `ASM/NITROS9/CLOCKS/clock.asm` — 6809-family assembly (6309 indicators) | ref 54,382 | `header / procedure / data`; L54: NewSvc   fcb   F$Time |
| `ASM/NITROS9/KERNEL/fallprc.asm` — 6809-family assembly (6309 indicators) | ref 68 | `LChinese`; L68: *         os9    F$Time        ignore any error... |
| `ASM/NITROS9/KERNEL/ffork.asm` — 6809-family assembly (6309 indicators) | ref 131 | `SveNPth`; L131: *         os9    F$Time      put date/time into it |
| `ASM/NITROS9/RBF/msf.asm` — 6809-family assembly (6309 indicators) | call 2672 | `setdate`; L2672: os9   F$TIME |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | call 619 | `L02D1`; L619: os9   F$Time     put currenttime there |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 1490 | `L0B35`; L1490: os9   F$Time |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 1707 | `L0B3B`; L1707: os9   F$Time         Get the date/time packet |
| `C/LIB/time.a` — 6809-family assembly (CPU not certified) | call 16 | `getime`; L16: os9 F$TIME call os9 |

### F$UnLink

Definition: `/dd/DEFS/os9.d:83`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/funlink.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 725 | `header / procedure / data`; L725: F$UnLink       EQU       2                   Unlink Module |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 2486 | `L0EB5`; L2486: os9   F$UnLink   Unlink the I-Code module |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 2513 | `L0EB5`; L2513: os9   F$UnLink   Unlink the I-Code module |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 442 | `L0397`; L442: os9   F$UnLink       Unlink the module |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 449 | `L0397`; L449: os9   F$UnLink       Unlink the module |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 456 | `L0397`; L456: os9   F$UnLink       Unlink the module |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 372,394 | `L304`; L372: os9   F$UnLink |
| `ASM/NITROS9/DW/scdwv.asm` — 6809-family assembly (6309 indicators) | call 638 | `L0244`; L638: os9       F$UnLink            unlink it from system map |
| `ASM/NITROS9/KERNEL/fchain.asm` — 6809-family assembly (6309 indicators) | call 65 | `L03CB`; L65: os9   F$UnLink     unlink from the primary module |
| `ASM/NITROS9/KERNEL/fexit.asm` — 6809-family assembly (6309 indicators) | call 95 | `L05CB`; L95: os9   F$UnLink     unlink aborted program |
| `ASM/NITROS9/KERNEL/funlink.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$UnLink |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 26,38,254 | `header / procedure / data`; L26: *                 - Changed LBEQ to BEQ in F$Unlink |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 26,38,253 | `header / procedure / data`; L26: *                 - Changed LBEQ to BEQ in F$Unlink |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 26,38,254 | `header / procedure / data`; L26: *                 - Changed LBEQ to BEQ in F$Unlink |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 892,894,896 | `L032B`; L892: os9   F$UnLink       unlink file manager |
| `ASM/NITROS9/SCF/cowin_beta6.asm` — 6809-family assembly (6309 indicators) | call 570,2409 | `Lp4`; L570: os9   F$UnLink       Unlink GRFDRV |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 409 | `L0200`; L409: os9   F$UnLink |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 410,2951,2952,2970,2971,3008,3009,3235,3236,3253,3254 | `L0200`; L410: os9   F$UnLink       Unlink it |
| `C/LIB/mod.a` — 6809-family assembly (CPU not certified) | call 46 | `munlink`; L46: os9 F$UNLINK call os9 |

### F$UnLoad

Definition: `/dd/DEFS/os9.d:113`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/funload.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 8605 | `F.UNLOAD`; L8605: os9   F$UnLoad |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 8389 | `F.UNLOAD`; L8389: os9   F$UnLoad |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 7560 | `F.UNLOAD`; L7560: os9   F$UnLoad |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 7627 | `F.UNLOAD`; L7627: os9   F$UnLoad |
| `ASM/NITROS9/KERNEL/funload.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$UnLoad |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 48,310 | `header / procedure / data`; L48: *                 - Changed L0A2B from BRA L0A4F to RTS (F$UnLoad error) |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 48,307 | `header / procedure / data`; L48: *                 - Changed L0A2B from BRA L0A4F to RTS (F$UnLoad error) |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 48,308 | `header / procedure / data`; L48: *                 - Changed L0A2B from BRA L0A4F to RTS (F$UnLoad error) |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 2526,2538,2559,2732,2742 | `L1427`; L2526: os9   F$UnLoad |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 2947,2966,3004,3231,3249 | `L1427`; L2947: os9   F$UnLoad       Unlink it |
| `ASM/VEFIO-WINFO/vefio.asm` — 6809-family assembly (CPU not certified) | call 735 | `movexy10`; L735: os9   F$UnLoad unlink/remove the module |
| `ASM/VEFIO-WINFO/witesta.asm` — 6809-family assembly (CPU not certified) | call 163 | `pshparms`; L163: os9   F$UnLoad     unlink/remove the module |

### F$VBlock

Definition: `/dd/DEFS/os9.d:185`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/ccbfsrqmem.asm:298`, `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fsrqmem.asm:285`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbfsrqmem.asm` — 6809-family assembly (6309 indicators) | ref 298 | `not.ext`; L298: * System Call: F$VBlock |
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 543 | `link`; L543: fcb	F$VBlock+SysState |
| `ASM/NITROS9/KERNEL/fsrqmem.asm` — 6809-family assembly (6309 indicators) | ref 285 | `not.ext`; L285: * System Call: F$VBlock |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 437 | `link`; L437: fcb    F$VBlock+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 427 | `link`; L427: fcb   F$VBlock+SysState |

### F$VIRQ

Definition: `/dd/DEFS/os9.d:133`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock.asm` — 6809-family assembly (6309 indicators) | ref 56,278 | `header / procedure / data`; L56: fcb   F$VIRQ |
| `ASM/NITROS9/DW/dwio.asm` — 6809-family assembly (CPU not certified) | call 209 | `loop@`; L209: os9       F$VIRQ              ; install |
| `ASM/NITROS9/DW/scdwv.asm` — 6809-family assembly (6309 indicators) | ref 38 | `header / procedure / data`; L38: * F$VIRQ (which means the clock module has not be initialized) |
| `ASM/NITROS9/MODS/dwio.asm` — 6809-family assembly (CPU not certified) | call 181 | `loop@`; L181: os9       F$VIRQ              ; install |
| `ASM/NITROS9/MODS/dwiomess.asm` — 6809-family assembly (CPU not certified) | call 193 | `loop@`; L193: os9       F$VIRQ              ; install |
| `ASM/NITROS9/RBF/cc3disk_disto.asm` — 6809-family assembly (CPU not certified) | call 85,633 | `TERM`; L85: os9   F$VIRQ |
| `ASM/NITROS9/RBF/cc3disk_sc2_irq.asm` — 6809-family assembly (CPU not certified) | call 72,574 | `header / procedure / data`; L72: os9   F$VIRQ |
| `ASM/NITROS9/RBF/cc3disk_sc2_slp.asm` — 6809-family assembly (CPU not certified) | call 68,531 | `header / procedure / data`; L68: os9   F$VIRQ |
| `ASM/NITROS9/RBF/rb1773.asm` — 6809-family assembly (6309 indicators) | call 293,1273 | `Term`; L293: os9   F$VIRQ         Remove VIRQ |
| `ASM/NITROS9/RBF/sdisk3_dmc.asm` — 6809-family assembly (CPU not certified) | call 136,138,920,938,949,1245 | `start`; L136: os9   F$VIRQ |
| `ASM/NITROS9/RBF/sdisk3_dpj.asm` — 6809-family assembly (CPU not certified) | call 75,791 | `start`; L75: os9   F$VIRQ      Disable our VIRQ entry |
| `ASM/NITROS9/SCF/vrn.asm` — 6809-family assembly (CPU not certified) | call 98,117 | `VEntry`; L98: os9   F$VIRQ |

### F$VModul

Definition: `/dd/DEFS/os9.d:140`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fvmodul.asm:2`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbfsrqmem.asm` — 6809-family assembly (6309 indicators) | call 346 | `name.prt`; L346: os9   F$VModul |
| `ASM/NITROS9/KERNEL/ccbkrn.asm` — 6809-family assembly (6309 indicators) | ref 495 | `link`; L495: fcb	F$VModul+SysState |
| `ASM/NITROS9/KERNEL/fsrqmem.asm` — 6809-family assembly (6309 indicators) | call 331 | `name.prt`; L331: os9   F$VModul |
| `ASM/NITROS9/KERNEL/fvmodul.asm` — 6809-family assembly (6309 indicators) | ref 2 | `header / procedure / data`; L2: * System Call: F$VModul |
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 389 | `link`; L389: fcb    F$VModul+SysState |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` — 6809-family assembly (6309 indicators) | ref 34,381 | `header / procedure / data`; L34: * Shrunk 6309 code by 1 byte in F$VModul |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 1724,1941 | `L0770`; L1724: os9   F$VModul       Validate the module (checks header parity & CRC) |

### F$Wait

Definition: `/dd/DEFS/os9.d:85`. Representative implementation header: `/dd/SOURCECODE/ASM/NITROS9/KERNEL/fallprc.asm:96`.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 727 | `header / procedure / data`; L727: F$Wait         EQU       4                   Wait for child process to die |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 1834,9675 | `L0A17`; L1834: L0A17    os9   F$Wait     Wait for death signal |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 1856,8839 | `L0A17`; L1856: L0A17    os9   F$Wait     Wait for death signal |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 2261 | `L0EAD`; L2261: L0EAD    os9   F$Wait         Wait until child process is done |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 2302 | `L0EAD`; L2302: L0EAD    os9   F$Wait         Wait until child process is done |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 2318 | `L0EAD`; L2318: L0EAD    os9   F$Wait         Wait until child process is done |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 1723 | `L636`; L1723: L636       os9   F$Wait         Wait for child to die |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 8645; ref 3284,3296,3309,8635 | `KILLPBUF`; L8645: os9   F$Wait |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 8429; ref 3251,3263,3276,8419 | `KILLPBUF`; L8429: os9   F$Wait |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 7600; ref 3253,3265,3278,7590 | `KILLPBUF`; L7600: os9   F$Wait |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 7667; ref 3293,3305,3318,7657 | `KILLPBUF`; L7667: os9   F$Wait |
| `ASM/NITROS9/KERNEL/fallprc.asm` — 6809-family assembly (6309 indicators) | ref 96 | `FDelPrc`; L96: * System Call: F$Wait |
| `ASM/NITROS9/KERNEL/fexit.asm` — 6809-family assembly (6309 indicators) | ref 54 | `L0584`; L54: bra   L05A2        so F$Wait will find us; next proc |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | ref 24,262 | `header / procedure / data`; L24: *                 - Slight opt in F$Wait alarm clearing |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | ref 24,261 | `header / procedure / data`; L24: *                 - Slight opt in F$Wait alarm clearing |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | ref 24,262 | `header / procedure / data`; L24: *                 - Slight opt in F$Wait alarm clearing |
| `ASM/NITROS9/MODS/sysgo.asm` — 6809-family assembly (CPU not certified) | call 244,253,277 | `DoStartup`; L244: os9   F$Wait |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 1292,2376 | `L0953`; L1292: os9   F$Wait |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 1452,2696; ref 2712 | `L0953`; L1452: os9   F$Wait         Wait until 'startup' is done |
| `C/LIB/process.a` — 6809-family assembly (CPU not certified) | call 16 | `wait`; L16: os9 F$WAIT |

### I$Attach

Definition: `/dd/DEFS/os9.d:206`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 738 | `header / procedure / data`; L738: I$Attach       RMB       1                   Attach I/O Device |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 1094 | `L0459`; L1094: os9   I$Attach       attach to device |
| `ASM/NITROS9/RBF/myram.asm` — 6809-family assembly (6309 indicators) | ref 595 | `Linkus`; L595: *        os9   I$Attach |
| `ASM/NITROS9/RBF/ram.asm` — 6809-family assembly (6309 indicators) | ref 596 | `Linkus`; L596: *        os9   I$Attach |
| `ASM/NITROS9/SCF/scf_beta6.asm` — 6809-family assembly (6309 indicators) | call 299; ref 373 | `CopyCR`; L299: os9   I$Attach     Attempt to attach to device name in device desc. |
| `ASM/NITROS9/SCF/scf_ver100.asm` — 6809-family assembly (6309 indicators) | call 305; ref 379 | `CopyCR`; L305: os9   I$Attach     Attempt to attach to device name in device desc. |
| `ASM/VEFIO-WINFO/winfo.asm` — 6809-family assembly (6309 indicators) | call 186; ref 182 | `not.l3`; L186: OS9   I$Attach     get U=address of device table entry |

### I$ChgDir

Definition: `/dd/DEFS/os9.d:212`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 744 | `header / procedure / data`; L744: I$ChgDir       RMB       1                   Change Default Directory |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 1850,9632 | `L0A2A`; L1850: os9   I$ChgDir   Change dir |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 1871,8796 | `L0A2A`; L1871: os9   I$ChgDir   Change dir |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 2223 | `L0E62`; L2223: os9   I$ChgDir       Change directory |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 2264 | `L0E62`; L2264: os9   I$ChgDir       Change directory |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 2280 | `L0E62`; L2280: os9   I$ChgDir       Change directory |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 1697 | `L630`; L1697: os9   I$ChgDir |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 7959 | `I.CHGDIR`; L7959: os9   I$ChgDir |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 7636 | `I.CHGDIR`; L7636: os9   I$ChgDir |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 6914 | `I.CHGDIR`; L6914: os9   I$ChgDir |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 6981 | `I.CHGDIR`; L6981: os9   I$ChgDir |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | call 197 | `L003A`; L197: os9    I$ChgDir    change to it |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | call 198 | `L003A`; L198: os9    I$ChgDir    change to it |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | call 199 | `L003A`; L199: os9    I$ChgDir    change to it |
| `ASM/NITROS9/MODS/sysgo.asm` — 6809-family assembly (CPU not certified) | call 182,185,189 | `start`; L182: os9   I$ChgDir                change exec. dir |
| `ASM/NITROS9/PIPE/pipeman.asm` — 6809-family assembly (CPU not certified) | ref 115 | `L00A9`; L115: * I$MakDir/I$ChgDir/I$Delete Entry Points |
| `ASM/NITROS9/PIPE/pipeman_beta6.asm` — 6809-family assembly (CPU not certified) | ref 124 | `L00A9`; L124: * I$MakDir/I$ChgDir/I$Delete Entry Points |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | ref 630 | `L02D1`; L630: * I$ChgDir Entry Point |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 1318,1321 | `L09B0`; L1318: os9   I$ChgDir |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 1486,1491 | `CmdCHX`; L1486: os9   I$ChgDir |
| `C/LIB/dir.a` — 6809-family assembly (CPU not certified) | call 12 | `chgdir10`; L12: os9 I$CHGDIR |

### I$Close

Definition: `/dd/DEFS/os9.d:221`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 753 | `header / procedure / data`; L753: I$Close        RMB       1                   Close Path |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 1534,1656,2278,2294,2780,9595 | `L07FC`; L1534: L07FC    os9   I$Close |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 1555,1677,2305,2321,2832,8759 | `L07FC`; L1555: L07FC    os9   I$Close |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 187,2189 | `L01D0`; L187: L01D0    os9   I$Close |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 193,2230 | `L01D0`; L193: L01D0    os9   I$Close |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 197,2246 | `L01D0`; L197: L01D0    os9   I$Close |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 196,1666 | `L92`; L196: L92        os9   I$Close |
| `ASM/DWNET/dwnet.a` — 6809-family assembly (CPU not certified) | call 216 | `TCPDisconnect`; L216: TCPDisconnect:  os9     I$Close |
| `ASM/FTP/dwnet.a` — 6809-family assembly (CPU not certified) | call 205 | `TCPDisconnect`; L205: TCPDisconnect:  os9     I$Close |
| `ASM/GLIB/SMASH/HScore.a` — 6809-family assembly (CPU not certified) | call 162,189 | `S011`; L162: S011     OS9   I$Close    close the file, |
| `ASM/GLIB/SMASH/LLoad.a` — 6809-family assembly (CPU not certified) | call 111 | `L030`; L111: OS9   I$Close    close the path |
| `ASM/GLIB/SMASH/S_Iniz.a` — 6809-family assembly (6309 indicators) | call 51 | `X000`; L51: OS9   I$Close    close the file |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 1685,3466,3468,3470,3499,3501,3503,7217,7239,7424,8733 | `NEWDIREC`; L1685: os9   I$Close    Close dir path 1st |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 1669,3433,3435,3437,3466,3468,3470,6166,6196,6566,8517 | `NEWDIREC`; L1669: os9   I$Close    Close dir path 1st |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 1671,3435,3437,3439,3468,3470,3472,6172,6194,6379,7688 | `NEWDIREC`; L1671: os9   I$Close    Close dir path 1st |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 1695,3475,3477,3479,3508,3510,3512,6230,6252,6437,7755 | `NEWDIREC`; L1695: os9   I$Close    Close dir path 1st |
| `ASM/NITROS9/KERNEL/fexit.asm` — 6809-family assembly (6309 indicators) | call 75 | `L05AC`; L75: os9   I$Close      close the path |
| `ASM/NITROS9/KERNEL/krnp3_ed3_perr.asm` — 6809-family assembly (CPU not certified) | call 172 | `Close`; L172: Close    os9   I$Close    ;Close the file |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 1757,1956; ref 1398 | `L07E9`; L1757: os9   I$Close        close path to file |
| `ASM/NITROS9/PIPE/pipeman_named.asm` — 6809-family assembly (CPU not certified) | ref 53,56,592 | `header / procedure / data`; L53: *   Opening an existing named pipe emulates IOMan's I$Close and |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | ref 559 | `MDir273`; L559: * I$Close Entry Point |
| `ASM/NITROS9/SCF/scf_beta6.asm` — 6809-family assembly (6309 indicators) | ref 379 | `OpenErr`; L379: * I$Close entry point |
| `ASM/NITROS9/SCF/scf_ver100.asm` — 6809-family assembly (6309 indicators) | ref 385 | `OpenErr`; L385: * I$Close entry point |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 463,1023,1135,1539,1546,1598,1796,2457,2486,2763,2928,2945 | `L0281`; L463: os9   I$Close |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 471,1112,1260,1767,1776,1837,2052,2857,2888,3282,3446,3464 | `L0281`; L471: os9   I$Close        Close the path |
| `ASM/SOUNDRV/DrvPlay.asm` — 6809-family assembly (CPU not certified) | call 57,68 | `header / procedure / data`; L57: os9 I$Close        close file |
| `ASM/VEFIO-WINFO/vefio.asm` — 6809-family assembly (CPU not certified) | call 227,367 | `header / procedure / data`; L227: os9   I$Close and close the file |
| `C/LIB/access.a` — 6809-family assembly (CPU not certified) | call 19,35,77 | `access`; L19: os9 I$CLOSE |
| `C/LIB/change.a` — 6809-family assembly (CPU not certified) | call 34,84 | `header / procedure / data`; L34: os9 I$CLOSE close the file |

### I$Create

Definition: `/dd/DEFS/os9.d:209`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 741 | `header / procedure / data`; L741: I$Create       RMB       1                   Create New File |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 2296,9201 | `L0D6B`; L2296: os9   I$Create   Attempt to create the file |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 2323,8365 | `L0D6B`; L2323: os9   I$Create   Attempt to create the file |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 1808 | `CREATE`; L1808: os9   I$Create       Create the file |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 1849 | `CREATE`; L1849: os9   I$Create       Create the file |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 1861 | `CREATE`; L1861: os9   I$Create       Create the file |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 1316 | `CREATE`; L1316: os9   I$Create |
| `ASM/GLIB/SMASH/HScore.a` — 6809-family assembly (CPU not certified) | call 146 | `H.Save`; L146: OS9   I$Create |
| `ASM/NITROS9/PIPE/pipeman.asm` — 6809-family assembly (CPU not certified) | ref 124 | `L00A9`; L124: * I$Create / I$Open entry Point |
| `ASM/NITROS9/PIPE/pipeman_beta6.asm` — 6809-family assembly (CPU not certified) | ref 133 | `L00A9`; L133: * I$Create / I$Open entry Point |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | ref 174 | `start`; L174: * I$Create Entry Point |
| `ASM/NITROS9/SCF/scf_beta6.asm` — 6809-family assembly (6309 indicators) | ref 234 | `oerr`; L234: * I$Create/I$Open entry point |
| `ASM/NITROS9/SCF/scf_ver100.asm` — 6809-family assembly (6309 indicators) | ref 240 | `oerr`; L240: * I$Create/I$Open entry point |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 1659 | `L0CB9`; L1659: os9   I$Create |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 1905 | `L0CB9`; L1905: os9   I$Create |
| `C/LIB/access.a` — 6809-family assembly (CPU not certified) | call 50 | `creat`; L50: os9 I$CREATE |

### I$Delete

Definition: `/dd/DEFS/os9.d:213`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 745 | `header / procedure / data`; L745: I$Delete       RMB       1                   Delete File |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 9623 | `L3970`; L9623: os9   I$Delete   Delete file |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 8787 | `L3970`; L8787: os9   I$Delete   Delete file |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 2215 | `DELETE`; L2215: os9   I$Delete       Delete file |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 2256 | `DELETE`; L2256: os9   I$Delete       Delete file |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 2272 | `DELETE`; L2272: os9   I$Delete       Delete file |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 1689 | `DELETE`; L1689: os9   I$Delete |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 8742 | `I.DELETE`; L8742: os9   I$Delete     Delete the file |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 8526 | `I.DELETE`; L8526: os9   I$Delete     Delete the file |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 7697 | `I.DELETE`; L7697: os9   I$Delete     Delete the file |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 7764 | `I.DELETE`; L7764: os9   I$Delete     Delete the file |
| `ASM/NITROS9/PIPE/pipeman.asm` — 6809-family assembly (CPU not certified) | ref 115 | `L00A9`; L115: * I$MakDir/I$ChgDir/I$Delete Entry Points |
| `ASM/NITROS9/PIPE/pipeman_beta6.asm` — 6809-family assembly (CPU not certified) | ref 124 | `L00A9`; L124: * I$MakDir/I$ChgDir/I$Delete Entry Points |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | ref 3,669 | `header / procedure / data`; L3: * Internally, RBF can read/write to such a file fine, but I$Delete/I$DeletX |
| `C/LIB/access.a` — 6809-family assembly (CPU not certified) | call 83 | `unlink`; L83: os9 I$DELETE |

### I$DeletX

Definition: `/dd/DEFS/os9.d:222`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 754 | `header / procedure / data`; L754: I$DeletX       RMB       1                   Delete from current exec dir |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | ref 3 | `header / procedure / data`; L3: * Internally, RBF can read/write to such a file fine, but I$Delete/I$DeletX |

### I$Detach

Definition: `/dd/DEFS/os9.d:207`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 739 | `header / procedure / data`; L739: I$Detach       RMB       1                   Detach I/O Device |
| `ASM/NITROS9/DW/rbdw.asm` — 6809-family assembly (CPU not certified) | ref 31 | `header / procedure / data`; L31: *  memory and I$Detach calls Term) |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 539,985; ref 13,16,17,138,746,748,750 | `L015C`; L539: os9   I$Detach       detach |
| `ASM/NITROS9/PIPE/pipeman_named.asm` — 6809-family assembly (CPU not certified) | call 613 | `IsAsOld`; L613: os9   I$Detach   ; Detach to compensate for IOMAN Attach |
| `ASM/NITROS9/SCF/scf_beta6.asm` — 6809-family assembly (6309 indicators) | call 404 | `L0136`; L404: os9   I$Detach     Detach it |
| `ASM/NITROS9/SCF/scf_ver100.asm` — 6809-family assembly (6309 indicators) | call 410 | `L0136`; L410: os9   I$Detach     Detach it |
| `ASM/VEFIO-WINFO/winfo.asm` — 6809-family assembly (6309 indicators) | call 189 | `not.l3`; L189: OS9   I$Detach     so link count is correct |

### I$Dup

Definition: `/dd/DEFS/os9.d:208`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 740 | `header / procedure / data`; L740: I$Dup          RMB       1                   Duplicate Path |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 1539,2280; ref 2778 | `L07FC`; L1539: os9   I$Dup |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 1560,2307; ref 2830 | `L07FC`; L1560: os9   I$Dup |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 192 | `L01D0`; L192: os9   I$Dup |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 198 | `L01D0`; L198: os9   I$Dup |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 202 | `L01D0`; L202: os9   I$Dup |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 3480,3483,3486,3505,3507,3509,8721 | `NEWSTDI2`; L3480: os9   I$Dup      Duplicate new path as std in |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 3447,3450,3453,3472,3474,3476,8505 | `NEWSTDI2`; L3447: os9   I$Dup      Duplicate new path as std in |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 3449,3452,3455,3474,3476,3478,7676 | `NEWSTDI2`; L3449: os9   I$Dup      Duplicate new path as std in |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 3489,3492,3495,3514,3516,3518,7743 | `NEWSTDI2`; L3489: os9   I$Dup      Duplicate new path as std in |
| `ASM/NITROS9/KERNEL/ffork.asm` — 6809-family assembly (6309 indicators) | call 72 | `GetOPth`; L72: os9    I$Dup       dupe it |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | call 222,224 | `L0066`; L222: os9    I$Dup       dupe it |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | call 223,225 | `L0066`; L223: os9    I$Dup       dupe it |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | call 224,226 | `L0066`; L224: os9    I$Dup       dupe it |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | ref 909,924 | `L0351`; L909: * User State I$Dup |
| `ASM/NITROS9/PIPE/pipeman_named.asm` — 6809-family assembly (CPU not certified) | ref 54,56,79,592 | `header / procedure / data`; L54: *   I$Dup calls.  This file manager contains subroutines that |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 1541,1593,1606,2759,2765 | `L0BA8`; L1541: os9   I$Dup |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 1769,1832,1846,3278,3284 | `L0BA8`; L1769: os9   I$Dup          Dupe it |
| `C/LIB/access.a` — 6809-family assembly (CPU not certified) | call 88 | `dup`; L88: os9 I$DUP |

### I$GetStt

Definition: `/dd/DEFS/os9.d:219`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 751 | `header / procedure / data`; L751: I$GetStt       RMB       1                   Get Path Status |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 12974 | `L5035`; L12974: os9   I$GetStt |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 12170 | `L5035`; L12170: os9   I$GetStt |
| `ASM/BASIC09/gfx2_ver1.asm` — 6809-family assembly (6309 indicators) | call 548,567,640,647,652 | `L039A`; L548: os9   I$GetStt |
| `ASM/BASIC09/gfx_beta6.asm` — 6809-family assembly (6309 indicators) | call 312,328,353 | `GLoc`; L312: os9   I$GetStt |
| `ASM/BASIC09/inkey.asm` — 6809-family assembly (CPU not certified) | call 59 | `L0043`; L59: os9   I$GetStt |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 6958 | `EOF`; L6958: os9   I$GetStt |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 5464 | `EOF`; L5464: os9   I$GetStt |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 5496 | `EOF`; L5496: os9   I$GetStt |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 1827,4628 | `L385`; L1827: os9   I$GetStt |
| `ASM/FTP/dwnet.a` — 6809-family assembly (CPU not certified) | call 18 | `getopts`; L18: os9     I$GetStt |
| `ASM/FTP/main.a` — 6809-family assembly (CPU not certified) | call 32 | `Main`; L32: os9     I$GetStt |
| `ASM/GLIB/GLIB/UJOY.a` — 6809-family assembly (CPU not certified) | call 22 | `U$JOY`; L22: OS9   I$GetStt   get info |
| `ASM/GLIB/GLIB/USLPNSEL.a` — 6809-family assembly (CPU not certified) | call 19 | `U010`; L19: OS9   I$GetStt   get the info |
| `ASM/GLIB/SMASH/HScore.a` — 6809-family assembly (CPU not certified) | call 104 | `Echo`; L104: OS9   I$GetStt   get path options status |
| `ASM/GLIB/SMASH/KeyPress.a` — 6809-family assembly (CPU not certified) | call 13,127 | `KeyPress`; L13: OS9   I$GetStt |
| `ASM/GLIB/SMASH/P_Setup.a` — 6809-family assembly (CPU not certified) | call 117 | `P020`; L117: OS9   I$GetStt |
| `ASM/GLIB/SMASH/Playball.a` — 6809-family assembly (CPU not certified) | call 93,193 | `G020`; L93: OS9   I$GetStt |
| `ASM/GLIB/SMASH/S_Iniz.a` — 6809-family assembly (6309 indicators) | call 141 | `Opts`; L141: OS9   I$GetStt |
| `ASM/GLIB/SMASH/S_Title.a` — 6809-family assembly (CPU not certified) | call 70,122,133,201,334 | `SD020`; L70: OS9   I$GetStt |
| `ASM/GLIB/SMASH/paddle.a` — 6809-family assembly (CPU not certified) | call 18 | `G$UPaddle`; L18: OS9   I$GetStt |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 531,754,4926,7204,7226,7230,7848,7858,8920,8951,8984 | `FIXWINDW`; L531: os9   I$GetStt |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 522,744,4877,6153,6175,6179,7414,7434,8704,8735,8768 | `FIXWINDW`; L522: os9   I$GetStt |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 524,746,4883,6159,6181,6185,6803,6813,7875,7906,7939 | `FIXWINDW`; L524: os9   I$GetStt |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 536,765,4933,6217,6239,6243,6866,6876,7942,7973,8006,8663 | `FIXWINDW`; L536: os9   I$GetStt |
| `ASM/NITROS9/MODS/sysgo.asm` — 6809-family assembly (CPU not certified) | call 234 | `L0151`; L234: os9   I$GetStt |
| `ASM/NITROS9/PIPE/pipeman_named.asm` — 6809-family assembly (CPU not certified) | ref 10 | `header / procedure / data`; L10: * Pipeman Modified to Include the SS.Ready I$GETSTT Call. |
| `ASM/NITROS9/SCF/scf_beta6.asm` — 6809-family assembly (6309 indicators) | ref 505 | `L01CA`; L505: * I$GetStt entry point |
| `ASM/NITROS9/SCF/scf_ver100.asm` — 6809-family assembly (6309 indicators) | ref 511 | `L01CA`; L511: * I$GetStt entry point |
| `ASM/PLAY/play.a` — 6809-family assembly (6309 indicators) | call 577 | `Not8SVX`; L577: os9   I$GetStt      Get file size |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 430,482,1429,1639,1720,2942 | `L0233`; L430: os9   I$GetStt |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 434,496,1633,1882,1977,2740,2748,3461,3569,3809 | `L0233`; L434: os9   I$GetStt       Get std input path options |
| `ASM/VEFIO-WINFO/vefio.asm` — 6809-family assembly (CPU not certified) | call 222,627,757 | `header / procedure / data`; L222: os9   I$GetStt do it |
| `C/LIB/change.a` — 6809-family assembly (CPU not certified) | call 57 | `header / procedure / data`; L57: os9 I$GETSTT read the FD |
| `C/LIB/io.a` — 6809-family assembly (CPU not certified) | call 92,99 | `end`; L92: os9 I$GETSTT |
| `C/LIB/stat.a` — 6809-family assembly (CPU not certified) | call 31,46 | `getst10`; L31: os9 I$GETSTT |

### I$MakDir

Definition: `/dd/DEFS/os9.d:211`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 743 | `header / procedure / data`; L743: I$MakDir       RMB       1                   Make Directory File |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 8738 | `I.MAKDIR`; L8738: os9   I$MakDir     Make the directory |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 8522 | `I.MAKDIR`; L8522: os9   I$MakDir     Make the directory |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 7693 | `I.MAKDIR`; L7693: os9   I$MakDir     Make the directory |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 7760 | `I.MAKDIR`; L7760: os9   I$MakDir     Make the directory |
| `ASM/NITROS9/PIPE/pipeman.asm` — 6809-family assembly (CPU not certified) | ref 115 | `L00A9`; L115: * I$MakDir/I$ChgDir/I$Delete Entry Points |
| `ASM/NITROS9/PIPE/pipeman_beta6.asm` — 6809-family assembly (CPU not certified) | ref 124 | `L00A9`; L124: * I$MakDir/I$ChgDir/I$Delete Entry Points |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | ref 486 | `ex`; L486: * I$MakDir Entry Point |
| `C/LIB/access.a` — 6809-family assembly (CPU not certified) | call 41 | `mknod`; L41: os9 I$MAKDIR |

### I$ModDsc

Definition: `/dd/DEFS/os9.d:223`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 9605; ref 42,7173,9585,9609 | `DoMod`; L9605: os9   I$ModDsc   Modify the descriptor to make into VDG screen |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 9389; ref 41,6122,9369,9393 | `DoMod`; L9389: os9   I$ModDsc   Modify the descriptor to make into VDG screen |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 8560; ref 41,6128,8540,8564 | `DoMod`; L8560: os9   I$ModDsc   Modify the descriptor to make into VDG screen |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 8627; ref 42,6186,8607,8631 | `DoMod`; L8627: os9   I$ModDsc   Modify the descriptor to make into VDG screen |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | ref 37,235,277,297,312,315 | `header / procedure / data`; L37: * Added I$ModDsc call (modify device descriptor in system memory) BN/LCB |

### I$Open

Definition: `/dd/DEFS/os9.d:210`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 742 | `header / procedure / data`; L742: I$Open         RMB       1                   Open Existing File |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 1951,2302,9206 | `L0AC3`; L1951: os9   I$Open     Open path |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 1969,2329,8370 | `L0AC3`; L1969: os9   I$Open     Open path |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 1812 | `OPEN`; L1812: os9   I$Open         Open the file |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 1854 | `OPEN`; L1854: os9   I$Open         Open the file |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 1866 | `OPEN`; L1866: os9   I$Open         Open the file |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 1320 | `OPEN`; L1320: os9   I$Open |
| `ASM/DWNET/dwnet.a` — 6809-family assembly (CPU not certified) | call 105 | `TCPOpen`; L105: os9     I$Open |
| `ASM/FTP/dwnet.a` — 6809-family assembly (CPU not certified) | call 105 | `TCPOpen`; L105: os9     I$Open |
| `ASM/GLIB/GLIB/UGWBLK.a` — 6809-family assembly (CPU not certified) | call 57 | `A000`; L57: OS9   I$Open     open a path to the window |
| `ASM/GLIB/GLIB_p2.doc` — prose | ref 151 | `ERROR`; L151: Allocates a window, and returns its starting block number.  The process is very similar to using I$Open with '/w' to get a window, and I$Write with DWSET to tel |
| `ASM/GLIB/SMASH/HScore.a` — 6809-family assembly (CPU not certified) | call 151,181 | `H.Save`; L151: OS9   I$Open     open a path to the file |
| `ASM/GLIB/SMASH/LLoad.a` — 6809-family assembly (CPU not certified) | call 17,26 | `D010`; L17: OS9   I$Open |
| `ASM/GLIB/SMASH/S_Iniz.a` — 6809-family assembly (6309 indicators) | call 44,104 | `X000`; L44: OS9   I$Open |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 3476,7194,7245,8726; ref 2630 | `NEWSTDI1`; L3476: os9   I$Open     & use it's path # for new std i/o paths |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 3443,6143,6208,8510; ref 2597 | `NEWSTDI1`; L3443: os9   I$Open     & use it's path # for new std i/o paths |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 3445,6149,6200,7681; ref 2599 | `NEWSTDI1`; L3445: os9   I$Open     & use it's path # for new std i/o paths |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 3485,6207,6258,7748; ref 2639 | `NEWSTDI1`; L3485: os9   I$Open     & use it's path # for new std i/o paths |
| `ASM/NITROS9/KERNEL/krnp2.asm` — 6809-family assembly (6309 indicators) | call 210 | `L004F`; L210: os9    I$Open      open path to it |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` — 6809-family assembly (6309 indicators) | call 209 | `L004F`; L209: os9    I$Open      open path to it |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` — 6809-family assembly (6309 indicators) | call 210 | `L004F`; L210: os9    I$Open      open path to it |
| `ASM/NITROS9/KERNEL/krnp3_ed3_perr.asm` — 6809-family assembly (CPU not certified) | call 146 | `PErr`; L146: os9   I$Open     ;Open errmsg file |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 1682,1909 | `L0731`; L1682: os9   I$Open         open it |
| `ASM/NITROS9/PIPE/pipeman.asm` — 6809-family assembly (CPU not certified) | ref 124 | `L00A9`; L124: * I$Create / I$Open entry Point |
| `ASM/NITROS9/PIPE/pipeman_beta6.asm` — 6809-family assembly (CPU not certified) | ref 133 | `L00A9`; L133: * I$Create / I$Open entry Point |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | ref 388 | `l1`; L388: * I$Open Entry Point |
| `ASM/NITROS9/SCF/scf_beta6.asm` — 6809-family assembly (6309 indicators) | ref 176,234,322 | `header / procedure / data`; L176: * When the user calls I$Open on a device, he passes the desired mode byte |
| `ASM/NITROS9/SCF/scf_ver100.asm` — 6809-family assembly (6309 indicators) | ref 176,240,328 | `header / procedure / data`; L176: * When the user calls I$Open on a device, he passes the desired mode byte |
| `ASM/PLAY/play.a` — 6809-family assembly (6309 indicators) | call 225 | `L011E`; L225: os9   I$Open |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 811,1618,1627,1635,1645,1724,1794,2420,2437,2960 | `L055E`; L811: os9   I$Open |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 846,1859,1869,1878,1889,1981,2050,2818,2836,3481 | `L055E`; L846: os9   I$Open |
| `ASM/SOUNDRV/DrvPlay.asm` — 6809-family assembly (CPU not certified) | call 48,61 | `header / procedure / data`; L48: os9 I$Open         open file |
| `ASM/VEFIO-WINFO/vefio.asm` — 6809-family assembly (CPU not certified) | call 202,584 | `header / procedure / data`; L202: os9   I$Open open the pix file |
| `C/LIB/access.a` — 6809-family assembly (CPU not certified) | call 16,26,65 | `access`; L16: os9 I$OPEN |
| `C/LIB/change.a` — 6809-family assembly (CPU not certified) | call 49 | `header / procedure / data`; L49: os9 I$OPEN open the file |

### I$Read

Definition: `/dd/DEFS/os9.d:215`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 747 | `header / procedure / data`; L747: I$Read         RMB       1                   Read Data |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 9564 | `L3917`; L9564: os9   I$Read     Read the data |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 8728 | `L3917`; L8728: os9   I$Read     Read the data |
| `ASM/BASIC09/inkey.asm` — 6809-family assembly (CPU not certified) | call 62 | `L0043`; L62: os9   I$Read |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 2157 | `GET`; L2157: os9   I$Read         Read the data |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 2201 | `GET`; L2201: os9   I$Read         Read the data |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 2217 | `GET`; L2217: os9   I$Read         Read the data |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 1638,1830; ref 1884 | `GET`; L1638: os9   I$Read |
| `ASM/FTP/regdump.a` — 6809-family assembly (CPU not certified) | call 14 | `header / procedure / data`; L14: os9    I$Read |
| `ASM/GLIB/SMASH/HScore.a` — 6809-family assembly (CPU not certified) | call 185 | `L010`; L185: OS9   I$Read     read the high score list into memory |
| `ASM/GLIB/SMASH/KeyPress.a` — 6809-family assembly (CPU not certified) | call 20,133 | `K010`; L20: OS9   I$Read     get it |
| `ASM/GLIB/SMASH/P_Setup.a` — 6809-family assembly (CPU not certified) | call 122 | `P021`; L122: OS9   I$Read |
| `ASM/GLIB/SMASH/Playball.a` — 6809-family assembly (CPU not certified) | call 99 | `O010`; L99: OS9   I$Read     flush the path |
| `ASM/GLIB/SMASH/S_Iniz.a` — 6809-family assembly (6309 indicators) | call 49 | `X000`; L49: OS9   I$Read     read the font data |
| `ASM/GLIB/SMASH/S_Title.a` — 6809-family assembly (CPU not certified) | call 140 | `S005`; L140: OS9   I$Read     read one byte |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 8675 | `I.READ`; L8675: os9   I$Read     Read data |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 8459 | `I.READ`; L8459: os9   I$Read     Read data |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 7630 | `I.READ`; L7630: os9   I$Read     Read data |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 7697 | `I.READ`; L7697: os9   I$Read     Read data |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 1898,1921,1938 | `L08C2`; L1898: os9   I$Read         Read it in & return |
| `ASM/NITROS9/PIPE/pipeman.asm` — 6809-family assembly (CPU not certified) | ref 176 | `ReadLn`; L176: * I$Read Entry Point |
| `ASM/NITROS9/PIPE/pipeman_beta6.asm` — 6809-family assembly (CPU not certified) | ref 185 | `ReadLn`; L185: * I$Read Entry Point |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | ref 973 | `RdLn49B`; L973: * I$Read Entry Point |
| `ASM/NITROS9/SCF/scf_beta6.asm` — 6809-family assembly (6309 indicators) | ref 688 | `L0282`; L688: * I$Read entry point |
| `ASM/NITROS9/SCF/scf_ver100.asm` — 6809-family assembly (6309 indicators) | ref 694 | `L0282`; L694: * I$Read entry point |
| `ASM/PLAY/play.a` — 6809-family assembly (6309 indicators) | call 230,238,282,327,346,356,364,442,492,528,553,570,605,633,1384 | `L011E`; L230: os9   I$Read |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 837,1756,2444,2476,2966 | `L0599`; L837: os9   I$Read |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 872,2008,2761,2843,2878,3488 | `L0599`; L872: os9   I$Read |
| `ASM/SOUNDRV/DrvPlay.asm` — 6809-family assembly (CPU not certified) | call 54 | `header / procedure / data`; L54: os9 I$Read         read up to that much in buffer |
| `ASM/VEFIO-WINFO/vefio.asm` — 6809-family assembly (CPU not certified) | call 208,217,226,301 | `header / procedure / data`; L208: os9   I$Read read it |
| `C/LIB/io.a` — 6809-family assembly (CPU not certified) | call 14 | `read`; L14: os9 I$READ |

### I$ReadLn

Definition: `/dd/DEFS/os9.d:217`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 749,923,924 | `header / procedure / data`; L749: I$ReadLn       RMB       1                   Read Line of ASCII Data |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 2004,2311,13837 | `L0B2D`; L2004: os9   I$ReadLn   Go read a line |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 2022,2338,13021 | `L0B2D`; L2022: os9   I$ReadLn   Go read a line |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 7792 | `READLN`; L7792: os9   I$ReadLn       Do read |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 6307 | `READLN`; L6307: os9   I$ReadLn       Do read |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 6325 | `READLN`; L6325: os9   I$ReadLn       Do read |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 5168 | `READLN`; L5168: os9   I$ReadLn |
| `ASM/DWNET/dwnet.a` — 6809-family assembly (CPU not certified) | call 175 | `readresponse`; L175: os9     I$ReadLn |
| `ASM/FTP/dwnet.a` — 6809-family assembly (CPU not certified) | call 164 | `readresponse`; L164: os9     I$ReadLn |
| `ASM/GLIB/SMASH/HScore.a` — 6809-family assembly (CPU not certified) | call 84 | `H070`; L84: OS9   I$ReadLn   read a line with editing |
| `ASM/GLIB/SMASH/LLoad.a` — 6809-family assembly (CPU not certified) | call 38,50,78 | `L010`; L38: OS9   I$ReadLn |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 8692 | `I.READLN`; L8692: os9   I$ReadLn     Read up to size or CR |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 8476 | `I.READLN`; L8476: os9   I$ReadLn     Read up to size or CR |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 7647 | `I.READLN`; L7647: os9   I$ReadLn     Read up to size or CR |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 7714 | `I.READLN`; L7714: os9   I$ReadLn     Read up to size or CR |
| `ASM/NITROS9/KERNEL/krnp3_ed3_perr.asm` — 6809-family assembly (CPU not certified) | call 151 | `Loop`; L151: os9   I$ReadLn   ;Read a line |
| `ASM/NITROS9/PIPE/pipeman.asm` — 6809-family assembly (CPU not certified) | ref 8,168 | `header / procedure / data`; L8: * 'show grf.3.a \| eat'  (eat is cat, but just does a I$ReadLn, and not I$WritLn) |
| `ASM/NITROS9/PIPE/pipeman_beta6.asm` — 6809-family assembly (CPU not certified) | ref 8,177 | `header / procedure / data`; L8: * 'show grf.3.a \| eat'  (eat is cat, but just does a I$ReadLn, and not I$WritLn) |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | ref 858 | `Seek41F`; L858: * I$ReadLn Entry Point |
| `ASM/NITROS9/SCF/scf_beta6.asm` — 6809-family assembly (6309 indicators) | ref 848 | `L0451`; L848: * I$ReadLn entry point |
| `ASM/NITROS9/SCF/scf_ver100.asm` — 6809-family assembly (6309 indicators) | ref 854 | `L0451`; L854: * I$ReadLn entry point |
| `ASM/PLAY/play.a` — 6809-family assembly (6309 indicators) | call 659 | `L01E1`; L659: os9   I$ReadLn |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 357,2117 | `L017F`; L357: os9   I$ReadLn |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 2405,3612 | `L1085`; L2405: os9   I$ReadLn |
| `C/LIB/io.a` — 6809-family assembly (CPU not certified) | call 36 | `readln`; L36: os9 I$READLN call os9 |

### I$Seek

Definition: `/dd/DEFS/os9.d:214`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 746 | `header / procedure / data`; L746: I$Seek         RMB       1                   Change Current Position |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 13908; ref 13873,13884 | `L555E`; L13908: os9   I$Seek     Seek to X:U |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 13105; ref 13070,13081 | `L555E`; L13105: os9   I$Seek     Seek to X:U |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 7859; ref 7823,7834 | `L2A2B`; L7859: os9   I$Seek         Seek to X:U |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 6374; ref 6338,6349 | `L2A2B`; L6374: os9   I$Seek         Seek to X:U |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 6406; ref 6370,6381 | `L2A2B`; L6406: os9   I$Seek         Seek to X:U |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 5212 | `L1596`; L5212: os9   I$Seek |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | ref 829 | `Del3F9`; L829: * I$Seek Entry Point |
| `ASM/PLAY/play.a` — 6809-family assembly (6309 indicators) | call 433 | `GotChanl`; L433: os9   I$Seek |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 830,1640,2043,2471 | `L0572`; L830: os9   I$Seek |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 865,1883,2322,2873 | `L0572`; L865: os9   I$Seek |
| `C/LIB/io.a` — 6809-family assembly (CPU not certified) | call 114 | `doseek`; L114: os9 I$SEEK |

### I$SetStt

Definition: `/dd/DEFS/os9.d:220`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 752 | `header / procedure / data`; L752: I$SetStt       RMB       1                   Set Path Status |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 2319 | `L0D6B`; L2319: os9   I$SetStt   Truncate file size to 0 bytes |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 2346 | `L0D6B`; L2346: os9   I$SetStt   Truncate file size to 0 bytes |
| `ASM/BASIC09/gfx2_ver1.asm` — 6809-family assembly (6309 indicators) | call 541,605,608,615,631,674,683,689 | `L038A`; L541: os9   I$SetStt |
| `ASM/DWNET/dwnet.a` — 6809-family assembly (CPU not certified) | call 18,23 | `getopts`; L18: os9     I$SetStt |
| `ASM/FTP/command.a` — 6809-family assembly (CPU not certified) | call 164 | `prtprsedn`; L164: os9     I$SetStt |
| `ASM/FTP/dwnet.a` — 6809-family assembly (CPU not certified) | call 23 | `setopts`; L23: os9    I$SetStt |
| `ASM/FTP/main.a` — 6809-family assembly (CPU not certified) | call 49,103 | `optlp`; L49: os9     I$SetStt |
| `ASM/GLIB/SMASH/HScore.a` — 6809-family assembly (CPU not certified) | call 109 | `Echo`; L109: OS9   I$SetStt   and set it |
| `ASM/GLIB/SMASH/Playball.a` — 6809-family assembly (CPU not certified) | call 65 | `S011`; L65: OS9   I$SetStt   and do a sound |
| `ASM/GLIB/SMASH/S_Iniz.a` — 6809-family assembly (6309 indicators) | call 148 | `Opts`; L148: OS9   I$SetStt   and set the options |
| `ASM/GLIB/SMASH/S_Title.a` — 6809-family assembly (CPU not certified) | call 120,129 | `S001`; L120: OS9   I$SetStt |
| `ASM/GLIB/SMASH/Smash.a` — 6809-family assembly (CPU not certified) | call 111,124 | `S021`; L111: OS9   I$SetStt |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 680,1704,1775,7496,7509,7735,7864,8941,8960,8966,9007,9015,9028 | `FINLINIT`; L680: os9   I$SetStt |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 670,1688,1759,6710,6736,7188,7446,8725,8744,8750,8791,8799,8812 | `FINLINIT`; L670: os9   I$SetStt |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 672,1690,1761,6451,6464,6690,6819,7896,7915,7921,7962,7970,7983 | `FINLINIT`; L672: os9   I$SetStt |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 691,1714,1785,6509,6522,6753,6886,7963,7982,7988,8029,8037,8050,8668 | `FINLINIT`; L691: os9   I$SetStt |
| `ASM/NITROS9/SCF/scf_beta6.asm` — 6809-family assembly (6309 indicators) | ref 565 | `LC486`; L565: * I$SetStt entry point |
| `ASM/NITROS9/SCF/scf_ver100.asm` — 6809-family assembly (6309 indicators) | ref 571 | `LC486`; L571: * I$SetStt entry point |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 342,346,423,1651,2275,2278,2283 | `L015B`; L342: os9   I$SetStt |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 325,329,426,1895,2571,2575,2581,2756,2758,2765,2770,2783,3584,3711,3727,3817 | `L015B`; L325: os9   I$SetStt       Release any keyboard signals |
| `ASM/VEFIO-WINFO/vefio.asm` — 6809-family assembly (CPU not certified) | call 337,342,764 | `header / procedure / data`; L337: os9   I$SetStt set the signal trap |
| `C/LIB/access.a` — 6809-family assembly (CPU not certified) | call 72 | `creat10`; L72: os9 I$SETSTT set the file size to zero |
| `C/LIB/change.a` — 6809-family assembly (CPU not certified) | call 32,82 | `header / procedure / data`; L32: os9 I$SETSTT write the FD |
| `C/LIB/stat.a` — 6809-family assembly (CPU not certified) | call 63,69 | `setst10`; L63: os9 I$SETSTT |

### I$Write

Definition: `/dd/DEFS/os9.d:216`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 748 | `header / procedure / data`; L748: I$Write        RMB       1                   Write Data |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 2163,6371,9569 | `L0C52`; L2163: os9   I$Write    Write out entire module |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 2188,6092,8733 | `L0C52`; L2188: os9   I$Write    Write out entire module |
| `ASM/BASIC09/gfx2_ver1.asm` — 6809-family assembly (6309 indicators) | call 1251,1485; ref 1247 | `L07CD`; L1251: os9   I$Write        Write it out |
| `ASM/BASIC09/gfx_beta6.asm` — 6809-family assembly (6309 indicators) | call 262,271; ref 269 | `L0149`; L262: os9   I$Write      Write it |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 2161 | `PUT`; L2161: os9   I$Write        Write the data |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 2205 | `PUT`; L2205: os9   I$Write        Write the data |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 2221 | `PUT`; L2221: os9   I$Write        Write the data |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 217,1643 | `ckexit`; L217: os9   I$Write |
| `ASM/DWNET/dwnet.a` — 6809-family assembly (CPU not certified) | call 120,134,150,156,160,167,198,204 | `TCPKill`; L120: os9     I$Write |
| `ASM/FTP/command.a` — 6809-family assembly (CPU not certified) | call 28 | `chksig`; L28: os9     I$Write |
| `ASM/GLIB/GLIB/UGWBLK.a` — 6809-family assembly (CPU not certified) | call 44,73 | `I$Again`; L44: OS9   I$Write    close the path |
| `ASM/GLIB/GLIB/UPALETTE.a` — 6809-family assembly (CPU not certified) | call 21 | `U010`; L21: OS9   I$Write    write 4 bytes at X to path A |
| `ASM/GLIB/GLIB/USELECT.a` — 6809-family assembly (CPU not certified) | call 15 | `U$SELECT`; L15: OS9   I$Write    select the path |
| `ASM/GLIB/GLIB_p2.doc` — prose | ref 151 | `ERROR`; L151: Allocates a window, and returns its starting block number.  The process is very similar to using I$Open with '/w' to get a window, and I$Write with DWSET to tel |
| `ASM/GLIB/GLIB_p3.doc` — prose | call 78 | `LOOP`; L78: OS9   I$Write |
| `ASM/GLIB/SMASH/D_Screen.a` — 6809-family assembly (CPU not certified) | call 37,155,161,170,179,183,194 | `D010`; L37: OS9   I$Write    write out text to screen |
| `ASM/GLIB/SMASH/HScore.a` — 6809-family assembly (CPU not certified) | call 74,114,156,226 | `H060`; L74: OS9   I$Write    dump out overlay window |
| `ASM/GLIB/SMASH/Help.a` — 6809-family assembly (CPU not certified) | call 5 | `Help`; L5: OS9   I$Write |
| `ASM/GLIB/SMASH/KeyPress.a` — 6809-family assembly (CPU not certified) | call 72,94,142,163 | `P011`; L72: OS9   I$Write    and dump it out |
| `ASM/GLIB/SMASH/LLoad.a` — 6809-family assembly (CPU not certified) | call 129 | `NotFnd`; L129: OS9   I$Write |
| `ASM/GLIB/SMASH/P_Setup.a` — 6809-family assembly (CPU not certified) | call 19,24,31,36,51,56,73,93,97,111 | `P.Setup`; L19: OS9   I$Write |
| `ASM/GLIB/SMASH/Playball.a` — 6809-family assembly (CPU not certified) | call 181 | `P100`; L181: OS9   I$Write |
| `ASM/GLIB/SMASH/S_Iniz.a` — 6809-family assembly (6309 indicators) | call 56,64,98,157 | `X005`; L56: OS9   I$Write    and dump the font data to it. |
| `ASM/GLIB/SMASH/S_Title.a` — 6809-family assembly (CPU not certified) | call 20,188,192,195,244,327,341 | `S.TPrint`; L20: OS9   I$Write |
| `ASM/GLIB/SMASH/Smash.a` — 6809-family assembly (CPU not certified) | call 152 | `Z200`; L152: OS9   I$Write |
| `ASM/GSHELL/debug.a` — 6809-family assembly (6309 indicators) | call 22,45,67,97,124,132,147 | `binout1`; L22: os9   I$Write |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 5666,7259,7518,7913,8700,9190,9204,9470; ref 2 | `OLAYGN07`; L5666: os9   I$Write    Change font (ignore error) |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 5368,6236,6754,7544,8484,8974,8988,9254; ref 2 | `OLAYGN07`; L5368: os9   I$Write    Change font (ignore error) |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 5374,6214,6473,6868,7655,8145,8159,8425; ref 2 | `OLAYGN07`; L5374: os9   I$Write    Change font (ignore error) |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 5432,6272,6531,6935,7722,8212,8226,8492; ref 2 | `OLAYGN07`; L5432: os9   I$Write    Change font (ignore error) |
| `ASM/NITROS9/GIMEX/gimexcheck.a` — 6809-family assembly (CPU not certified) | call 62 | `XFndMsg`; L62: os9   I$Write |
| `ASM/NITROS9/KERNEL/krnp3_ed3_perr.asm` — 6809-family assembly (CPU not certified) | call 137 | `PErr`; L137: os9   I$Write    ;Write out Error message |
| `ASM/NITROS9/MODS/sysgo.asm` — 6809-family assembly (CPU not certified) | call 157,164,173 | `start`; L157: os9   I$Write |
| `ASM/NITROS9/PIPE/pipeman.asm` — 6809-family assembly (CPU not certified) | ref 359 | `WritLn`; L359: * I$Write Entry Point |
| `ASM/NITROS9/PIPE/pipeman_beta6.asm` — 6809-family assembly (CPU not certified) | ref 369 | `WritLn`; L369: * I$Write Entry Point |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | ref 1113 | `WtLn55E`; L1113: * I$Write Entry Point |
| `ASM/NITROS9/SCF/scf_beta6.asm` — 6809-family assembly (6309 indicators) | ref 960 | `writln`; L960: * I$Write entry point |
| `ASM/NITROS9/SCF/scf_ver100.asm` — 6809-family assembly (6309 indicators) | ref 966 | `writln`; L966: * I$Write entry point |
| `ASM/PLAY/play.a` — 6809-family assembly (6309 indicators) | call 254,555,558,656,692,698,858 | `Error`; L254: os9   I$Write |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 288,489,1128,1624 | `L00CC`; L288: os9   I$Write |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 266,503,1253,1865,3370,3716,3725 | `L00CC`; L266: os9   I$Write        Write it |
| `ASM/SOUNDRV/DrvPlay.asm` — 6809-family assembly (CPU not certified) | call 66,76,82 | `header / procedure / data`; L66: os9 I$Write        write that many bytes in one blast |
| `ASM/VEFIO-WINFO/vefio.asm` — 6809-family assembly (CPU not certified) | call 275,365,406,418,429,526,543,619,640,795 | `header / procedure / data`; L275: os9   I$Write write it to switch palettes |
| `ASM/VEFIO-WINFO/witesta.asm` — 6809-family assembly (CPU not certified) | call 174,202,231,249,264,278,288,296 | `prtloop`; L174: os9   i$write |
| `C/LIB/abort.a` — 6809-family assembly (CPU not certified) | call 48,53,60 | `doabort`; L48: os9 I$WRITE |
| `C/LIB/io.a` — 6809-family assembly (CPU not certified) | call 46 | `write`; L46: os9 I$WRITE |

### I$WritLn

Definition: `/dd/DEFS/os9.d:218`. No standalone implementation header found in this scope; trace kernel/IOMan dispatch before assigning semantics.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/BASIC09/basic09.d` — 6809-family assembly (6309 indicators) | ref 750 | `header / procedure / data`; L750: I$WritLn       RMB       1                   Write Line of ASCII Data |
| `ASM/BASIC09/basic09_ver100.asm` — 6809-family assembly (6309 indicators) | call 2307,3068,4715,13850 | `L0D6B`; L2307: os9   I$WritLn   Prompt user |
| `ASM/BASIC09/basic09_ver101.asm` — 6809-family assembly (6309 indicators) | call 2334,3135,4859,13047 | `L0D6B`; L2334: os9   I$WritLn   Prompt user |
| `ASM/BASIC09/runb_beta6.asm` — 6809-family assembly (6309 indicators) | call 735,7804 | `PrintErr`; L735: os9   I$WritLn       Print to error path |
| `ASM/BASIC09/runb_ver100.asm` — 6809-family assembly (6309 indicators) | call 742,6319 | `PrintErr`; L742: os9   I$WritLn       Print to error path |
| `ASM/BASIC09/runb_ver101.asm` — 6809-family assembly (6309 indicators) | call 749,6351 | `PrintErr`; L749: os9   I$WritLn       Print to error path |
| `ASM/BASIC09/runbcd.asm` — 6809-family assembly (6309 indicators) | call 541,1373,5178 | `prnterr`; L541: os9   I$WritLn |
| `ASM/DWNET/dwnet.a` — 6809-family assembly (CPU not certified) | call 170,207 | `writeport`; L170: os9     I$WritLn |
| `ASM/FTP/command.a` — 6809-family assembly (CPU not certified) | call 36 | `peerclosed`; L36: os9     I$WritLn |
| `ASM/FTP/regdump.a` — 6809-family assembly (CPU not certified) | call 138; ref 150 | `reg070`; L138: os9   I$WritLn |
| `ASM/GLIB/GLIB/IPRINT.a` — 6809-family assembly (CPU not certified) | call 82 | `I020`; L82: OS9   I$WritLn   write a line |
| `ASM/GLIB/GLIB/UDEBUG.a` — 6809-family assembly (CPU not certified) | call 85 | `D010`; L85: OS9   I$WritLn   write a line and ignore any errors |
| `ASM/GLIB/SMASH/HScore.a` — 6809-family assembly (CPU not certified) | call 284 | `P030`; L284: OS9   I$WritLn   dump it out |
| `ASM/GLIB/SMASH/LLoad.a` — 6809-family assembly (CPU not certified) | call 135 | `N010`; L135: OS9   I$WritLn   until a CR occurs |
| `ASM/GLIB/SMASH/P_Setup.a` — 6809-family assembly (CPU not certified) | call 79 | `P002`; L79: OS9   I$WritLn   dump out the text |
| `ASM/GSHELL/debug.a` — 6809-family assembly (6309 indicators) | call 174 | `Chrout`; L174: os9   I$WritLn |
| `ASM/GSHELL/gshell101_prehelp.asm` — 6809-family assembly (6309 indicators) | call 2992,2998,8718 | `WRLNWCR`; L2992: os9   I$WritLn   Write it |
| `ASM/GSHELL/gshell_beta6.asm` — 6809-family assembly (6309 indicators) | call 2959,2965,8502 | `WRLNWCR`; L2959: os9   I$WritLn   Write it |
| `ASM/GSHELL/gshell_ver1_0_0.asm` — 6809-family assembly (6309 indicators) | call 2961,2967,7673 | `WRLNWCR`; L2961: os9   I$WritLn   Write it |
| `ASM/GSHELL/gshell_ver1_0_1.asm` — 6809-family assembly (6309 indicators) | call 3001,3007,7740 | `WRLNWCR`; L3001: os9   I$WritLn   Write it |
| `ASM/NITROS9/CLOCKS/clock2_smart.asm` — 6809-family assembly (6309 indicators) | call 321,372 | `A4`; L321: os9   I$WritLn |
| `ASM/NITROS9/GIMEX/gimexcheck.a` — 6809-family assembly (CPU not certified) | call 56,94 | `XFndMsg`; L56: XFndMsg  os9   I$WritLn |
| `ASM/NITROS9/KERNEL/krnp3_ed3_perr.asm` — 6809-family assembly (CPU not certified) | call 265 | `PrinBuf`; L265: os9   I$WritLn   ;Write out message |
| `ASM/NITROS9/MODS/ioman_beta5.asm` — 6809-family assembly (6309 indicators) | call 2013,2022 | `L0913`; L2013: os9   I$WritLn       write the text |
| `ASM/NITROS9/MODS/krnp4_regdump.asm` — 6809-family assembly (6309 indicators) | call 201 | `reg070`; L201: os9   I$WritLn   send it |
| `ASM/NITROS9/MODS/sysgo.asm` — 6809-family assembly (CPU not certified) | call 134 | `WriteCR`; L134: os9   I$WritLn |
| `ASM/NITROS9/PIPE/pipeman.asm` — 6809-family assembly (CPU not certified) | ref 8,346 | `header / procedure / data`; L8: * 'show grf.3.a \| eat'  (eat is cat, but just does a I$ReadLn, and not I$WritLn) |
| `ASM/NITROS9/PIPE/pipeman_beta6.asm` — 6809-family assembly (CPU not certified) | ref 8,356 | `header / procedure / data`; L8: * 'show grf.3.a \| eat'  (eat is cat, but just does a I$ReadLn, and not I$WritLn) |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | ref 1070 | `Read530`; L1070: * I$WritLn Entry Point |
| `ASM/NITROS9/SCF/krnp4_regdump.asm` — 6809-family assembly (6309 indicators) | call 201 | `reg070`; L201: os9   I$WritLn   send it |
| `ASM/NITROS9/SCF/scf_beta6.asm` — 6809-family assembly (6309 indicators) | ref 956 | `L04D3`; L956: * I$WritLn entry point |
| `ASM/NITROS9/SCF/scf_ver100.asm` — 6809-family assembly (6309 indicators) | ref 962 | `L04D3`; L962: * I$WritLn entry point |
| `ASM/SHELL/shell21.a` — 6809-family assembly (CPU not certified) | call 418,1062,1133,1769,1809,2173,2266,2317,3032 | `L021F`; L418: os9   I$WritLn |
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | call 421,1174,1258,2021,2066,2455,2563,2621,3562,3609,3673 | `L021F`; L421: os9   I$WritLn       Write message out & return |
| `ASM/VEFIO-WINFO/vefio.asm` — 6809-family assembly (CPU not certified) | call 790 | `movexy10`; L790: os9   I$WritLn write the line |
| `ASM/VEFIO-WINFO/witesta.asm` — 6809-family assembly (CPU not certified) | call 215,227,260,274,301 | `prntfill`; L215: os9   I$writln |
| `C/LIB/cstart.a` — 6809-family assembly (CPU not certified) | call 220 | `erexit`; L220: os9 I$WRITLN write it |
| `C/LIB/io.a` — 6809-family assembly (CPU not certified) | call 63 | `writeln`; L63: os9 I$WRITLN call os9 |

## Unconfirmed tokens / local symbols — not established syscalls

### F$ALL

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | ref 2538 | `L0DF7`; L2538: * which is a lot of time... but maybe not compared to F$All/F$Sch, and |

### F$ETTSK

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/krn.asm` — 6809-family assembly (6309 indicators) | ref 417 | `link`; L417: fcb    F$etTsk+SysState |

### F$EXITED

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/SHELL/shellplus2.2a.asm` — 6809-family assembly (6309 indicators) | ref 2709 | `L12D4`; L2709: * Child F$Exited or was aborted - eat should go here |

### F$LDDXY

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/fld.asm` — 6809-family assembly (6309 indicators) | ref 50 | `AdjBlk0`; L50: * System Call: F$LDDXY |

### F$LINKS

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/DW/scdwv.asm` — 6809-family assembly (6309 indicators) | ref 21 | `header / procedure / data`; L21: * F$Links to it.  It then takes the descriptor module address and sticks it |

### F$MOVES

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/fmove.asm` — 6809-family assembly (6309 indicators) | ref 30 | `header / procedure / data`; L30: * Should speed up larger F$Moves |

### F$RSYSMEM

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.txt` — prose | ref 32,37 | `header / procedure / data`; L32: f$rsysmem do this for us?!?! |

### F$SCH

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | ref 2538 | `L0DF7`; L2538: * which is a lot of time... but maybe not compared to F$All/F$Sch, and |

### F$WAITING

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/KERNEL/fallprc.asm` — 6809-family assembly (6309 indicators) | ref 140 | `L035A`; L140: * No dead child & no signal...execute next F$Waiting process in line |

### I$A

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB/UGWBLK.a` — 6809-family assembly (CPU not certified) | ref 30,31 | `header / procedure / data`; L30: I$A      rmb   1          pointers saved on the stack |

### I$AGAIN

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB/UGWBLK.a` — 6809-family assembly (CPU not certified) | ref 41,126 | `I$Again`; L41: I$Again  lda   <Path,s |

### I$END

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB/URESTORE.a` — 6809-family assembly (CPU not certified) | ref 18,28,29 | `U$RESTORE`; L18: beq   I$END      skip if zero |
| `ASM/GLIB/GLIB/USAVE.a` — 6809-family assembly (CPU not certified) | ref 31 | `U010`; L31: leau  I$END,pcr  done the save/put/restore cycle, and start over |

### I$GETSTAT

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | ref 1203 | `Writ5C9`; L1203: * I$GetStat Entry Point |

### I$HXA

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB/IPRINT.a` — 6809-family assembly (CPU not certified) | ref 48,56,87,89 | `I020`; L48: lbsr  I$HXA      print out hex # in A |
| `ASM/GLIB/GLIB/UDEBUG.a` — 6809-family assembly (CPU not certified) | ref 51,59,90,92 | `D010`; L51: lbsr  I$HXA      print out hex # in A |

### I$HXD

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB/IPRINT.a` — 6809-family assembly (CPU not certified) | ref 52,60,64,68,73,77,87 | `I020`; L52: lbsr  I$HXD |
| `ASM/GLIB/GLIB/UDEBUG.a` — 6809-family assembly (CPU not certified) | ref 55,63,67,71,76,80,90 | `D010`; L55: lbsr  I$HXD |

### I$JNK

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB/UGWBLK.a` — 6809-family assembly (CPU not certified) | ref 35,36 | `header / procedure / data`; L35: I$Jnk    rmb   1          current count |

### I$MISC0

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB/DataMem.a` — 6809-family assembly (CPU not certified) | ref 9 | `header / procedure / data`; L9: I$Misc0: rmb   2          miscellaneous pointer zero |
| `ASM/GLIB/GLIB/USAVE.a` — 6809-family assembly (CPU not certified) | ref 37,42 | `S010`; L37: S010     stx   >I$Misc0   save current pointer |

### I$MISC1

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB/DataMem.a` — 6809-family assembly (CPU not certified) | ref 10 | `header / procedure / data`; L10: I$Misc1: rmb   2          miscellaneous pointer one |

### I$PRINT

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB/IPRINT.a` — 6809-family assembly (CPU not certified) | ref 3,13,19 | `header / procedure / data`; L3: * Called by PDBG "/text/",##, using LBRA I$PRINT |

### I$RETURN

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB/USAVE.a` — 6809-family assembly (CPU not certified) | ref 34,42 | `U010`; L34: leau  I$Return,pc where to return to after saving an element |

### I$SAVE

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB.tech` — prose | ref 71 | `header / procedure / data`; L71: Removed I$SAVE (U$YSAVE) in U$SAVE, to simplify rewrites. |

### I$SETSTAT

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/NITROS9/DW/scdwv.asm` — 6809-family assembly (6309 indicators) | ref 16 | `header / procedure / data`; L16: * The SS.Open I$SetStat entry point is called. That routine also detects |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` — 6809-family assembly (6309 indicators) | ref 1312 | `Gst640`; L1312: * I$SetStat Entry Point |

### I$SIZE

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB/UGWBLK.a` — 6809-family assembly (CPU not certified) | ref 31,46,60,142 | `header / procedure / data`; L31: I$Size   equ   I$A        size of stuff on the stack |

### I$SIZE1

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB/UGWBLK.a` — 6809-family assembly (CPU not certified) | ref 36,131 | `header / procedure / data`; L36: I$Size1  equ   I$Jnk+1 |

### I$STACK

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB/DataMem.a` — 6809-family assembly (CPU not certified) | ref 8 | `header / procedure / data`; L8: I$Stack: rmb   2          where the stack is saved before updating sprites |
| `ASM/GLIB/GLIB/URESTORE.a` — 6809-family assembly (CPU not certified) | ref 13,29 | `U$RESTORE`; L13: sts   <I$Stack   save old stack |
| `ASM/GLIB/GLIB/USAVE.a` — 6809-family assembly (CPU not certified) | ref 14,47 | `U$SAVE`; L14: sts   >I$Stack   save old stack |

### I$SWAPB

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB.links` — unknown | ref 13,19,20 | `U$IPFLIP`; L13: U$IPFLIP       GLIBDefs, OS9Defs, U$CPYMEM, I$SwapP, I$SwapB |
| `ASM/GLIB/GLIB/UBOTH.a` — 6809-family assembly (CPU not certified) | ref 25,32 | `U$BOTH`; L25: lbsr  I$SwapB    swap the blocks in memory |
| `ASM/GLIB/GLIB/UIPFLIP.a` — 6809-family assembly (CPU not certified) | ref 21,65 | `U$IPFLIP`; L21: lbsr  I$SwapB    map S1 current |
| `ASM/GLIB/GLIB/USSWAP.a` — 6809-family assembly (CPU not certified) | ref 30,43 | `U$SSWAP`; L30: bsr   I$SwapB |
| `ASM/GLIB/SMASH/Blocks.a` — 6809-family assembly (CPU not certified) | ref 188,192 | `B020`; L188: lbsr  I$SwapB    swap block pointers |
| `ASM/GLIB/SMASH/score.a` — 6809-family assembly (CPU not certified) | ref 88,91 | `SD040`; L88: lbsr  I$SwapB    swap the blocks in W$Mem |

### I$SWAPP

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB.links` — unknown | ref 13,19,20 | `U$IPFLIP`; L13: U$IPFLIP       GLIBDefs, OS9Defs, U$CPYMEM, I$SwapP, I$SwapB |
| `ASM/GLIB/GLIB/UBOTH.a` — 6809-family assembly (CPU not certified) | ref 26,33 | `U$BOTH`; L26: lbsr  I$SwapP |
| `ASM/GLIB/GLIB/UIPFLIP.a` — 6809-family assembly (CPU not certified) | ref 22 | `U$IPFLIP`; L22: lbsr  I$SwapP    path S1 current |
| `ASM/GLIB/GLIB/USSWAP.a` — 6809-family assembly (CPU not certified) | ref 18,38 | `U$SSWAP`; L18: bsr   I$SwapP |

### I$U

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB/UGWBLK.a` — 6809-family assembly (CPU not certified) | ref 34,71 | `header / procedure / data`; L34: I$U      rmb   2 |

### I$X

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB/UGWBLK.a` — 6809-family assembly (CPU not certified) | ref 32 | `header / procedure / data`; L32: I$X      rmb   2          before it's moved around. |

### I$Y

No matching declaration found in the inspected os9.d. Do not treat this spelling as a callable service based on this index.

| Source / language | Sites | Surrounding purpose / navigation |
|---|---|---|
| `ASM/GLIB/GLIB/UGWBLK.a` — 6809-family assembly (CPU not certified) | ref 33,72 | `header / procedure / data`; L33: I$Y      rmb   2 |
