# M5 — restore the Level II gameplay overlay slot

## Proven cause and bounded correction

The missing logical slot was the **heartbeat pack's application mapping**, not
scheduler code growth or a callback-ABI failure. The opening helper used `F$Load`
for `/d1/dhbpack`, retaining its mapped pointer at `$4000`. With four program
blocks, two data blocks and CoWin, this filled all eight slots. The first
`F$Link dodsched` consequently returned **207 / E$MemFul**.

The correction is acquisition through **`F$NMLoad`**, retaining the same complete
pack in system memory without mapping it into `dodintro`. It changes exactly one
executable byte at module offset `$2F85` (`01 → 22`) plus the three CRC bytes.
No production scheduling, graphics, timing, heartbeat, audio, phase, or Game-state
logic was changed by this correction. The later M5 continuation builds on this
mapping correction without changing the ownership rule.

## Live DAT evidence

Disposable instrumented public launches recorded the executing process descriptor
through `F$ID` and `F$GPrDsc`, using the Level II 512-byte descriptor contract from
`level2/cmds/pmap.asm` and `level2/modules/kernel/fgprdsc.asm`. Offsets are
`P$PModul=$11`, `P$DATImg=$40`, `P$Links=$80`. These are from the selected Level II
`defs/os9.d`, not inferred from executable size. The local manual directory was
unavailable; identified source and installed EOU behavior supply the evidence.

The private probes preserve CMOC Y/U around both syscalls, buffer snapshots in
process data, and print only after graphics teardown. They remain four-program /
two-data-block builds (before: 30,106 bytes, 47 data pages; after: 30,502 bytes,
48 data pages). Thus their diagnostic buffers do not add a DAT block. They are
separate diagnostic artifacts, not the exact production binary. Both fresh runs
observed **PID 3, primary module `$8000`**; physical block numbers are run-specific.

| Logical CPU range | Intro entry | PREPARE/adopted | Map / before link, old helper | Corrected before link | During scheduler | After unlink | During command |
|---|---|---|---|---|---|---|---|
| `$0000–$1FFF` | `000C` data | `000C` | `000C` | `000C` | `000C` | `000C` | `000C` |
| `$2000–$3FFF` | `001B` data | `001B` | `001B` | `001B` | `001B` | `001B` | `001B` |
| `$4000–$5FFF` | `333E` free | `333E` free | **`001C` heartbeat pack** | **`333E` free** | **`001E` dodsched** | **`333E` free** | **`001D` dodcmd** |
| `$6000–$7FFF` | `333E` free | `0019` CoWin | `0019` | `0019` | `0019` | `0019` | `0019` |
| `$8000–$9FFF` | `0007` program | `0007` | `0007` | `0007` | `0007` | `0007` | `0007` |
| `$A000–$BFFF` | `000D` program | `000D` | `000D` | `000D` | `000D` | `000D` | `000D` |
| `$C000–$DFFF` | `0010` program | `0010` | `0010` | `0010` | `0010` | `0010` | `0010` |
| `$E000–$FFFF` | `001A` program | `001A` | `001A` | `001A` | `001A` | `001A` | `001A` |

Old-helper stages S1/S2/S4/S5/S6 respectively capture entry, PREPARE, map,
immediately before initialization, and after failed link; S5 and S6 are identical.
The old `F$Load` returned module pointer `$4000`, independently identifying the
new mapping. Corrected S3 records acquisition complete with `$4000` still free.

Corrected stages S7/S8/S9 show scheduler mapped / returned / unlinked. S12/S13/S14
show command mapped / returned / unlinked. S10/S11 show an actual resident health
callback entry/return while the scheduler is mapped. All recorded calls returned
success; the private run stopped after EXAMINE, closed its resources, and returned
**status 000**. It did not dispatch command 2.

Mapped `dodsched` header `$4000`, entry `$4016`, entry bytes:

```text
81 01 26 17 E1 02 26 13 34 10 34 60 AE 64 34 10
```

These match offset `$16` of the exact staged 8,192-byte artifact; its mapped final
CRC bytes are `DA 87 21`, also identical. The new acquisition permits ordinary
`I$Open /dhb`, rate/enable, heartbeat snapshots during command work and native
close; the driver's code never needs an application mapping.

Evidence: [before map](assets/daggorath-giant-m5/overlay-slot/before-dat.png),
[corrected map](assets/daggorath-giant-m5/overlay-slot/after-dat.png),
[callback/cleanup results](assets/daggorath-giant-m5/overlay-slot/after-cleanup.png).
Raw AVI, observer source and probe binaries remain disposable.

## Reference ownership

Source provenance: read-only `nitros9-reference` commit
`f470fa52eb172b59b22c1b722074998cb42de9b1`.

- `level1/modules/ioman.asm:1591–1657`: FNMLoad and FLoad share `LoadMod`; FNMLoad
  retains a module-directory reference through `ReturnModLinkInfo`, while FLoad
  additionally uses F$ELink to create the application mapping.
- `ioman.asm:502–572`: IAttach switches to the system process for descriptor,
  driver and file-manager linking, then restores the original process. Device
  path ownership does not require keeping the driver's code in the user's DAT.
- `level2/modules/kernel/funload.asm`: F$UnLoad decrements the named module's
  system reference; I/O-module deletion checks active device use.
- Existing application cleanup calls `native_heartbeat_close` first and releases
  the pack only when `heartbeat.opened` is false. That ordering is unchanged.

The helper still owns one nonmapping pack reference. The active device has its
own system-side links. The helper does not release that reference to make space
while the device is active: it avoids creating the unnecessary mapping in the
first place, then releases ownership through the existing normal cleanup path.
The concatenated `DHeartbeat` + `dhb` pack is preserved, including the earlier
EOU result that loading the members separately was not equivalent.

No kernel, driver, descriptor, or module reference counter was patched. The
successful private open/use/close/cleanup run supports this lifecycle; exact
system link-count values were not sampled, and no cumulative leak study is claimed.

## Responsibility and architecture comparison

| Current intro content | Needed after opening? | Placement assessment |
|---|---|---|
| PREPARE/copyright orchestration | No | Could disappear at a future coarse phase |
| Full opening map | No for ordinary commands | Isolated map object code: 464 bytes |
| Demo maze/object/creature initialization | No after state exists | Shared source-derived Game initialization; do not duplicate/recompute casually |
| Four-row transcript | Yes | Persistent text/cursor state and normal rendering |
| Dungeon/status renderer, framebuffer | Yes | Resident presentation ownership |
| Authoritative Game/RNG/equipment | Yes | Resident caller-owned state, 2,608-byte Game |
| Bounded demo runner | Yes through command 10 | Same semantic command/scheduler paths |
| Heartbeat client and module acquisition | Yes | Driver lives system-side; client and ownership stay resident |
| Audio queue/client | Yes | Optional asynchronous presentation, separate worker |
| Scheduler host/gateways | Yes | Mandatory temporary mapping |
| Command host/gateways | Yes | Alternates in same slot |

**A — selected:** retain the pack without acquiring its transient caller mapping.
This preserves the current module architecture and ownership with one syscall-byte
change. It solves the measured blocker without throwing away valid Game state.

**B — not selected:** restoring three program blocks requires removing at least
3,693 bytes from the 28,269-byte candidate. The isolated opening-map code is only
464 bytes. No demonstrated clean extraction of sufficient opening-only content
was needed or established here; arbitrary size golfing would conceal ownership.

**C — seriously considered, deferred:** `dodintro → beat 012 → F$Chain → attract`
is consistent with the coarse-phase master plan, but it does not inherently cure
an unnecessarily mapped driver. It also needs an explicit state-transfer contract.
The current helper accepts at most 255 parameter bytes, so a 2,608-byte Game cannot
be passed through it unchanged. A future phase could use a validated Data module
containing scalar/packed Game and transcript state; callback/service pointers,
CoWin mappings, signal interception and process-local handles must be rebuilt.
Deterministic seed reconstruction is an alternative but repeats measured maze
work and adds visible latency. Neither mechanism is implemented in this correction.

**D — no additional arrangement needed:** replacing ordinary temporary overlays
with manual banking, increasing program/data limits, or moving scheduler code into
`dodcmd` would not address the unnecessary mapping cleanly.

No phase boundary was introduced. Game, RNG, transcript, graphics path and native
heartbeat state remain in the same process; there is no serialization, second maze
generation, Shell+ transition or new intentional black frame.

## Memory and artifact gates

```text
Old steady map: 4 program + 2 data + 1 CoWin + 1 pack = 8/8
Corrected:      4 program + 2 data + 1 CoWin          = 7/8
Scheduler:      4 program + 2 data + 1 CoWin + dodsched = 8/8
Unlink:         7/8
Command:        4 program + 2 data + 1 CoWin + dodcmd   = 8/8
Unlink:         7/8
```

| Module | Bytes | Data request | CRC |
|---|---:|---:|---|
| Corrected dodintro | 28,269 | 11,027 / two blocks | `3C604E` |
| dodgame, unchanged | 27,287 | 11,109 / two blocks | `8BB638` |
| dodcmd, unchanged | 7,004 | caller-owned | `C3B942` |
| dodsched, unchanged | 8,192 | caller-owned | `DA8721` |
| dodaudio, unchanged | 10,086 | 2,142 | `CD93E9` |
| doddemo, unchanged | 23,905 | 10,896 | `9A932C` |
| daggorath, unchanged | 863 | 1,573 | `7CE837` |
| dodwiz, unchanged | 24,562 | 7,855 | `A25DCA` |

Corrected `dodintro` SHA-256:
`22840a70bb85999172ece16015ef36325de490e4e5b512bd0457182a79db009e`.
Two clean builds are byte-identical; ToolShed validates module CRC and header.
`dodsched` remains exactly at the no-growth gate. All ratified size limits remain.

## Tests and limits of proof

- **41/41 Daggorath scripts** pass; no existing assertion/mock changed.
- New generated-assembly regression requires F$NMLoad acquisition, exact owned
  F$UnLoad release, resident-register restoration and preserved error handling.
  A second check protects native-close-before-pack-release ordering.
- **MCP 115/115**, TypeScript build pass. The first sandboxed MCP invocation was
  denied a local tsx socket; the authorized local-socket rerun passed unchanged.
- This task repairs mapping availability and proves EXAMINE reachability. It does
  not establish Giant visibility, audio fidelity, heartbeat presentation latency,
  later command choreography, or completion of M5.

## Exact production public-launch proof

The final run used the production `dodintro` SHA above, independently extracted
from the disposable disk and compared byte-for-byte before launch. The ordinary
public `/d1/daggorath` entry passed through Wizard, PREPARE, map, initial dungeon
and EXAMINE. No manual module preload, injected gameplay input, state patch or
production diagnostic code was used. The private observer employed bounded
built-in debugger breakpoints and completed-frame recording, without Lua taps.

For the observed resident mapping at `$8000`, breakpoints checked eight actual
instruction bytes before recording an event. Examples:

| Routine | Runtime address | Validated bytes |
|---|---|---|
| Intro main | `$8C0A` | `34 40 17 F4 BA FF 9D 33` |
| Scheduler link | `$D5C0` | `34 60 30 8D 00 33 86 21` |
| Scheduler mapped call | `$D5D8` | `34 60 EE 66 10 AE 42 AE` |
| Scheduler unlink | `$D5EA` | `34 60 AE 66 EE 84 10 3F` |
| Resident callback gateway | `$D618` | `AE 62 10 AE 84 30 8D B1` |
| Resident task callback | `$87AC` | `34 40 17 F9 18 FF C0 33` |
| Command dispatcher | `$8AC8` | `34 40 17 F5 FC FF AC 33` |

These exact-production instruction observations complement the separate private
DAT snapshots above; they are not represented as a descriptor dump from the
unmodified binary. The gateway was entered with mapped Y=`$4016`; the resident
C target was reached with Y=`$0000`. Subsequent return/unlink and the next link
were reached. An incidental B register value at shim entry is not a syscall
error; the recorded scheduler result at unlink entry was zero.

| Observed boundary | MAME emulated seconds |
|---|---:|
| Intro main | 201.293845249 |
| First scheduler link entry | 222.271801659 |
| First scheduler unlink entry | 222.286342617 |
| Next scheduler link entry | 222.288162961 |
| Callback gateway | 222.292041667 |
| Resident callback | 222.292053959 |
| Scheduler unlink entry | 222.299025797 |
| EXAMINE dispatcher entry | 233.979513591 |
| Scheduler work after command | 234.001330336 |
| Final observed scheduler unlink entry | 234.364015639 |

The first link-through-call-to-unlink-entry interval is **14.541 ms**; the next
link is reached 1.820 ms later (including intervening caller work). No new phase
transition or state reconstruction was introduced. Intro entry to initial link
is 20.978 s, and the following initial boundary to EXAMINE entry is 11.680 s.
Those existing setup/presentation intervals were not partitioned in this bounded
repair and are not attributed speculatively to a particular subsystem. Restoring
the slot is not a claim that opening choreography or latency is now accepted.

The observer saved [the production EXAMINE frame](assets/daggorath-giant-m5/overlay-slot/013-production-examine.png)
and emitted `BEAT013_DONE`, then quit MAME at the dispatcher entry for the next
`PULL` command, before that instruction executed. This proves command 1 returned,
scheduler work/redraw/dwell completed and the inventory presentation was reached.
The screenshot visibly contains the inventory, but also a punctuation-heavy
separator and only the previous dot/cursor in the primary area. It is **reachability
evidence, not an original-cartridge visual MATCH**. No EXAMINE/text correction was
attempted here; those presentation observations remain for the next approved
M5 review. The exact production run was debugger-stopped, so no normal guest exit
status is claimed for it. Status 000 belongs to the separately identified bounded
private lifecycle run.

Earlier private observer attempts were rejected as bounded acceptance evidence:
startup breakpoint installation was unreliable; one command had a quoting error;
and bare debugger stops auto-resumed with the headless debugger. Some earlier
attempts consequently reached later dispatches before being stopped. They caused
no production changes or Giant work. The final observer installs and verifies
breakpoints after restore and uses an explicit `quit` action at the verified
command-2 entry. The final snapshot and log confirm this stop. A subsequent bridge
not-connected result is expected after that deliberate emulator exit.

## Final integrity

All private MAME/observer processes ended. Canonical SHA-256 values were rechecked:

| File | SHA-256 |
|---|---|
| `media/63SDC.VHD` (mode 0444) | `db2f0f444073d3b88a3610a63ec9f7d2e2dd4299347181faa6b2b2aa9637de2c` |
| `media/63SDC-MCP-DEV.VHD` | `4c6bdc0cd2f68aa57a9c75f75f15fe429f30926aa522917dd8d14aa1b9ae622e` |
| `media/63EMU.DSK` | `9a51adb8656b9f003c7f822352c291487362f1b4c43aac1a16672d5d2bcb4c40` |
| `nos9_ready_v2.sta` | `a0f4ee094db471c596b9ecb9826922f80d982d2571fe7e72672a255ebca2effa` |

The existing M5 working tree remains uncommitted. This repair adds only the
opening-heartbeat acquisition correction, its new focused regression, this report
and small curated evidence, plus the main M5 report's updated disposition.
No generated modules, disposable disks, raw recordings or debugger scripts are
staged. `cfg/` and `snap/` remain excluded from the intended change set.
