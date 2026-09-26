# NitrOS-9 EOU live boot investigations

**Current baseline:** see [MPI / Disto RTC follow-up](#mpi--disto-rtc-follow-up). The earlier `nos9_ready_test` checkpoint is historical; the final checkpoint is `nos9_ready`.

Executed September 25, 2026, America/Los_Angeles (tool log UTC crosses into September 26).

## Outcome and scope

EOU booted successfully from `63EMU.DSK` plus **only** `63SDC-MCP-DEV.VHD`. The stock `63SDC.VHD` was never mounted. The native 6309 / 2048K banner, working shell, and save/restore were observed. All three media hashes were unchanged after the boot investigation and clean MCP stop. No MCP source or new MCP tools were changed; no commit was made.

During the investigation the user requested correcting the guest clock with `setime`, then appending `montype r` to startup. The clock correction was performed in the running guest. The startup edit was performed **after** MAME stopped and after the boot-only hashes were recorded, on the development image only. Its distinct hash is recorded below. The startup edit has been verified by readback, but has not been boot-tested.

## Media safety and hashes

Before launch, `stat` confirmed stock mode 444, with no write bits. Sizes and modes remained unchanged after the boot test and startup edit.

| Media | Bytes | Permissions | Before SHA-256 | After boot/restore/stop SHA-256 | After authorized startup edit |
|---|---:|---|---|---|---|
| `63SDC.VHD` | 134212608 | `-r--r--r--` / 444 | `db2f0f444073d3b88a3610a63ec9f7d2e2dd4299347181faa6b2b2aa9637de2c` | `db2f0f444073d3b88a3610a63ec9f7d2e2dd4299347181faa6b2b2aa9637de2c` | `db2f0f444073d3b88a3610a63ec9f7d2e2dd4299347181faa6b2b2aa9637de2c` |
| `63SDC-MCP-DEV.VHD` | 134212608 | `-rw-r--r--` / 644 | `db2f0f444073d3b88a3610a63ec9f7d2e2dd4299347181faa6b2b2aa9637de2c` | `db2f0f444073d3b88a3610a63ec9f7d2e2dd4299347181faa6b2b2aa9637de2c` | `2d4aa91f0683438ba5a56e5d3b15562958586d3fe99d9a6e705965d91cdafba5` |
| `63EMU.DSK` | 184320 | `-rw-rw-r--` / 664 | `0e29356d5897f733cad935c899a9a8dbb779f2887f5ee5476abec46afdda6763` | `0e29356d5897f733cad935c899a9a8dbb779f2887f5ee5476abec46afdda6763` | `0e29356d5897f733cad935c899a9a8dbb779f2887f5ee5476abec46afdda6763` |

Normal boot, clock setting, directory/module/process queries, save, restore, and shutdown produced **no persistent byte changes** in either mounted image. This is a result for this run, not a guarantee for arbitrary guest commands.

## Device discovery and canonical configuration

Installed binary: `/Applications/Emulators/Ample.app/Contents/MacOS/mame64`, MAME 0.289 / Ample. Discovery used `coco3h -listmedia`, `-listslots`, `-listdevices`, `-listxml`, and `-showconfig`. The final media arguments were also accepted by a read-only `-listmedia` invocation.

- `floppydisk1` / `flop1` supports `.dsk`; it selects floppy drive 0.
- `harddisk1` / `hard1` supports `.vhd`; it selects the built-in CoCo virtual hard disk device `vhd0`.
- `ext` has the default `fdc` option; device enumeration shows WD1773 and 5.25-inch DD floppy connectors. No SDC/IDE cartridge or multipak was needed for this emulator boot path.
- `listdevices` identifies the main CPU as Hitachi HD6309E. Launch uses `-ramsize 2M`; guest startup displays `CPU type 6309 <Native Mode>` and `Memory size 2048K`.
- RGB uses the existing canonical cfg in `MCP/work/mame-cfg/coco3h.cfg`: `<port tag=":screen_config" type="CONFIG" mask="1" defvalue="0" value="1"/>`. BASIC was visibly bright green. XML discovery labels the selection Monitor Type, Composite 0 / RGB 1. No live port-read tool was used in this investigation; RGB evidence is cfg plus BASIC snapshot, not an independent live-port measurement.

The existing [guest inventory](NITROS9_GUEST_ENVIRONMENT.md) documents the EOU guide's emulator pair and `DOS` boot command. It also establishes that the floppy OS9Boot matches `BOOTS/OS9Boot.emusoftclock`, with `EmuDsk`, `DD`, `H1`, `SCF`, `VTIO`, and `Term`. The VHD's root SDC OS9Boot is not the boot file selected by this floppy-led launch.

### Launch through the existing MCP

The current MCP exposes floppy mounting, but has no hard-disk mounting tool or media launch setting. To preserve source unchanged, a temporary Node launch adapter under `/private/tmp/nos9-live` imports the existing compiled MCP modules and injects a subprocess spawner that appends the two discovered media arguments. `coco_start` still invokes the existing MAME controller, and all guest input/snapshots/save/load/stop use existing MCP tools and the unmodified Lua bridge. This is a diagnostic adapter, not a new product capability. It does not boot the VHD automatically: `DOS` was entered through `coco_type`.

Exact launch command (also in [launch.json](assets/nos9-live/launch.json)):

```sh
BRIDGE_PORT=18765 /Applications/Emulators/Ample.app/Contents/MacOS/mame64 coco3h -window -skip_gameinfo -natural -nomouse -mouse_device none -ext fdc -ramsize 2M -rompath '/Users/magneto-optimus/Library/Application Support/Ample/roms' -autoboot_script /Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/scripts/bridge.lua -autoboot_delay 0 -snapshot_directory /Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots -state_directory /Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/states -statename %g -cfg_directory /Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/work/mame-cfg -flop1 /Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/media/63EMU.DSK -hard1 /Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/media/63SDC-MCP-DEV.VHD
```

MAME subprocess cwd: `/Applications/Emulators/Ample.app/Contents/MacOS`. ROMs are explicitly selected from the Ample support directory. The bridge TCP port is 18765.

## Boot sequence and snapshot evidence

1. MAME starts in Disk Extended Color BASIC 2.1; RGB green screen (snapshot 3).
2. `DOS` begins floppy boot (snapshot 5). The actual first request included a newline and the tool's automatic Enter: `DOS\n{ENTER}`. Subsequent commands use plain text with automatic Enter.
3. White boot screen displays `KREL Boot Krn tb` and `NITROS9 6309` (snapshot 6).
4. Configuration and utility loading (snapshot 7).
5. NitrOS9 Level 2 EOU v1.0.1 banner, 2048K, native 6309, DriveWire/Becker/CoCoSDC detection messages, then clock prompt (snapshot 8). Those transport detection messages do not indicate a failure of this EmuDsk boot.
6. Clock input; windows, fonts, patterns, pointers, and MVCanvas patterns load (snapshots 10, 11, 13).
7. `To run the GUI, simply type GSHELL`, Shell+ v2.2a banner, and `<Term:02>DD:` prompt. `date` then prints a date and returns to the prompt (snapshot 14). No GUI was started.

Initial clock entry mistakenly used `26/09/25 12:00:00` although the prompt specifies `yyyy/mm/dd hh:mm:ss`; `date` reported 1950. At the user's request, `setime` was invoked and supplied `2026/09/25 21:22:00`, using local Pacific time near the actual 04:21:31 UTC tool reading. `date` then reported September 25, 2026. This was an explicit clock input, not host clock synchronization.

### Shell-ready observation

For this fixture, require completion of startup, the Shell+ banner, and the active `<Term:02>DD:` prompt with input cursor; then execute a known read-only probe and observe its output **and a new prompt**. `date` and `pwd` established that commands were actually executing. This is a reliable human-observed readiness criterion for the tested session, not an implemented general detector. Prompt text contains window/process/directory state and should not be globally hard-coded.

`coco_type` success means the natural keyboard queue was drained, not that guest execution completed. An early `date` was queued while graphics startup was still finishing and executed later. Immediate post-input snapshots also sometimes show just the command line. Later snapshots were necessary to see completed output. A generalized automation needs separate startup/clock-prompt/command-running/pager/idle states and bounded timeouts; a quiet screenshot or `posting=false, empty=true` alone cannot prove shell readiness.

## Actual command results

| Command | Actual observed result | Evidence |
|---|---|---|
| `date` | Exists; first date 1950 due to short year input; after `setime`, September 25, 2026; returns prompt | 14, 22 |
| `pwd` | Exists; `/DD`; returns prompt | 19, 31 |
| `dir` | Exists; root listing includes APPS, BOOTS, CMDS, DEFS, DOCS, SYS, OS9Boot, startup, sysgo, etc.; returns prompt | 24 |
| `mdir` | Exists; Module Directory at 21:22:44, including EmuDsk, DD, H1, SCF, VTIO, Term, window modules, Shell, MDir; later `procs` accepted | 26, 28 |
| `procs` | Exists; lists Shell processes (IDs 2, 3, 5, 6) and the running Procs process (ID 4); returns prompt | 28 |
| `setime` | Exists; interactive prompt accepted full year; subsequent `date` confirms correction | 19, 22 |

No tested command was reported missing. MCP tools return queue status and PNG screenshots, not structured guest stdout or guest exit codes. These success conclusions come from observed output, not merely tool success. No memory read/write tools were used.

## Save and restore

After `procs` returned to the idle prompt, `coco_save_state(name="nos9_ready_test")` returned:

```json
{"scheduled":true,"name":"nos9_ready_test","fileFound":true,"file":"/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/states/coco3h/nos9_ready_test.sta"}
```

The file exists, mode 644, 122967 bytes. A subsequent `pwd` printed `/DD` and returned to the prompt (snapshot 31). Loading returned:

```json
{"scheduled":true,"name":"nos9_ready_test"}
```

The restored screen returned to the earlier process-list display and removed the later `pwd` command/output (snapshot 33). The first restored capture did not visibly repaint the bottom prompt, so readiness was verified further by entering `date`: it executed and returned to `<Term:02>DD:` (snapshot 36). MAME retained PID 9826 and the bridge connection. No BASIC screen, boot banner, startup sequence, or date-setting prompt recurred. This supports restoration of the running idle shell without a cold boot; screenshot byte identity was not asserted.

Clean `coco_stop` returned `{"ok":true}`; subsequent status returned `{"running":false,"pid":null,"bridge":false}`.

### Disk activity and consistency limits

Boot visibly loaded kernel/modules, configuration, fonts/patterns, and commands from the attached media. No block-level I/O tracing or guest write counters were collected, so write activity during execution cannot be inferred from screen messages. Final hashes demonstrate no persistent media changes during this trial.

A save state is not a disk-image backup. Disk persistence across arbitrary guest writes/restores was not tested. Restoring an older in-memory filesystem/cache state against a later modified backing image may create inconsistency; no claim of transactional guest-disk restoration is made. In particular, `nos9_ready_test` was saved **before the later authorized startup edit**. Treat it as tied to the original dev-image hash, not as a validated checkpoint for the newly edited image. A new cold boot and new checkpoint are needed to validate `montype r` on startup.

## Authorized startup adjustment after the test

With MAME stopped, ToolShed exported `/startup` (599 bytes, CR endings), and `ident` confirmed `CMDS/montype` is a valid MonType edition 4 module with good CRC. A temporary backup of the development image was made at `/private/tmp/nos9-live/63SDC-MCP-DEV.before-startup.VHD`. ToolShed `copy -r` replaced only the requested startup file content with the original bytes plus `montype r` followed by CR. Export/readback asserted exact equality to that concatenation (609 bytes). Its guest permissions remained `----r-wr`, owner 0.0. The image's filesystem metadata necessarily changes with the replacement. Stock and floppy hashes remain unchanged. This offline edit's runtime effect has not been tested.

## Exact inspection and modification commands

Commands ran from the repository root unless shown otherwise. The first hash/stat commands were repeated after clean stop, and hashes repeated again after the authorized edit.

```sh
stat -f '%Sp %OLp %z %N' media/63SDC.VHD media/63SDC-MCP-DEV.VHD media/63EMU.DSK
sha256sum media/63SDC.VHD media/63SDC-MCP-DEV.VHD media/63EMU.DSK
/Applications/Emulators/Ample.app/Contents/MacOS/mame64 coco3h -listmedia
/Applications/Emulators/Ample.app/Contents/MacOS/mame64 coco3h -listslots
/Applications/Emulators/Ample.app/Contents/MacOS/mame64 coco3h -listdevices
/Applications/Emulators/Ample.app/Contents/MacOS/mame64 coco3h -listxml > /private/tmp/nos9-live/installed.xml
/Applications/Emulators/Ample.app/Contents/MacOS/mame64 coco3h -showconfig > /private/tmp/nos9-live/showconfig.txt
/Applications/Emulators/Ample.app/Contents/MacOS/mame64 coco3h -ext fdc -flop1 /Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/media/63EMU.DSK -hard1 /Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/media/63SDC-MCP-DEV.VHD -listmedia
node /private/tmp/nos9-live/client.mjs
stat -f '%Sp %z %N' MCP/states/coco3h/nos9_ready_test.sta
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 list media/63SDC-MCP-DEV.VHD,startup
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 copy -h
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 dir -e media/63SDC-MCP-DEV.VHD,CMDS/montype
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 copy media/63SDC-MCP-DEV.VHD,startup /private/tmp/nos9-live/startup.original
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 ident media/63SDC-MCP-DEV.VHD,CMDS/montype
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 dir -e media/63SDC-MCP-DEV.VHD,
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 attr -h
cp media/63SDC-MCP-DEV.VHD /private/tmp/nos9-live/63SDC-MCP-DEV.before-startup.VHD
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 copy -r /private/tmp/nos9-live/startup.updated media/63SDC-MCP-DEV.VHD,startup
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 copy media/63SDC-MCP-DEV.VHD,startup /private/tmp/nos9-live/startup.verified
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 attr media/63SDC-MCP-DEV.VHD,startup
```

The `dir` attempt on `CMDS/montype` returned error 214 because it is a file, not a directory. `ident` succeeded; the error did not establish that the command was missing. ToolShed's `TSCopyFile` source was inspected for overwrite and metadata behavior before writing. Temporary byte construction/readback:

```python
original = Path('/private/tmp/nos9-live/startup.original').read_bytes()
Path('/private/tmp/nos9-live/startup.updated').write_bytes(original + b'montype r\r')
assert Path('/private/tmp/nos9-live/startup.verified').read_bytes() == original + b'montype r\r'
```

## Complete MCP call record

Raw exact MCP results include every request and result, including image base64. PNGs linked above are the decoded images. Every MCP call completed without an MCP error; guest completion is assessed separately above. Snapshot calls all returned an image plus the exact text path `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png`. The unique archived images avoid that path's overwrite behavior.

| # | MCP tool | Arguments | Exact returned text / image evidence |
|---:|---|---|---|
| 1 | `coco_start` | `{}` | `{"ok":true,"alreadyRunning":false,"pid":9826,"bridge":true}` |
| 2 | `coco_status` | `{}` | `{"running":true,"pid":9826,"bridge":true,"driver":"coco3h","flop1":"/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/media/63EMU.DSK","posting":false,"empty":true}` |
| 3 | `coco_snapshot` | `{}` | PNG (exact encoded result in JSONL); `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 4 | `coco_type` | `{"text": "DOS\n"}` | `{"queued":true,"text":"DOS\n{ENTER}"}` |
| 5 | `coco_snapshot` | `{}` | PNG (exact encoded result in JSONL); `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 6 | `coco_snapshot` | `{}` | PNG (exact encoded result in JSONL); `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 7 | `coco_snapshot` | `{}` | PNG (exact encoded result in JSONL); `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 8 | `coco_snapshot` | `{}` | PNG (exact encoded result in JSONL); `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 9 | `coco_type` | `{"text": "26/09/25 12:00:00"}` | `{"queued":true,"text":"26/09/25 12:00:00{ENTER}"}` |
| 10 | `coco_snapshot` | `{}` | PNG (exact encoded result in JSONL); `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 11 | `coco_snapshot` | `{}` | PNG (exact encoded result in JSONL); `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 12 | `coco_type` | `{"text": "date"}` | `{"queued":true,"text":"date{ENTER}"}` |
| 13 | `coco_snapshot` | `{}` | PNG (exact encoded result in JSONL); `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 14 | `coco_snapshot` | `{}` | PNG (exact encoded result in JSONL); `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 15 | `coco_type` | `{"text": "pwd"}` | `{"queued":true,"text":"pwd{ENTER}"}` |
| 16 | `coco_snapshot` | `{}` | PNG (exact encoded result in JSONL); `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 17 | `coco_type` | `{"text": "setime"}` | `{"queued":true,"text":"setime{ENTER}"}` |
| 18 | `coco_snapshot` | `{}` | PNG (exact encoded result in JSONL); `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 19 | `coco_snapshot` | `{}` | PNG (exact encoded result in JSONL); `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 20 | `coco_type` | `{"text": "2026/09/25 21:22:00"}` | `{"queued":true,"text":"2026/09/25 21:22:00{ENTER}"}` |
| 21 | `coco_type` | `{"text": "date"}` | `{"queued":true,"text":"date{ENTER}"}` |
| 22 | `coco_snapshot` | `{}` | PNG (exact encoded result in JSONL); `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 23 | `coco_type` | `{"text": "dir"}` | `{"queued":true,"text":"dir{ENTER}"}` |
| 24 | `coco_snapshot` | `{}` | PNG (exact encoded result in JSONL); `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 25 | `coco_type` | `{"text": "mdir"}` | `{"queued":true,"text":"mdir{ENTER}"}` |
| 26 | `coco_snapshot` | `{}` | PNG (exact encoded result in JSONL); `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 27 | `coco_type` | `{"text": "procs"}` | `{"queued":true,"text":"procs{ENTER}"}` |
| 28 | `coco_snapshot` | `{}` | PNG (exact encoded result in JSONL); `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 29 | `coco_save_state` | `{"name": "nos9_ready_test"}` | `{"scheduled":true,"name":"nos9_ready_test","fileFound":true,"file":"/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/states/coco3h/nos9_ready_test.sta"}` |
| 30 | `coco_type` | `{"text": "pwd"}` | `{"queued":true,"text":"pwd{ENTER}"}` |
| 31 | `coco_snapshot` | `{}` | PNG (exact encoded result in JSONL); `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 32 | `coco_load_state` | `{"name": "nos9_ready_test"}` | `{"scheduled":true,"name":"nos9_ready_test"}` |
| 33 | `coco_snapshot` | `{}` | PNG (exact encoded result in JSONL); `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 34 | `coco_status` | `{}` | `{"running":true,"pid":9826,"bridge":true,"driver":"coco3h","flop1":"/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/media/63EMU.DSK","posting":false,"empty":true}` |
| 35 | `coco_type` | `{"text": "date"}` | `{"queued":true,"text":"date{ENTER}"}` |
| 36 | `coco_snapshot` | `{}` | PNG (exact encoded result in JSONL); `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 37 | `coco_stop` | `{}` | `{"ok":true}` |
| 38 | `coco_status` | `{}` | `{"running":false,"pid":null,"bridge":false}` |


## MPI / Disto RTC follow-up

Executed September 25, 2026 Pacific time. Final result: canonical HD6309 / 2M / RGB configuration with MPI, Glenside IDE in slot 3, Disto SCII FDC and Disto RTC in slot 4, EMUHWCLK EOU floppy boot, verified `montype r`, and a newly saved and restored `nos9_ready` baseline. MAME is stopped. No commit was made.

### Verified slot/device syntax and RTC correction

The installed MAME was interrogated with `-listslots`, `-listdevices`, `-listmedia`, and `-listxml`. `coco3h -ext multi -listslots` exposes `ext:multi:slot1` through `slot4`; `ide` is **Glenside IDE Adapter**, internal device `coco_ide`. `multi` is **CoCo Multi-Pak Interface**, internal device `coco_multi`. Slot 3 is literally `-ext:multi:slot3 ide`, not a zero-based index. FDC normally lives in slot 4.

**Glenside IDE in this build does not provide a RTC.** Installed device discovery showed ATA connectors and a passthrough cartridge slot, with a J2 address jumper, but no clock device/configuration. The matching [upstream MAME 0.289 implementation](https://github.com/mamedev/mame/blob/mame0289/src/devices/bus/coco/coco_ide.cpp) was fetched and inspected; `device_add_mconfig` creates ATA and passthrough only. No ATA disk is required to launch it; its additional `hard3` media is optional and was left empty. The EOU VHD remains attached to **hard1**, the built-in `vhd0`, not to the IDE adapter. No IDE disk or ROM was mounted.

The user explicitly approved **Disto** after this finding. The EOU beginner guide, emulator clocks section, recommends selecting Disto for MAME. Installed slot discovery verifies `scii` = Disto Super Controller II and its `meb` option `rtime` = Disto Real Time Clock Card. Device enumeration shows the WD1773 floppy controller plus an **OKI MSM6242 RTC**. This preserves floppy boot while supplying the separate clock; there is no Glenside RTC verification claim.

Exact final topology:

```sh
-ext multi -ext:multi:slot3 ide -ext:multi:slot4 scii -ext:multi:slot4:scii:meb rtime
```

Slots 1 and 2 remain at MAME's defaults. No extra RTC media or config port selection was needed. The Glenside J2 jumper remains at its default. The built-in SCII RTC was successfully read with this topology, so no speculative address changes were made.

Read-only discoveries:

```sh
/Applications/Emulators/Ample.app/Contents/MacOS/mame64 coco3h -ext multi -listslots
/Applications/Emulators/Ample.app/Contents/MacOS/mame64 coco3h -ext multi -ext:multi:slot3 ide -listdevices
/Applications/Emulators/Ample.app/Contents/MacOS/mame64 coco3h -ext multi -ext:multi:slot3 ide -listmedia
/Applications/Emulators/Ample.app/Contents/MacOS/mame64 coco3h -ext multi -ext:multi:slot3 ide -listslots
/Applications/Emulators/Ample.app/Contents/MacOS/mame64 coco3h -listxml
/Applications/Emulators/Ample.app/Contents/MacOS/mame64 coco3h -ext multi -ext:multi:slot3 ide -ext:multi:slot4 scii -listdevices
/Applications/Emulators/Ample.app/Contents/MacOS/mame64 coco3h -ext multi -ext:multi:slot3 ide -ext:multi:slot4 scii -listslots
/Applications/Emulators/Ample.app/Contents/MacOS/mame64 coco3h -ext multi -ext:multi:slot3 ide -ext:multi:slot4 scii -ext:multi:slot4:scii:meb rtime -listmedia
```

This build's `-listxml` mode rejected the combined dynamic `-ext` invocation as `unknown option: -ext`; unqualified `coco3h -listxml` succeeded and includes device/slot definitions. Configured topology was established with `-listdevices` and `-listslots`, not guessed from XML alone. The existing Ample `cfg/coco3h.cfg` was read and confirmed `:screen_config=1`; it contained the earlier plain FDC setup, not a reusable MPI/RTC setup. Local manual/context files named in repository instructions were absent; primary evidence here is installed MAME discovery, version-matched MAME source, and the included EOU RTF guides.

Discovery XML, [final slots](assets/nos9-rtc/mame-listslots.txt), [final media](assets/nos9-rtc/mame-listmedia.txt).

### Centralized configurable launch settings

`AppConfig` now parses `MAME_SLOTS` as a JSON map of slot name to device option. `buildMameArgs` emits that map in order. Slot names/options are validated; MAME still validates which combinations exist. Empty options permit an explicitly empty slot. Without `MAME_SLOTS`, the prior `ext=fdc` behavior remains available.

Optional `MAME_BOOT_FLOPPY` and `MAME_VHD` settings resolve paths using the existing Windows/POSIX-aware path helper and emit `-flop1` / `-hard1`. They allow this launch to use the actual rebuilt MCP configuration directly; the earlier temporary media-argument injection is no longer needed. The diagnostic wrapper only records the exact command and forwards it unchanged. No new MCP tools were added.

Both `.env.example` and local ignored `.env` select:

```dotenv
MAME_MACHINE=coco3h
COCO_RAM=2M
COCO_MONITOR=rgb
MAME_SLOTS={"ext":"multi","ext:multi:slot3":"ide","ext:multi:slot4":"scii","ext:multi:slot4:scii:meb":"rtime"}
MAME_BOOT_FLOPPY=../media/63EMU.DSK
MAME_VHD=../media/63SDC-MCP-DEV.VHD
```

The RGB cfg still uses `tag=":screen_config"`, value 1. Existing Lua bridge and configured-machine state discovery are unchanged. Arguments are passed as a subprocess array, without shell parsing. Tests retain the old Windows launch assertions and add verified MPI/Disto arguments, media resolution, and invalid slot-map checks.

Exact final launch, cwd `/Applications/Emulators/Ample.app/Contents/MacOS`:

```sh
BRIDGE_PORT=18765 /Applications/Emulators/Ample.app/Contents/MacOS/mame64 coco3h -window -skip_gameinfo -natural -nomouse -mouse_device none -ext multi -ext:multi:slot3 ide -ext:multi:slot4 scii -ext:multi:slot4:scii:meb rtime -ramsize 2M -rompath '/Users/magneto-optimus/Library/Application Support/Ample/roms' -autoboot_script /Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/scripts/bridge.lua -autoboot_delay 0 -snapshot_directory /Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots -state_directory /Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/states -statename %g -cfg_directory /Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/work/mame-cfg -flop1 /Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/media/63EMU.DSK -hard1 /Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/media/63SDC-MCP-DEV.VHD
```

### SwapBoot procedure, mistakes, and recovery

Initial MPI/Disto boot used the software-clock floppy and reached the shell with the prior `montype r` edit visible. `swapboot` with no arguments presented usage/warnings and five discovered boot sets. It warns that it replaces startup, SYS/env.file, and OS9Boot, and advises backups. Before selecting a set, the mounted dev VHD and floppy were copied to `/private/tmp/63SDC-before-rtc.VHD` and `/private/tmp/63EMU-before-rtc.DSK`.

The exact verified menu is:

1. SDC with DriveWire (`DW`).
2. SDC with DriveWire (`DW_RS232PAK`).
3. Emulators, OS9Boot on `/d0` (`EMUHWCLK` / `EMUHWARECLOCK`).
4. Emulators, OS9Boot on `/d0` (`EMUSOFTCLK` / `EMUSOFTCLOCK`).
5. SDC (`SDC`).

**Investigation error:** option 2 was initially misread and selected. The user corrected it to option 3. This temporarily replaced the dev VHD startup/environment/root boot with the wrong variant; it was not accepted as the final configuration. The first corrected option-3 run was also stopped prematurely while still copying the floppy boot file. Offline comparison showed the old software-clock file, so the procedure was rerun fully. These failed/intermediate attempts are preserved in the MCP transcript.

Successful final procedure:

1. At the shell, run `swapboot`.
2. Answer `y` to its warning.
3. Select **3**, the explicitly displayed emulator hardware-clock set.
4. Wait for all updates and `Linking Bootfile`, then the explicit `Press <ENTER> to reboot your Coco 3. Type DOS after Disk Extended Basic comes up...` message. This took substantially longer than keyboard-queue completion; do not stop at `Updating OS9Boot...`.
5. Stop MAME cleanly to preserve the user's startup customization before cold boot.
6. ToolShed compared `/d0/OS9Boot` to `/dd/BOOTS/OS9Boot.emuhwareclock`: **33300 bytes, zero differences**. `/dd/SYS/env.file` also matches `BOOTS/env.file.emuhwareclock` (3391 bytes, zero differences).
7. Export the freshly selected `/dd/startup`, append `montype r` with CR, and replace `/dd/startup` with ToolShed while MAME is stopped. `swapboot` had overwritten the previous customization, as its warning says. Readback confirms the line remains last. No boot-template source file was altered.
8. Start MAME again and type `DOS`. This implements the required cold reboot with the same attachments.

Useful offline commands (ToolShed prefix is `/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9`):

```sh
os9 cmp media/63EMU.DSK,OS9Boot media/63SDC-MCP-DEV.VHD,BOOTS/OS9Boot.emuhwareclock
os9 cmp media/63SDC-MCP-DEV.VHD,SYS/env.file media/63SDC-MCP-DEV.VHD,BOOTS/env.file.emuhwareclock
os9 copy media/63SDC-MCP-DEV.VHD,startup /private/tmp/startup-rtc3
# Append b'montype r\r' to the exported bytes using Python.
os9 copy -r /private/tmp/startup-rtc3 media/63SDC-MCP-DEV.VHD,startup
os9 copy media/63SDC-MCP-DEV.VHD,startup /private/tmp/startup-final
```

[Readable option menu](assets/nos9-rtc/snapshot-32.png), successful completion, [byte comparison](assets/nos9-rtc/os9boot-compare.txt).

### Final boot / RTC / RGB evidence

Final MAME PID 14840 booted EOU with 2048K and native 6309, an 80-column shell, and **no setime prompt**. No manual date setting occurred during this final cold boot. The final startup has no software-clock `setime` line and ends with `montype r`. It ran to the idle shell without a command error. MCP's generated cfg independently retains Monitor Type RGB (`:screen_config=1`).

- Final boot stage, [idle final shell](assets/nos9-rtc/snapshot-38.png).
- `date -t`: **September 25, 2026 21:46:41** ([snapshot 40](assets/nos9-rtc/snapshot-40.png)). Host tool time was `2026-09-26 04:46:54 UTC`, i.e. 21:46:54 Pacific; the captures are 13 seconds apart. This supports successful boot-time RTC initialization.
- `mdir`: Clock and Clock2 loaded, along with EmuDsk, DD/H1, SCF, VTIO, Term and window modules (snapshot 42). An RTC submodule does not require a separate disk/device descriptor in this boot.
- `ident -m clock2`: module size `$0076` (118), CRC **$6CF198 (Good)**, edition 1, 6809 subroutine module ([snapshot 44](assets/nos9-rtc/snapshot-44.png)). This matches the EMUHWCLK boot inventory's Clock2 rather than the old software clock.
- `list /dd/startup`: trailing **montype r**, no setime prompt line (snapshot 46).
- Another `date -t`: **21:47:57**, showing normal advancement (snapshot 48).

Together the configured Disto RTC, verified hardware boot file/loaded Clock2, and correct automatically initialized guest time establish the hardware-clock path. This does not claim that Glenside supplies a clock, or that guest clock writes to RTC and long-term accuracy were tested. Save-state restore can rewind the clock; it does not automatically resynchronize the restored guest to current host wall time.

### New baseline and restore verification

After the final shell returned idle, `coco_save_state` returned:

```json
{"scheduled":true,"name":"nos9_ready","fileFound":true,"file":"/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/states/coco3h/nos9_ready.sta"}
```

State file: **126057 bytes**, mode 644. `pwd` then printed `/DD`. `coco_load_state` returned `{"scheduled":true,"name":"nos9_ready"}`. The restored screenshot removed that later pwd command/output and returned to the saved idle prompt with the earlier 21:47:57 date (snapshot 53). A further `date -t` succeeded and returned to the idle prompt at 21:48:22 ([snapshot 55](assets/nos9-rtc/snapshot-55.png)). No boot sequence or clock-entry prompt occurred; PID remained 14840. The shell and MCP bridge remained healthy.

The final baseline is **`nos9_ready`**, not `nos9_ready_test`. Pair it with the final topology and final media hashes below. The earlier checkpoint is incompatible with the startup/boot changes. Later image writes still require checkpoint/media consistency management; MAME save states are not disk snapshots.

### Before / after media hashes

Permissions and sizes remained the same throughout. Stock was never mounted or written.

| Media | Before this RTC task | After final restore / clean stop | Result |
|---|---|---|---|
| `63SDC.VHD` (444) | `db2f0f444073d3b88a3610a63ec9f7d2e2dd4299347181faa6b2b2aa9637de2c` | `db2f0f444073d3b88a3610a63ec9f7d2e2dd4299347181faa6b2b2aa9637de2c` | Unchanged immutable reference |
| `63SDC-MCP-DEV.VHD` (644) | `2d4aa91f0683438ba5a56e5d3b15562958586d3fe99d9a6e705965d91cdafba5` | `b1f8613e92ba25094a3c324563a5a2d7a34e565fc81408f7368bab375b271c7c` | Changed by authorized swapboot/startup operations, including recovery attempts |
| `63EMU.DSK` (664) | `0e29356d5897f733cad935c899a9a8dbb779f2887f5ee5476abec46afdda6763` | `9a51adb8656b9f003c7f822352c291487362f1b4c43aac1a16672d5d2bcb4c40` | Changed to EMUHWCLK OS9Boot by swapboot |

Hashes used `sha256sum media/63SDC.VHD media/63SDC-MCP-DEV.VHD media/63EMU.DSK`; modes used `stat -f '%Sp %OLp %z %N'` with the same files and the state path. The old root `/dd/OS9Boot` may reflect an earlier SDC selection; it is not used by the final `/d0` emulator boot. The final floppy boot and environment were positively compared, rather than inferred from the root boot file.

### Tests, build, and source diff

`npm test`: **57 passed, 0 failed, 0 skipped**, 37.7 seconds. The initial sandboxed attempt failed because tsx could not create an IPC socket; the full suite was rerun with filesystem/process permission and passed. `npm run build`: passed. `git diff --check`: passed. Existing test assertions were retained; new tests cover the verified slot/media arguments and config validation.

Full source diff. Changed tracked source/config/test files: `MCP/src/config.ts`, `MCP/src/mame-process.ts`, `MCP/.env.example`, `MCP/test/config.test.ts`, `MCP/test/mame-args.test.ts`. Local ignored `.env` was updated to use the canonical slots/media; media and state files are also ignored. No commit was made.

### Exact MCP results for this follow-up

Complete raw requests/results, including exact image data; [exact recorded launch](assets/nos9-rtc/launch.json); [test log](assets/nos9-rtc/tests.log). All MCP calls completed without MCP errors, although incorrect guest option selection and premature interruption are documented above. PNGs show actual guest output; keyboard tool success is not guest command completion.

| # | Tool | Arguments | Exact returned text / image |
|---:|---|---|---|
| 1 | `coco_start` | `{}` | `{"ok":true,"alreadyRunning":false,"pid":13240,"bridge":true}` |
| 2 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 3 | `coco_type` | `{"text": "DOS"}` | `{"queued":true,"text":"DOS{ENTER}"}` |
| 4 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 5 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 6 | `coco_type` | `{"text": "2026/09/25 21:40:00"}` | `{"queued":true,"text":"2026/09/25 21:40:00{ENTER}"}` |
| 7 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 8 | `coco_type` | `{"text": "swapboot"}` | `{"queued":true,"text":"swapboot{ENTER}"}` |
| 9 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 10 | `coco_type` | `{"text": "y"}` | `{"queued":true,"text":"y{ENTER}"}` |
| 11 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 12 | `coco_type` | `{"text": "2"}` | `{"queued":true,"text":"2{ENTER}"}` |
| 13 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 14 | `coco_stop` | `{}` | `{"ok":true}` |
| 15 | `coco_start` | `{}` | `{"ok":true,"alreadyRunning":false,"pid":13704,"bridge":true}` |
| 16 | `coco_type` | `{"text": "DOS"}` | `{"queued":true,"text":"DOS{ENTER}"}` |
| 17 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 18 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 19 | `coco_type` | `{"text": "swapboot"}` | `{"queued":true,"text":"swapboot{ENTER}"}` |
| 20 | `coco_type` | `{"text": "y"}` | `{"queued":true,"text":"y{ENTER}"}` |
| 21 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 22 | `coco_type` | `{"text": "3"}` | `{"queued":true,"text":"3{ENTER}"}` |
| 23 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 24 | `coco_stop` | `{}` | `{"ok":true}` |
| 25 | `coco_start` | `{}` | `{"ok":true,"alreadyRunning":false,"pid":13973,"bridge":true}` |
| 26 | `coco_type` | `{"text": "DOS"}` | `{"queued":true,"text":"DOS{ENTER}"}` |
| 27 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 28 | `coco_type` | `{"text": "swapboot"}` | `{"queued":true,"text":"swapboot{ENTER}"}` |
| 29 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 30 | `coco_type` | `{"text": "y"}` | `{"queued":true,"text":"y{ENTER}"}` |
| 31 | `coco_type` | `{"text": "3"}` | `{"queued":true,"text":"3{ENTER}"}` |
| 32 | `coco_snapshot` | `{}` | [PNG](assets/nos9-rtc/snapshot-32.png); exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 33 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 34 | `coco_stop` | `{}` | `{"ok":true}` |
| 35 | `coco_start` | `{}` | `{"ok":true,"alreadyRunning":false,"pid":14840,"bridge":true}` |
| 36 | `coco_type` | `{"text": "DOS"}` | `{"queued":true,"text":"DOS{ENTER}"}` |
| 37 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 38 | `coco_snapshot` | `{}` | [PNG](assets/nos9-rtc/snapshot-38.png); exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 39 | `coco_type` | `{"text": "date -t"}` | `{"queued":true,"text":"date -t{ENTER}"}` |
| 40 | `coco_snapshot` | `{}` | [PNG](assets/nos9-rtc/snapshot-40.png); exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 41 | `coco_type` | `{"text": "mdir"}` | `{"queued":true,"text":"mdir{ENTER}"}` |
| 42 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 43 | `coco_type` | `{"text": "ident -m clock2"}` | `{"queued":true,"text":"ident -m clock2{ENTER}"}` |
| 44 | `coco_snapshot` | `{}` | [PNG](assets/nos9-rtc/snapshot-44.png); exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 45 | `coco_type` | `{"text": "list /dd/startup"}` | `{"queued":true,"text":"list /dd/startup{ENTER}"}` |
| 46 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 47 | `coco_type` | `{"text": "date -t"}` | `{"queued":true,"text":"date -t{ENTER}"}` |
| 48 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 49 | `coco_save_state` | `{"name": "nos9_ready"}` | `{"scheduled":true,"name":"nos9_ready","fileFound":true,"file":"/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/states/coco3h/nos9_ready.sta"}` |
| 50 | `coco_type` | `{"text": "pwd"}` | `{"queued":true,"text":"pwd{ENTER}"}` |
| 51 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 52 | `coco_load_state` | `{"name": "nos9_ready"}` | `{"scheduled":true,"name":"nos9_ready"}` |
| 53 | `coco_snapshot` | `{}` | PNG; exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 54 | `coco_type` | `{"text": "date -t"}` | `{"queued":true,"text":"date -t{ENTER}"}` |
| 55 | `coco_snapshot` | `{}` | [PNG](assets/nos9-rtc/snapshot-55.png); exact encoded result in JSONL; `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png` |
| 56 | `coco_status` | `{}` | `{"running":true,"pid":14840,"bridge":true,"driver":"coco3h","flop1":"/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/media/63EMU.DSK","posting":false,"empty":true}` |
| 57 | `coco_stop` | `{}` | `{"ok":true}` |
| 58 | `coco_status` | `{}` | `{"running":false,"pid":null,"bridge":false}` |
