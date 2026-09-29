# Daggorath opening and attract mode: source and MAME reference

This is an implementation plan, not a port of the opening. The reference is the read-only recovered source at `/Volumes/SEDONA/Projects/daggorath-reference`, commit `a94326f00ebb16a106b540c58bc2ccf5f7b66dac`. Its `DAGGORATH.ASM` built with `lwasm --format=raw` into an 8 KiB ROM (SHA-256 `35e6a77354dcf1a3048f276824b7a0f9f759115fdd40603664cebfb3a7da6571`). Controlled MAME 0.289 runs of **that source-built ROM** provide the timings and images below. This is reproducible recovered-source behavior, **not independent authentication of a retail cartridge dump**. The existing [Wizard M1](DAGGORATH_WIZARD_M1.md) is a separately measured partial port.

## Reproduction boundary

The ROM was assembled outside this repository with `/usr/local/bin/lwasm` from lwtools 4.22: from the reference checkout, `lwasm --format=raw --output=/private/tmp/daggorath-attract/original.rom DAGGORATH.ASM`. It then ran as `coco3h` at 2 MB with RGB and no disk attached. The launch command was:

```text
/Applications/Emulators/Ample.app/Contents/MacOS/mame64 coco3h -window -skip_gameinfo -ramsize 2M -rompath '/Users/magneto-optimus/Library/Application Support/Ample/roms' -cfg_directory /private/tmp/daggorath-attract-full/cfg -cart /private/tmp/daggorath-attract/original.rom -wavwrite /private/tmp/daggorath-attract-full/audio.wav -autoboot_delay 0 -autoboot_script /private/tmp/daggorath-attract-full/capture.lua
```

The isolated MAME configuration used the verified RGB input port `:screen_config` with value `1`. Lua sampled video frames and source state; MAME's WAV output was captured without guest disk I/O. Three runs covered no input, a key during autoplay, and the full framebuffer/audio trace. Timing is MAME emulated time from launch, at approximately 59.923 video frames/s, not wall-clock time. The raw per-frame trace and WAV remain disposable under `/private/tmp/daggorath-attract-full/`; the representative images and a short unaltered-time opening audio excerpt are curated [here](assets/daggorath-attract/).

## Source-defined sequence

| Order | Original source | Behavior |
| --- | --- | --- |
| Reset | `ONCE.ASM:ONCE/DEMO/COMINI` | Initializes PIA, RAM, video and copyright/status text, then enters `DEMO10`. No separate title-menu routine was found on this path. |
| Wizard | `ONCE.ASM:DEMO10`; `MISC.ASM:WIZIN0/WIZI20/WIZOUT`; `D4.ASM:WIZ1` | Sets `AUTFLG`, synchronizes the IRQ, fades in the original wizard vectors, emits the first explosion, draws the two encoded welcome messages, waits twice, emits the second explosion and fades out. The fade retains the copyright/status text as the source requests. |
| Transition | `ONCE.ASM` immediately after `WIZOUT` | Blanks/flips the display, chooses demo level 2 (the third level), and supplies `DEMDAT`. |
| Preparation | `ONCE.ASM:GAME20/GAME30`; `COMSWI.ASM:PREPAR`; `MISC.ASM` preparation text; `COMDAT.ASM:DEMDAT` | Shows `PREPARE!`, builds the level, then creates and bags the demo objects: iron sword, pine torch and leather shield. |
| Map | `ONCE.ASM:GAME40`; `MAPPER.ASM:MAPPER` | Disables the ordinary scheduler while showing the complete demo map, waits twice, then resumes and synchronizes. This map is part of autoplay, not the ordinary new-game path. |
| Dungeon | `ONCE.ASM:GAME50`; `PLOOK.ASM:INIVUX`; `TOKEN.ASM:AUTTAB`; `HUMAN.ASM:PLAYER` | Initializes the resident view/status and heartbeat, draws a prompt, and lets the scripted demo command stream run through the scheduler. `AUTTAB` begins `EXAMINE`, `PULL RIGHT TORCH`, `USE RIGHT`, `LOOK`, movement/equipment/attack commands, then waits and restarts the demo. |
| User interruption | `COMMON.ASM:CLOCK/CLK50`; `ONCE.ASM:GAME/GAME10` | **Any keyboard depression in autoplay** rewrites the interrupted return address to `GAME`, beginning a fresh normal game. This is not a stage-advance or a character passed through to the demo. `GAME10` establishes the normal start location and `GAMDAT`; `GAME40` skips the demo map when `AUTFLG` is clear. |

The normal game begins dark. Its initial torch is carried but not lit; `PUSE.ASM:PUSE12` ignites a used torch and requests source sound `A$TORC` (`CHUCK`). The automatically illuminated corridor is therefore an **autoplay consequence** of scripted `USE RIGHT`, not a universal opening state.

## Measured visual timeline

The transitions below are sampled frames of the source-built ROM. They are not claims about an authenticated retail cartridge or an exact scanline boundary.

| Frame | Emulated time | Observation |
| ---: | ---: | --- |
| 102 | 1.701 s | First visible wizard fade frame |
| 395 | 6.590 s | Wizard geometry complete |
| 479 | 7.992 s | Both welcome messages complete |
| 641 | 10.696 s | Messages clear |
| 760 | 12.682 s | Fade-out begins its visible phase |
| 1014 | 16.920 s | Final blank frame |
| 1017 | 16.970 s | `PREPARE!` visible |
| 1403 | 23.412 s | Demo map visible |
| 1568 | 26.166 s | Heartbeat enable state observed |
| 1577 | 26.316 s | Demo dungeon view visible |
| 1679 | 28.018 s | Scripted `EXAMINE` begins displaying |
| 2180 | 36.379 s | `PULL RIGHT TORCH` visible |
| 2350 | 39.216 s | First lit 3D corridor, after `USE RIGHT` |

The no-input trace continued through frame 3600 (about one minute) in autoplay. A separate run injected `X` at frame 1800 (30.037 s), while `EXAMINE` was active. `AUTFLG` cleared by frame 1804 (30.104 s), and the normal initial position was established by frame 1833 (30.588 s). A dark playable dungeon was captured by frame 2400. This verifies the any-key restart path in one controlled condition; it does **not** establish the first input-ready frame or response to every key at every stage. The source's `GAME50 → INIVU → PROMPT → SCHED` path identifies when ordinary command processing can begin.

The curated full-trace images are frames 480 (messages), 1080 (`PREPARE!`), 1500 (map), 1680 (`EXAMINE`), and 2400 (lit corridor). The normal dark-dungeon image is frame 2400 of the separate key-interruption run.

![Wizard messages](assets/daggorath-attract/wizard-messages.png)
![Preparation](assets/daggorath-attract/prepare.png)
![Autoplay map](assets/daggorath-attract/autoplay-map.png)
![Autoplay EXAMINE](assets/daggorath-attract/autoplay-examine.png)
![Autoplay lit corridor](assets/daggorath-attract/torch-corridor.png)
![Normal dark dungeon after interruption](assets/daggorath-attract/normal-dark-dungeon.png)

## Audio and timing relationships

`COMMON.ASM:CLOCK/CLK20` emits the original roughly 30 Hz DAC wizard buzz while `NOISEF` is set during the fades. `MISC.ASM:WIZI20` requests `A$EXP1` (`KABOOM`) after the fade-in and before fade-out. The captured MAME audio contains two strong explosion-like intervals and fade-associated buzz, consistent with that call order; the source is the evidence for exact call identity. The opening audio excerpt is [available here](assets/daggorath-attract/opening-source-build.mp3). It is a source-built CoCo DAC recording, **not** output from the port's SSC backend. The demo's `USE RIGHT` later requests `A$TORC` (`CHUCK`); subsequent autoplay commands may request other data-dependent effects, which this pass did not isolate event by event. `PLOOK.ASM:INIVUX` enables the heart and its native PB1 heartbeat during dungeon entry; the source heartbeat audio and visual heart share the clock path rather than independent schedulers.

The two `WAIT` calls after the welcome messages and two map waits are part of the cadence. Wizard M1 already has a separately measured fade/content comparison; extending it must preserve its logical frames and canonical 512×192 presentation, and must place any later sounds at the source-defined transitions without inserting new foreground delays. Audio fidelity of the remaining attract sequence has not been established under NitrOS-9.

## Text geometry and prompt semantics

The original does not need a smaller font to obtain four message rows. `COMDAT.ASM`
places the status block at logical Y=152 and the primary text block at logical Y=160.
The latter is 32 columns by four rows. `COMTXT.ASM` advances rows by eight logical
pixels and supplies seven-byte glyphs, giving baselines/rows at Y=160, 168, 176 and
184 inside the existing 256x192 logical framebuffer. The canonical 2x1 presentation
therefore maps the four rows without changing the font, framebuffer or 512x192
viewport.

`MISC.ASM:PROMPX` defines `M$PROM1` as `I.CR,I.DOT`. The dot follows a carriage
return when the command prompt is emitted. It is not a prefix on every response and
must not be added by the message renderer generally.

## Exact autoplay command stream

`TOKEN.ASM:AUTTAB` contains these 17 commands, in this order:

1. `EXAMINE`
2. `PULL RIGHT TORCH`
3. `USE RIGHT`
4. `LOOK`
5. `MOVE`
6. `PULL LEFT SHIELD`
7. `PULL RIGHT SWORD`
8. `MOVE`
9. `MOVE`
10. `ATTACK RIGHT`
11. `TURN RIGHT`
12. `MOVE`
13. `MOVE`
14. `MOVE`
15. `TURN RIGHT`
16. `MOVE`
17. `MOVE`

`HUMAN.ASM:PLAYER` expands each token string through the ordinary input/parser path,
waits between words, and supplies carriage return. At the `-1` terminator it waits
twice and jumps to `DEMO`, restarting the whole sequence. `COMMON.ASM:CLK50` detects
any depressed key during autoplay and changes the interrupted return to
`ONCE.ASM:GAME`; `GAME10` then performs complete normal-game initialization with
the normal start position and `GAMDAT`. The pressed key is not the first command of
the new game.

The demo starts at source level value 2 (the displayed third level) with `DEMDAT`:
iron sword, pine torch and leather shield. The Wizard path requests two KABOOM events.
The scripted `USE RIGHT` follows `PUSE.ASM:PUSE12` and requests CHUCK.

## OS-9 process and module architecture

The maintained source used for this analysis is the external official checkout
`/Volumes/SEDONA/Projects/nitros9-reference`. The applicable implementation is the
CoCo 3 Level II 6309 kernel recipe; the relevant routines are
`level1/modules/kernel/fchain.asm`, `ffork.asm`, `fwait.asm`, `flink.asm`, and
`fload.asm`, included by the Level II kernel sources. Runtime evidence below is from
the verified EOU target and takes precedence where it narrows the source reading.

### System-call comparison

| Mechanism | Verified semantics | Suitability here |
| --- | --- | --- |
| `F$Chain` | Replaces the current primary module and its program/data allocation without creating a child. Level II allocates a replacement process descriptor, copies the preserved descriptor body beginning at `P$SP`, replaces the task/DAT image, and clears pending signals and intercept vectors. Success never returns. | Best fit. It retains a single process identity/lifecycle while each phase gets a fresh address space. Every phase must reinstall its signal handler and remap any GP buffer it needs. |
| `F$Fork` | Creates a concurrent child, copies CWD/CXD, and duplicates only standard paths 0, 1 and 2. A separately opened graphics path above 2 is not inherited automatically. | Poor fit for phase replacement. A parent/controller would remain resident, consume another process/data area, need explicit path duplication/IPC, and wait/reap each child. |
| `F$Wait` | Blocks a parent until one of its children dies and returns that child PID/status. It is relevant to a Fork controller, not to a Chain sequence. | Avoided in the preferred design. It adds child ownership and cannot preserve a nonstandard graphics path by itself. |
| `F$Link` / `F$Load` | `F$Link` locates a compatible module already in the module directory; `F$Load` loads from the execution path when linking fails. `F$Chain` uses this Link-then-Load behavior itself. | Phase modules can be resident or loaded by an explicit execution path. The controller must not assume GShell's current execution directory. |

`F$Chain` preserves paths because Level II copies the process descriptor state into the
replacement descriptor, but it destroys the old program/data image and DAT mappings.
Consequences for Daggorath are precise:

- The game-owned graphics **path** can remain open across a chain.
- CMOC globals, the 6,144-byte logical framebuffer, stack, cached pointers and
  `SS.MpGPB` mappings do not survive. A phase must receive compact scalar identifiers
  in the chain parameter area and rebuild its process-local state.
- CoWin GP buffer storage is not ordinary process data. `cowin.asm:SS.MpGPB` maps its
  system buffer blocks into the caller's DAT image; the mapping must be recreated
  after a chain. CoWin's graphics table records creator process identity and block.
- A successful Chain does not close paths or return to the caller. A failed Chain does
  return an error, so the current phase still owns cleanup.
- `F$Chain` clears the signal/intercept state. Every new phase must install its own
  cancellation handler before doing work.
- The final exiting phase owns orderly removal of heartbeat VIRQ state, audio shutdown,
  GP-buffer deletion, `DWEnd`, graphics-path close and Term/GShell return. Abnormal
  exit still receives OS-9 path cleanup, but Daggorath must not rely on that for its
  device-specific state.

This supports one continuous GShell child: GShell forks the opening module once, the
same process chains through the phases, and only the final `F$Exit` supplies child
status. It avoids a presenter process and preserves exclusive graphics ownership.

## Disposable EOU graphics-chain proof

A private probe under `/private/tmp/dod-chain-probe` tested:

```text
proba (open 640x200 graphics, create/present GP 196/1)
  -> F$Chain /d1/probeb (PutBlk the same GP 196/1 on the inherited path)
  -> F$Chain /d1/proba (PutBlk it again, DWEnd/close, return to shell)
```

Both modules were CMOC OS-9 program modules. The executable names in their module
headers were deliberately kept as `proba` and `probeb`; an earlier disposable build
used an absolute host output path as the module name and correctly failed with 216,
which was probe setup rather than Chain behavior. A fresh read-only disposable floppy
was mounted before cold boot so an older save-state disk binding could not affect the
result.

On the canonical EOU machine, the final run returned to Shell+ and `echo STATUS %*`
reported `STATUS 000`. B and resumed A both successfully wrote GP 196/1 through the
same nonstandard graphics path before A closed it. Thus the live target establishes:

- the graphics path survives A -> B -> A;
- the CoWin buffer storage survives even though each module has fresh process data;
- the replacement phases can issue GFX2 operations without reopening the window;
- final cleanup restores Term and propagates status 000;
- no canonical VHD or saved state was changed by the probe.

The proof does **not** establish that an old mapped pointer survives; it must not be
used that way. It also did not retain frame-timed evidence capable of excluding a
single-video-frame flash, nor did it stress thousands of cycles or interrupt exactly
inside `F$Chain`. Source semantics show that old primary modules are unlinked as the
process is replaced, so module link counts should not accumulate, but repeated-chain
leak measurement and Shift-BREAK at every phase boundary remain implementation
acceptance tests. The probe's successful uninterrupted screen ownership is sufficient
to select the architecture; those lifecycle cases are not silently claimed as proven.

## Exact current memory accounting

These figures come from fresh current builds, ToolShed `os9 ident`, and linker maps.
Both programs request approximately 1.5 KiB of CMOC/OS-9 stack within `M$Mem`; the
module header's data size is the authoritative total process data request.

| Artifact | Module/code image | Initialized writable data | BSS including runtime globals | Stack/runtime remainder in `M$Mem` | `M$Mem` total | Combined module + data |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| `dodwiz` | 22,739 bytes (`$58D3`) | 68 bytes (`$0001..$0044`) | 6,319 bytes (`$0045..$18F3`), including the 6,144-byte logical frame | 1,468 bytes | 7,855 bytes (`$1EAF`) | 30,594 bytes |
| `dodgame` | 33,815 bytes (`$8417`) | 71 bytes (`$0001..$0047`) | 9,229 bytes (`$0048..$2454`), including game/scheduler state and the 6,144-byte logical frame | 1,465 bytes | 10,765 bytes (`$2A0D`) | 44,580 bytes |
| disposable B-like chain phase | 2,911 bytes | included in total | no logical framebuffer | included in total | 299 bytes | 3,210 bytes |

The linker maps place `program_end` at `$5885` for `dodwiz` and `$83C5` for
`dodgame`; the difference between those offsets and module size is module header/CRC
packaging rather than process BSS. The stack/runtime remainder above is `M$Mem` minus
the mapped initialized-data-plus-BSS extent; it includes the requested CMOC stack and
small runtime/header effects, so it is not asserted to be stack bytes alone.

The last row is a measured upper-bound example for a tiny chain coordinator, not the
identity of a proposed production controller. A production controller needs only a
phase/restart policy, explicit module names and a compact handoff structure; its exact
size cannot be reported until it exists.

The current `dodgame` build identifies as edition 1, CRC `9D2CD2`; the current
`dodwiz` build identifies as edition 1, CRC `8C7FA5`. Both are re-entrant read-only
6809 object modules and execute on the verified 6309-native EOU target.

The 2 MB physical-memory setting does not give one Level II process a flat 2 MB address
space. Each phase still executes in its 64 KiB logical map. Module code, process data,
stack, CMOC runtime and temporary mappings must fit that map. CoWin owns the GP buffer
storage outside ordinary process data, but an `SS.MpGPB` mapping consumes logical map
space while mapped. Loaded shared system modules consume system/module-pool memory,
not copies inside every process data area.

It is therefore incorrect to prove or disprove a monolith merely by adding the two
module file sizes: duplicate runtime/presentation code could be folded, while
`dodwiz`'s roughly 13,282-byte read-only frame cache and the game's 10,765-byte data
request still create severe real pressure. Only an actual monolithic link/map would
prove its fit. This milestone deliberately does not build one; process chaining gives
each already-valid phase its own measured address-space budget.

## Resource ownership model

The preferred handoff contract is:

1. The first opening phase opens and selects one game-owned graphics path and creates
   the agreed CoWin group/buffers.
2. Before Chain, it unmaps all mapped GP blocks, leaves the physical window/path and
   named buffers alive, drains phase-local audio, removes phase-local VIRQ state, and
   passes only path number, group/buffer identifiers, phase and restart reason.
3. The next phase validates the inherited path, reinstalls cancellation handling,
   remaps only buffers it needs, and reconstructs process-local presentation state.
4. Wizard -> demo and demo -> Wizard use `F$Chain`; neither returns to Term or GShell.
5. Autoplay interruption chains to a **fresh** normal `dodgame` initialization. No
   demo object, level, parser, RNG, scheduler or heartbeat state is reused.
6. Only the terminal phase closes the window and exits to its parent. Chain failure is
   handled by the current phase, which still owns the resources.

Heartbeat and SSC services must be quiescent at a phase boundary unless their driver
contract is explicitly independent of the replaced process image. The conservative
design removes/reinstalls native heartbeat around the same source boundaries as the
original (`INIVUX` enables it for dungeon play) and finishes the phase's foreground
SSC request before Chain. No GFX2 operation moves into VIRQ context.

## Combat M1 dependency for `ATTACK RIGHT`

`ATTACK RIGHT` cannot be a no-op. `PATTK.ASM:PATTK` requires this minimum connected
slice:

- ordinary parser hand selection (`PARHND`) and the right-hand OCB chosen by the
  preceding demo `PULL RIGHT SWORD`;
- iron-sword class/type and offensive fields from the source object tables;
- player attack block, power and accumulated damage;
- creature lookup at the player's exact cell (`CFIND`) after the preceding moves;
- `PATTK.ASM:ATTACK`, including the defender-power/attacker-power probability index
  and original RNG result;
- darkness handling: without a live torch an otherwise successful ordinary weapon hit
  has the additional source 25% test; the demo has already used its pine torch;
- object-class attack sound, KLINK on a hit, and explosion on a kill, through the
  established semantic audio service without changing the combat timing contract;
- `DAMAGE`: separately scale magical and physical offense by the defender's respective
  defenses, accumulate defender damage, and compare damage with power;
- hit/miss messaging and view update;
- on death, drop carried objects, decrement the level population, disable the creature,
  absorb one eighth of its power with the source cap, and retain the wizard/endgame
  branches even if the exact demo target does not take them;
- the final `HUPDAT`, because attack energy expenditure changes player damage and thus
  heartbeat rate/countdown;
- scheduler ordering sufficient to keep creature state, attack target and subsequent
  `TURN/MOVE` commands source-correct.

The unresolved item is the exact deterministic demo encounter at command 10: its
creature type/state, RNG outcome, resulting damage/death and exact sounds must be
captured from the source-built cartridge at that boundary. Similar source control flow
does not prove the same runtime outcome. Combat M1 must establish that state before
coding the slice.

## Staged implementation recommendation

### Attract M1A

- implement the chain handoff/lifecycle contract;
- reproduce the four-row message geometry and authentic CR-plus-dot prompt semantics;
- integrate Wizard, PREPARE, map and demo initialization;
- run the exact AUTTAB parser stream only through the first unsupported source
  operation, with no approximation;
- prove normal/cancellation cleanup, GShell child status and repeated phase chaining.

### Combat M1

- capture the exact command-10 demo state from the source-built cartridge;
- implement the smallest connected `PATTK/ATTACK/DAMAGE` behavior listed above;
- verify state, messages, heartbeat consequences and semantic sound events against the
  cartridge reference.

### Attract M1B

- complete all 17 commands;
- wait twice and chain back to Wizard indefinitely;
- implement source-faithful any-key interruption into freshly initialized normal
  `dodgame`;
- retain the accepted heartbeat generation/presentation synchronization, 33.34 ms
  gate, 512x32 dirty strips, native PB1 heartbeat, SSC foreground service, Wizard buzz,
  game-owned graphics and zero unintended FDC activity.

## Remaining risks and required acceptance evidence

- GP storage survived the bounded probe, but every production phase needs explicit
  buffer inventory, mapping and deletion ownership to prevent double-delete or leaks.
- `F$Chain` clears signal interception; a missing per-phase reinstall could turn
  Shift-BREAK into incomplete device cleanup.
- Phase handoff cannot carry raw pointers, mapped addresses or CMOC object addresses.
- Module lookup must use verified loaded names or explicit execution paths; relying on
  an inherited GShell CXD is fragile.
- The exact visual chain boundary still needs frame-level capture proving no flash to
  Term/GShell.
- Repeated-chain module/path/system-memory accounting and cancellation at each phase
  boundary remain live acceptance gates.
- The exact ATTACK RIGHT encounter is the blocking source-fidelity fact for full
  autoplay.
- The original full-loop timing and all sound/event overlap after command 10 remain to
  be measured.

No production Daggorath or MCP source was changed in this architecture/proof pass.
