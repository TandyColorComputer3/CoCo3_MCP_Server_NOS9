# Wizard source provenance

Reference (read-only): `/Volumes/SEDONA/Projects/daggorath-reference`, commit
`a94326f00ebb16a106b540c58bc2ccf5f7b66dac`. Original copyright: Dyna Micro,
MCMLXXXII. The reconstructed repository includes original listings, recovered
macros and `grant_of_license.png` (Douglas J. Morgan's 2002 grant to Michael J.
Spencer Jr.). This port preserves attribution; it does not assert a blanket new
license or independently verified transcription of every original listing.

`import_data.py` reads only the selected data from that commit. It records full
input hashes and per-segment source line/label locations in
`src/original/provenance.json`. It never modifies the reference checkout.

| Port material | Original source/labels | Adaptation |
|---|---|---|
| `original/data.h` 84 segments | `D4.ASM` WIZ1 crescent then WIZ0 body; `missing-macros.asm` SVORG/SVECT/SVNEW/SVEND; `VCTLST.ASM` VCTLSX/VCTREL/VCTJMP | Expand the selected unity-scale list into ordered absolute endpoint pairs. No runtime absolute pointers, other creatures or general vector interpreter. Signed two-pixel deltas checked during import. |
| `original/logical.c` line raster | `VECTOR.ASM` VECTOR, INCRE, DIVIDE, VECT30–60; BITMSK; `COMTXT.ASM` LSLD5 | Preserve fixed-point step division, half-pixel rounding, endpoint exclusion, per-line dotted fade and 32-byte stride; replace machine instructions with C operating only on private memory. Selected coordinates fit the accumulator range. |
| Frame regions | `COMDAT.ASM` DSP0/DSP1, TXTSTS, TXTPRI, STSVDB, PRIVDB; `CLEAR.ASM` ZFLOPX/CLRSTX/CLRPRX | One private 6144-byte frame instead of original physical double buffers. Rebuild status/messages consistently per frame. |
| Packed font and messages | `SWCHAR.ASM` SWCTAB; `EXPAND.ASM` GETFIV/EXPA10; `COMTXT.ASM` NDPB10/TXTDPB/TXTCR; `TXTSER.ASM` TXTSTI/TXTSTR; `ONCE.ASM` COMINI copyright and DEMO10 two OUTSTI strings | Retain 31 packed glyphs and decode original message bytes, including CRs and original spelling. No replacement host font. |
| `main.c` sequence | `ONCE.ASM` DEMO10 before GAME20; `MISC.ASM` WIZIX0/WIZI10/WIZI20/WIZOX/WIZO10/WIZZES/WAITX | 32..0 by -2; messages; 81+81 tick wait; clear messages; 0..30 by +2; blank vector area; exit instead of gameplay. |
| Sound hooks | `COMMON.ASM` CLK20 NOISEV buzz; `MISC.ASM` WIZI20; `SOUNDS.ASM` KABOOM/BOOMER/SNOUT/SNWAIT; `SWCHAR.ASM` THUDD | Event boundaries retained, silent backend. No PIA writes/private IRQ/busy-loop audio copied. |

OS-9 presentation derives from the verified `apps/graphics-probe` lifecycle and
`apps/window-manager` CMOC syscall wrappers. Upstream
`nitros9-reference` commit `f470fa52eb172b59b22c1b722074998cb42de9b1`:
`level2/coco3/modules/cowin.asm` public escape dispatch / GPLoad, and
`level2/cmds/grfdrv.asm` L08E1 (exclusive DefGPB), L0B3F (GPLoad), L0CBB
(PutBlk), L086A.25 (640×200). Cursor off is CoWin L0396 / GrfDrv $3E with $20.

Logical code contains no MMU, screen addresses, ROM calls, OS-9 syscalls, sound
hardware or physical viewport translation. The presentation owns the (+192,+4)
offset and its reserved buffer. Allocation failure is fatal and does not replace
or free another application's buffer.
