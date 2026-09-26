# Stock NitrOS-9 EOU guest environment

## Scope and evidence

This is a read-only inventory of `media/63SDC.VHD` (EOU 6309, version 1.0.1). No emulator was started and neither disk image was mounted. The requested `media/63SDC-MCP-DEV.VHD` was absent at the stated path, so only the stock VHD was examined. The companion `media/63EMU.DSK` was read to identify the documented emulator boot path. The stock VHD SHA-256 before and after inspection was `db2f0f444073d3b88a3610a63ec9f7d2e2dd4299347181faa6b2b2aa9637de2c`.

The image itself and the bundled [EOU beginner guide](../../media/NitrOS9_Ease_of_Use-Beginners_documentation_(VERSION1.0.1).rtf) and [environment-file guide](../../media/ENV_FILE_DOCUMENTATION.rtf) are the primary evidence. `MCP/Documents/` and `DOCS_INDEX.md` mentioned by the repository instructions are absent. Image contents establish *available files* and boot-image composition, not that a particular peripheral works at runtime.

## Container and filesystem layout

The 134,212,608-byte VHD is exactly 524,268 × 256 bytes. ToolShed `os9 id media/63SDC.VHD` reads the OS-9 RBF identification sector directly at the beginning of the file; no partition offset, partition-table selection, or VHD-header stripping was needed. It reports:

| RBF field | Value |
| --- | --- |
| Name | `NitrOS-9 EOU 6309` |
| Sectors / bytes per sector | 524,268 / 256 |
| Sectors per cluster | 1 |
| Root directory sector | 257 |
| Allocation bitmap bytes | 65,534 |
| Track size / sectors per track | 18 / 18 |
| Boot sector / bootfile size | 48,335 / 34,560 bytes |
| Disk format field | `$2`, displayed by ToolShed as 48 TPI, double density, single sided |
| Creation date | 2020-04-06 20:11 |

The apparent geometry field should not be treated as a physical 128 MB floppy geometry. `os9 dir` and `os9 ident` read directories and modules successfully. There is no conventional MBR or GPT partition table evidenced by these checks. The EOU guide calls this a “skitzo” image with a small BASIC area. `os9 dir` sees only the RBF namespace, so it cannot list the BASIC directory; that is a filesystem-format limit, not failure to open the VHD. As a second read-only check, ToolShed `decb dir media/63SDC.VHD` reports disk `NitrOS-9 EOU` and one `AUTO.BAS` entry (`0 B 1`). This is a mixed DECB/RBF layout in one image, not evidence of two offset-based partitions. `decb list media/63SDC.VHD,AUTO.BAS` exited 139 (segmentation fault), so the BASIC program's content remains unverified. No copy, repair, format, or mount command was used.

The EOU guide says the emulator boot pair is `63EMU.DSK` in floppy 0 plus `63SDC.VHD` in hard drive 0, followed by `DOS`. The companion floppy is a separate 720-sector RBF image containing a 33,284-byte `OS9Boot`. ToolShed `os9 cmp` found it byte-identical to the VHD's `BOOTS/OS9Boot.emusoftclock`. This distinction matters: the root VHD `OS9Boot` is the **SDC** variant, while the documented emulator path boots from the floppy image.

## Filesystem inventory

`os9 dir -r -e` traversed 380 directories and reported 7,105 entries: 379 directory entries and 6,726 file entries. The large content collection spans applications, games, documentation, source, images, music, and utilities. Principal top-level directories are `APPS`, `BBS`, `BOOTS`, `CMDS`, `DEFS`, `DOCS`, `GAMES`, `KERNEL_TRACKS`, `MODULES`, `SOURCECODE`, and `SYS`. Relevant subtree sizes, including directories: `SOURCECODE` 2,439; `CMDS` 1,248; `SYS` 926; `GAMES` 807; `MODULES` 246; `DEFS` 101; `BOOTS` 16. These are inventory counts, not a filesystem integrity check.

| Path in RBF filesystem | Inventory and role |
| --- | --- |
| `/OS9Boot` | 34,560-byte merged boot file, byte-identical to `/BOOTS/OS9Boot.sdc`. The RBF bootfile size matches it. |
| `/sysgo` | 5,305-byte executable OS-9 module; ToolShed `ident` reports good CRC. |
| `/startup` | 599-byte text script, byte-identical to `/BOOTS/startup.sdc`. Prints EOU banner, prompts for date/time with `setime<>>>/1`, starts shells on `/w1` and `/w2`, loads fonts and patterns, and gives the `GSHELL` instruction. Its “with DriveWire 4” banner is not proof that the selected boot contains DriveWire modules. |
| `/BOOTS` | Five boot sets: `.sdc`, `.dw`, `.dw_rs232pak`, `.emusoftclock`, `.emuhwareclock`, with corresponding startup files and environment files (except startup naming follows each set). The guides describe `SwapBoot` as selecting these sets. |
| `/CMDS` | 1,246 files under the command subtree. Relevant examples: `Shell`, `GShell`, `AutoEx`, `SwapBoot`, `sdc`, `sdcdriver`, `DWCmdTest`, `dw`, `vdrive`, `inetd`, `telnet`, `reboot`. Presence does not mean started. |
| `/SYS` | 894 files under its subtree, including `env.file`, `inetd.conf`, `motd`, `TermCap`, font and pattern data. `inetd.conf` lists ports 6809 and 8080, but `/startup` does not launch `inetd`. |
| `/DEFS` | 100 files under its subtree, including `cocosdc.d`, `drivewire.d`, `rbf.d`, `scf.d`, `cocovtio.d`, `os9.d`, C headers and assembler definitions. These are definition files, not loaded guest device drivers. |
| `/MODULES/6309L2` | `BOOTLISTS`, `SCRIPTS`, and module libraries split into `BOOTTRACK`, `CLOCKS`, `KERNEL`, `PIPE`, `RBF`, `SCF`, `SYSMODS`, and `CMDS`. Library modules are options for building a boot; they are not all present in the current merged `OS9Boot`. |
| `/KERNEL_TRACKS` | Boot track and kernel build material, retained on the filesystem. |

### Boot and startup configuration

The current `/SYS/env.file` is byte-identical to `/BOOTS/env.file.sdc`. Its active settings include `RBFDEV=/DD,/H1,/D0`, `SCFDEV=/p,/t1`, `DATA=/dd`, `EXEC=/dd/cmds`, `CONDVTYP=1`, `CONXSIZ=40`, `CONYSIZ=25`, `CONSHELL=Shell`, `CONSHPRM=i=/1`, `CONSTRT=startup -p`, and `CONAUTO=AutoEx`. The `/X0` DriveWire RBF entry is commented out. `SCFDEV` is a GShell menu setting according to the EOU environment-file guide; `/t1` there does **not** establish that a `/t1` descriptor is in the selected boot image. The `.dw` environment file switches the GShell RBF list to `/DD,/H1,/X0`.

ToolShed `ident -s` of the **root VHD** `OS9Boot` lists `RBF`, `RBSuper`, `llcocosdc`, `DD`, `H1`, `rb1773` with `D0`–`D2`, `SCF`, `VTIO`, `Term`, window descriptors `W` and `W1`–`W15`, and printer driver/descriptor `scbbp`/`p`. It contains no `rbdw`, `dwio`, `X0`, `scdwv`, `N`, `sc6551`, `scbbt`, or `T1` module. `ident` of the emulator floppy `OS9Boot` lists `EmuDsk` and emulator `DD`/`H1` instead of the SDC storage driver, but the same `SCF`, `VTIO`, `Term`, and windows. The `Term` descriptor in the root boot declares file manager `SCF` and driver `VTIO`. The normal boot console therefore appears to be the local video/keyboard `Term` path, with the interactive shell configured through `i=/1` on that window. This is a boot-file/configuration conclusion; no live boot was performed.

## Exact image-inspection commands

Commands were run from the repository root. `os9` and `decb` are the native ToolShed 2.2 executables. Output redirected to `/private/tmp` only; no image write command was used. The `decb list` failure is included for reproducibility.

```sh
stat -f '%z bytes %N' media/63SDC.VHD media/63EMU.DSK
sha256sum media/63SDC.VHD
xxd -l 256 media/63SDC.VHD
tail -c 512 media/63SDC.VHD | xxd
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 id media/63SDC.VHD
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 dir -e media/63SDC.VHD,
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 dir -r -e media/63SDC.VHD, > /private/tmp/63sdc-os9-recursive.txt
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 dir -e media/63SDC.VHD,BOOTS
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 dir -e media/63SDC.VHD,SYS > /private/tmp/63sdc-sys.txt
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 dir -e media/63SDC.VHD,DEFS > /private/tmp/63sdc-defs.txt
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 dir -e media/63SDC.VHD,CMDS > /private/tmp/63sdc-cmds.txt
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 list media/63SDC.VHD,startup
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 list media/63SDC.VHD,SYS/env.file
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 list media/63SDC.VHD,SYS/inetd.conf
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 list media/63SDC.VHD,BOOTS/startup.dw
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 list media/63SDC.VHD,BOOTS/startup.dw_rs232pak
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 list media/63SDC.VHD,BOOTS/env.file.dw
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 ident media/63SDC.VHD,OS9Boot
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 ident -s media/63SDC.VHD,OS9Boot
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 ident -s media/63SDC.VHD,BOOTS/OS9Boot.dw
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 ident -s media/63SDC.VHD,BOOTS/OS9Boot.dw_rs232pak
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 ident -s media/63SDC.VHD,BOOTS/OS9Boot.emuhwareclock
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 ident media/63SDC.VHD,sysgo
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 cmp media/63SDC.VHD,OS9Boot media/63SDC.VHD,BOOTS/OS9Boot.sdc
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 cmp media/63SDC.VHD,startup media/63SDC.VHD,BOOTS/startup.sdc
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 cmp media/63SDC.VHD,SYS/env.file media/63SDC.VHD,BOOTS/env.file.sdc
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/decb/decb dir media/63SDC.VHD
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/decb/decb list media/63SDC.VHD,AUTO.BAS
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 id media/63EMU.DSK
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 dir -e media/63EMU.DSK,
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 ident -s media/63EMU.DSK,OS9Boot
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 cmp media/63EMU.DSK,OS9Boot media/63SDC.VHD,BOOTS/OS9Boot.emusoftclock
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 cmp media/63EMU.DSK,OS9Boot media/63SDC.VHD,BOOTS/OS9Boot.emuhwareclock
```

Additional `os9 ident` commands for individual transport modules are recorded in [NITROS9_TRANSPORT_INVENTORY.md](NITROS9_TRANSPORT_INVENTORY.md). File search, counting, source inspection, and `textutil -convert txt -stdout` on bundled RTF documentation were read-only host operations. The final `sha256sum` matched the initial value above.
