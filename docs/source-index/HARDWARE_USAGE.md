# Hardware-address usage

[Navigation](README.md) · [Memory/MMU](MEMORY_MMU.md) · [6309](CPU_6309.md)

This index records literal `$FFxx` and `0xFFxx` references in the bounded priority source, plus address symbols from inspected definitions. It does **not** equate every literal with hardware access: immediates can be masks, vectors, limits or descriptor defaults. Instruction and surrounding source must be checked. Symbol-only accesses are addressed in the additional search routes below; indirect/calculated accesses are not an exhaustive bus trace.

Addresses are CPU-visible values in the cited source, not host physical RAM offsets. Both 6809 and 6309 source paths can access them. A 6309 indicator means the file also contains matched extended instructions; it does not assign the peripheral a CPU type. Source comments establish the symbolic labels below; manuals/hardware validation remain required.

## Verified symbolic anchors and caveats

- `/dd/DEFS/coco.d:94–100`: A.AciaP=$FF68, A.ModP=$FF6C, DPort=$FF40, MPI.Slct=$FF7F, PIA0Base=$FF00 and PIA1Base=$FF20.
- `/dd/DEFS/coco.d:168–170`: DAT.Task=$FF91 and DAT.Regs=$FFA0. The declaration supplies a base, not proof of every range or valid mapping on this emulator.
- `/dd/DEFS/coco.d:201–204`: GIMERegs is declared $FF00 (a source-specific base), IrqEnR=$FF92, BordReg=$FF9A, PalAdr=$FFB0. Do not silently reinterpret GIMERegs as $FF90.
- `/dd/DEFS/cocosdc.d:22–30`: controller, command/status, parameter and flash register symbols. `C/SDCCMDR/commsdc.h` has corresponding C constants; `commsdc.c` uses volatile pointer accesses and MPI save/select logic.
- `/dd/DEFS/drivewire.d:15–16`: BBOUT=$FF20 and BBIN=$FF22. The same numeric locations have other uses in other source contexts.
- `ASM/NITROS9/CLOCKS/clock2_messemu.asm` defines RTC.Base=$FF50 and explicitly documents its emulator/MPI assumptions. `ASM/SOUNDRV/SounDrv.asm` labels its $FF20 write as sound output.

## Literal references by address

Paths below are relative to `/dd/SOURCECODE`. Purpose excerpts are source evidence, not independently verified hardware identities.

### $FF00

Source symbols: `PIA0Base` — `/dd/DEFS/coco.d:99` (no comment); `GIMERegs` — `/dd/DEFS/coco.d:201` (Base address of GIME registers); `SDAddr` — `/dd/DEFS/rbsuper.d:32` (no comment); `SDAddr` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/rbsuper.d:29` (no comment); `SDAddr` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/rbsuper.d:29` (no comment); `leay` — `/dd/SOURCECODE/ASM/NITROS9/KERNEL/ccbfsrqmem.asm:85` (>>8,x); `fdb` — `/dd/SOURCECODE/ASM/NITROS9/SCF/nil.asm:24` (hardware port)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/DW/dw4read.asm`: 49,179 | lda	<$FF00		; ensure that IRQ outputs are cleared | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/NEW/rbsuper.d`: 29 | SDAddr      SET     $FF00 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/GIMEX/NEW/rel.asm`: 395 | deca             now D=$FF00, versus STU >-$0100,x (saves 1 byte) | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rbsuper.d`: 29 | SDAddr      SET     $FF00 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/GIMEX/rel.asm`: 392 | deca             now D=$FF00, versus STU >-$0100,x (saves 1 byte) | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel_beta6.asm`: 368 | deca               now D=$FF00, versus STU >-$0100,x (saves 1 byte) | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/ccbfsrqmem.asm`: 85 | leay	$ff00>>8,x | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/ccbkrn.txt`: 103 | physical block 0x3f will be banked in at 0xe000-0xff00 in cpu-space | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/KERNEL/rel.asm`: 125 | deca             now D=$FF00, versus STU >-$0100,x (saves 1 byte) | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel_beta6.asm`: 368 | deca               now D=$FF00, versus STU >-$0100,x (saves 1 byte) | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/MODS/dw4read.asm`: 49,179 | lda	<$FF00		; ensure that IRQ outputs are cleared | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/nil.asm`: 24 | fdb   $FF00      hardware port | No scanned extended-instruction site; not proof of 6809-only |
| `C/MAMOU/util.c`: 283,289,334 | as->E_bytes[as->E_total++] = (size & 0xFF00) >> 8; | No scanned extended-instruction site; not proof of 6809-only |
| `C/RDUMP/rdump.c`: 329 | return (((nbr & 0xff00) >> 8) + ((nbr & 0xff) << 8)); | No scanned extended-instruction site; not proof of 6809-only |

### $FF01

Source symbols: `fdb` — `/dd/SOURCECODE/ASM/NITROS9/SCF/ftdd.asm:24` (hardware port); `lda` — `/dd/SOURCECODE/ASM/SOUNDRV/SounDrv.asm:62` (unset bit 3 here and ff03 for DAC audio select); `sta` — `/dd/SOURCECODE/ASM/SOUNDRV/SounDrv.asm:64` (no comment); `lda` — `/dd/SOURCECODE/ASM/SOUNDRV/SounDrv.asm:75` (else turn off sound hardware); `sta` — `/dd/SOURCECODE/ASM/SOUNDRV/SounDrv.asm:77` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/DW/dw4read.asm`: 43,47,116,172,177,252 | lda	<$FF01		; save PIA 0 controls on the stack | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/MODS/dw4read.asm`: 43,47,116,172,177,252 | lda	<$FF01		; save PIA 0 controls on the stack | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/ftdd.asm`: 24 | fdb   $FF01      hardware port | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/PLAY/play.a`: 937,939 | lda   >$ff01 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/SOUNDRV/SounDrv.asm`: 62,64,75,77 | lda $FF01          unset bit 3 here and ff03 for DAC audio select | No scanned extended-instruction site; not proof of 6809-only |

### $FF02

Source symbols: `fdb` — `/dd/SOURCECODE/ASM/NITROS9/SCF/vi.asm:24` (hardware port)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/DW/dw4read.asm`: 50,180 | ldb	<$FF02 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/MODS/dw4read.asm`: 50,180 | ldb	<$FF02 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/RBF/emudsk_beta601.asm`: 111 | INIT     ldd   #$FF02        'Invalid' value & # of drives | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/vi.asm`: 24 | fdb   $FF02      hardware port | No scanned extended-instruction site; not proof of 6809-only |

### $FF03

Source symbols: `lda` — `/dd/SOURCECODE/ASM/SOUNDRV/SounDrv.asm:65` (no comment); `sta` — `/dd/SOURCECODE/ASM/SOUNDRV/SounDrv.asm:67` (no comment); `lda` — `/dd/SOURCECODE/ASM/SOUNDRV/SounDrv.asm:78` (no comment); `sta` — `/dd/SOURCECODE/ASM/SOUNDRV/SounDrv.asm:80` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/DW/dw4read.asm`: 44,48,117,173,178,253 | ldb	<$FF03 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/MODS/dw4read.asm`: 44,48,117,173,178,253 | ldb	<$FF03 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/wordpakii.asm`: 522 | L03EE    sta   >$FF03 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/PLAY/play.a`: 934,936 | lda   >$ff03        Set up PIA for sound output on Coco | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/SOUNDRV/SounDrv.asm`: 65,67,78,80 | lda $FF03 | No scanned extended-instruction site; not proof of 6809-only |

### $FF04

Source symbols: `fdb` — `/dd/SOURCECODE/ASM/NITROS9/SCF/p1_sc6551dragon.asm:16` (physical controller address)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/SCF/p1_sc6551dragon.asm`: 16 | fdb   $FF04  physical controller address | No scanned extended-instruction site; not proof of 6809-only |

### $FF08

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/GLIB/SMASH/S_Title.a`: 37 | ldd   #$FF08 | No scanned extended-instruction site; not proof of 6809-only |

### $FF0F

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/GLIB/SMASH/PutBall.a`: 81 | ldd   #$FF0F | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/msf.asm`: 1013,3017 | ldd   #$FF0F         end of file mark | Contains extended-instruction candidates; inspect guards |

### $FF11

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/GIMEX/NEW/rel.asm`: 390 | stb   >$FF11 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel_beta6.asm`: 363 | stb   >$FF11 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel_beta6.asm`: 363 | stb   >$FF11 | Contains extended-instruction candidates; inspect guards |

### $FF18

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/PLAY/mulaw_alaw.a`: 14 | sta   >$ff18        5/4 Send voltage to 8 bit TC-9 DAC | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/PLAY/play.a`: 962,990,1018,1049,1088 | sta   >$FF18        5/4 Send voltage to 8 bit TC-9 DAC | No scanned extended-instruction site; not proof of 6809-only |

### $FF20

Source symbols: `PIA1Base` — `/dd/DEFS/coco.d:100` (no comment); `BBOUT` — `/dd/DEFS/drivewire.d:15` (no comment); `BBOUT` — `/dd/SOURCECODE/ASM/NITROS9/DW/dw4write.asm:18` (; bit banger output port); `BBOUT` — `/dd/SOURCECODE/ASM/NITROS9/MODS/dw4write.asm:18` (; bit banger output port); `fdb` — `/dd/SOURCECODE/ASM/NITROS9/SCF/t1_bbt.asm:27` (physical controller address); `fdb` — `/dd/SOURCECODE/ASM/NITROS9/SCF/term_t1.asm:27` (physical controller address); `sta` — `/dd/SOURCECODE/ASM/SOUNDRV/SounDrv.asm:50` (send to sound hardware)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/DW/dw4read.asm`: 76,118,211,239,254 | rxByte	lda	<$FF20		; reset FIRQ | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/DW/dw4write.asm`: 18 | BBOUT	equ	$FF20		; bit banger output port | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/DW/dwinit.asm`: 27 | ldx     #PIA1Base           $FF20 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/DW/dwread.asm`: 260 | ldd       #$ff20              ; A = timeout msb, B = shift counter | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/MODS/dw4read.asm`: 76,118,211,239,254 | rxByte	lda	<$FF20		; reset FIRQ | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/MODS/dw4write.asm`: 18 | BBOUT	equ	$FF20		; bit banger output port | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dwinit.asm`: 27 | ldx       #PIA1Base           $FF20 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dwread.asm`: 186 | ldd       #$ff20              ; A = timeout msb, B = shift counter | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/scbbp.asm`: 226 | L00AC    bsr   L007D      write B to $FF20 and wait | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/t1_bbt.asm`: 27 | fdb   $FF20      physical controller address | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/SCF/term_t1.asm`: 27 | fdb   $FF20      physical controller address | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/PLAY/mulaw_alaw.a`: 12 | sta   >$ff20        5/4 Send voltage to 6 bit Coco DAC | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/PLAY/play.a`: 960,988,1016,1047,1086 | sta   >$ff20        5/4 Send voltage to 6 bit Coco DAC | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/SOUNDRV/SounDrv.asm`: 50 | sta $ff20          send to sound hardware | No scanned extended-instruction site; not proof of 6809-only |

### $FF21

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/DW/dw4read.asm`: 51,56,113,181,187,249 | lda	<$FF21		; save PIA 1 controls on the stack | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/MODS/dw4read.asm`: 51,56,113,181,187,249 | lda	<$FF21		; save PIA 1 controls on the stack | Contains extended-instruction candidates; inspect guards |

### $FF22

Source symbols: `BBIN` — `/dd/DEFS/drivewire.d:16` (no comment); `BBIN` — `/dd/SOURCECODE/ASM/NITROS9/DW/dw4read.asm:28` (; bit banger input port); `lda` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/llcocosdc.asm:521` (reset any latched CART interrupt); `lda` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/rel.asm:617` (no comment); `sta` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/rel.asm:619` (no comment); `lda` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/rel_beta6.asm:620` (no comment); `sta` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/rel_beta6.asm:622` (no comment); `lda` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/llcocosdc_beta6.asm:503` (reset any latched CART interrupt)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/DW/dw4read.asm`: 28 | BBIN	equ	$FF22		; bit banger input port | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/NEW/rel.asm`: 631,633 | lda    $ff22 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/llcocosdc.asm`: 521 | lda       $FF22              reset any latched CART interrupt | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/llcocosdc_beta6.asm`: 503 | lda   $FF22        reset any latched CART interrupt | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/llcocosdc_ver101.asm`: 538 | lda       $FF22              reset any latched CART interrupt | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel.asm`: 617,619 | lda    $ff22 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel_beta6.asm`: 620,622 | lda    $ff22 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel.asm`: 302,304 | lda    $ff22 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel_beta6.asm`: 620,622 | lda    $ff22 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/MODS/dw4read.asm`: 28 | BBIN	equ	$FF22		; bit banger input port | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/RBF/cc3disk_disto.asm`: 142 | lda   $FF22 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_sc2_irq.asm`: 125 | lda   >$FF22 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/SCF/covdg_beta6.asm`: 660,816 | DispAlfa pshs  x,y,a        Preserve regs (A=video bits to merge into $FF22) | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta61.asm`: 693,849 | DispAlfa pshs  x,y,a        Preserve regs (A=video bits to merge into $FF22) | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_ver100.asm`: 681,837 | DispAlfa pshs  x,y,a        Preserve regs (A=video bits to merge into $FF22) | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/p_bbp.asm`: 28 | fdb   $FF22      physical controller address | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/SCF/s16550v16_lOS9.asm`: 190 | lda   >$FF22 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/SCF/scbbp.asm`: 131 | lbsr  L0104        get low bit of $FF22 into carry | Contains extended-instruction candidates; inspect guards |

### $FF23

Source symbols: `lda` — `/dd/SOURCECODE/ASM/NITROS9/RBF/cc3disk_disto.asm:138` (no comment); `sta` — `/dd/SOURCECODE/ASM/NITROS9/RBF/cc3disk_disto.asm:141` (no comment); `lda` — `/dd/SOURCECODE/ASM/SOUNDRV/SounDrv.asm:68` (set bit 3 for sound enable); `sta` — `/dd/SOURCECODE/ASM/SOUNDRV/SounDrv.asm:70` (no comment); `lda` — `/dd/SOURCECODE/ASM/SOUNDRV/SounDrv.asm:81` (no comment); `sta` — `/dd/SOURCECODE/ASM/SOUNDRV/SounDrv.asm:83` (reset bit 8, sound disable)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/DW/dw4read.asm`: 52,57,114,182,188,250 | ldb	<$FF23 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/DW/dwwrite.asm`: 102,104 | ldb       <$ff23              ; read PIA 1-B control register | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dw4read.asm`: 52,57,114,182,188,250 | ldb	<$FF23 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/MODS/dwwrite.asm`: 75,77 | ldb       <$ff23              ; read PIA 1-B control register | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_disto.asm`: 138,141 | lda   $FF23 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_sc2_irq.asm`: 122,124 | lda   >$FF23 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/SCF/s16550v16_lOS9.asm`: 187,189 | L011B    lda   >$FF23 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/PLAY/play.a`: 940,942 | lda   >$ff23 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/SOUNDRV/SounDrv.asm`: 68,70,81,83 | lda $FF23          set bit 3 for sound enable | No scanned extended-instruction site; not proof of 6809-only |

### $FF34

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/SHELL/shell21.a`: 664 | inc   >$FF34 | Contains extended-instruction candidates; inspect guards |

### $FF40

Source symbols: `DPort` — `/dd/DEFS/coco.d:96` (Disk controller base address); `CTRLATCH` — `/dd/DEFS/cocosdc.d:22` (controller latch (write)); `CTRLATCH` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/cocosdc.d:19` (controller latch (write)); `CTRLATCH` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/cocosdc.d:19` (controller latch (write)); `clr` — `/dd/SOURCECODE/ASM/NITROS9/KERNEL/boot_vhd.asm:58` (make sure motors are turned off); `RG.Ctrl` — `/dd/SOURCECODE/ASM/NITROS9/RBF/cc3disk_disto.asm:41` (no comment); `fdb` — `/dd/SOURCECODE/ASM/NITROS9/RBF/d0.asm:26` (port address); `fdb` — `/dd/SOURCECODE/ASM/NITROS9/RBF/msfdesc.asm:16` (port address)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/GIMEX/NEW/cocosdc.d`: 19 | CTRLATCH       equ       $FF40              controller latch (write) | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/GIMEX/NEW/rel.asm`: 459 | clr   >$FF40     turn off disk drives | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/cocosdc.d`: 19 | CTRLATCH       equ       $FF40              controller latch (write) | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/GIMEX/rel.asm`: 460 | clr   >$FF40     turn off disk drives | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel_beta6.asm`: 424 | clr   >$FF40       turn off disk drives | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/boot_rampak.asm`: 110 | Address  fdb   $FF40      address of the device to boot from | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/KERNEL/boot_scsi.asm`: 115 | clr       >$FF40              stop the disk motors | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/KERNEL/boot_vhd.asm`: 58 | clr   $FF40      make sure motors are turned off | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/KERNEL/boot_wd1002.asm`: 114 | cmpy  #$FF40     base address too low? | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/KERNEL/rel.asm`: 190 | clr   >$FF40     turn off disk drives | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel_beta6.asm`: 424 | clr   >$FF40       turn off disk drives | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/RBF/cc3disk_disto.asm`: 41 | RG.Ctrl  equ   $ff40 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_sc2_irq.asm`: 77,258,510,520,529,550,593 | stb   >$FF40 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_sc2_slp.asm`: 71,155,233,382,467,477,486,507,542 | stb   >$FF40 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/d0.asm`: 26 | fdb   $FF40      port address | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/msfdesc.asm`: 16 | fdb   $ff40      port address | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/rb1773desc.asm`: 43 | fdb   $FF40      physical controller address | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/sdisk3_dmc.asm`: 9 | fdcdrv   equ   $ff40 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/sdisk3_dpj.asm`: 179,220,266,549,763,803,806 | stb   >$FF40 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/sdisk3desc.asm`: 36 | fdb   $FF40      physical controller address | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/SCF/wordpakii.asm`: 284 | sta   >$FF40 | No scanned extended-instruction site; not proof of 6809-only |
| `C/SDCCMDR/commsdc.h`: 7 | #define CTRLATCH 0xff40 | No scanned extended-instruction site; not proof of 6809-only |

### $FF41

Source symbols: `BECKBASE` — `/dd/SOURCECODE/ASM/NITROS9/DW/dwio.asm:35` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/DW/dwio.asm`: 35 | BECKBASE       equ     $ff41 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dwread.asm`: 76 | loop@     ldb    $FF41 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/RBF/sdisk3_dmc.asm`: 144,159,422,1005,1224,1230,1268 | sta   >$FF41 | No scanned extended-instruction site; not proof of 6809-only |

### $FF42

Source symbols: `FLSHDAT` — `/dd/DEFS/cocosdc.d:30` (flash data register); `FLSHDAT` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/cocosdc.d:27` (flash data register); `FLSHDAT` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/cocosdc.d:27` (flash data register); `ldb` — `/dd/SOURCECODE/ASM/NITROS9/MODS/dwread.asm:79` (no comment); `sta` — `/dd/SOURCECODE/ASM/NITROS9/MODS/dwwrite.asm:56` (no comment); `fdptrl` — `/dd/SOURCECODE/ASM/NITROS9/RBF/sdisk3_dmc.asm:19` (no comment); `FLSHDATA` — `/dd/SOURCECODE/C/SDCCMDR/commsdc.h:8` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/GIMEX/NEW/cocosdc.d`: 27 | FLSHDAT        equ       $FF42              flash data register | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/GIMEX/cocosdc.d`: 27 | FLSHDAT        equ       $FF42              flash data register | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dwread.asm`: 79 | ldb    $FF42 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/MODS/dwwrite.asm`: 56 | sta       $FF42 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/sdisk3_dmc.asm`: 19 | fdptrl   equ   $ff42 | No scanned extended-instruction site; not proof of 6809-only |
| `C/SDCCMDR/commsdc.h`: 8 | #define FLSHDATA 0xff42 | No scanned extended-instruction site; not proof of 6809-only |

### $FF43

Source symbols: `FLSHCTRL` — `/dd/SOURCECODE/C/SDCCMDR/commsdc.h:9` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `C/SDCCMDR/commsdc.h`: 9 | #define FLSHCTRL 0xff43 | No scanned extended-instruction site; not proof of 6809-only |

### $FF44

Source symbols: `ldb` — `/dd/SOURCECODE/ASM/NITROS9/DW/dwread.asm:85` (no comment); `sta` — `/dd/SOURCECODE/ASM/NITROS9/DW/dwwrite.asm:58` (no comment); `ldb` — `/dd/SOURCECODE/ASM/NITROS9/MODS/dwread.asm:51` (no comment); `sta` — `/dd/SOURCECODE/ASM/NITROS9/MODS/dwwrite.asm:38` (no comment); `fdwrit` — `/dd/SOURCECODE/ASM/NITROS9/RBF/sdisk3_dmc.asm:15` (no comment); `disdma` — `/dd/SOURCECODE/ASM/NITROS9/RBF/sdisk3_dmc.asm:17` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/DW/dwread.asm`: 85 | ldb    $FF44 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/DW/dwwrite.asm`: 58 | sta       $FF44 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dwread.asm`: 51 | ldb    $FF44 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/MODS/dwwrite.asm`: 38 | sta       $FF44 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/sdisk3_dmc.asm`: 15,17 | fdwrit   equ   $ff44 | No scanned extended-instruction site; not proof of 6809-only |

### $FF46

Source symbols: `fdptrh` — `/dd/SOURCECODE/ASM/NITROS9/RBF/sdisk3_dmc.asm:18` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/RBF/sdisk3_dmc.asm`: 18 | fdptrh   equ   $ff46 | No scanned extended-instruction site; not proof of 6809-only |

### $FF48

Source symbols: `CMDREG` — `/dd/DEFS/cocosdc.d:23` (command register (write)); `STATREG` — `/dd/DEFS/cocosdc.d:24` (status register (read)); `CMDREG` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/cocosdc.d:20` (command register (write)); `STATREG` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/cocosdc.d:21` (status register (read)); `CMDREG` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/cocosdc.d:20` (command register (write)); `STATREG` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/cocosdc.d:21` (status register (read)); `sta` — `/dd/SOURCECODE/ASM/NITROS9/KERNEL/boot_vhd.asm:53` (command register); `lda` — `/dd/SOURCECODE/ASM/NITROS9/KERNEL/boot_vhd.asm:57` (clear controller)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/GIMEX/NEW/cocosdc.d`: 20,21 | CMDREG         equ       $FF48              command register (write) | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/GIMEX/cocosdc.d`: 20,21 | CMDREG         equ       $FF48              command register (write) | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/KERNEL/boot_vhd.asm`: 53,57 | sta   $FF48      command register | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_disto.asm`: 42 | RG.Stat  equ   $ff48 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_sc2_irq.asm`: 88,239,253,283,308,426,506,514,522,586 | ldx   #$FF48 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_sc2_slp.asm`: 79,151,219,229,248,253,370,383,463,471,479 | ldx   #$FF48 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/sdisk3_dmc.asm`: 10,11 | fdccmd   equ   $ff48 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/sdisk3_dpj.asm`: 97,165,169,184,188,193,208,210,214,252,256,263,273,277,282,296,300,305,316,543,550 | lda   >$FF48 | No scanned extended-instruction site; not proof of 6809-only |
| `C/SDCCMDR/commsdc.h`: 10,11 | #define CMDREG   0xff48 | No scanned extended-instruction site; not proof of 6809-only |

### $FF49

Source symbols: `PREG1` — `/dd/DEFS/cocosdc.d:25` (param register 1); `PREG1` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/cocosdc.d:22` (param register 1); `PREG1` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/cocosdc.d:22` (param register 1); `RG.Trk` — `/dd/SOURCECODE/ASM/NITROS9/RBF/cc3disk_disto.asm:43` (no comment); `fdctrk` — `/dd/SOURCECODE/ASM/NITROS9/RBF/sdisk3_dmc.asm:12` (no comment); `PREG1` — `/dd/SOURCECODE/C/SDCCMDR/commsdc.h:12` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/GIMEX/NEW/cocosdc.d`: 22 | PREG1          equ       $FF49              param register 1 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/GIMEX/cocosdc.d`: 22 | PREG1          equ       $FF49              param register 1 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_disto.asm`: 43 | RG.Trk   equ   $ff49 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_sc2_irq.asm`: 408,421 | L030A    stb   >$FF49 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_sc2_slp.asm`: 353,366 | L028E    stb   >$FF49 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/sdisk3_dmc.asm`: 12 | fdctrk   equ   $ff49 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/sdisk3_dpj.asm`: 484,505 | stb   >$FF49 | No scanned extended-instruction site; not proof of 6809-only |
| `C/SDCCMDR/commsdc.h`: 12 | #define PREG1    0xff49 | No scanned extended-instruction site; not proof of 6809-only |

### $FF4A

Source symbols: `SDAddr` — `/dd/DEFS/cocosdc.d:19` (no comment); `PREG2` — `/dd/DEFS/cocosdc.d:26` (param register 2); `SDAddr` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/cocosdc.d:16` (no comment); `PREG2` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/cocosdc.d:23` (param register 2); `SDAddr` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/cocosdc.d:16` (no comment); `PREG2` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/cocosdc.d:23` (param register 2); `RG.Sect` — `/dd/SOURCECODE/ASM/NITROS9/RBF/cc3disk_disto.asm:44` (no comment); `fdcsec` — `/dd/SOURCECODE/ASM/NITROS9/RBF/sdisk3_dmc.asm:13` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/GIMEX/NEW/cocosdc.d`: 16,23 | SDAddr         SET       $FF4A | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/GIMEX/cocosdc.d`: 16,23 | SDAddr         SET       $FF4A | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_disto.asm`: 44 | RG.Sect  equ   $ff4a | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_sc2_irq.asm`: 395 | L02EE    stb   >$FF4A | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_sc2_slp.asm`: 340 | L0272    stb   >$FF4A | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/sdisk3_dmc.asm`: 13,439 | fdcsec   equ   $ff4A | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/sdisk3_dpj.asm`: 391,451 | stb   >$FF4A | No scanned extended-instruction site; not proof of 6809-only |
| `C/SDCCMDR/commsdc.h`: 13 | #define PREG2    0xff4a | No scanned extended-instruction site; not proof of 6809-only |

### $FF4B

Source symbols: `PREG3` — `/dd/DEFS/cocosdc.d:27` (param register 3); `PREG3` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/cocosdc.d:24` (param register 3); `PREG3` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/cocosdc.d:24` (param register 3); `RG.Data` — `/dd/SOURCECODE/ASM/NITROS9/RBF/cc3disk_disto.asm:45` (no comment); `fdcdta` — `/dd/SOURCECODE/ASM/NITROS9/RBF/sdisk3_dmc.asm:14` (no comment); `PREG3` — `/dd/SOURCECODE/C/SDCCMDR/commsdc.h:14` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/GIMEX/NEW/cocosdc.d`: 24 | PREG3          equ       $FF4B              param register 3 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/GIMEX/cocosdc.d`: 24 | PREG3          equ       $FF4B              param register 3 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_disto.asm`: 45 | RG.Data  equ   $ff4b | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_sc2_irq.asm`: 414,528 | L0318    sta   >$FF4B | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_sc2_slp.asm`: 359,485 | L029C    sta   >$FF4B | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/sdisk3_dmc.asm`: 14 | fdcdta   equ   $ff4B | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/sdisk3_dpj.asm`: 177,197,268,287,312,491 | L0136    lda   >$FF4B | No scanned extended-instruction site; not proof of 6809-only |
| `C/SDCCMDR/commsdc.h`: 14 | #define PREG3    0xff4b | No scanned extended-instruction site; not proof of 6809-only |

### $FF4C

Source symbols: `fdread` — `/dd/SOURCECODE/ASM/NITROS9/RBF/sdisk3_dmc.asm:16` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/DW/dwread.asm`: 82 | loop@     ldb    $FF4C | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/MODS/dwread.asm`: 48 | loop@     ldb    $FF4C | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/RBF/sdisk3_dmc.asm`: 16 | fdread   equ   $ff4c | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/SHELL/shell21.a`: 553 | jsr   >$FF4C | Contains extended-instruction candidates; inspect guards |

### $FF4E

Source symbols: `buffer` — `/dd/SOURCECODE/ASM/NITROS9/RBF/sdisk3_dmc.asm:20` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/RBF/sdisk3_dmc.asm`: 20 | buffer   equ   $ff4e | No scanned extended-instruction site; not proof of 6809-only |

### $FF50

Source symbols: `RTC.Base` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_disto2.asm:24` (Base address of clock); `RTC.Base` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_disto4.asm:24` (Base address of clock); `RTC.Base` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_messemu.asm:31` (no comment); `clr` — `/dd/SOURCECODE/ASM/NITROS9/DW/dwinit.asm:11` (no comment); `ldb` — `/dd/SOURCECODE/ASM/NITROS9/DW/dwread.asm:29` (; clear CA1 bit in status register); `clr` — `/dd/SOURCECODE/ASM/NITROS9/MODS/dwinit.asm:11` (no comment); `ldb` — `/dd/SOURCECODE/ASM/NITROS9/MODS/dwread.asm:29` (; clear CA1 bit in status register)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_disto2.asm`: 24 | RTC.Base equ   $FF50      Base address of clock | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/CLOCKS/clock2_disto4.asm`: 24 | RTC.Base equ   $FF50      Base address of clock | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/CLOCKS/clock2_messemu.asm`: 31 | RTC.Base equ   $FF50 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/DW/dwinit.asm`: 11 | clr     $FF50 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/DW/dwread.asm`: 29 | ldb    $FF50               ; clear CA1 bit in status register | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/boot_burke.asm`: 236,295,326 | sta   >$FF50     Store in data register | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dwinit.asm`: 11 | clr       $FF50 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dwread.asm`: 29 | ldb    $FF50               ; clear CA1 bit in status register | Contains extended-instruction candidates; inspect guards |

### $FF51

Source symbols: `clr` — `/dd/SOURCECODE/ASM/NITROS9/DW/dwinit.asm:10` (no comment); `sta` — `/dd/SOURCECODE/ASM/NITROS9/DW/dwinit.asm:13` (no comment); `clr` — `/dd/SOURCECODE/ASM/NITROS9/MODS/dwinit.asm:10` (no comment); `sta` — `/dd/SOURCECODE/ASM/NITROS9/MODS/dwinit.asm:13` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/DW/dwinit.asm`: 10,13 | clr     $FF51 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/DW/dwread.asm`: 27 | dwrloop   tst    $FF51        ; check for CA1 bit (1=Arduino has byte ready) | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/boot_burke.asm`: 68,135,260,261 | clr   >$FF51       Reset controller | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dwinit.asm`: 10,13 | clr       $FF51 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dwread.asm`: 27 | loop@     tst    $FF51               ; check for CA1 bit (1=Arduino has byte ready) | Contains extended-instruction candidates; inspect guards |

### $FF52

Source symbols: `sta` — `/dd/SOURCECODE/ASM/NITROS9/DW/dwinit.asm:18` (no comment); `sta` — `/dd/SOURCECODE/ASM/NITROS9/DW/dwwrite.asm:23` (; put it to PIA); `tst` — `/dd/SOURCECODE/ASM/NITROS9/DW/dwwrite.asm:26` (; clear CB1 in status register); `sta` — `/dd/SOURCECODE/ASM/NITROS9/MODS/dwinit.asm:18` (no comment); `sta` — `/dd/SOURCECODE/ASM/NITROS9/MODS/dwwrite.asm:23` (; put it to PIA); `tst` — `/dd/SOURCECODE/ASM/NITROS9/MODS/dwwrite.asm:26` (; clear CB1 in status register); `fdb` — `/dd/SOURCECODE/ASM/NITROS9/RBF/pp.asm:18` (physical controller address)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/DW/dwinit.asm`: 18 | sta     $FF52 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/DW/dwwrite.asm`: 23,26 | sta       $FF52              ; put it to PIA | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/KERNEL/boot_burke.asm`: 255 | clr   >$FF52     data, keep reading status register until it is ready | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dwinit.asm`: 18 | sta       $FF52 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dwwrite.asm`: 23,26 | sta       $FF52              ; put it to PIA | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/parallel.asm`: 53,55,56,64 | tst   >$FF52 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/pp.asm`: 18 | fdb   $FF52      physical controller address | No scanned extended-instruction site; not proof of 6809-only |

### $FF53

Source symbols: `clr` — `/dd/SOURCECODE/ASM/NITROS9/DW/dwinit.asm:16` (no comment); `sta` — `/dd/SOURCECODE/ASM/NITROS9/DW/dwinit.asm:20` (no comment); `clr` — `/dd/SOURCECODE/ASM/NITROS9/MODS/dwinit.asm:16` (no comment); `sta` — `/dd/SOURCECODE/ASM/NITROS9/MODS/dwinit.asm:20` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/DW/dwinit.asm`: 16,20 | clr     $FF53 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/DW/dwwrite.asm`: 24 | loop@     tst       $FF53              ; check status register | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/KERNEL/boot_burke.asm`: 70 | clr   >$FF53       Set controller mode to 0 (part of init) | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dwinit.asm`: 16,20 | clr       $FF53 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dwwrite.asm`: 24 | loop@     tst       $FF53              ; check status register | No scanned extended-instruction site; not proof of 6809-only |

### $FF58

Source symbols: `RW.Dat` — `/dd/SOURCECODE/ASM/NITROS9/RBF/rb1773.asm:119` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/RBF/rb1773.asm`: 119 | RW.Dat   equ   $FF58 | Contains extended-instruction candidates; inspect guards |

### $FF5A

Source symbols: `RW.Ctrl` — `/dd/SOURCECODE/ASM/NITROS9/RBF/rb1773.asm:120` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/RBF/rb1773.asm`: 120 | RW.Ctrl  equ   $FF5A | Contains extended-instruction candidates; inspect guards |

### $FF5C

Source symbols: `RTC.Base` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_ds1315.asm:28` (In SCS* Decode)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_ds1315.asm`: 28 | RTC.Base equ   $FF5C      In SCS* Decode | No scanned extended-instruction site; not proof of 6809-only |

### $FF60

Source symbols: `RTC.Base` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_harris.asm:24` (Base address for clock)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_harris.asm`: 24 | RTC.Base equ   $FF60      Base address for clock | No scanned extended-instruction site; not proof of 6809-only |

### $FF64

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/SCF/joydrv_6552l.asm`: 29 | mod   eom,name,tylg,atrv,start,$FF64 | No scanned extended-instruction site; not proof of 6809-only |

### $FF68

Source symbols: `A.AciaP` — `/dd/DEFS/coco.d:94` (Aciapak Address); `SY6551B` — `/dd/SOURCECODE/ASM/NITROS9/DW/dwio.asm:27` (no comment); `fdb` — `/dd/SOURCECODE/ASM/NITROS9/SCF/t2_sc6551.asm:27` (physical controller address); `fdb` — `/dd/SOURCECODE/ASM/NITROS9/SCF/t2_sc6552.asm:27` (physical controller address); `fdb` — `/dd/SOURCECODE/ASM/NITROS9/SCF/term_sc6551.asm:27` (physical controller address)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/DW/dwio.asm`: 27 | SY6551B        equ     $ff68 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/SCF/joydrv_6551m.asm`: 29 | mod   eom,name,tylg,atrv,start,$FF68 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/SCF/t2_sc6551.asm`: 27 | fdb   $FF68      physical controller address | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/SCF/t2_sc6552.asm`: 27 | fdb   $FF68      physical controller address | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/SCF/term_sc6551.asm`: 27 | fdb   $FF68      physical controller address | No scanned extended-instruction site; not proof of 6809-only |

### $FF69

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/SCF/scbbt.asm`: 402 | lda   >$FF69 | No scanned extended-instruction site; not proof of 6809-only |

### $FF6C

Source symbols: `A.ModP` — `/dd/DEFS/coco.d:95` (ModPak Address); `fdb` — `/dd/SOURCECODE/ASM/NITROS9/SCF/t3_sc6551.asm:27` (physical controller address); `fdb` — `/dd/SOURCECODE/ASM/NITROS9/SCF/t3_sc6552.asm:27` (physical controller address)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/SCF/joydrv_6551l.asm`: 82 | mod   eom,name,tylg,atrv,start,$FF6C | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/SCF/t3_sc6551.asm`: 27 | fdb   $FF6C      physical controller address | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/SCF/t3_sc6552.asm`: 27 | fdb   $FF6C      physical controller address | No scanned extended-instruction site; not proof of 6809-only |

### $FF70

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/RBF/rammer_ed6_8-64mb.asm`: 170,191 | sta  >$FF70          Collyer | Contains extended-instruction candidates; inspect guards |

### $FF72

Source symbols: `RTC.Base` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_elim.asm:27` (I don't know base for this chip.)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_elim.asm`: 27 | RTC.Base equ   $FF72      I don't know base for this chip. | No scanned extended-instruction site; not proof of 6809-only |

### $FF74

Source symbols: `RW.Dat` — `/dd/SOURCECODE/ASM/NITROS9/RBF/cc3disk_disto.asm:48` (no comment); `nh_base` — `/dd/SOURCECODE/ASM/NITROS9/RBF/cc3disk_sc2_irq.asm:22` (no comment); `nh_base` — `/dd/SOURCECODE/ASM/NITROS9/RBF/cc3disk_sc2_slp.asm:22` (no comment); `RW.Dat` — `/dd/SOURCECODE/ASM/NITROS9/RBF/rb1773.asm:116` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/RBF/cc3disk_disto.asm`: 48 | RW.Dat   equ   $ff74 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_sc2_irq.asm`: 22 | nh_base  equ   $FF74 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_sc2_slp.asm`: 22 | nh_base  equ   $FF74 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/rb1773.asm`: 116 | RW.Dat   equ   $FF74 | Contains extended-instruction candidates; inspect guards |

### $FF76

Source symbols: `RW.Ctrl` — `/dd/SOURCECODE/ASM/NITROS9/RBF/cc3disk_disto.asm:49` (no comment); `RW.Ctrl` — `/dd/SOURCECODE/ASM/NITROS9/RBF/rb1773.asm:117` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/RBF/cc3disk_disto.asm`: 49 | RW.Ctrl  equ   $ff76 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/rb1773.asm`: 117 | RW.Ctrl  equ   $FF76 | Contains extended-instruction candidates; inspect guards |

### $FF7A

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/PLAY/mulaw_alaw.a`: 39 | sta   >$ff7a        5/4 Send voltage to left channel of ORCH90 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/PLAY/play.a`: 1113,1138,1163,1190 | sta   >$ff7a        5/4 Send voltage to left channel of ORCH90 | No scanned extended-instruction site; not proof of 6809-only |

### $FF7B

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/PLAY/mulaw_alaw.a`: 40 | sta   >$ff7b        5/4 Send voltage to right channel of ORCH90 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/PLAY/play.a`: 1114,1139 | sta   >$ff7b        5/4 Send voltage to right channel of ORCH90 | No scanned extended-instruction site; not proof of 6809-only |

### $FF7C

Source symbols: `RTC.Base` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_ds1315.asm:39` (Fully decoded RTC)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_ds1315.asm`: 39 | RTC.Base equ   $FF7C      Fully decoded RTC | No scanned extended-instruction site; not proof of 6809-only |

### $FF7D

Source symbols: `PortAddr` — `/dd/SOURCECODE/ASM/NITROS9/SCF/ssp.asm:22` (Speech-Sound Pak base address)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/SCF/ssp.asm`: 22 | PortAddr equ   $FF7D      Speech-Sound Pak base address | No scanned extended-instruction site; not proof of 6809-only |

### $FF7F

Source symbols: `MPI.Slct` — `/dd/DEFS/coco.d:97` (Multi-Pak slot select); `lda` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_smart.asm:102` (no comment); `sta` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_smart.asm:105` (no comment); `lda` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_smart.asm:119` (no comment); `sta` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_smart.asm:122` (no comment); `stb` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_smart.asm:242` (no comment); `sta` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_smart.asm:300` (restore MPI); `ldb` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_smart.asm:380` (get MPI values)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_smart.asm`: 102,105,119,122,242,300,380 | lda   $FF7F | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/llcocosdc.asm`: 33 | MPIREG         equ       $FF7F | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/llcocosdc_beta6.asm`: 33 | MPIREG   equ   $FF7F | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/llcocosdc_ver101.asm`: 42 | MPIREG         equ       $FF7F              Address of MPI slot select register | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/boot_wd1002.asm`: 116 | cmpy  #$FF7F     base address too high? | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_disto.asm`: 50 | MPICtrl  equ   $ff7f | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_sc2_irq.asm`: 113,278,281,304 | lda   >$FF7F | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/SCF/s16550v16_lOS9.asm`: 186 | sta   >$FF7F | No scanned extended-instruction site; not proof of 6809-only |
| `C/SDCCMDR/commsdc.h`: 17 | #define MPIREG   0xff7f | No scanned extended-instruction site; not proof of 6809-only |

### $FF80

Source symbols: `RTC.base` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_coco3fpga.asm:45` (no comment); `dataport` — `/dd/SOURCECODE/ASM/NITROS9/KERNEL/boot_vhd.asm:27` (no comment); `LSN` — `/dd/SOURCECODE/ASM/NITROS9/RBF/emudsk_beta601.asm:35` (where to put the logical sector number)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_coco3fpga.asm`: 45 | RTC.base equ   $FF80 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/DW/dwread.asm`: 208 | ldd       #$ff80              ; A = timeout msb, B = shift counter | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/llcocosdc.asm`: 528 | ldx       #$FF80             mask/flag bytes | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/llcocosdc_beta6.asm`: 509 | ldx   #$FF80       mask/flag bytes | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/llcocosdc_ver101.asm`: 545 | ldx       #$FF80             mask/flag bytes | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/boot_vhd.asm`: 27 | dataport equ   $FF80 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dwread.asm`: 134 | ldd       #$ff80              ; A = timeout msb, B = shift counter | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/RBF/emudsk_beta601.asm`: 35 | LSN      equ  $FF80           where to put the logical sector number | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/RBF/rammer_ed6_8-64mb.asm`: 164,184 | sta   >$ff80         Nocan64; map ram drive block into MMU block #0 | Contains extended-instruction candidates; inspect guards |

### $FF81

Source symbols: `RTC.data` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_coco3fpga.asm:46` (data I/O)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_coco3fpga.asm`: 46 | RTC.data equ   $FF81      data I/O | No scanned extended-instruction site; not proof of 6809-only |

### $FF82

Source symbols: `RTC.cmd` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_coco3fpga.asm:47` ($D1=read, $D0=write)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_coco3fpga.asm`: 47 | RTC.cmd  equ   $FF82      $D1=read, $D0=write | No scanned extended-instruction site; not proof of 6809-only |

### $FF83

Source symbols: `RTC.adr` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_coco3fpga.asm:48` (indicates 00h-06h see above); `command` — `/dd/SOURCECODE/ASM/NITROS9/RBF/emudsk_beta601.asm:41` (where to put the commands)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_coco3fpga.asm`: 48 | RTC.adr  equ   $FF83      indicates 00h-06h see above | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/emudsk_beta601.asm`: 41 | command  equ  $FF83           where to put the commands | Contains extended-instruction candidates; inspect guards |

### $FF84

Source symbols: `buffer` — `/dd/SOURCECODE/ASM/NITROS9/RBF/emudsk_beta601.asm:46` (pointer to the buffer)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/RBF/emudsk_beta601.asm`: 46 | buffer   equ  $FF84           pointer to the buffer | Contains extended-instruction candidates; inspect guards |

### $FF86

Source symbols: `vhdnum` — `/dd/SOURCECODE/ASM/NITROS9/RBF/emudsk_beta601.asm:49` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/RBF/emudsk_beta601.asm`: 49 | vhdnum   equ  $FF86 | Contains extended-instruction candidates; inspect guards |

### $FF90

Source symbols: `sta` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_smart.asm:166` (no comment); `sta` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_smart.asm:248` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_smart.asm`: 166,248 | sta   $FF90 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/ccbkrn.txt`: 125 | starting at 0xff90: | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/KERNEL/fdebug.asm`: 57 | sta   >$FF90 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/SCF/covdg_beta6.asm`: 699,2057 | stb   >$FF90       And to actual GIME | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta61.asm`: 732,2091 | stb   >$FF90       And to actual GIME | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_ver100.asm`: 720,2088 | stb   >$FF90       And to actual GIME | Contains extended-instruction candidates; inspect guards |

### $FF91

Source symbols: `DAT.Task` — `/dd/DEFS/coco.d:168` (Task Register address)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/GIMEX/NEW/rel.asm`: 535 | L003F    clr   >$FF91     go to map type 0 - called by CC3Go from map 1 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel.asm`: 535 | L003F    clr   >$FF91     go to map type 0 - called by CC3Go from map 1 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel_beta6.asm`: 519 | L003F    clr   >$FF91       go to map type 0 - called by CC3Go from map 1 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/fdebug.asm`: 58 | clr   >$FF91       go to map type 0 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/KERNEL/rel.asm`: 266 | L003F    clr   >$FF91     go to map type 0 - called by CC3Go from map 1 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel_beta6.asm`: 519 | L003F    clr   >$FF91       go to map type 0 - called by CC3Go from map 1 | Contains extended-instruction candidates; inspect guards |

### $FF92

Source symbols: `IrqEnR` — `/dd/DEFS/coco.d:202` (GIME IRQ enable/status register)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/DW/dw4read.asm`: 39,40,123,124,167,168,259,260 | stx	<$FF92		; disable GIME interrupts | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/MODS/dw4read.asm`: 39,40,123,124,167,168,259,260 | stx	<$FF92		; disable GIME interrupts | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/RBF/cc3disk_sc2_irq.asm`: 129,291,293 | sta   >$FF92 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/SCF/covdg_beta61.asm`: 164,175,177,184,190,191 | sta   >IrqEnR        $FF92 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/s16550v16_lOS9.asm`: 194 | sta   >$FF92 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/SCF/sc6551.asm`: 41,43 | IRQBit     equ   %00000100      GIME IRQ bit to use for IRQ ($FF92) | Contains extended-instruction candidates; inspect guards |

### $FF98

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock.asm`: 457 | stb   >$FF98     set 50 Hz VSYNC | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta6.asm`: 702,2062 | stb   >$FF98       Save onto actual GIME | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta61.asm`: 735,2096 | stb   >$FF98       Save onto actual GIME | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_ver100.asm`: 723,2093 | stb   >$FF98       Save onto actual GIME | Contains extended-instruction candidates; inspect guards |

### $FF99

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/SCF/covdg_beta6.asm`: 711,2052 | std   >$FF99       set resolution AND border color (to black) | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta61.asm`: 744,2086 | std   >$FF99       set resolution AND border color (to black) | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_ver100.asm`: 732,2083 | std   >$FF99       set resolution AND border color (to black) | Contains extended-instruction candidates; inspect guards |

### $FF9B

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/RBF/rammer_ed6_8-64mb.asm`: 106,167,188 | lda >$9B           save default $FF9B image for future use | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta6.asm`: 733,2073 | sta   >$FF9B       Select 512K video bank for >512K machines | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta61.asm`: 766,2107 | sta   >$FF9B       Select 512K video bank for >512K machines | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_ver100.asm`: 754,2104 | sta   >$FF9B       Select 512K video bank for >512K machines | Contains extended-instruction candidates; inspect guards |

### $FF9C

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/SCF/covdg_beta6.asm`: 741,2080 | sta   >$FF9C       And to actual GIME | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta61.asm`: 774,2114 | sta   >$FF9C       And to actual GIME | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_ver100.asm`: 762,2111 | sta   >$FF9C       And to actual GIME | Contains extended-instruction candidates; inspect guards |

### $FF9D

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/GLIB/GLIB/USSWAP.a`: 27 | stb   >$FF9D     and select it MYSELF! | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/SCF/covdg_beta6.asm`: 738,2078 | std   >$FF9D       And to actual GIME | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta61.asm`: 771,2112 | std   >$FF9D       And to actual GIME | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_ver100.asm`: 759,2109 | std   >$FF9D       And to actual GIME | Contains extended-instruction candidates; inspect guards |

### $FFA0

Source symbols: `DAT.Regs` — `/dd/DEFS/coco.d:170` (DAT Block Registers base address)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm`: 126,130 | stb	>$FFA0		map the boot screen into block 0 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/fdebug.asm`: 25,28,50 | stb   >$FFA0       map in block 0 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/KERNEL/rel.asm`: 109,236,240 | clr   >$FFA0     map in block 0 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/RBF/rammer_ed6_8-64mb.asm`: 172,193 | stb   >$ffa0 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/RBF/sdisk3_dmc.asm`: 1157,1196 | stb   >$FFA0 | No scanned extended-instruction site; not proof of 6809-only |

### $FFA1

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/RBF/rb1773.asm`: 777,820 | sta   >$FFA1          otherwise map the block in | Contains extended-instruction candidates; inspect guards |

### $FFA4

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/GIMEX/NEW/rel.asm`: 408 | sta   >$FFA4     map in the block | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel.asm`: 409 | sta   >$FFA4     map in the block | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel_beta6.asm`: 380 | sta   >$FFA4       map in the block | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel.asm`: 139 | sta   >$FFA4     map in the block | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel_beta6.asm`: 380 | sta   >$FFA4       map in the block | Contains extended-instruction candidates; inspect guards |

### $FFA5

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/KERNEL/ccbkrn.asm`: 331,341,687,701 | stb	>$FFA5		Map block into block 6 of my task | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/fmove_old.asm`: 93,95,161,189 | std   >$FFA5       map in the blocks | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/krn.asm`: 276,286,581,595 | stb    >$FFA5        Map block into block 6 of my task | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm`: 270,280,570,584 | stb   >$FFA5      Map block into block 6 of my task | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/krnp2.asm`: 160,174 | std    >$FFA5      map in the blocks | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm`: 160,174 | std    >$FFA5      map in the blocks | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm`: 161,175 | std    >$FFA5      map in the blocks | Contains extended-instruction candidates; inspect guards |

### $FFA6

Source symbols: `ldb` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_smart.asm:168` (choose to use normal location); `stb` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_smart.asm:171` (reset MMU for clock); `sta` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_smart.asm:252` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_smart.asm`: 168,171,252 | ldb   $FFA6       choose to use normal location | Contains extended-instruction candidates; inspect guards |

### $FFA8

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/GLIB/GLIB/UIPFLIP.a`: 42 | ldx   #$FFA8 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/GLIB/GLIB/UMAPSC.a`: 14,28 | ldx   #$FFA8     to GIME memory | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/KERNEL/ccbkrn.asm`: 961 | ldx	#$FFA8		get MMU start register for process's | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/krn.asm`: 851 | ldx    #$FFA8        get MMU start register for process's | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm`: 808 | ldx   #$FFA8      get MMU start register for process's | Contains extended-instruction candidates; inspect guards |

### $FFA9

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/PLAY/mulaw_alaw.a`: 25,50 | stb   >$FFA9        Forcefully map in new block of RAM into MMU #2 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/PLAY/play.a`: 970,998,1026,1060,1099,1121,1149,1170,1200 | stb   >$FFA9        Forcefully map in new block of RAM into MMU #2 | No scanned extended-instruction site; not proof of 6809-only |

### $FFB0

Source symbols: `PalAdr` — `/dd/DEFS/coco.d:204` (Palette registers)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/GIMEX/NEW/rel.asm`: 406 | std   >$FFB0     set only the first two palettes, B=$00 already | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel.asm`: 407 | std   >$FFB0     set only the first two palettes, B=$00 already | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel_beta6.asm`: 378 | std   >$FFB0       set only the first two palettes, B=$00 already | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel.asm`: 137 | std   >$FFB0     set only the first two palettes, B=$00 already | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel_beta6.asm`: 378 | std   >$FFB0       set only the first two palettes, B=$00 already | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta6.asm`: 172 | ldy   #$FFB0         point Y to palette register | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta61.asm`: 201 | ldy   #$FFB0         point Y to palette register | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_ver100.asm`: 192 | ldy   #$FFB0         point Y to palette register | Contains extended-instruction candidates; inspect guards |

### $FFC0

Source symbols: `A.TermV` — `/dd/DEFS/coco.d:106` (VDG Term); `RTC.Base` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_jvemu.asm:31` (no comment); `clr` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/rel.asm:613` (Reset to text mode if Dragon Alpha); `clr` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/rel_beta6.asm:616` (Reset to text mode if Dragon Alpha); `clr` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/rel.asm:627` (Reset to text mode if Dragon Alpha); `clr` — `/dd/SOURCECODE/ASM/NITROS9/KERNEL/rel.asm:298` (* Reset to text mode if Dragon Alpha); `clr` — `/dd/SOURCECODE/ASM/NITROS9/KERNEL/rel_beta6.asm:616` (Reset to text mode if Dragon Alpha)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_jvemu.asm`: 31 | RTC.Base equ   $FFC0 | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/GIMEX/NEW/rel.asm`: 627 | clr    $ffc0                Reset to text mode if Dragon Alpha | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel.asm`: 613 | clr    $ffc0                Reset to text mode if Dragon Alpha | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel_beta6.asm`: 616 | clr    $ffc0                Reset to text mode if Dragon Alpha | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel.asm`: 298 | clr    $ffc0       * Reset to text mode if Dragon Alpha | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel_beta6.asm`: 616 | clr    $ffc0                Reset to text mode if Dragon Alpha | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta6.asm`: 682,690 | stb   -6,y         $FFC0 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta61.asm`: 715,723 | stb   -6,y         $FFC0 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_ver100.asm`: 703,711 | stb   -6,y         $FFC0 | Contains extended-instruction candidates; inspect guards |

### $FFC2

Source symbols: `A.V2` — `/dd/DEFS/coco.d:108` (no comment); `clr` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/rel.asm:614` (no comment); `clr` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/rel_beta6.asm:617` (no comment); `clr` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/rel.asm:628` (no comment); `clr` — `/dd/SOURCECODE/ASM/NITROS9/KERNEL/rel.asm:299` (no comment); `clr` — `/dd/SOURCECODE/ASM/NITROS9/KERNEL/rel_beta6.asm:617` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/GIMEX/NEW/rel.asm`: 628 | clr    $ffc2 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel.asm`: 614 | clr    $ffc2 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel_beta6.asm`: 617 | clr    $ffc2 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel.asm`: 299 | clr    $ffc2 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel_beta6.asm`: 617 | clr    $ffc2 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta6.asm`: 683 | stb   -4,y         $FFC2 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta61.asm`: 716 | stb   -4,y         $FFC2 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_ver100.asm`: 704 | stb   -4,y         $FFC2 | Contains extended-instruction candidates; inspect guards |

### $FFC3

Source symbols: `A.V3` — `/dd/DEFS/coco.d:109` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/SCF/covdg_beta6.asm`: 691 | stb   -3,y         $FFC3 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta61.asm`: 724 | stb   -3,y         $FFC3 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_ver100.asm`: 712 | stb   -3,y         $FFC3 | Contains extended-instruction candidates; inspect guards |

### $FFC4

Source symbols: `A.V4` — `/dd/DEFS/coco.d:110` (no comment); `clr` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/rel.asm:615` (no comment); `clr` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/rel_beta6.asm:618` (no comment); `clr` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/rel.asm:629` (no comment); `clr` — `/dd/SOURCECODE/ASM/NITROS9/KERNEL/rel.asm:300` (no comment); `clr` — `/dd/SOURCECODE/ASM/NITROS9/KERNEL/rel_beta6.asm:618` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/GIMEX/NEW/rel.asm`: 629 | clr    $ffc4 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel.asm`: 615 | clr    $ffc4 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel_beta6.asm`: 618 | clr    $ffc4 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel.asm`: 300 | clr    $ffc4 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel_beta6.asm`: 618 | clr    $ffc4 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta6.asm`: 684 | stb   -2,y         $FFC4 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta61.asm`: 717 | stb   -2,y         $FFC4 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_ver100.asm`: 705 | stb   -2,y         $FFC4 | Contains extended-instruction candidates; inspect guards |

### $FFC5

Source symbols: `A.V5` — `/dd/DEFS/coco.d:111` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/SCF/covdg_beta6.asm`: 692 | stb   -1,y         $FFC5 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta61.asm`: 725 | stb   -1,y         $FFC5 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_ver100.asm`: 713 | stb   -1,y         $FFC5 | Contains extended-instruction candidates; inspect guards |

### $FFC6

Source symbols: `A.V6` — `/dd/DEFS/coco.d:112` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/GIMEX/NEW/rel.asm`: 620 | ldx   #$FFC6 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel.asm`: 606 | ldx   #$FFC6 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel_beta6.asm`: 609 | ldx   #$FFC6 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel.asm`: 291 | ldx   #$FFC6 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel_beta6.asm`: 609 | ldx   #$FFC6 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta6.asm`: 676,750 | ldy   #$FFC6       Ok, now set up via old CoCo 2 mode the graphics video mode. | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta61.asm`: 709,783 | ldy   #$FFC6       Ok, now set up via old CoCo 2 mode the graphics video mode. | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_ver100.asm`: 697,771 | ldy   #$FFC6       Ok, now set up via old CoCo 2 mode the graphics video mode. | Contains extended-instruction candidates; inspect guards |

### $FFD8

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_disto2.asm`: 82 | clr   >$FFD8     1 MHz | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/KERNEL/fdebug.asm`: 45 | clr   >$FFD8       go to low speed | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/rb1773.asm`: 808 | sta   >$FFD8 | Contains extended-instruction candidates; inspect guards |

### $FFD9

Source symbols: `sta` — `/dd/SOURCECODE/ASM/NITROS9/KERNEL/boot_vhd.asm:59` (fast clock)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_disto2.asm`: 58 | RTCPost  clr   >$FFD9     2 MHz  (Really should check $A0 first) | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/GIMEX/NEW/boot_sdc.asm`: 161,188,190 | clr       >$FFD9             drop GimeX to 1.89mhz | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/GIMEX/NEW/rel.asm`: 383,386 | stb   >$FFD9     set to high speed | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/boot_sdc.asm`: 161,188,190 | clr       >$FFD9             drop GimeX to 1.89mhz | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/GIMEX/boot_sdc_beta6.asm`: 153,180,182 | clr   >$FFD9       drop GimeX to 1.89mhz | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/GIMEX/llcocosdc.asm`: 269,384,386 | clr       >$ffd9 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/llcocosdc_beta6.asm`: 260,371,373 | clr   >$ffd9       Force to 1.78 MHz | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/llcocosdc_ver101.asm`: 282,398,400 | clr       >$ffd9             Drop to 1.78MHz (SDC can't handle 2.86MHz) | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel.asm`: 383,387 | stb   >$FFD9     set to high speed (1.78 Mhz) | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel_beta6.asm`: 355,358 | stb   >$FFD9       set to high speed | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/boot_burke.asm`: 137 | sta   >$FFD9         Double speed on | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/KERNEL/boot_vhd.asm`: 59 | sta   $FFD9      fast clock | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/KERNEL/rel.asm`: 122 | stb   >$FFD9     set to high speed | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel_beta6.asm`: 355,358 | stb   >$FFD9       set to high speed | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/RBF/rb1773.asm`: 817 | sta   >$FFD9 | Contains extended-instruction candidates; inspect guards |

### $FFDE

Source symbols: `sta` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_smart.asm:172` (no comment)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_smart.asm`: 172 | sta   $FFDE | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/fdebug.asm`: 59 | clr   >$FFDE       and to all-ROM mode | No scanned extended-instruction site; not proof of 6809-only |

### $FFDF

Source symbols: `sta` — `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_smart.asm:250` (no comment); `sta` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/rel.asm:603` (turn off ROM); `sta` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/rel_beta6.asm:606` (turn off ROM); `sta` — `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/rel.asm:617` (turn off ROM); `sta` — `/dd/SOURCECODE/ASM/NITROS9/KERNEL/rel.asm:288` (turn off ROM); `sta` — `/dd/SOURCECODE/ASM/NITROS9/KERNEL/rel_beta6.asm:606` (turn off ROM)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/CLOCKS/clock2_smart.asm`: 250 | sta   $FFDF | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/NEW/rel.asm`: 279,345,544,617 | clr   >$FFDF     go to all RAM mode | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel.asm`: 279,345,544,603 | clr   >$FFDF     go to all RAM mode | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/rel_beta6.asm`: 258,317,528,606 | clr   >$FFDF       go to all RAM mode | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel.asm`: 98,275,288 | clr   >$FFDF     added for OS-9 ROM Kit boots +BGP+ | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/rel_beta6.asm`: 258,317,528,606 | clr   >$FFDF       go to all RAM mode | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/MODS/sysgo.asm`: 115 | sta   >$FFDF           turn off ROM mode | No scanned extended-instruction site; not proof of 6809-only |

### $FFE0

Source symbols: `ldb` — `/dd/SOURCECODE/ASM/NITROS9/MODS/dwrdmess.asm:28` (; read data value); `stb` — `/dd/SOURCECODE/ASM/NITROS9/MODS/dwwrmess.asm:22` (; write it to the FIFO); `fdb` — `/dd/SOURCECODE/ASM/NITROS9/RBF/md.asm:25` (physical controller address); `fdb` — `/dd/SOURCECODE/ASM/NITROS9/RBF/r0.asm:28` (physical controller address)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/DW/dwio.asm`: 88 | jmp       [$FFE0] | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dwio.asm`: 71 | jmp       [$FFE0] | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dwrdmess.asm`: 28 | ldb       $ffe0               ; read data value | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dwwrmess.asm`: 22 | stb       $ffe0               ; write it to the FIFO | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/md.asm`: 25 | fdb   $FFE0      physical controller address | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/r0.asm`: 28 | fdb   $FFE0      physical controller address | No scanned extended-instruction site; not proof of 6809-only |

### $FFE1

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/GIMEX/gimexcheck.a`: 45 | std   >$ffe1       Save to upper 16 bits of DMA address | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dwrdmess.asm`: 26 | rxByte   ldb       $ffe1               ; check for data in FIFO | No scanned extended-instruction site; not proof of 6809-only |

### $FFE2

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/DW/dwio.asm`: 95 | jmp       [$FFE2] | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/dwio.asm`: 78 | jmp       [$FFE2] | No scanned extended-instruction site; not proof of 6809-only |

### $FFE3

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/GIMEX/gimexcheck.a`: 46 | clr   >$ffe3       Clear out lower 8 bits of DMA address | No scanned extended-instruction site; not proof of 6809-only |

### $FFE8

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/GIMEX/gimexcheck.a`: 48 | sta   >$ffe8       Send to DMA (will copy to DMAByte) | No scanned extended-instruction site; not proof of 6809-only |

### $FFEF

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/GIMEX/gimexcheck.a`: 63 | lda   >$ffef       Get GIME-X version number | No scanned extended-instruction site; not proof of 6809-only |

### $FFF0

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `C/RDUMP/6309dasm.c`: 774 | buffer += sprintf (buffer, "illegal postbyte (jmp [$FFF0])" ); | No scanned extended-instruction site; not proof of 6809-only |

### $FFF3

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/SCF/s16550v16_lOS9.asm`: 692 | addd  >$FFF3 | No scanned extended-instruction site; not proof of 6809-only |

### $FFFE

Source symbols: none established in inspected definitions; identity unresolved.

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/NITROS9/KERNEL/fdebug.asm`: 67 | Reset    jmp   [$FFFE]    do a reset | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/MODS/ioman_beta5.asm`: 126 | jmp   [>$FFFE] | Contains extended-instruction candidates; inspect guards |
| `C/MAMOU/h6309.c`: 1096,1242 | /* Indrect mode (i.e. [$FFFE]) */ | Contains extended-instruction candidates; inspect guards |

### $FFFF

Source symbols: `AMASK` — `/dd/SOURCECODE/C/RDUMP/6309dasm.c:39` (;)

| Source / sites | Source purpose / instruction | CPU relevance |
|---|---|---|
| `ASM/BASIC09/basic09_ver100.asm`: 2146,2504,2512,7355,8063,8070,8109 | ldd   #$FFFF     Init CRC to $FF's | Contains extended-instruction candidates; inspect guards |
| `ASM/BASIC09/basic09_ver101.asm`: 2171,2532,2540,6585,7216,7223,7262 | ldd   #$FFFF     Init CRC to $FF's | Contains extended-instruction candidates; inspect guards |
| `ASM/BASIC09/runb_beta6.asm`: 444,451 | ldd   #$FFFF         Flag module as unused in runb module directory | Contains extended-instruction candidates; inspect guards |
| `ASM/BASIC09/runb_ver100.asm`: 451,458 | ldd   #$FFFF         Flag module as unused in runb module directory | Contains extended-instruction candidates; inspect guards |
| `ASM/BASIC09/runb_ver101.asm`: 458,465 | ldd   #$FFFF         Flag module as unused in runb module directory | Contains extended-instruction candidates; inspect guards |
| `ASM/BASIC09/runbcd.asm`: 246,1910,1914,1968,1991 | ldx   #$FFFF         init links | Contains extended-instruction candidates; inspect guards |
| `ASM/GSHELL/gshell101_prehelp.asm`: 7449 | ldd   #$ffff     Defaults for keyboard & mouse stuff | Contains extended-instruction candidates; inspect guards |
| `ASM/GSHELL/gshell_beta6.asm`: 6616 | ldd   #$ffff     Defaults for keyboard & mouse stuff | Contains extended-instruction candidates; inspect guards |
| `ASM/GSHELL/gshell_ver1_0_0.asm`: 6404 | ldd   #$ffff     Defaults for keyboard & mouse stuff | Contains extended-instruction candidates; inspect guards |
| `ASM/GSHELL/gshell_ver1_0_1.asm`: 6462 | ldd   #$ffff     Defaults for keyboard & mouse stuff | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/CLOCKS/clock.asm`: 226 | endalarm ldd   #$FFFF | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/DW/scdwv.asm`: 547 | open           tst       <V.PORT+1,u         check if this is $FFFF (wildcard) | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/llcocosdc.asm`: 124,347,530 | ldu       #$FFFF             I/O buffer = none | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/llcocosdc_beta6.asm`: 121,333,511 | ldu   #$FFFF       I/O buffer = none | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/GIMEX/llcocosdc_ver101.asm`: 134,356,547 | ldu       #$FFFF             I/O buffer = none | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/boot_1773.asm`: 214 | ldx   #$FFFF | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/KERNEL/boot_burke.asm`: 146 | ldx   #$FFFF         (Init X so it will be 0 in loop) | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/KERNEL/fdebug.asm`: 60 | ldd   #$FFFF | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/KERNEL/ffreehb.asm`: 28 | ldd   #$FFFF         -1' | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/flink.asm`: 120 | beq    L03FC       If wraps to 0, leave at $FFFF | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/KERNEL/fvmodul.asm`: 297 | ldd   #$FFFF       initial CRC value of $FFFFFF | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/MODS/ioman_beta5.asm`: 421 | ldd   #$FFFF         Init CRC on stack | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/RBF/cc3disk_disto.asm`: 561 | ldy   #$ffff | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_sc2_irq.asm`: 507 | ldy   #$FFFF | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/cc3disk_sc2_slp.asm`: 464 | ldy   #$FFFF | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm`: 228,1421,1423,1424,1435,1435,2195,2316,2522,2703 | ldd   #$FFFF       force it to 64k | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/RBF/sdisk3_dmc.asm`: 1337,1365 | cmpx  #$FFFF | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/SCF/covdg_beta6.asm`: 1057 | ldd   #$FFFF       If 0, init ?? to -1 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_beta61.asm`: 1090 | ldd   #$FFFF       If 0, init ?? to -1 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/covdg_ver100.asm`: 1083 | ldd   #-1          If vertical line, change 4,s to $FFFF (-1 / -1) | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/cowin_beta6.asm`: 264,842,984,989,1469,4381,5019 | ldu   #$FFFF | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/scbbt.asm`: 145 | ldy   #$FFFF | No scanned extended-instruction site; not proof of 6809-only |
| `ASM/NITROS9/SCF/scf_beta6.asm`: 859 | ldd   #$FFFF       Get maximum character count | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/scf_ver100.asm`: 865 | ldd   #$FFFF       Get maximum character count | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/vtio_beta6.asm`: 196,1834 | ldd   #$FFFF       initialize last keyboard code to $FF, and key repeat counter to -1 | Contains extended-instruction candidates; inspect guards |
| `ASM/NITROS9/SCF/wordpakii.asm`: 274 | L01A7    ldd   #$FFFF | No scanned extended-instruction site; not proof of 6809-only |
| `C/MAMOU/pseudo.c`: 1790 | if (result > 0xFFFF && as->line.force_byte == 0) | No scanned extended-instruction site; not proof of 6809-only |
| `C/MAMOU/util.c`: 623,636 | return i & 0xFFFF; | No scanned extended-instruction site; not proof of 6809-only |
| `C/RDUMP/6309dasm.c`: 39,791,801,854,874,907,911,915,919,923,927,942 | #define AMASK 0xffff; | No scanned extended-instruction site; not proof of 6809-only |
| `C/RMA/part3.c`: 221,222,929 | storInt(opr_ptr, (nmbr_int >> 16) & 0xffff);    /* save 16 MSB */ | No scanned extended-instruction site; not proof of 6809-only |
| `C/SDCCMDR/commsdc.c`: 215 | if (r & READY && buf != 0xffff) | No scanned extended-instruction site; not proof of 6809-only |
| `C/SDCCMDR/libsdc.c`: 250 | return CommSDC(0xc0, 0x2b, 0, 0, (char *)0xffff); | No scanned extended-instruction site; not proof of 6809-only |

## Symbol-only and indirect routes

- Follow `DAT.Regs`, `DAT.Task`, `P$DATImg` in `ASM/NITROS9/KERNEL/fmapblk.asm`, `fdatlog.asm` and relocation sources; some mapping operations use OS services rather than literal I/O addresses.
- Follow `RTC.Base` in CLOCKS, `MPIREG`/`CTRLATCH` in C/SDCCMDR, `BBIN`/`BBOUT` in DW, and register offsets relative to base pointers in VTIO/CoWin.
- `$FFFF` and nearby constants frequently serve as masks/sentinels or vectors. This index preserves them as candidates rather than inventing peripheral names. Range lengths and extended/GIMEX mappings require a manual/upstream follow-up.
