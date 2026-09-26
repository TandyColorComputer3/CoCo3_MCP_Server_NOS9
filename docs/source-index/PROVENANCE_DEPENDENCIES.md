# Provenance, revisions and dependencies

[Navigation](README.md) · [Runtime matches](RUNTIME_MATCHES.md)

## Twenty priority collections

Classifications describe evidence in source headers/build files, not authenticated ownership or license grants. “System source” is lineage/function, not an assertion of current upstream authority. EOU-specific edits can coexist with historical or third-party code.

| Collection | Supported provenance | Anchor | Revision / limits |
|---|---|---|---|
| KERNEL | NitrOS-9 system source; multiple EOU-era variants | `/dd/SOURCECODE/ASM/NITROS9/KERNEL/krnp2_ver101.asm` | Header/source declaration and adjacent binary verified for KrnP2; krn, Beta5, rel and GIMEX variants are not interchangeable. |
| MODS | NitrOS-9 system / derived historical source with revisions | `/dd/SOURCECODE/ASM/NITROS9/MODS/ioman_beta5.asm` | Disassembly credit; edition 13/revision 6 agrees with adjacent binary and live fingerprint. llcocosdc is a separate SDC implementation. |
| SCF/VTIO/CoWin | NitrOS-9 system with explicit EOU modifications | `/dd/SOURCECODE/ASM/NITROS9/SCF/vtio_beta6.asm` | EOU Beta 5/6 history and edition 4; CoWin edition 2 and SCF ver100 edition 18 have matching binaries. |
| RBF | NitrOS-9 system; post-beta revisions | `/dd/SOURCECODE/ASM/NITROS9/RBF/rbf_postbeta6.asm` | Edition 37/revision 3 matches live header metadata, not a rebuilt source match. EmuDsk beta601 binary is matched. |
| PIPE | NitrOS-9 system / historical rewritten-backported source | `/dd/SOURCECODE/ASM/NITROS9/PIPE/pipeman_beta6.asm` | Alan DeKok rewrite; Boisy Pitre 2003 backport; Curtis Boyle 2020 optimization; edition 5/revision 1 matched artifact. |
| DW | NitrOS-9 system / third-party transport work | `/dd/SOURCECODE/ASM/NITROS9/DW/dwread.asm` | Darren Atkinson 2009 attribution and conditional implementations. No live driver match or protocol certification. |
| DWNET + FTP | Third-party libraries/application | `/dd/SOURCECODE/ASM/DWNET/dwnet.a` | Boisy Pitre 2010 attribution; FTP/main.a credits Bill Nobel. No precise released-version or runtime match established. |
| Shell | NitrOS-9 Shell+ / derived disassembly | `/dd/SOURCECODE/ASM/SHELL/shellplus2.2a.asm` | Curtis Boyle changes from 2.2 disassembly; shell21.a and shell.orig are separate older evidence. Current binary edition 23 matched. |
| BASIC09 implementation | Historical Microware/Motorola; Tandy distribution-derived helpers; later NitrOS-9 modifications | `/dd/SOURCECODE/ASM/BASIC09/basic09_ver101.asm` | Copyright 1980 in header; gfx2_ver1.asm records Tandy distribution and later edits. Basic09/RunB disk packs edition 25; no source rebuild. |
| GShell | Historical derived application, later changes | `/dd/SOURCECODE/ASM/GSHELL/gshell_ver1_0_1.asm` | Credits derived source to Kent D. Meyers; ver100/ver101/beta6/prehelp variants. Disk gshell identified only. |
| GLIB | Third-party/example library; full authorship unknown in inspected material | `/dd/SOURCECODE/ASM/GLIB/GLIB.links` | Dependency document explicitly distinguishes G$/U$ helpers and I$SwapP/I$SwapB. Do not infer Microware authorship from symbols. |
| VEFIO-WINFO | Utility/example collection; provenance not fully established | `/dd/SOURCECODE/ASM/VEFIO-WINFO/vefio.asm` | Module/edition declarations and BASIC09 client exist; no licensed-release or runtime source match established. |
| GFX5 + GUIB30 | Examples/third-party GUI procedures | `/dd/SOURCECODE/BASIC09/GUIB30/Guib.b09` | GuiB credits Shawn Driscoll 1992 and Br. Jeremy 1993, version 3.0. GFX5 demo purpose is explicit; no unified authorship inferred. |
| C/LIB | Historical C runtime wrappers; author/release unknown | `/dd/SOURCECODE/C/LIB/process.a` | Old include path and explicit stack/ABI comments; no proof this is the installed clib.l source. |
| CONTROL + CoCoThello | EOU-specific control panel; third-party game | `/dd/SOURCECODE/C/CONTROL/AboutCtrl.c` | Explicit EOU Project/2022 copyright text; CoCoThello has separate 1989 copyright in cocothello.c. Not a single provenance category. |
| SDCCMDR | Third-party cross-target application; exact release unknown | `/dd/SOURCECODE/C/SDCCMDR/sdccmdr.c` | Source has OS9/DECB/FLEX conditionals; Makefile selects DCC. out.txt credits MikeyN6IL, supporting application identity but not every file’s authorship. |
| SOUNDRV + PLAY | Third-party sound driver/player | `/dd/SOURCECODE/ASM/SOUNDRV/SounDrv.asm` | Allen C. Huffman/Sub-Etha copyright 1994/95, V1.02; PLAY header credits Curtis Boyle, V2.10. Different from EOU SndDrv. |
| CLOCKS | NitrOS-9 system with historical emulator/hardware providers | `/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock2_messemu.asm` | Robert Gault 2004/2009 and Boisy Pitre 2004 history; source comment revision 2 coexists with edition field 1. Live matched artifact is messemu. |
| GIMEX | NitrOS-9 hardware-specific variants; archive lineage partly resolved | `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/rel.asm` | Six-member frombill archive matches NEW after newline normalization. Other rel variants differ; archive filename attribution alone is not authorship. |
| MAMOU/RMA/RLINK/RDUMP | Third-party development tools; mixed/partly unknown provenance | `/dd/SOURCECODE/C/MAMOU/mamou.c` | Boisy Pitre 2004 copyright; RDUMP/6309dasm.c credits tim lindner and Sean Riddle. RMA/RLINK release lineage remains unverified. |

## Important dependency chains — verified references

| Chain | Evidence and resolution |
|---|---|
| Kernel/SCF/RBF → current definitions | `ioman_beta5.asm`, `vtio_beta6.asm`, `rbf_postbeta6.asm` include `/dd/defs/deffile`; inspected `/dd/DEFS/deffile` sets Level=2, H6309=1, GIMEX=0 and includes os9.d, rbf.d, scf.d, coco.d, cocovtio.d, rbsuper.d, cocosdc.d, vdgdefs. Those included files were read successfully. |
| Legacy local defsfile → missing filenames | `ASM/NITROS9/{KERNEL,RBF,MODS}/defsfile` reference os9defs/rbfdefs/scfdefs and additional old names. ToolShed reported pathname-not-found for the first three in /dd/DEFS. Do not replace them silently: their historic contents may differ. |
| C runtime wrappers → unresolved historical include | `C/LIB/process.a` uses `..../defs/os9defs.a`; no corresponding current path was resolved. Stack offsets in wrappers are compiler-specific. |
| FTP → RMA/RLINK + libraries | `ASM/FTP/makefile` selects rma/rlink, /dd/lib/alib.l and /dd/lib/sys.l; also expands NLIB without defining it in this file. /dd/LIB has alib.l, nlib.l, dwnet.l, sys.l_6309 and sys.l_6809, but no sys.l in the inspected listing. Thus this recipe is not demonstrated self-contained. |
| GLIB helper graph | `ASM/GLIB/GLIB.links` explicitly documents dependencies among G$/U$ helpers, GLIBDefs/DataMem, mapping routines and local I$SwapP/I$SwapB labels. It is stronger evidence than guessing from filenames. |
| Descriptor → manager → driver | Installed boot identification names Term → SCF/VTIO, DD/H1 → RBF/EmuDsk, D0–D2 → RBF/rb1773. Source descriptors have independent revisions; do not assume source defaults equal installed bytes. |
| Window path → rendering modules | `SCF/vtio_beta6.asm:LinkSys` calls F$Link; `SCF/cowin_beta6.asm` references GrfDrv entry points. `BASIC09/GFX5/GFX5Demo.b09` calls GFX2 and loads its procedure group. Runtime evidence identifies VTIO, CoWin, GrfDrv; it does not prove every GUI dependency is present. |
| DriveWire → transport definitions and source includes | `DW/dwio.asm` includes /dd/defs/drivewire.d, dwread.asm, dwwrite.asm, dwinit.asm. deffile flags BECKER/ARDUINO/SY6551N and related alternatives are zero; no transport was activated. |
| Clock core → Clock2 | `CLOCKS/clock.asm` dispatches via D.Clock2. Multiple source files declare the same Clock2 module name; live identification and stored-byte equality select messemu evidence for this baseline. |
| SDCCMDR → DCC OS9 branch and local headers | `C/SDCCMDR/Makefile` chooses dcc, OS9=1, .r objects and os9/screen.r; `sdccmdr.c`/`commsdc.c` contain other target branches. CMOC include presence does not prove a CMOC OS-9 build. |
| Build outputs are side effects | `ASM/BASIC09/makegfx2`, `ASM/NITROS9/DW/makedwio`, `ASM/SHELL/makeshell` and FTP Makefile write outputs/listings, sometimes using absolute /dd or /h1 paths. No recipe was executed. |

## Include/reference inventory

This table records direct include directives for all selected priority text files. Paths are relative to `/dd/SOURCECODE`; conditional references are retained. It is a dependency search index, not a linker-resolved graph. Parent assembly files, runtime F$Link names, generated dependencies and macro expansion can add edges.

| File | Direct include directives (line: reference) |
|---|---|
| `ASM/BASIC09/basic09_ver100.asm` | 260: `USE   basic09defsfile`; 261: `USE   basic09.d`; 10742: `use   basic09.real.add.63.asm`; 10744: `use   basic09.real.add.68.asm`; 10749: `use   basic09.real.mul.63.asm`; 10751: `use   basic09.real.mul.68.asm`; 10756: `use   basic09.real.div.63.asm`; 10758: `use   basic09.real.div.68.asm` |
| `ASM/BASIC09/basic09_ver101.asm` | 266: `USE   basic09defsfile`; 267: `USE   basic09.d`; 9906: `use   basic09.real.add.63.asm`; 9908: `use   basic09.real.add.68.asm`; 9913: `use   basic09.real.mul.63.asm`; 9915: `use   basic09.real.mul.68.asm`; 9920: `use   basic09.real.div.63.asm`; 9922: `use   basic09.real.div.68.asm` |
| `ASM/BASIC09/defsfile` | 2: `use   /dd/defs/os9.d`; 3: `use   /dd/defs/scf.d`; 4: `use   /dd/defs/cocovtio.d` |
| `ASM/BASIC09/gfx2_ver1.asm` | 28: `use   defsfile` |
| `ASM/BASIC09/gfx_beta6.asm` | 24: `use   defsfile` |
| `ASM/BASIC09/inkey.asm` | 21: `use   defsfile` |
| `ASM/BASIC09/runb_beta6.asm` | 45: `use   basic09.d`; 46: `use   defsfile` |
| `ASM/BASIC09/runb_ver100.asm` | 50: `use   basic09defsfile`; 51: `use   basic09.d`; 3321: `use   basic09.real.add.63.asm`; 3323: `use   basic09.real.add.68.asm`; 3328: `use   basic09.real.mul.63.asm`; 3330: `use   basic09.real.mul.68.asm`; 3335: `use   basic09.real.div.63.asm`; 3337: `use   basic09.real.div.68.asm` |
| `ASM/BASIC09/runb_ver101.asm` | 54: `use   basic09defsfile`; 55: `use   basic09.d`; 3337: `use   basic09.real.add.63.asm`; 3339: `use   basic09.real.add.68.asm`; 3344: `use   basic09.real.mul.63.asm`; 3346: `use   basic09.real.mul.68.asm`; 3351: `use   basic09.real.div.63.asm`; 3353: `use   basic09.real.div.68.asm` |
| `ASM/BASIC09/runbcd.asm` | 4: `USE   defsfile` |
| `ASM/BASIC09/syscall_6309.asm` | 23: `use   /dd/defs/os9.d` |
| `ASM/GLIB/GLIB/G10x10.a` | 2: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/G12x8.a` | 2: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/G14x8.a` | 2: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/G16x16.a` | 2: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/G4x4.a` | 2: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/G8x8.a` | 2: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/IPRINT.a` | 10: `use   /dd/defs/os9defs.a` |
| `ASM/GLIB/GLIB/UAUTO.a` | 10: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/UBOTH.a` | 14: `use   GLIBDefs.a`; 15: `use   /dd/defs/os9defs.a` |
| `ASM/GLIB/GLIB/UBOUNCE.a` | 11: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/UCALCXY.a` | 8: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/UCOLLIDE.a` | 21: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/UCPYMEM.a` | 12: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/UDEBUG.a` | 2: `use   /dd/defs/os9defs.a` |
| `ASM/GLIB/GLIB/UDELETE.a` | 7: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/UFXFLIP.a` | 7: `use   ./GLIBDefs.a` |
| `ASM/GLIB/GLIB/UGWBLK.a` | 18: `use   /dd/defs/os9defs.a`; 19: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/UINSERT.a` | 8: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/UIPFLIP.a` | 15: `use   /dd/defs/os9defs.a`; 16: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/UISPRITE.a` | 8: `use   GLIBDefs.a`; 9: `use   /dd/defs/os9defs.a` |
| `ASM/GLIB/GLIB/UJOY.a` | 16: `use   GLIBDefs.a`; 17: `use   /dd/defs/os9defs.a` |
| `ASM/GLIB/GLIB/ULINK.a` | 11: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/UMAPSC.a` | 8: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/UMAPSCB.a` | 9: `use   /dd/defs/os9defs.a`; 10: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/UMSKPUT.a` | 14: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/UPALETTE.a` | 5: `use   /dd/defs/os9defs.a` |
| `ASM/GLIB/GLIB/UPRINT1.a` | 12: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/UPRINT2.a` | 10: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/UPUT.a` | 12: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/UPUT8L.a` | 10: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/URESTORE.a` | 7: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/USAVE.a` | 9: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/USELECT.a` | 5: `use   /dd/defs/os9defs.a`; 6: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/USLPNSEL.a` | 8: `use   /dd/defs/os9defs.a` |
| `ASM/GLIB/GLIB/USPFLIP.a` | 12: `use   /dd/defs/os9defs.a`; 13: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/USSWAP.a` | 11: `use   /dd/defs/os9defs.a`; 12: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/UUPDATE.a` | 14: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB/ucpymem.o` | 12: `use   GLIBDefs.a` |
| `ASM/GLIB/GLIB_p2.doc` | 152: `Use F$MapBlk and F$ClrBlk to map and unmap the window from user memory, if you want.  Each window is four blocks long, taking up 32K of memory.  Screen types 5/6 will NOT return properly.  This is ONL` |
| `ASM/GLIB/SMASH/Blocks.a` | 2: `use   GLIBDefs.a`; 3: `use   Smashdefs.a`; 4: `use   Blockdefs.d` |
| `ASM/GLIB/SMASH/D_Screen.a` | 4: `use   GLIBDefs.a`; 5: `use   SmashDefs.a`; 6: `use   SMacros.a` |
| `ASM/GLIB/SMASH/HScore.a` | 2: `use   GLIBDefs.a`; 3: `use   Smashdefs.a`; 4: `use   SMacros.a` |
| `ASM/GLIB/SMASH/KeyPress.a` | 2: `use   GLIBDefs.a`; 3: `use   SMacros.a` |
| `ASM/GLIB/SMASH/LLoad.a` | 5: `use   GLIBDefs.a`; 6: `use   Smashdefs.a`; 7: `use   Blockdefs.d` |
| `ASM/GLIB/SMASH/Multi.a` | 2: `use   GLIBDefs.a`; 3: `use   Smashdefs.a` |
| `ASM/GLIB/SMASH/P_Iniz.a` | 8: `use   GLIBDefs.a`; 9: `use   Smashdefs.a`; 10: `use   Blockdefs.d` |
| `ASM/GLIB/SMASH/P_Setup.a` | 2: `use   GLIBDefs.a`; 3: `use   Smashdefs.a`; 4: `use   SMacros.a` |
| `ASM/GLIB/SMASH/Playball.a` | 7: `use   GLIBDefs.a`; 8: `use   Smashdefs.a`; 9: `use   SMacros.a` |
| `ASM/GLIB/SMASH/PutBall.a` | 2: `use   GLIBDefs.a` |
| `ASM/GLIB/SMASH/S_Iniz.a` | 2: `use   Glibdefs.a`; 3: `use   Smashdefs.a`; 4: `use   SMacros.a` |
| `ASM/GLIB/SMASH/S_Title.a` | 9: `use   GLIBDefs.a`; 10: `use   Smashdefs.a`; 11: `use   SMacros.a` |
| `ASM/GLIB/SMASH/Scroll.a` | 2: `use   GLIBDefs.a` |
| `ASM/GLIB/SMASH/Smash.a` | 2: `use   GLIBDefs.a`; 3: `use   Smashdefs.a`; 4: `use   SMacros.a` |
| `ASM/GLIB/SMASH/ball.a` | 2: `use   GLIBDefs.a`; 3: `use   SmashDefs.a` |
| `ASM/GLIB/SMASH/bounce.a` | 2: `use   GLIBDefs.a`; 3: `use   Smashdefs.a`; 4: `use   Blockdefs.d` |
| `ASM/GLIB/SMASH/bounce.o` | 2: `use   GLIBDefs.a`; 3: `use   Smashdefs.a`; 4: `use   Blockdefs.d` |
| `ASM/GLIB/SMASH/bounce_test.a` | 2: `use   GLIBDefs.a`; 3: `use   Smashdefs.a`; 4: `use   Blockdefs.d` |
| `ASM/GLIB/SMASH/data.a` | 5: `use   GLIBDefs.a`; 6: `use   SmashDefs.a` |
| `ASM/GLIB/SMASH/paddle.a` | 2: `use   GLIBDefs.a`; 3: `use   SmashDefs.a` |
| `ASM/GLIB/SMASH/score.a` | 2: `use   GLIBDefs.a`; 3: `use   Smashdefs.a` |
| `ASM/GSHELL/gshell101_prehelp.asm` | 61: `use   /dd/defs/deffile` |
| `ASM/GSHELL/gshell_beta6.asm` | 54: `use   defsfile` |
| `ASM/GSHELL/gshell_ver1_0_0.asm` | 56: `use   /dd/defs/deffile` |
| `ASM/GSHELL/gshell_ver1_0_1.asm` | 62: `use   /dd/defs/deffile` |
| `ASM/NITROS9/CLOCKS/clock.asm` | 35: `use   /dd/defs/deffile` |
| `ASM/NITROS9/CLOCKS/clock2_coco3fpga.asm` | 29: `use   /dd/defs/deffile` |
| `ASM/NITROS9/CLOCKS/clock2_disto2.asm` | 16: `use   /dd/defs/deffile` |
| `ASM/NITROS9/CLOCKS/clock2_disto4.asm` | 16: `use   /dd/defs/deffile` |
| `ASM/NITROS9/CLOCKS/clock2_ds1315.asm` | 19: `use   /dd/defs/deffile` |
| `ASM/NITROS9/CLOCKS/clock2_dw.asm` | 19: `use       /dd/defs/deffile`; 20: `use       /dd/defs/drivewire.d` |
| `ASM/NITROS9/CLOCKS/clock2_elim.asm` | 16: `use   /dd/defs/deffile` |
| `ASM/NITROS9/CLOCKS/clock2_harris.asm` | 16: `use   /dd/defs/deffile` |
| `ASM/NITROS9/CLOCKS/clock2_jvemu.asm` | 23: `use   /dd/defs/deffile` |
| `ASM/NITROS9/CLOCKS/clock2_messemu.asm` | 23: `use   /dd/defs/deffile` |
| `ASM/NITROS9/CLOCKS/clock2_smart.asm` | 62: `use   defsfile` |
| `ASM/NITROS9/CLOCKS/clock2_soft.asm` | 16: `use   /dd/defs/deffile` |
| `ASM/NITROS9/DW/dwio.asm` | 22: `use       /dd/defs/deffile`; 23: `use       /dd/defs/drivewire.d`; 90: `use       dwread.asm`; 97: `use       dwwrite.asm`; 104: `use		dwinit.asm` |
| `ASM/NITROS9/DW/rbdw.asm` | 39: `use     /dd/defs/deffile`; 40: `use     /dd/defs/drivewire.d`; 335: `use     dwcheck.asm` |
| `ASM/NITROS9/DW/scdwp.asm` | 16: `use   defsfile`; 17: `use   drivewire.d` |
| `ASM/NITROS9/DW/scdwv.asm` | 56: `use       defsfile`; 57: `use       drivewire.d` |
| `ASM/NITROS9/GIMEX/NEW/boot_sdc.asm` | 18: `use     /dd/defs/deffile`; 101: `use  boot_common.asm` |
| `ASM/NITROS9/GIMEX/NEW/deffile` | 13: `use   /dd/defs/os9.d`; 14: `use   /dd/defs/rbf.d`; 15: `use   /dd/defs/scf.d`; 16: `use   /dd/defs/coco.d`; 17: `use   /dd/defs/cocovtio.d`; 18: `use   /dd/defs/rbsuper.d`; 19: `use   /dd/defs/cocosdc.d`; 20: `use   /dd/defs/vdgdefs` |
| `ASM/NITROS9/GIMEX/NEW/rel.asm` | 20: `use   /dd/defs/deffile` |
| `ASM/NITROS9/GIMEX/boot_sdc.asm` | 18: `use     /dd/defs/deffile`; 101: `use  boot_common.asm` |
| `ASM/NITROS9/GIMEX/boot_sdc_beta6.asm` | 18: `use   /dd/defs/deffile`; 99: `use   boot_common.asm` |
| `ASM/NITROS9/GIMEX/deffile` | 13: `use   /dd/defs/os9.d`; 14: `use   /dd/defs/rbf.d`; 15: `use   /dd/defs/scf.d`; 16: `use   /dd/defs/coco.d`; 17: `use   /dd/defs/cocovtio.d`; 18: `use   /dd/defs/rbsuper.d`; 19: `use   /dd/defs/cocosdc.d`; 20: `use   /dd/defs/vdgdefs` |
| `ASM/NITROS9/GIMEX/gimexcheck.a` | 8: `use /dd/defs/deffile` |
| `ASM/NITROS9/GIMEX/llcocosdc.asm` | 30: `use       /dd/defs/deffile` |
| `ASM/NITROS9/GIMEX/llcocosdc_beta6.asm` | 30: `use   /dd/defs/deffile` |
| `ASM/NITROS9/GIMEX/llcocosdc_ver101.asm` | 39: `use       /dd/defs/deffile` |
| `ASM/NITROS9/GIMEX/rel.asm` | 20: `use   /dd/defs/deffile` |
| `ASM/NITROS9/GIMEX/rel_beta6.asm` | 23: `use   /dd/defs/deffile` |
| `ASM/NITROS9/KERNEL/boot_1773.asm` | 47: `use   defsfile`; 174: `use   boot_common.asm` |
| `ASM/NITROS9/KERNEL/boot_burke.asm` | 33: `use   defsfile` |
| `ASM/NITROS9/KERNEL/boot_d64.asm` | 47: `use 	defsfile`; 220: `use   	boot_common.asm` |
| `ASM/NITROS9/KERNEL/boot_dw.asm` | 16: `USE       defsfile`; 17: `USE       drivewire.d`; 47: `USE       boot_common.asm`; 61: `use       dwinit.asm`; 116: `USE       dwread.asm`; 117: `USE       dwwrite.asm` |
| `ASM/NITROS9/KERNEL/boot_ide.asm` | 32: `USE       defsfile`; 33: `USE       ide.d`; 67: `USE       boot_common.asm` |
| `ASM/NITROS9/KERNEL/boot_rampak.asm` | 24: `use   defsfile`; 56: `use   boot_common.asm` |
| `ASM/NITROS9/KERNEL/boot_rom.asm` | 20: `use   defsfile` |
| `ASM/NITROS9/KERNEL/boot_scsi.asm` | 53: `USE       defsfile`; 54: `USE       rbsuper.d`; 55: `USE       scsi.d`; 102: `USE       boot_common.asm` |
| `ASM/NITROS9/KERNEL/boot_vhd.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/KERNEL/boot_wd1002.asm` | 34: `use   defsfile` |
| `ASM/NITROS9/KERNEL/ccbkrn.asm` | 29: `use	defsfile`; 760: `use	fssvc.asm`; 762: `use	flink.asm`; 764: `use	fvmodul.asm`; 766: `use	ffmodul.asm`; 768: `use	fprsnam.asm`; 770: `use	fcmpnam.asm`; 772: `use	ccbfsrqmem.asm`; 778: `use	fdelram.asm`; 781: `use	fallimg.asm`; 783: `use	ffreehb.asm`; 785: `use	fdatlog.asm`; 787: `use	fld.asm`; 790: `use	fcpymem.asm`; 793: `use	fmove.asm`; 795: `use	fldabx.asm`; 797: `use	falltsk.asm`; 799: `use	faproc.asm`; 851: `use	fnproc.asm` |
| `ASM/NITROS9/KERNEL/defsfile` | 9: `use /dd/defs/os9defs`; 10: `use /dd/defs/rbfdefs`; 11: `use /dd/defs/scfdefs`; 12: `use /dd/defs/vtiodefs_cc3`; 13: `use /dd/defs/systype`; 14: `use /dd/defs/releasedefs` |
| `ASM/NITROS9/KERNEL/krn.asm` | 29: `use    defsfile`; 654: `use    fssvc.asm`; 656: `use    flink.asm`; 658: `use    fvmodul.asm`; 660: `use    ffmodul.asm`; 662: `use    fprsnam.asm`; 664: `use    fcmpnam.asm`; 666: `use    fsrqmem.asm`; 672: `use    fdelram.asm`; 675: `use    fallimg.asm`; 677: `use    ffreehb.asm`; 679: `use    fdatlog.asm`; 681: `use    fld.asm`; 684: `use    fcpymem.asm`; 687: `use    fmove.asm`; 689: `use    fldabx.asm`; 691: `use    falltsk.asm`; 693: `use    faproc.asm`; 745: `use    fnproc.asm` |
| `ASM/NITROS9/KERNEL/krn_Beta5.asm` | 44: `use   /dd/defs/deffile`; 641: `use   fssvc.asm`; 642: `use   flink.asm`; 643: `use   fvmodul.asm`; 644: `use   ffmodul.asm`; 645: `use   fprsnam.asm`; 646: `use   fcmpnam.asm`; 647: `use   fsrqmem.asm`; 649: `use   fdelram.asm`; 651: `use   fallimg.asm`; 652: `use   ffreehb.asm`; 653: `use   fdatlog.asm`; 654: `use   fld.asm`; 655: `use   fcpymem_beta5.asm`; 656: `use   fmove.asm`; 657: `use   fldabx.asm`; 658: `use   falltsk.asm`; 659: `use   faproc.asm`; 705: `use   fnproc.asm` |
| `ASM/NITROS9/KERNEL/krnp2.asm` | 74: `use    defsfile`; 75: `use    cocovtio.d`; 340: `use    fcrcmod.asm`; 370: `use    funlink.asm`; 372: `use    ffork.asm`; 374: `use    fallprc.asm`; 376: `use    fchain.asm`; 378: `use    fexit.asm`; 380: `use    fmem.asm`; 382: `use    fsend.asm`; 384: `use    ficpt.asm`; 386: `use    fsleep.asm`; 388: `use    fallram.asm`; 390: `use    fsprior.asm`; 392: `use    fid.asm`; 395: `use    fcpymem.asm`; 397: `use    fdelram.asm`; 400: `use    fsswi.asm`; 402: `use    fstime.asm`; 404: `use    fallbit.asm`; 406: `use    fgprdsc.asm`; 408: `use    fgblkmp.asm`; 410: `use    fgmoddr.asm`; 412: `use    fsuser.asm`; 414: `use    funload.asm`; 416: `use    ffind64.asm`; 418: `use    fgprocp.asm`; 420: `use    fdelimg.asm`; 422: `use    fmapblk.asm`; 424: `use    fclrblk.asm`; 426: `use    fgcmdir.asm`; 428: `use    fdebug.asm` |
| `ASM/NITROS9/KERNEL/krnp2_beta5.asm` | 84: `use    /dd/defs/deffile`; 337: `use    fcrcmod.asm`; 367: `use    funlink.asm`; 369: `use    ffork.asm`; 371: `use    fallprc.asm`; 373: `use    fchain.asm`; 375: `use    fexit.asm`; 377: `use    fmem.asm`; 379: `use    fsend.asm`; 381: `use    ficpt.asm`; 383: `use    fsleep.asm`; 385: `use    fallram.asm`; 387: `use    fsprior.asm`; 389: `use    fid.asm`; 392: `use    fdelram.asm`; 395: `use    fsswi.asm`; 397: `use    fstime.asm`; 399: `use    fallbit.asm`; 401: `use    fgprdsc.asm`; 403: `use    fgblkmp.asm`; 405: `use    fgmoddr.asm`; 407: `use    fsuser.asm`; 409: `use    funload.asm`; 411: `use    ffind64.asm`; 413: `use    fgprocp.asm`; 415: `use    fdelimg.asm`; 417: `use    fmapblk.asm`; 419: `use    fclrblk.asm`; 421: `use    fgcmdir.asm`; 423: `use    fdebug.asm` |
| `ASM/NITROS9/KERNEL/krnp2_ver101.asm` | 85: `use    /dd/defs/deffile`; 338: `use    fcrcmod.asm`; 368: `use    funlink.asm`; 370: `use    ffork.asm`; 372: `use    fallprc.asm`; 374: `use    fchain.asm`; 376: `use    fexit.asm`; 378: `use    fmem.asm`; 380: `use    fsend.asm`; 382: `use    ficpt.asm`; 384: `use    fsleep.asm`; 386: `use    fallram.asm`; 388: `use    fsprior.asm`; 390: `use    fid.asm`; 393: `use    fdelram.asm`; 396: `use    fsswi.asm`; 398: `use    fstime.asm`; 400: `use    fallbit.asm`; 402: `use    fgprdsc.asm`; 404: `use    fgblkmp.asm`; 406: `use    fgmoddr.asm`; 408: `use    fsuser.asm`; 410: `use    funload.asm`; 412: `use    ffind64.asm`; 414: `use    fgprocp.asm`; 416: `use    fdelimg.asm`; 418: `use    fmapblk.asm`; 420: `use    fclrblk.asm`; 422: `use    fgcmdir.asm`; 424: `use    fdebug.asm` |
| `ASM/NITROS9/KERNEL/krnp3_ed3_perr.asm` | 66: `use   /dd/defs/deffile` |
| `ASM/NITROS9/KERNEL/rel.asm` | 20: `use   defsfile` |
| `ASM/NITROS9/KERNEL/rel_beta6.asm` | 23: `use   /dd/defs/deffile` |
| `ASM/NITROS9/MODS/defsfile` | 16: `use /dd/defs/os9defs`; 17: `use /dd/defs/rbfdefs`; 18: `use /dd/defs/scfdefs`; 19: `use /dd/defs/vtiodefs_cc3`; 20: `use /dd/defs/systype`; 21: `use /dd/defs/releasedefs` |
| `ASM/NITROS9/MODS/dwdesc.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/MODS/dwio.asm` | 22: `use       defsfile`; 23: `use       drivewire.d`; 73: `use       dwread.asm`; 80: `use       dwwrite.asm`; 87: `use		dwinit.asm` |
| `ASM/NITROS9/MODS/dwiomess.asm` | 26: `use       defsfile`; 27: `use       dwdefs.d`; 86: `use       dwrdmess.asm`; 92: `use       dwwrmess.asm` |
| `ASM/NITROS9/MODS/init.asm` | 36: `use   defsfile`; 38: `use	cocovtio.d` |
| `ASM/NITROS9/MODS/ioman_beta5.asm` | 45: `use   /dd/defs/deffile` |
| `ASM/NITROS9/MODS/krnp4_regdump.asm` | 30: `use   defsfile` |
| `ASM/NITROS9/MODS/llcocosdc.asm` | 15: `USE       defsfile`; 16: `USE       rbsuper.d` |
| `ASM/NITROS9/MODS/sysgo.asm` | 28: `use   defsfile` |
| `ASM/NITROS9/PIPE/pipe.asm` | 16: `use   defsfile`; 17: `use   pipedefs` |
| `ASM/NITROS9/PIPE/pipeman.asm` | 46: `use   /dd/defs/deffile`; 48: `use   /dd/defs/pipedefs` |
| `ASM/NITROS9/PIPE/pipeman_beta6.asm` | 48: `use   /dd/defs/deffile`; 50: `use   /dd/defs/pipedefs` |
| `ASM/NITROS9/PIPE/pipeman_named.asm` | 117: `use   defsfile`; 118: `use   pipedefs` |
| `ASM/NITROS9/PIPE/piper.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/RBF/cc3disk_disto.asm` | 29: `use   defsfile`; 30: `use   rbfdefs` |
| `ASM/NITROS9/RBF/cc3disk_sc2_irq.asm` | 27: `use   os9defs` |
| `ASM/NITROS9/RBF/cc3disk_sc2_slp.asm` | 27: `use   defsfile` |
| `ASM/NITROS9/RBF/d0.asm` | 10: `use   defsfile` |
| `ASM/NITROS9/RBF/ddh0.asm` | 11: `USE os9defs` |
| `ASM/NITROS9/RBF/ddr0_128k.asm` | 18: `use   defsfile` |
| `ASM/NITROS9/RBF/ddr0_192k.asm` | 18: `use   defsfile` |
| `ASM/NITROS9/RBF/ddr0_8k.asm` | 18: `use   defsfile` |
| `ASM/NITROS9/RBF/ddr0_96k.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/RBF/defsfile` | 16: `use /dd/defs/os9defs`; 17: `use /dd/defs/rbfdefs`; 18: `use /dd/defs/scfdefs`; 19: `use /dd/defs/vtiodefs_cc3`; 20: `use /dd/defs/systype`; 21: `use /dd/defs/releasedefs` |
| `ASM/NITROS9/RBF/emudsk_beta601.asm` | 71: `use   /dd/DEFS/deffile` |
| `ASM/NITROS9/RBF/h0.asm` | 11: `USE os9defs` |
| `ASM/NITROS9/RBF/m0_40d.asm` | 7: `use msfdesc.asm` |
| `ASM/NITROS9/RBF/md.asm` | 14: `use   /dd/defs/deffile` |
| `ASM/NITROS9/RBF/msf.asm` | 25: `use   defsfile` |
| `ASM/NITROS9/RBF/msfdesc.asm` | 3: `use   os9defs`; 4: `use   rbfdefs`; 5: `use   msfdefs` |
| `ASM/NITROS9/RBF/myram.asm` | 31: `use   defsfile` |
| `ASM/NITROS9/RBF/parallel.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/RBF/pp.asm` | 7: `use   defsfile` |
| `ASM/NITROS9/RBF/r0.asm` | 14: `use   defsfile` |
| `ASM/NITROS9/RBF/ram.asm` | 32: `use   defsfile` |
| `ASM/NITROS9/RBF/rammer_2mb_beta6.asm` | 41: `use   /dd/defs/deffile` |
| `ASM/NITROS9/RBF/rammer_ed6_8-64mb.asm` | 59: `use   defsfile` |
| `ASM/NITROS9/RBF/rampak.asm` | 20: `use   defsfile`; 21: `use   rbfdefs` |
| `ASM/NITROS9/RBF/rb1773.asm` | 101: `use   defsfile` |
| `ASM/NITROS9/RBF/rb1773desc.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/RBF/rbf_postbeta6.asm` | 124: `use   /dd/defs/deffile` |
| `ASM/NITROS9/RBF/rx.asm` | 14: `use   defsfile` |
| `ASM/NITROS9/RBF/sdisk3_dmc.asm` | 26: `use   defsfile` |
| `ASM/NITROS9/RBF/sdisk3_dpj.asm` | 12: `use   defsfile` |
| `ASM/NITROS9/RBF/sdisk3desc.asm` | 13: `use   defsfile` |
| `ASM/NITROS9/SCF/covdg_beta6.asm` | 53: `use   /dd/defs/deffile` |
| `ASM/NITROS9/SCF/covdg_beta61.asm` | 58: `use   /dd/defs/deffile` |
| `ASM/NITROS9/SCF/covdg_ver100.asm` | 58: `use   /dd/defs/deffile` |
| `ASM/NITROS9/SCF/cowin_beta6.asm` | 56: `use   /dd/defs/deffile` |
| `ASM/NITROS9/SCF/ftdd.asm` | 14: `use   defsfile` |
| `ASM/NITROS9/SCF/joydrv_6551l.asm` | 68: `use   defsfile`; 69: `use   l51.defs` |
| `ASM/NITROS9/SCF/joydrv_6551m.asm` | 18: `use   /dd/defs/deffile`; 19: `use   /dd/defs/m51.defs` |
| `ASM/NITROS9/SCF/joydrv_6552l.asm` | 18: `use   defsfile`; 19: `use   l52.defs` |
| `ASM/NITROS9/SCF/joydrv_6552m.asm` | 20: `use   defsfile` |
| `ASM/NITROS9/SCF/joydrv_joy_beta6.asm` | 56: `use   /dd/defs/deffile` |
| `ASM/NITROS9/SCF/krnp4_regdump.asm` | 32: `use   defsfile` |
| `ASM/NITROS9/SCF/nil.asm` | 14: `use   defsfile` |
| `ASM/NITROS9/SCF/p1_sc6551dragon.asm` | 8: `use defsfile` |
| `ASM/NITROS9/SCF/p_bbp.asm` | 17: `use   /dd/defs/deffile` |
| `ASM/NITROS9/SCF/p_dpp.asm` | 11: `use     defsfile` |
| `ASM/NITROS9/SCF/s16550v16_lOS9.asm` | 7: `use   /dd/defs/defsfile` |
| `ASM/NITROS9/SCF/sc6551.asm` | 26: `use   defsfile`; 27: `use   scfdefs` |
| `ASM/NITROS9/SCF/sc6551dragon.asm` | 26: `use     defsfile` |
| `ASM/NITROS9/SCF/scbbp.asm` | 25: `use   /dd/defs/deffile` |
| `ASM/NITROS9/SCF/scbbt.asm` | 24: `use   defsfile` |
| `ASM/NITROS9/SCF/scdpp.asm` | 12: `use     defsfile` |
| `ASM/NITROS9/SCF/scf_beta6.asm` | 201: `use   /dd/defs/deffile    EOU "current" deffile`; 206: `use   /dd/defs/cocovtio.d` |
| `ASM/NITROS9/SCF/scf_ver100.asm` | 207: `use   /dd/defs/deffile    EOU "current" deffile`; 212: `use   /dd/defs/cocovtio.d` |
| `ASM/NITROS9/SCF/snddrv_beta6.asm` | 29: `use   /dd/defs/deffile` |
| `ASM/NITROS9/SCF/ssp.asm` | 18: `use   defsfile` |
| `ASM/NITROS9/SCF/sspak.asm` | 33: `use   defsfile` |
| `ASM/NITROS9/SCF/t1_bbt.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/t2_sc6551.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/t2_sc6552.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/t3_sc6551.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/t3_sc6552.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/term_bbt.asm` | 18: `use   defsfile`; 19: `use   scfdefs` |
| `ASM/NITROS9/SCF/term_sc6551.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/term_t1.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/term_vdg.asm` | 14: `use   defsfile` |
| `ASM/NITROS9/SCF/term_win40.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/term_win80.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/v1.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/v2.asm` | 16: `use   /dd/defs/deffile` |
| `ASM/NITROS9/SCF/v3.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/v4.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/v5.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/v6.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/v7.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/vi.asm` | 14: `use   defsfile` |
| `ASM/NITROS9/SCF/vrn.asm` | 25: `use   defsfile` |
| `ASM/NITROS9/SCF/vtio_beta6.asm` | 90: `use   /dd/defs/deffile` |
| `ASM/NITROS9/SCF/w.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/w1.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/w2.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/w3.asm` | 16: `use   /dd/defs/deffile` |
| `ASM/NITROS9/SCF/w4.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/w5.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/w6.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/w7.asm` | 16: `use   defsfile` |
| `ASM/NITROS9/SCF/wordpakii.asm` | 22: `use   defsfile`; 23: `use   vtiodefs` |
| `ASM/PLAY/play.a` | 6: `use   /dd/defs/os9defs`; 7: `use   /dd/defs/scfdefs`; 1388: `use    two_byte_bin2dec.a`; 1389: `use    mulaw_alaw.a` |
| `ASM/SHELL/shell21.a` | 7: `use   /dd/defs/os9defs` |
| `ASM/SHELL/shellplus2.2a.asm` | 33: `use   /dd/defs/deffile` |
| `ASM/VEFIO-WINFO/defsfile` | 2: `use os9.d`; 3: `use scf.d` |
| `ASM/VEFIO-WINFO/makefile` | 1: `include ../../../rules.mak` |
| `ASM/VEFIO-WINFO/vefio.asm` | 16: `use   /dd/defs/os9.d`; 64: `use   winfodefs` |
| `ASM/VEFIO-WINFO/winfo.asm` | 11: `use   /dd/defs/deffile`; 26: `use   winfodefs` |
| `ASM/VEFIO-WINFO/witesta.asm` | 12: `use   defsfile`; 35: `use   /dd/defs/winfodefs` |
| `C/COCOTHELLO/cocothello.c` | 11: `#include <stdio.h>`; 12: `#include <wind.h>`; 13: `#include <mouse.h>`; 14: `#include <buffs.h>`; 15: `#include <time.h>`; 60: `#include "cocothello.h"` |
| `C/CONTROL/AboutCtrl.c` | 1: `#include <stdio.h>`; 2: `#include <stdlib.h>`; 3: `#include <buffs.h>`; 4: `#include <string.h>`; 5: `#include <wind.h>`; 6: `#include <mouse.h>` |
| `C/CONTROL/ControlHlp.c` | 1: `#include <stdio.h>`; 2: `#include <stdlib.h>`; 3: `#include <buffs.h>`; 4: `#include <string.h>`; 5: `#include <wind.h>`; 6: `#include <mouse.h>` |
| `C/CONTROL/control.c` | 1: `#include <stdio.h>`; 2: `#include <stdlib.h>`; 3: `#include <buffs.h>`; 4: `#include <string.h>`; 5: `#include <wind.h>`; 6: `#include <mouse.h>`; 7: `#include <sgstat.h>` |
| `C/LIB/abort.a` | 1: `use ..../defs/os9defs.a` |
| `C/LIB/access.a` | 8: `use ..../defs/os9defs.a` |
| `C/LIB/cfinish.a` | 1: `use ...../defs/os9defs.a` |
| `C/LIB/change.a` | 2: `use ..../defs/os9defs.a` |
| `C/LIB/cstart.a` | 2: `use ..../defs/os9defs.a` |
| `C/LIB/dir.a` | 5: `use ..../defs/os9defs.a` |
| `C/LIB/id.a` | 4: `use ..../defs/os9defs.a` |
| `C/LIB/intercept.a` | 8: `use ..../defs/os9defs.a` |
| `C/LIB/io.a` | 5: `use ..../defs/os9defs.a` |
| `C/LIB/line.c` | 3: `#include <stdio.h>` |
| `C/LIB/mem.a` | 4: `use ..../defs/os9defs.a` |
| `C/LIB/misc.a` | 5: `use ..../defs/os9defs.a` |
| `C/LIB/mod.a` | 11: `use ..../defs/os9defs.a` |
| `C/LIB/process.a` | 4: `use ..../defs/os9defs.a` |
| `C/LIB/prof.c` | 10: `#include <stdio.h>` |
| `C/LIB/rdump.c` | 1: `#include <stdio.h>` |
| `C/LIB/signal.a` | 1: `use ..../defs/os9defs.a` |
| `C/LIB/stat.a` | 6: `use ..../defs/os9defs.a` |
| `C/LIB/time.a` | 6: `use ..../defs/os9defs.a` |
| `C/MAMOU/evaluator.c` | 45: `#include "mamou.h"` |
| `C/MAMOU/ffwd.c` | 11: `#include "mamou.h"` |
| `C/MAMOU/h6309.c` | 11: `#include "mamou.h"` |
| `C/MAMOU/mamou.c` | 11: `#include "mamou.h"`; 542: `include = getenv("MAMOU_INCLUDE");` |
| `C/MAMOU/mamou.h` | 11: `#include <stdio.h>`; 12: `#include <stdlib.h>`; 13: `#include <string.h>`; 14: `#include <fcntl.h>`; 15: `#include <unistd.h>`; 16: `#include <ctype.h>`; 17: `#include <sys/types.h>`; 19: `#include "cocopath.h"` |
| `C/MAMOU/print.c` | 11: `#include <time.h>`; 13: `#include "mamou.h"` |
| `C/MAMOU/pseudo.c` | 11: `#include <string.h>`; 12: `#include "mamou.h"` |
| `C/MAMOU/symbol_bucket.c` | 11: `#include "mamou.h"`; 12: `#include "h6309.h"`; 13: `#include "pseudo.h"` |
| `C/MAMOU/util.c` | 11: `#include    "mamou.h"`; 12: `#include    "os9module.h"` |
| `C/RDUMP/6309dasm.c` | 23: `#include <stdio.h>`; 24: `#include <string.h>`; 26: `#include "hd6309.h"`; 27: `#include "rof.h"` |
| `C/RDUMP/rdump.c` | 8: `#include <stdio.h>`; 9: `#include <stdlib.h>`; 10: `#include <string.h>`; 11: `#include <rof.h>`; 12: `#include "hd6309.h"` |
| `C/RLINK/decbout.c` | 1: `#include <stdio.h>`; 2: `#include <string.h>`; 3: `#include "rlink.h"` |
| `C/RLINK/os9out.c` | 1: `#include <stdio.h>`; 2: `#include <string.h>`; 3: `#include "rlink.h"` |
| `C/RLINK/rl_pass1a.c` | 14: `#include <stdio.h>`; 16: `#include <stdlib.h>`; 17: `#include <string.h>`; 18: `#include <libgen.h>`; 20: `#include "rlink.h"` |
| `C/RLINK/rl_pass1b.c` | 14: `#include <stdio.h>`; 16: `#include <stdlib.h>`; 17: `#include <string.h>`; 18: `#include <libgen.h>`; 19: `#include <netinet/in.h>`; 21: `#include "rlink.h"` |
| `C/RLINK/rl_pass2.c` | 14: `#include <stdio.h>`; 16: `#include <stdlib.h>`; 17: `#include <string.h>`; 18: `#include <libgen.h>`; 20: `#include "rlink.h"` |
| `C/RLINK/rlink.c` | 14: `#include <stdio.h>`; 16: `#include <stdlib.h>`; 17: `#include <string.h>`; 18: `#include <libgen.h>`; 20: `#include "rlink.h"` |
| `C/RLINK/rlink.h` | 1: `#include <stdlib.h>`; 2: `#include <rof.h>`; 3: `#include "out.h"` |
| `C/RMA/part2.c` | 4: `#include "rma.h"` |
| `C/RMA/part3.c` | 3: `#include "rma.h"` |
| `C/RMA/part4.c` | 5: `#include "rma.h"` |
| `C/RMA/part5.c` | 3: `#include "rma.h"` |
| `C/RMA/part6.c` | 3: `#include "rma.h"` |
| `C/RMA/part7.c` | 5: `#include "rma.h"` |
| `C/RMA/part8.c` | 3: `#include <time.h>`; 4: `#include "rma.h"` |
| `C/RMA/part9.c` | 8: `#include "rma.h"` |
| `C/RMA/rma.c` | 5: `#include "rma.h"`; 6: `#include <errno.h>` |
| `C/RMA/rma.h` | 26: `#include "cocotype.h"`; 27: `#include <stdio.h>`; 28: `#include <string.h>`; 29: `#include "rstruct.h"`; 177: `#include "rtables.h"`; 247: `#include "proto.h"` |
| `C/SDCCMDR/OS9/screen.c` | 2: `#include <cmoc.h>`; 6: `#include <stdio.h>`; 7: `#include <unistd.h>`; 8: `#include <string.h>`; 9: `#include <sgstat.h>`; 11: `#include "os9/screen.h"` |
| `C/SDCCMDR/OS9/screen.h` | 1: `#include "../proto.h"` |
| `C/SDCCMDR/commsdc.c` | 2: `#include <cmoc.h>`; 4: `#include "commsdc.h"` |
| `C/SDCCMDR/commsdc.h` | 1: `#include "proto.h"` |
| `C/SDCCMDR/libsdc.c` | 2: `#include <cmoc.h>`; 3: `#include "string.h"`; 5: `#include "commsdc.h"`; 6: `#include "libsdc.h"`; 8: `#include <string.h>` |
| `C/SDCCMDR/libsdc.h` | 1: `#include "proto.h"` |
| `C/SDCCMDR/sdccmdr.c` | 2: `#include <cmoc.h>`; 3: `#include <coco.h>`; 6: `#include "libsdc.h"`; 7: `#include "commsdc.h"`; 8: `#include "string.h"`; 9: `#include "sdccmdr.h"`; 12: `#include "hirestxt/screen.h"`; 16: `#include <stdio.h>`; 17: `#include <string.h>`; 18: `#include "os9/screen.h"`; 22: `#include <stdio.h>`; 23: `#include "screen.h"`; 27: `#include <stdio.h>`; 28: `#include <string.h>`; 29: `#include "screen.h"` |
| `C/SDCCMDR/sdccmdr.h` | 1: `#include "proto.h"` |
| `C/SDCCMDR/string.c` | 2: `#include <cmoc.h>`; 4: `#include "string.h"`; 6: `#include <string.h>` |
| `C/SDCCMDR/string.h` | 1: `#include "proto.h"` |
| `C/SDCCMDR/test.c` | 1: `#include "proto.h"` |

## Build recipe inventory

Only command/recipe files already selected by Pass 1 classification are listed. This is navigation, not a statement that all build dependencies are installed.

| Recipe | First nonblank line (evidence only) |
|---|---|
| `ASM/BASIC09/makebasic09` | asm basic09_ver101.asm -o=/dd/sourcecode/asm/basic09/basic09_63_ver101 l w106 #56k >-basic09_63_ver101.listing |
| `ASM/BASIC09/makegfx` | asm gfx_beta6.asm -o=/dd/sourcecode/asm/basic09/gfx_beta6 l #56k >-gfx_beta6.listing |
| `ASM/BASIC09/makegfx2` | asm gfx2_ver1.asm -o=/dd/sourcecode/asm/basic09/gfx2_ver1 w106 l #56k >-gfx2_ver1.listing |
| `ASM/BASIC09/makeinkey` | asm inkey.asm -o=/dd/sourcecode/asm/basic09/inkey l #56k >-inkey.listing |
| `ASM/BASIC09/makemergedbasic09` | merge basic09_63_ver101 inkey syscall gfx_beta6 >-basic09_ver101 #32k |
| `ASM/BASIC09/makemergedrunb` | t |
| `ASM/BASIC09/makerunb` | asm runb_ver101.asm -o=/dd/sourcecode/asm/basic09/runb_63_ver101 l w106 #56k >-runb_63_ver101.listing |
| `ASM/BASIC09/makesyscall` | asm syscall_6309.asm -o=/dd/sourcecode/asm/basic09/syscall l #56k >-syscall.listing |
| `ASM/DWNET/makefile` | ****************************** |
| `ASM/FTP/makefile` | ASM = rma |
| `ASM/GLIB/GLIB/makefile` | ****************************** |
| `ASM/GLIB/SMASH/makefile` | ****************************** |
| `ASM/GSHELL/makegshell` | asm gshell_ver1_0_1.asm -o=/dd/sourcecode/asm/gshell/gshell l w132 #56k >-gshell_ver1_0_1.listing |
| `ASM/NITROS9/CLOCKS/makeclock` | asm clock.asm l -o=/h1/asm/nitros9/clocks/clock #48k >- clock.listing |
| `ASM/NITROS9/CLOCKS/makeclock2_dw` | asm clock2_dw.asm l -o=/h1/asm/nitros9/clocks/clock2_dw #48k >- c2_dw.listing |
| `ASM/NITROS9/CLOCKS/makeclock2_messemu` | asm clock2_messemu.asm l -o=/h1/asm/nitros9/clocks/clock2_messemu #48k >- c2_messemu.listing |
| `ASM/NITROS9/CLOCKS/makeclock2_soft` | asm clock2_soft.asm l -o=/h1/asm/nitros9/clocks/clock2_soft #48k >- c2_soft.listing |
| `ASM/NITROS9/DW/makedwio` | asm dwio.asm l -o=/h1/asm/nitros9/dw/dwio #48k >- dwio.listing |
| `ASM/NITROS9/GIMEX/makerel` | asm rel_beta6.asm -o=/dd/sourcecode/asm/nitros9/gimex/rel_beta6_6309 l w106 #56k >-rel_beta6_6309.listing |
| `ASM/NITROS9/GIMEX/makebootsdc` | asm boot_sdc_beta6.asm -o=/dd/sourcecode/asm/nitros9/gimex/bootsdc_beta6_6309 l w106 #56k >-bootsdc_beta6_6309.listing |
| `ASM/NITROS9/GIMEX/makellcocosdc` | asm llcocosdc_ver101.asm -o=/dd/sourcecode/asm/nitros9/gimex/llcocosdc_ver101 l w106 >-llcocosdc_ver101.listing #56k |
| `ASM/NITROS9/KERNEL/makekrn` | asm krn_beta5.asm w106 -o=/dd/sourcecode/asm/nitros9/kernel/krn_beta5 l >-krn_beta5.listing #48k |
| `ASM/NITROS9/KERNEL/makekrnp2` | asm krnp2_ver101.asm w106 -o=/dd/sourcecode/asm/nitros9/kernel/krnp2_ver101 l >-krnp2_ver101.listing #48k |
| `ASM/NITROS9/KERNEL/makekrnp3` | asm krnp3_ed3_perr.asm -o=/dd/sourcecode/asm/nitros9/kernel/krnp3_perr_ed3 l >-krnp3_perr_ed3.listing #48k |
| `ASM/NITROS9/KERNEL/makerel` | asm rel_beta6.asm -o=/dd/sourcecode/asm/nitros9/kernel/rel_beta6_6309 l w106 #56k >-rel_beta6_6309.listing |
| `ASM/NITROS9/MODS/makeioman` | asm ioman_beta5.asm -o=/dd/sourcecode/asm/nitros9/mods/ioman l w132 >-ioman.listing #56k |
| `ASM/NITROS9/PIPE/makepipeman` | asm pipeman_beta6.asm -o=/dd/sourcecode/asm/nitros9/pipe/pipeman_beta6 w106 l #56k >-pipeman_beta6.listing |
| `ASM/NITROS9/RBF/makeemudsk` | asm emudsk_beta601.asm -o=/dd/sourcecode/asm/nitros9/rbf/emudsk_beta601 l #56k >-emudsk_beta601.listing |
| `ASM/NITROS9/RBF/makerammer` | asm rammer_2mb_beta6.asm -o=/dd/sourcecode/asm/nitros9/rbf/rammer_beta6 l w106 #48k >-rammer_beta6.listing |
| `ASM/NITROS9/SCF/makecovdg` | asm covdg_ver100.asm -o=/dd/sourcecode/asm/nitros9/scf/covdg_ver100 l w106 #56k >-covdg_ver100.listing |
| `ASM/NITROS9/SCF/makecowin` | asm cowin_beta6.asm -o=/dd/sourcecode/asm/nitros9/scf/cowin_beta6 w 106 l #56k >-cowin_beta6.listing |
| `ASM/NITROS9/SCF/makejoydrv` | asm joydrv_joy_beta6.asm -o=/dd/sourcecode/asm/nitros9/scf/joydrv_joy_beta6 l -w105 #48k >-joydrv_joy_beta6.listing |
| `ASM/NITROS9/SCF/makescf` | asm scf_ver100.asm -o=/dd/sourcecode/asm/nitros9/scf/scf_ver100 l w106 #56k >-scf_ver100.listing |
| `ASM/NITROS9/SCF/makesnddrv` | asm snddrv_beta6.asm -o=/dd/sourcecode/asm/nitros9/scf/snddrv_beta6 l #48k >-snddrv_beta6.listing |
| `ASM/NITROS9/SCF/makevtio` | asm vtio_beta6.asm -o=/dd/sourcecode/asm/nitros9/scf/vtio_beta6 w105 l #56k >-vtio_beta6.listing |
| `ASM/SHELL/makeshell` | asm shellplus2.2a.asm -o=/dd/sourcecode/asm/shell/shell l w106 #56k >-shell.listing |
| `ASM/VEFIO-WINFO/makefile` | include ../../../rules.mak |
| `ASM/VEFIO-WINFO/makevefio` | asm vefio.asm l w106 -o=/dd/sourcecode/asm/vefio-winfo/vefio >-vefio.listing #56k |
| `ASM/VEFIO-WINFO/makewinfo` | asm winfo.asm l w106 -o=/dd/sourcecode/asm/vefio-winfo/winfo >-winfo.listing #56k |
| `C/MAMOU/makefile` | # Makefile for Mamou |
| `C/RMA/makefile` | * Makefile to compile RMA |
| `C/SDCCMDR/Makefile` | CC = dcc |

## Targeted archive investigation

Only two GIMEX ZIP archives and the two GrfDrv LZH archive directories were investigated; no bulk extraction was attempted. GIMEX members were read in memory for hashes/newline-normalized comparisons and their definitions file.

| Archive member | Comparison |
|---|---|
| `ASM/NITROS9/GIMEX/gimex_src_frombill08-25.zip!boot_common.asm` | different after newline normalization to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/boot_common.asm`; newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/boot_common.asm` |
| `ASM/NITROS9/GIMEX/gimex_src_frombill08-25.zip!boot_sdc.asm` | newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/boot_sdc.asm`; newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/boot_sdc.asm` |
| `ASM/NITROS9/GIMEX/gimex_src_frombill08-25.zip!cocosdc.d` | newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/cocosdc.d`; newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/cocosdc.d` |
| `ASM/NITROS9/GIMEX/gimex_src_frombill08-25.zip!deffile` | different after newline normalization to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/deffile`; newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/deffile` |
| `ASM/NITROS9/GIMEX/gimex_src_frombill08-25.zip!rbsuper.d` | newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/rbsuper.d`; newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/rbsuper.d` |
| `ASM/NITROS9/GIMEX/gimex_src_frombill08-25.zip!rel.asm` | different after newline normalization to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/rel.asm`; newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/rel.asm` |
| `ASM/NITROS9/GIMEX/GIMEX_source.zip!boot_common.asm` | different after newline normalization to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/boot_common.asm`; newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/boot_common.asm` |
| `ASM/NITROS9/GIMEX/GIMEX_source.zip!boot_sdc.asm` | newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/boot_sdc.asm`; newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/boot_sdc.asm` |
| `ASM/NITROS9/GIMEX/GIMEX_source.zip!cocosdc.d` | newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/cocosdc.d`; newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/cocosdc.d` |
| `ASM/NITROS9/GIMEX/GIMEX_source.zip!deffile` | different after newline normalization to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/deffile`; newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/deffile` |
| `ASM/NITROS9/GIMEX/GIMEX_source.zip!gimexcheck.a` | newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/gimexcheck.a` |
| `ASM/NITROS9/GIMEX/GIMEX_source.zip!llcocosdc.asm` | newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/llcocosdc.asm` |
| `ASM/NITROS9/GIMEX/GIMEX_source.zip!rbsuper.d` | newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/rbsuper.d`; newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/rbsuper.d` |
| `ASM/NITROS9/GIMEX/GIMEX_source.zip!rel.asm` | newline-normalized equal to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/rel.asm`; different after newline normalization to `/dd/SOURCECODE/ASM/NITROS9/GIMEX/NEW/rel.asm` |

All six members of `gimex_src_frombill08-25.zip` equal their `GIMEX/NEW` counterparts after CRLF/CR normalization; raw hashes differ. The older ZIP’s rel.asm instead matches the loose GIMEX/rel.asm after newline normalization. The archive deffile sets GIMEX=1, while current `/dd/DEFS/deffile` sets GIMEX=0. These are real variant boundaries, not files to merge indiscriminately.

`ASM/grfdrv.lzh` directory: readme.txt, grfdrv, view, readme.org, megafont.bas, iso_latin1.fnt, ibm_edc.fnt. `ASM/grfdrv0724_1998.lzh` directory: grf.a, grf.3.a, grfdrv, read.me. Host tar listed both, but reading their readme members failed with **Unsupported lzh compression method -lh1-**. No member source/binary content match is claimed. A compatible read-only LH1 decoder is a follow-up; current GrfDrv source remains unresolved.

## Conflicts requiring attention

- Modern versus missing legacy definitions; different H6309/GIMEX/Width settings across source and archive variants.
- Shell edition 22 artifact versus current edition 23; multiple filenames do not establish build chronology.
- Clock2 shared names/edition across incompatible provider implementations; both CPU-named messemu artifacts are byte-identical.
- `syscall_6309.asm` deliberately forces H6309=0; historical C wrapper ABI/include assumptions remain unresolved.
- RBF deletion warning, DW ARDUINO no-timeout comment, native/FIRQ sound TODOs and derived disassembly comments are evidence of limitations, not validated current fixes.
- License rights, reconstructed/disassembled source authenticity, exact upstream commits and complete toolchain reproducibility are still open.
