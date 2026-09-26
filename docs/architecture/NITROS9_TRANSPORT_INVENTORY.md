# NitrOS-9 EOU transport inventory

## Evidence boundary

This records modules **inside the stock `63SDC.VHD`** and distinguishes the root SDC boot, the separate emulator boot floppy, and boot alternatives stored as files. Module presence on disk does not imply a loaded driver, an open guest device, a running DriveWire server, or a working shell. No live guest session was used. The EOU guides are [beginner documentation](../../media/NitrOS9_Ease_of_Use-Beginners_documentation_(VERSION1.0.1).rtf) and [environment-file documentation](../../media/ENV_FILE_DOCUMENTATION.rtf); the current MCP launch is in [mame-process.ts](../../MCP/src/mame-process.ts). Upstream MAME source explicitly configures the CoCo 3 [Becker device, RS-232 port and virtual hard drives](https://github.com/mamedev/mame/blob/master/src/mame/trs/coco3.cpp) and the [Becker device connects to a DriveWire server over TCP](https://github.com/mamedev/mame/blob/master/src/devices/bus/coco/coco_dwsock.cpp). These MAME capabilities are separate from what the current MCP configures and what this guest boots.

## Selected boot versus stored alternatives

| Boot source | Storage modules | Console-related modules found by `os9 ident -s` | Interpretation |
| --- | --- | --- | --- |
| VHD `/OS9Boot` = `/BOOTS/OS9Boot.sdc` | `RBSuper`, `llcocosdc`, `DD`, `H1`, `rb1773`, `D0`–`D2` | `SCF`, `VTIO`, `Term`, `W` and `W1`–`W15`, `scbbp`, `p` | Stock root SDC boot. `Term` identifies `SCF` + `VTIO`; no DriveWire or serial shell modules are included. |
| Companion `63EMU.DSK` `/OS9Boot` = VHD `/BOOTS/OS9Boot.emusoftclock` | `EmuDsk`, emulator `DD`/`H1`, `rb1773`, `D0`–`D2` | Same `SCF`, `VTIO`, `Term`, window set, `scbbp`, `p` | Documented emulator boot. It also retains the local video/keyboard console. |
| VHD `/BOOTS/OS9Boot.dw` | Adds `rbdw`, `dwio`, `X0`–`X3`; still includes SDC driver | Adds `scdwv`, `N`, `N1`–`N4`, `MIDI`, `Z1`–`Z3`, `scdwp`; still includes `SCF`, `VTIO`, `Term`, windows | Alternative DriveWire boot stored on disk. The EOU guide labels `.dw` as real-hardware CocoSDC plus back-of-CoCo serial/bitbanger DriveWire. `Term` is still `VTIO`, so this does not establish a remote login console. |
| VHD `/BOOTS/OS9Boot.dw_rs232pak` | Same categories as `.dw`, with a different `dwio` CRC | Same categories as `.dw`; `Term` still `VTIO` | Alternative boot for a 6551 RS-232 Pak, according to the EOU guide. Different `dwio` bytes support a separate transport build, but the merged module names alone do not prove runtime wiring. |
| VHD `/BOOTS/OS9Boot.emuhwareclock` | `EmuDsk` and emulated storage descriptors | `Term`/`VTIO` local console | Emulator clock variant; not the byte-identical companion floppy boot. |

The root `/startup` is byte-identical to `/BOOTS/startup.sdc`. `/SYS/env.file` is byte-identical to `/BOOTS/env.file.sdc`. The latter sets `RBFDEV=/DD,/H1,/D0`, comments out `/X0`, and sets `CONSHELL=Shell`, `CONSHPRM=i=/1`, `CONSTRT=startup -p`, `CONDVTYP=1`, `CONXSIZ=40`, `CONYSIZ=25`. The EOU environment-file guide calls this the 40×25 `/term` window. Thus the **normal boot console appears to be `Term` via `SCF`/`VTIO` on the local display and keyboard**. This is an image/configuration conclusion, not a live console test. The `SCFDEV=/p,/t1` line configures GShell's device list; it does not load `/t1` into the current boot. The banner's “with DriveWire 4” text is similarly not evidence of an active DriveWire boot.

## Relevant module and descriptor files

All paths below are in `/MODULES/6309L2/MODULES/` unless shown otherwise. These are filesystem files available for building or swapping a boot.

| Area | Inventory | What can be established |
| --- | --- | --- |
| RBF storage | `RBF/rbf.mn`, `RBF/rbsuper.dr`, `RBF/llcocosdc.dr`, `RBF/ddsd0_cocosdc.dd`, `RBF/sd0_cocosdc.dd`, `RBF/rb1773.dr`, `RBF/d0_35s.dd`, `RBF/rbdw.dr`, `RBF/dwio.sb`, `RBF/dwio_becker.sb`, `RBF/dwio_rs232pak.sb`, `RBF/x0.dd`–`x3.dd` | The image has CocoSDC, floppy and DriveWire disk-module choices. `dwio_becker.sb` exists as a candidate, but no shipped `BOOTS/OS9Boot.*` name identifies a Becker boot and neither selected boot contains `dwio`. An `/X0` RBF descriptor is a disk path, not a console. |
| Local video and printer SCF | `SCF/scf.mn`, `SCF/vtio_beta6.dr`, `SCF/term_vdg.dt`, `SCF/term_win40.dt`, `SCF/term_win80.dt`, `SCF/w*.dw`, `SCF/scbbp.dr`, `SCF/p_scbbp.dd` | Active boots have `SCF`, `VTIO`, `Term`, windows and `scbbp`/`p`. The active `Term` descriptor reports driver `VTIO`. Printer `/p` is not a command console. |
| DriveWire virtual SCF | `SCF/scdwv.dr`, `SCF/n_scdwv.dd`, `SCF/n1_scdwv.dd` through `n13_scdwv.dd`, `SCF/z1_scdwv.dd` through `z7_scdwv.dd`, `SCF/midi_scdwv.dd`, `SCF/term_scdwv.dt`, `SCF/term_z_scdwv.dt`, `SCF/scdwp.dr`, `SCF/p_scdwp.dd` | The `.dw` boot includes `scdwv`, some `N`/`Z` descriptors and `scdwp`; the stock SDC and emulator boots do not. `term_scdwv.dt` identifies a `Term` descriptor whose driver is `scdwv`, but it is a library file and not the selected `Term`. A host-side virtual-port service or shell binding was not demonstrated. |
| Serial and UART SCF | `SCF/scbbt.dr`, `SCF/t1_scbbt.dd`, `SCF/term_scbbt.dt`, `SCF/sc6551.dr`, `SCF/t2_sc6551.dd`, `SCF/t3_sc6551.dd`, `SCF/term_sc6551.dt`, `SCF/sspak.dr`, `SCF/ssp.dd` | `t1_scbbt.dd` identifies `T1` as `SCF` + `scbbt`; `t2_sc6551.dd` identifies `t2` as `SCF` + `sc6551`; `term_sc6551.dt` identifies a possible serial `Term`. None of these appear in the selected merged boots inspected. A module file is not proof that a physical UART card is installed or emulated. |
| Boot tracks and clocks | `BOOTTRACK/boot_dw`, `boot_dw_becker`, `boot_dw_rs232pak`, `boot_sdc`, `boot_emu`; `CLOCKS/clock2_dw` | Build components exist for transport-specific boot tracks and clocks. The `boot_dw_becker` library component alone is not a complete active Becker boot image. |
| Commands and configuration | `/CMDS/dw`, `/CMDS/DWCmdTest`, `/CMDS/vdrive`, `/CMDS/inetd`, `/CMDS/telnet`, `/SYS/inetd.conf`, `/BOOTS/env.file.dw` | Utility and config files exist. `/SYS/inetd.conf` contains 6809/telnet and 8080/httpd entries, but no observed startup script launches `inetd`; no network console is established by inventory alone. |

`/DEFS/drivewire.d`, `/DEFS/cocosdc.d`, `/DEFS/rbf.d`, and `/DEFS/scf.d` are development definitions. They must not be counted as loadable drivers. The image contains source and documentation trees as well as binary modules, but this audit does not prove that any optional transport can presently accept or return a shell command.

## Implications for console control

The only console route supported by the selected boot-file evidence is local `SCF`/`VTIO` display and keyboard. The DriveWire alternatives add disk and virtual serial facilities, yet leave `Term` on `VTIO`. A remote command channel would need proof of a usable guest `/N` or other SCF path, a shell or login attached to it, a functioning host server, and completion/output framing. The standalone `term_scdwv.dt` and `term_sc6551.dt` files show that alternate `Term` descriptors have been built, but neither is in the selected merged boot. No missing guest support is inferred from MAME alone.

The current MCP starts `coco3` with `-ext fdc` and a Lua bridge. Its exposed mount operation addresses only `flop1`/`flop2`; it does not configure the MAME virtual hard drive, RS-232 endpoint, Becker server, or guest boot workflow. This is a bridge limitation separate from the VHD contents. The stock EOU guide's emulator boot procedure therefore cannot be executed by the current mount tool alone. See [current bridge flow](MAME_BRIDGE_FLOW.md) and [console design](NITROS9_CONSOLE_DESIGN.md).

## Exact additional image-inspection commands

Commands below supplemented the complete image-command list in [NITROS9_GUEST_ENVIRONMENT.md](NITROS9_GUEST_ENVIRONMENT.md). All `ident` calls only parsed existing module headers.

```sh
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 ident media/63SDC.VHD,MODULES/6309L2/MODULES/RBF/dwio_becker.sb
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 ident media/63SDC.VHD,MODULES/6309L2/MODULES/RBF/dwio_rs232pak.sb
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 ident media/63SDC.VHD,MODULES/6309L2/MODULES/SCF/t1_scbbt.dd
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 ident media/63SDC.VHD,MODULES/6309L2/MODULES/SCF/t2_sc6551.dd
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 ident media/63SDC.VHD,MODULES/6309L2/MODULES/SCF/term_scbbt.dt
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 ident media/63SDC.VHD,MODULES/6309L2/MODULES/SCF/term_sc6551.dt
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 ident media/63SDC.VHD,MODULES/6309L2/MODULES/SCF/term_scdwv.dt
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 ident media/63SDC.VHD,MODULES/6309L2/MODULES/SCF/term_z_scdwv.dt
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 list media/63SDC.VHD,MODULES/6309L2/MODULES/SYSMODS/sysgo_dd
```

## Transport decision table

“Console-capable now?” refers to the selected guest boot/configuration evidenced here. No row claims live end-to-end validation.

| Transport | MAME support | Guest driver | Guest descriptor | Console-capable now? | Guest changes required | Real hardware path |
| --- | --- | --- | --- | --- | --- | --- |
| Local keyboard and video | CoCo 3 screen and keyboard; used by current MCP | `SCF` + `VTIO` in SDC and emulator boots | `Term` → `VTIO`; `W`, `W1`–`W15` | **Appears yes** for the normal shell; live boot not tested | None to use existing local shell; reliable textual capture still needs implementation | CoCo 3 keyboard and display |
| CocoSDC or emulated hard disk | MAME has virtual hard-drive devices; current MCP does not mount them | SDC boot: `RBSuper` + `llcocosdc`; emulator floppy: `EmuDsk` | `DD`, `H1`; SDC library also has `sd0_cocosdc.dd` | **No**: disk transport, not a console | None for the existing disk path; separate console route needed | CocoSDC VHD on SD card |
| DriveWire via back-of-CoCo serial/bitbanger | MAME RS-232 port exists; MCP does not configure a DriveWire server | `.dw` boot: `rbdw` + `dwio`, `scdwv`, `scdwp` | `X0`–`X3`, `N`/`N1`–`N4`, `Z1`–`Z3`; `Term` remains `VTIO` | **No demonstrated remote shell** | Select `.dw` boot; run compatible server; attach shell/login to proven SCF path and verify output framing | Documented `.dw` real-CoCo serial/bitbanger boot |
| DriveWire via 6551 RS-232 Pak | MAME CoCo 3 RS-232 port does not by itself prove emulation of this Pak; MCP has no UART transport | `.dw_rs232pak` boot has distinct `dwio` bytes plus `rbdw`, `scdwv`, `scdwp` | `X0`–`X3`, `N`/`Z` virtual paths; `Term` remains `VTIO` | **No demonstrated remote shell** | Select `.dw_rs232pak`; configure Pak/server; attach and validate shell path | EOU documents 6551 Pak in MPI |
| DriveWire via Becker port | MAME Becker TCP device exists; MCP does not manage server or socket | `dwio_becker.sb`, `boot_dw_becker` are stored library files; absent from selected boots | Generic `x0.dd`–`x3.dd` and `N` descriptors are available, but no Becker-specific active descriptor established | **No** | Build/select Becker boot using stored modules; configure server; prove disk/SCF paths and shell routing | Requires verified Becker-compatible hardware and guest boot; not established by this image |
| Direct SCF serial terminal | MAME RS-232 port exists; a 6551 Pak in the chosen machine configuration is unverified | Library `scbbt.dr`, `sc6551.dr`; absent from selected boots | Library `T1`, `t2`, `t3`, `term_scbbt.dt`, `term_sc6551.dt`; selected `Term` is `VTIO` | **No** | Include matching driver/descriptor in boot; configure serial endpoint and bind shell/login | CoCo serial/bitbanger or verified 6551 Pak, with matching guest modules |

