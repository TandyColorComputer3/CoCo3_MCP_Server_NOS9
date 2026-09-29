# Daggorath Level II modularization study

This is an architecture study and disposable ABI proof against the committed Combat M1 baseline at
`2666170c6f2dbaa2a8eca261ab7b56235ded556f`. It does not change `dodgame`, propose a
private MMU convention, or begin Attract M1B. The purpose is to choose a growth path
before the primary program module crosses its next 8 KiB Level II mapping boundary.

Evidence is labelled as follows:

- **Build observation** means a fresh build or linker-map result from this repository.
- **Upstream source** means the official read-only checkout at
  `/Volumes/SEDONA/Projects/nitros9-reference`, commit
  `f470fa52eb172b59b22c1b722074998cb42de9b1` (`v3.3.0-1129-gf470fa52`).
- **EOU runtime evidence** refers to the already verified target documented in
  [DAGGORATH_ATTRACT_M1.md](DAGGORATH_ATTRACT_M1.md) and the architecture reports.
- **Recommendation** is an engineering conclusion from those facts. It is not an
  undocumented OS-9 guarantee.

## Reproduction

The committed program was rebuilt outside the repository tree:

```text
python3 apps/daggorath/build_gameplay.py \
  --out /private/tmp/dod-level2-study/build
```

The script invoked:

```text
cmoc --os9 -O0 --intermediate --verbose --add-os9-stack-space=1536 \
  --intdir=/private/tmp/dod-level2-study/build \
  -Iapps/daggorath/src -Iapps/daggorath/src/audio \
  -I/private/tmp/dod-level2-study/build -o dodgame \
  gameplay/main.c gameplay/game.c gameplay/creature.c gameplay/input.c \
  gameplay/module.asm presentation.c window-path.c os9.c original/logical.c \
  audio/native_heartbeat.c audio/ipc.c audio/client.c audio/event.c
```

CMOC itself invoked `lwasm -fobj` once per translation unit and then `lwlink
--format=os9` with `libcmoc-crt-os9` and `libcmoc-std-os9`. No Docker or CoCoDev
container participated. Installed versions were CMOC 0.1.90 and lwasm/lwlink from
LWTOOLS 4.22.

The output exactly reproduced the accepted artifact:

| Field | Result |
| --- | ---: |
| Module | `dodgame`, edition 1, program/6809 object, reentrant/read-only |
| Module size | `$96B1` / 38,577 bytes |
| Data request (`M$Mem`) | `$2A14` / 10,772 bytes |
| CRC | `90C7EF` (good) |
| SHA-256 | `9d802e55458c7024bed1bb9df6bda0787c37b11fa461c6ab0a2496573f91ce2f` |

## Current program image

The fresh `dodgame.map` gives the following linked ranges. Ranges are offsets within
the OS-9 module, not assertions about fixed CPU logical addresses at runtime.

| Range | Bytes | Contributor |
| --- | ---: | --- |
| `$000D..$001E` | 18 | CMOC start stub |
| `$001F..$0C55` | 3,127 | CMOC CRT and pulled standard-library/runtime routines |
| `$0C56..$1CB1` | 4,188 | gameplay `main.c` |
| `$1CB2..$55CC` | 14,619 | `game.c` |
| `$55CD..$6191` | 3,013 | `creature.c` |
| `$6192..$62B7` | 294 | `input.c` |
| `$62B8..$7085` | 3,534 | `presentation.c` |
| `$7086..$70E1` | 92 | `window-path.c` |
| `$70E2..$7421` | 832 | `os9.c` |
| `$7422..$7973` | 1,362 | original-derived logical renderer |
| `$7974..$7D3F` | 972 | native heartbeat client |
| `$7D40..$7EE4` | 421 | audio IPC primitives |
| `$7EE5..$8698` | 1,972 | audio client |
| `$8699..$886C` | 468 | semantic audio event mapping |
| `$886D..$886F` | 3 | constructor/destructor/init sentinels |
| `$8870..$8955` | 230 | main read-only data |
| `$8956..$93AE` | 2,649 | game tables, strings and parser data |
| `$93AF..$965E` | 688 | remaining read-only data |

The module packaging adds 82 bytes after the map's `$965F` `program_end`, producing
the `$96B1` file. The module therefore consumes five physical/module blocks and five
logical DAT slots when linked. Its raw unused bytes to the sixth-block boundary are
`$A000-$96B1 = 2,383`; that raw slack does not constitute another logical slot.

The largest current source unit is `game.c`, but it is not one cohesive subsystem.
The map identifies useful internal groups:

| Group | Approximate code bytes | Notable functions |
| --- | ---: | --- |
| initialization, maze and object setup | 5,120 | `game_init`, `maze`, `fill`, `birth`, `creature`, `health` |
| bag/parser classification | 1,837 | `token`, `classify`, `on_floor`, `bag_command` |
| Combat M1 | 2,910 | `combat_event`, `attack`, `game_command_combat`, dispatcher |
| ordinary renderer/status/input | 5,165 | `draw`, text/status/input and `game_render_with_progress` |
| EXAMINE renderer | 1,244 | `examine_*`, `game_render_examine` |

Those are symbol-boundary estimates, not object-file sections suitable for automatic
extraction. Static helpers and shared tables still have to be assigned deliberately.

## Current writable memory

The link map has 71 initialized writable bytes (`$0001..$0047`), 9,236 BSS bytes
(`$0048..$245B`), and an authoritative `M$Mem` request of 10,772 bytes. The difference
between initialized data plus BSS and `M$Mem` is 1,465 bytes. The build requested 1,536
additional OS-9 stack bytes; the small difference follows CMOC's data-layout/runtime
accounting, so the 1,465-byte remainder must not be described as an independently
measured stack extent. The 6,144-byte logical framebuffer is inside BSS.

Level II allocates data in 8 KiB blocks, so the data request consumes two DAT slots:

```text
raw initialized data + BSS         9,307 bytes
M$Mem remainder                     1,465 bytes
M$Mem                              10,772 bytes
Level II mapped allocation         16,384 bytes (2 blocks)
unused bytes inside allocation      5,612 bytes
```

`M$Mem` is the authoritative process-data request; the command-line stack addition is
an input to CMOC's calculation, not a separate Level II allocation.

## Practical eight-block map

The following diagram is an allocation-class diagram. The exact slot numbers chosen
by the kernel can vary; callers must use returned addresses. Upstream `F$Link` searches
the process DAT image for contiguous free slots, and CoWin `SS.MpGPB` uses `F$MapBlk`.
The build does not record the live DAT slot order.

```text
Level II logical DAT slots (8 x 8 KiB = 64 KiB)

  [P0] dodgame program block 1       8 KiB
  [P1] dodgame program block 2       8 KiB
  [P2] dodgame program block 3       8 KiB
  [P3] dodgame program block 4       8 KiB
  [P4] dodgame program block 5       8 KiB (2,383 raw bytes unused)
  [D0] process data/stack block 1     8 KiB
  [D1] process data/stack block 2     8 KiB (5,612 allocation bytes unused)
  [G0] mapped CoWin GP strip block    8 KiB mapping; current payload is 2 KiB

  Free logical slots during mapped graphical gameplay: 0
```

The CoWin storage is system-owned graphics-buffer memory, not part of `M$Mem`.
Nevertheless, mapping even the current 2 KiB strip via `SS.MpGPB` consumes one whole
logical block. Earlier live work already demonstrated that a second mapped GP buffer
fails with process-memory-full under this layout. Unmapped CoWin buffers do not consume
caller DAT slots.

Raw module bytes, process-data bytes, physical system allocation and logical mapping
consumption are consequently different quantities:

- 38,577 module bytes occupy five 8 KiB physical module blocks and five caller slots.
- 10,772 requested data bytes occupy two private physical blocks and two caller slots.
- the 2 KiB GP strip lives outside process data but occupies one caller slot while mapped.
- the process has no spare DAT slot in steady graphical operation even though the two
  rounded allocations contain 7,995 unused bytes in aggregate.

## Level II mechanisms

### `F$Link` and `F$UnLink`

The applicable source is upstream `level1/modules/kernel/flink.asm` and
`funlink.asm`, compiled into the Level II kernel. `F$Link` takes a module name and
type/language, requires an already loaded compatible module, rejects a busy
non-reentrant module, rounds its memory-block size to 8 KiB, and either reuses its
existing process mapping or places its contiguous physical blocks into free entries in
the caller's DAT image. It increments both per-process block link counts and the system
module link count. It returns the logical module header in U and execution entry in Y.

`F$UnLink` takes the mapped header in U. It decrements module and per-process block link
counts and marks the caller's DAT entries free when the latter reaches zero. If the
system module link count reaches zero and the module is not ROM or otherwise referenced,
the kernel may remove it from the module directory and release its physical blocks.

Consequences:

- mapping granularity is the module's rounded 8 KiB block count, not a function or page;
- a one-block subroutine module needs one genuinely free DAT slot;
- linking/unlinking around every render primitive would be inappropriate;
- a module remains in physical RAM while another system/process link holds it, but the
  caller must not assume that the last `F$UnLink` leaves it resident;
- `F$Load` is needed when the module is absent. Loading from disk in the resident game
  loop would conflict with the established no-FDC-activity objective. Installation or a
  launcher should preload required modules, or the game should load once before the
  heartbeat-critical resident interval.

### Callable module type

The correct OS-9 term is a **subroutine module**, type `Sbrtn` (`$20`). For current
6809 object code its type/language is `Sbrtn+Objct` (`$21`); the definitions are in
upstream `defs/os9.d`. `level2/coco3/modules/vtio.asm` provides a maintained example:
it links `co3hires` with `F$Link`, saves Y as the entry point and calls through `jsr
,y` under a documented register contract. Clock2 and low-level storage modules use the
same module type.

An ordinary `Prgrm+Objct` module is a process primary module and is not the right
semantic type for a callable overlay. `F$Chain` accepts program/system modules for
phase replacement; it is a separate mechanism.

### Data modules

OS-9 defines type `Data` (`$40`). Current upstream fonts and palettes are concrete
examples of read-only data modules. A data module can move immutable tables out of the
primary executable and lets multiple processes share one physical copy. It still
costs whole 8 KiB caller mappings while linked.

Current candidates include game/parser tables (2,649 bytes), dungeon tables, creature
and object definitions, strings, graphics/font resources, and future map/attract data.
Mutable authoritative `Game` state is a poor first candidate because it is small enough
to fit the existing two data blocks, contains process pointers/relationships, and would
make ownership and chain transitions more complex.

Moving the current 2.6 KiB game tables to a one-block data module alone would not help:
the executable would remain above four blocks and the data module would consume the
only slot used by CoWin. Data modules become useful when table extraction participates
in reducing the permanent executable to four blocks, or when large phase-specific
resources can be linked only while CoWin is unmapped or another overlay is absent.

### Separate helper processes

`dodaudio` validates the service pattern: independent ownership, explicit IPC, isolated
failure, and hardware work outside the game process. It is suitable for audio, future
transport, logging or build/debug services. It is not attractive for ordinary combat,
creature or rendering code:

- Level II process pointers are not meaningful in another process's map;
- packed `Game` state would need serialization or a shared module protocol;
- process scheduling and pipe IPC would be added to frequent source-derived logic;
- a presentation helper would disturb the proven single graphics-path owner and was
  already rejected by measured GFX2 serialization work.

Keep service ownership separate, but keep authoritative gameplay in `dodgame`.

### Direct GIME MMU banking

Manual writes to `$FFA0..$FFAF` are inappropriate for an ordinary Level II process.
The kernel owns process DAT images and switches tasks; `F$Link`, `F$MapBlk`, `F$Move`
and `SS.MpGPB` are the supported abstractions. A direct register change can disagree
with the process descriptor, be replaced on scheduling, expose the wrong physical
block after an interrupt/task switch, and corrupt kernel or another process state.
Avoiding those hazards would amount to implementing a private memory manager around
OS-9 rather than using Level II. No requirement found in Daggorath justifies it.

### `F$Chain`

`F$Chain` is ideal for coarse phases. Upstream `fchain.asm` unlinks the old primary
module, resets program/data mappings, links or loads the replacement and starts it in
the same process lifecycle. The live `proba -> probeb -> proba` experiment documented
in [DAGGORATH_ATTRACT_M1.md](DAGGORATH_ATTRACT_M1.md) proves that the inherited graphics
path and CoWin buffer survive while process-local data, mapped GP pointers and signal
intercepts must be rebuilt.

Use Chain for Wizard, attract/demo gameplay and normal gameplay transitions. Do not use
it for `ATTACK`, individual commands, render stages or creature ticks: those require
persistent live `Game` state and fine call/return semantics.

## CMOC and LWTOOLS feasibility

### What the installed toolchain supports

CMOC 0.1.90 can stop at assembly (`-S`) or LWTOOLS relocatable object (`--compile`),
accept prebuilt `.o`/`.a` inputs, omit default libraries, and invoke `lwlink` for an
OS-9 module. The current generated code uses PC-relative calls and references suitable
for an OS-9 relocatable module. LWTOOLS resolves external symbols at static link time;
OS-9 `F$Link` returns one module entry point and is not a runtime C symbol resolver.

CMOC's normal OS-9 final link builds a **program module** with its CRT `start`, `main`,
data initialization and library routines. It has no command-line mode that turns an
arbitrary set of C functions into an OS-9 subroutine module with a stable exported C
symbol table. Therefore “put `game.o` in a second module” is not sufficient.

### Required callable-module ABI

A practical callable module requires:

1. an assembly module header declaring `Sbrtn+Objct`, reentrant/read-only;
2. one stable assembly entry or a small jump/operation dispatcher;
3. a host-side assembly wrapper that performs `F$Link`, preserves the returned U for
   `F$UnLink`, and calls the returned Y entry;
4. an explicit register/stack ABI for operation number, `Game *`, arguments, result,
   carry/error and preserved registers;
5. a module-side shim that translates that ABI into CMOC's stack calling convention;
6. a deliberate policy for Y, because CMOC uses Y as its writable-data base;
7. no unresolved references back into the host module. Required callbacks must be in an
   explicit callback table or dispatcher passed by the host;
8. module-local runtime helpers linked into the subroutine module, or source written to
   avoid helpers that would duplicate a large part of CMOC's CRT/library;
9. no module-local mutable globals unless their lifetime and data-base initialization
   are explicitly designed.

The generated C convention observed in `.s`/`.lst` passes ordinary arguments on the
stack, returns word/pointer values in D and uses Y for globals. An assembly shim is
therefore the safest boundary. Directly jumping from host C to a linked module's CMOC
function is too implicit and would couple two independently linked data/runtime images.

The disposable EOU prototype below now proves the primary
`host -> F$Link -> one-block Sbrtn -> F$UnLink -> relink` boundary. Cancellation inside
a real extracted command and production absent-module behavior remain migration tests;
they are not properties inferred from this deliberately tiny call.

## Authoritative state ownership

`dodgame` should remain the permanent owner of `Game`, parser state, RNG, heartbeat
inputs and the logical framebuffer during normal gameplay. A callable subsystem gets a
pointer to `Game` through the explicit shim and must finish before it is unlinked.
Because linking changes only code mappings, ordinary pointers into the caller's two
data blocks remain valid while the module is mapped.

This model is simpler than a shared mutable data module:

- one process owns synchronization and invariants;
- the existing packed OCB/CCB representation remains unchanged;
- no cross-process pointer or lock protocol is introduced;
- a module cannot retain a callback or state pointer after return/unlink;
- `F$Chain` remains an explicit boundary: the next phase reconstructs state or receives
  a compact serialized handoff. It cannot inherit process pointers.

A read-only data module may later supply tables. It should expose offsets/lengths, not
pointers cached after unlink. Reentrant subroutine modules must keep mutable call state
in the caller-provided context or stack.

## Extraction candidates

| Candidate | Current size evidence | Frequency/coupling | Classification | Assessment |
| --- | ---: | --- | --- | --- |
| main loop and heartbeat coordination | main 4,188; heartbeat 972 | continuous; owns lifecycle | permanent core | Do not extract. |
| presentation and logical renderer | 3,534 + 1,362 | every visible update; owns mapped strip | frequently used resident | Dynamic linking would add latency and complicate exclusive graphics ownership. |
| creature scheduler | 3,013 | every scheduler cadence; direct packed state | frequently used resident | Keep resident initially. |
| ordinary render/status/input in `game.c` | about 5,165 | every command/redraw | frequently used resident | Split source for clarity later, but do not overlay. |
| Combat M1 | about 2,910 | command-triggered; explicit `Game *`; audio callback | dynamic candidate | Cohesive and infrequent, but alone is too small to reduce the core below four blocks. |
| bag/parser command group | about 1,837 plus tables | command-triggered, shared token tables | dynamic candidate | Natural partner with Combat in a command subsystem. |
| EXAMINE | about 1,244 | command-triggered; renderer callbacks | dynamic candidate | Low frequency, but needs a narrow host rendering callback ABI. |
| audio IPC/client/event | 2,861 total | only semantic events; service already external | resident facade / later slim | Attractive size target, but extraction alone leaves a five-block core and an overlay call per sound adds little value. A smaller assembly IPC facade may be better than a mapped module. |
| game/parser read-only data | 2,649 | broadly referenced | future data module | Helps only as part of a block-boundary plan; one small data module by itself costs a full slot. |
| dungeon/object/creature tables | included above; future growth expected | initialization and command logic | resident or phase data | Group by use phase once sizes exceed a useful block. |
| Wizard/map/attract resources | separate today / future | coarse opening phase | `F$Chain` phase or data | Keep out of normal `dodgame`; Chain is already proven. |
| CMOC CRT/library | 3,145 before app code | broad arithmetic/printf use | resident runtime | Audit `printf` and 32-bit helper use later. Moving it wholesale is not feasible; reducing pulls may save code without an ABI boundary. |

The Combat audit's roughly 2,164 bytes of newly linked audio machinery is real, but it
does not make audio the best first overlay. Removing all 2,861 current audio IPC/event
bytes from 38,577 would leave about 35,716 bytes, still five program blocks. It is a
good later size optimization because `dodaudio` already owns synthesis, but the first
architectural extraction must save at least 5,809 bytes to produce a four-block core
(`<=32,768` module bytes; exact packaging must be verified by a real link).

## Candidate architectures

These estimates use whole 8 KiB mappings and include two data blocks plus one mapped
CoWin block. Exact byte results require implementation links.

### A. Monolith plus one low-frequency overlay

Extract a combined command module of at least 5.8 KiB: Combat, bag/parser operations,
EXAMINE, and their private tables/helpers. Target a primary module at or below 32 KiB
and a callable module at or below 8 KiB.

```text
normal resident:  core P=4 + data D=2 + CoWin G=1 = 7 blocks, 1 free
command call:     core P=4 + data D=2 + CoWin G=1 + overlay O=1 = 8 blocks
```

Complexity is moderate. Mapping occurs only for explicit low-frequency commands.
Authoritative state remains in the core. The main risk is keeping the extracted module
within one block after its duplicated helper/runtime costs are included. This is the
smallest architecture that actually creates usable headroom.

### B. Small gameplay kernel with several overlays

Reduce the permanent kernel to lifecycle, state, parser dispatch, heartbeat and a
minimal presentation API; place command logic, creature logic and specialized renderers
in separate one-block subroutine modules.

```text
target resident: core P=3 + data D=2 + CoWin G=1 = 6 blocks, 2 free
one subsystem:   core P=3 + data D=2 + CoWin G=1 + overlay O=1 = 7, 1 free
two-block module:core P=3 + data D=2 + CoWin G=1 + overlay O=2 = 8
```

This offers the most growth but requires a larger callback ABI, more module lifecycle
paths, duplicated CMOC helpers and mapping activity in frequent creature/render work.
It raises source-fidelity and timing risk. Do not adopt it as the first migration.

### C. Resident gameplay plus external services/data

Keep gameplay code monolithic, slim the audio facade, keep `dodaudio` separate, move
read-only resources to data modules, and use Chain for attract phases.

```text
likely core:      still P=5 + D=2 + G=1 = 8 blocks
linked data:      requires another slot and therefore cannot coexist
```

This improves physical sharing and organization but does not solve the immediate
logical-map boundary unless combined removals reduce the primary module to four blocks.
It is a useful secondary strategy, not a sufficient architecture by itself.

### D. Coarse phase programs with `F$Chain`

Keep Wizard, demo gameplay and normal gameplay as separate primary modules sharing the
open path/CoWin resources across Chain. Each phase gets its own 64 KiB map.

```text
wizard phase: its own measured program/data map
demo phase:   separate program/data map sized to autoplay needs
normal game:  current 5+2+1 map unless modularized internally
```

This is the right Attract architecture and prevents opening/demo code from bloating
normal `dodgame`. It does not provide fine-grained growth inside normal gameplay and
should be combined with Architecture A.

## Recommended architecture

Use a combination of **coarse `F$Chain` phases**, a **four-block normal-game core**, a
**one-block `Sbrtn+Objct` command overlay**, the existing **separate `dodaudio`
process**, and later **read-only data modules only when their block economics help**.

The first extraction target should be a command subsystem comprising Combat, bag
operations and EXAMINE, with enough private parser/table code to reduce the primary
module by at least 5,809 bytes after accounting for its assembly shim and duplicated
CMOC helpers. Do not select the exact boundary from source estimates alone. First make
a disposable callable-module proof, then produce two real link maps and adjust the
boundary until the core is four blocks and the overlay is one block.

During ordinary graphical gameplay the resulting core uses seven slots and leaves one
free. During a command-overlay call all eight slots are occupied. That is a deliberate,
bounded contract. Frequently called rendering, heartbeat, creature scheduling and the
authoritative state remain resident. The overlay is loaded/prelinked before entering
the no-FDC resident interval, linked only for a command, called through an assembly ABI,
then unlinked.

This architecture does not impose a 64 KiB total on the whole project. Program, demo,
Wizard, service, data and subroutine modules may all coexist in the 2 MB machine's
physical module/system memory subject to kernel, graphics, other processes, module
directory and fragmentation needs. Only the subset mapped in one process at a time is
limited to eight slots. A conservative project budget is therefore **hundreds of
kilobytes of modules/resources**, not a promised fixed number; exact available physical
RAM must be measured on the target under representative GShell/system load. Modules
whose last link is removed are not guaranteed to remain resident, so preload/residency
policy must be explicit.

## Attract M1B implication

The current normal game has only 2,383 module bytes before it requests a sixth program
block. The exact Attract M1B implementation has not been linked, so it cannot honestly
be declared to fit. Its likely dependencies include the 17-command table, demo-phase
state/setup, autoplay control, map presentation, interruption/restart handling and
chain orchestration. Folding those into normal `dodgame` would consume scarce slack and
couple opening behavior to the resident game.

Architectural work should precede Attract M1B, but it can be narrowly scoped:

1. define and prove the callable subroutine-module ABI;
2. establish a four-block normal core with a one-block command overlay;
3. keep Attract M1B itself as the already planned separate demo program reached by
   `F$Chain`, not as growth in normal `dodgame`;
4. share code with the demo through the proven overlay only where it avoids meaningful
   duplication and does not require disk activity during resident play.

This avoids accruing a sixth-block dependency that would make the current mapped CoWin
strategy impossible.

## Disposable dynamic-module ABI proof

### Scope and sources

The prototype lived entirely under `/private/tmp/dod-overlay-abi/`. It changed no
Daggorath production source, tests, build scripts or canonical media. Its guest used
copies of `63EMU.DSK` and `63SDC-MCP-DEV.VHD` on the canonical `coco3h`, 2 MiB, RGB,
MPI target. It used no GIME register manipulation or Lua memory tap.

The verified mechanism follows current upstream source at commit `f470fa52`:

- `level1/modules/kernel/flink.asm` maps a module's rounded block count and increments
  both process block links and its system module link;
- `level1/modules/kernel/funlink.asm` decrements those links and clears caller DAT
  entries when their process count reaches zero;
- `level1/modules/kernel/fload.asm` loads a module and leaves a link;
- `level2/cmds/load.asm` intentionally retains a successful `F$Load` link;
- `level2/cmds/unlink.asm` uses Level II `F$UnLoad` to release that retained module;
- `level2/cmds/pmap.asm` reads `P$DATImg` through `F$GPrDsc`; and
- `defs/os9.d` defines `Sbrtn+Objct` as `$21` and the Level II DAT image.

### Artifacts and toolchain

The primary host and ABI wrapper were built by CMOC 0.1.90, which invoked LWTOOLS 4.22
`lwasm`/`lwlink` and the normal OS-9 CRT/library. The callable modules used an assembly
OS-9 header. ToolShed `os9 ident` verified each module.

The essential disposable build and guest commands were:

```text
lwasm --format=os9 --pragma=forwardrefmax -o abimod abimod.asm
cmoc --os9 -O2 --function-stack=0 --intermediate \
  -o abihost host.c shim.asm host-module.asm
cmoc --os9 --function-stack=0 -O2 -S overlay_c.c
lwasm --format=os9 --pragma=forwardrefmax -o abicmod abicmod.asm

load /dd/abimod
/dd/abihost
```

The first assembly command used a tiny definitions include selecting Level II, 6309
and upstream `defs/os9.d`. `abicmod.asm` used the equivalent verified numeric module
and error constants plus the CMOC-generated leaf body. Guest artifacts were staged
only while disposable MAME was stopped.

| Component | Identity | Size | Data request | Level II map cost |
| --- | --- | ---: | ---: | ---: |
| `abimod` | `$21` subroutine/object, reentrant/R/O, edition 1, CRC `3D5BA9` | 45 B | 0 | one 8 KiB slot |
| `abihost` | `$11` program/object, reentrant/R/O, edition 1 | 2,789 B | 1,061 B | one program + one data slot |
| `abicmod` | `$21` subroutine/object, reentrant/R/O, edition 1, CRC `16A2DE` | 77 B | 0 | one 8 KiB slot |

`abicmod` is the secondary CMOC-generated experiment. CMOC compiled the leaf
`*counter = *counter + 1` with `--os9 --function-stack=0 -O2 -S`; an assembly header
and entry shim wrap the emitted body. The body is PC-relative, has no writable globals,
retains no caller pointer and imports no helper/library routine. This proves that a
small audited CMOC leaf can live behind the shim. It does not create an automatic C
overlay format: ordinary generated code may import `_stkcheck`, arithmetic, memory or
library helpers, which each overlay would have to supply and size deliberately.

### Explicit ABI

The primary entry contract was intentionally smaller than the eventual game contract:

| Register | On entry | On return |
| --- | --- | --- |
| A | ABI version, exactly 1 | unspecified on success |
| B | operation 1, increment | 0 on success; error when carry is set |
| X | caller-owned state pointer | preserved |
| Y | entry returned by `F$Link` | preserved by the module |
| U | CMOC host data base | preserved by wrapper and module |
| CC.C | unspecified | clear success; set error |

The state was `{ uint16_t counter; uint8_t guard; }`. The host wrapper saves CMOC's Y
and U, stores the U module header and Y entry returned by `F$Link`, and reloads only
those explicit values for `JSR ,Y` and `F$UnLink`. The module is reentrant, uses only
bounded stack storage, has no private data request and does not retain X.

### Live EOU result

The CMOC host initialized `counter=10`, `guard=$5A`. With `abimod` preloaded, the live
sequence was:

```text
ABIHOST START counter=10 guard=5A
LINK1 0 hdr=C000 entry=C014
CALL1 0 counter=11 guard=5A
UNLINK1 0 counter=11
LINK2 0 hdr=C000 entry=C014
CALL2 0 counter=12 guard=5A
BADOP 187 counter=12 guard=5A
UNLINK2 0 counter=12
ABIHOST PASS
```

The host returned status `000` and the shell remained usable. The unsupported
operation reached the module and returned carry plus E$IllArg (`187`) without changing
the state. The counter and guard establish that authoritative C state survives
unlink/relink and belongs to the resident host.

### Residency, links and logical mapping

Guest `load /dd/abimod` intentionally established the residency anchor. Before the
host ran, `mdir -e` showed `abimod`, size `$002D`, type `$21`, `Use 1`. That retained
load reference owns the physical module-directory residency independently of the host.

The first `F$Link` returned header `$C000` and entry `$C014`. A 45-byte module is still
rounded to one 8 KiB Level II mapping, so it occupied logical `$C000..$DFFF`. The first
`F$UnLink` returned success and released the host mapping. The second `F$Link` again
returned `$C000/$C014`, followed by another call and unlink. Reuse of `$C000` is an
observation, not an ABI promise; production always uses the current returned U/Y.

An initial diagnostic tried to print raw `P$DATImg` bytes through CMOC varargs without
word-promoting them. Its resulting words were invalid and are explicitly discarded.
The valid evidence is the returned logical header, exact module size, successful
unlink/relink, initial `mdir` count and cited kernel implementation:

| Stage | Host logical map attributable to module | System/physical state |
| --- | --- | --- |
| before link | none | resident, retained `Use 1` |
| linked call 1 | `$C000..$DFFF`, one slot | preload reference plus host link |
| after unlink 1 | none | preload reference retains physical module |
| linked call 2 | `$C000..$DFFF` in this run, one slot | same retained module; no load |
| after unlink 2 | none | preload reference remains |

The system-use count of two during each host link is source-derived from verified
`Use 1` plus `F$Link`'s increment; it was not independently captured by `mdir` during
the short call. This distinction prevents presenting inferred telemetry as measured.

The host and wrapper contain no `F$Load`, open or read. `F$Link` searches the module
directory and returns E$MNF when absent. Consequently the second link cannot reread the
disk. The retained `load` reference keeps the physical blocks resident, while each
temporary `F$UnLink` removes them from the host's 64 KiB DAT image. The intended final
release is Level II `unlink abimod`/`F$UnLoad`, not the temporary host unlink.

The production residency lifecycle is therefore:

1. installer or launcher `F$Load`s required overlays and retains those references;
2. `dodgame` links, calls and unlinks an overlay around a bounded command;
3. retained modules stay in 2 MiB physical system memory but consume no `dodgame` DAT
   slot between calls; and
4. controlled shutdown/uninstall releases retained references with `F$UnLoad`.

Many overlays can remain physically resident while only the active one consumes the
host's remaining logical slot. There is no fixed safe count: kernel/modules, CoWin,
other processes, fragmentation and each rounded physical allocation share the 2 MiB.

### Failure policy and remaining limits

- If not preloaded, `F$Link` returns E$MNF (`221`) without a filesystem search. A
  required gameplay overlay should fail explicitly and must not introduce resident
  command-time FDC activity.
- ABI mismatch or unsupported operation returns E$IllArg (`187`) without mutation;
  the unsupported-operation path was live tested.
- A command error leaves ownership of authoritative state with the host. Production
  extraction needs explicit commit/rollback rules for partially performed commands.
- An unlink failure must preserve the link record and surface the error; the caller
  must not guess that the DAT slot is free.
- Cancellation during an overlay call was not exercised by the tiny synchronous
  operation. Production calls must remain bounded and keep signal/audio lifecycle at
  the resident host boundary.
- Generated C is practical only when its imports, globals and stack/data assumptions
  are audited. The 77-byte leaf has no duplicated runtime; that is not a general promise
  for larger C modules.

### Proposed Daggorath ABI v1

Keep A/B/X as the stable assembly boundary and put extensible fields behind X:

```c
typedef struct {
    uint8_t abi_version;
    uint8_t context_size;
    Game *game;
    const DagHostServices *services;
    const void *arguments;
    void *result;
} DagOverlayContextV1;
```

- A is `DAG_OVERLAY_ABI_V1`, B is the command selector and X points to the context.
- U, Y and X are preserved; clear carry/B=0 is success, set carry/B is an OS-9 or
  documented application error.
- `Game` remains resident and authoritative. No overlay retains a pointer after return
  or across unlink, and no large object is passed by value.
- `DagHostServices` is separately versioned and initially exposes only proven needs,
  such as semantic-audio enqueue and bounded presentation/message services.
- Module-local library helpers are explicit; an overlay never imports host C globals or
  assumes the host's U/data layout.

The proof supports the recommended four-block core plus one one-block command overlay.
Extraction must still demonstrate those exact link sizes, retained-load ownership,
cancellation and complete gameplay behavior before replacing production code.

## Staged migration plan

## Production command-overlay extraction (uncommitted)

The first extraction moves the low-frequency command subsystem from the permanent
`dodgame` map into a one-block callable module named `dodcmd`.  It deliberately
keeps `Game`, the RNG seed, native-heartbeat state, the logical framebuffer and
all CoWin/presentation ownership resident in `dodgame`.

### Boundary and ABI

`command-overlay.c` contains the existing general implementations of command
tokenization/classification, bag operations, `ATTACK LEFT|RIGHT`, `EXAMINE`, and
their private tables.  It has no persistent game copy.  `overlay-api.h` defines
ABI v1: A=`1`, B=`DOD_OVERLAY_COMMAND` or `DOD_OVERLAY_EXAMINE`, and X points to
the caller-owned `DagOverlayContextV1`.  The context carries the resident
`Game *`, an explicit versioned callback table, command/result fields and an
optional framebuffer pointer.  The callable header in
`command-overlay-module.asm` validates A/B before calling the CMOC leaf and
returns `E$IllArg` (`187`) without modifying the context or `Game` for a bad ABI.

The resident callbacks are intentionally narrow: health calculation, object-name
lookup and status rendering.  This prevents the overlay from importing host C
globals or assuming the host module's U/data layout.  Combat audio remains in
the resident best-effort client: the overlay reports semantic combat events to
the ordinary host path after gameplay state and heartbeat propagation have
completed.  Missing or failed `dodaudio` therefore cannot affect combat, RNG or
heartbeat state.

### Retained residency and mapping

The production host follows the proven retained-reference lifetime, using the
Level II non-mapping load form `F$NMLoad` (`$22`) for `/d1/dodcmd`, then a
temporary `F$Link` (`$00`) by module name for each command, and `F$UnLink`
(`$02`) immediately after the call.  The final retained reference is released
with `F$UnLoad` (`$1D`) during normal game teardown.  This is the non-mapping
equivalent needed for the requested retained load: using ordinary mapping
`F$Load` would occupy the free DAT slot that must remain available for the
temporary link.  Current upstream evidence is `level1/modules/ioman.asm`
(`FNMLoad`) and `level1/modules/kernel/funlink.asm` in
[`nitros9-reference`](/Volumes/SEDONA/Projects/nitros9-reference); the earlier
EOU ABI proof supplies the runtime evidence for the retained-reference pattern.

The intended Level II logical-map accounting is:

| State | Program | Process data | CoWin | `dodcmd` | Total |
| --- | ---: | ---: | ---: | ---: | ---: |
| Normal graphical gameplay | 4 × 8K | 2 × 8K | 1 × 8K | 0 | 7 / 8 |
| During a command-overlay call | 4 × 8K | 2 × 8K | 1 × 8K | 1 × 8K | 8 / 8 |
| After `F$UnLink` | 4 × 8K | 2 × 8K | 1 × 8K | 0 | 7 / 8 |

`dodcmd` remains physically resident through the retained non-mapping reference,
so later calls should relink from the module directory without a disk read.  The
host disables future calls if an unlink failure makes the mapping lifetime
uncertain; an unavailable module makes the command fail explicitly before
authoritative execution rather than partially updating `Game`.

### Exact build result

Two clean builds on 2026-09-29 were byte-identical.

| Artifact | Size | Data request | Identity | SHA-256 | Mapping |
| --- | ---: | ---: | --- | --- | --- |
| pre-extraction `dodgame` | 38,577 B | 10,772 B | committed Combat M1 baseline | `9d802e55458c7024bed1bb9df6bda0787c37b11fa461c6ab0a2496573f91ce2f` | 5 program + 2 data blocks |
| extracted `dodgame` | 32,758 B | 10,786 B | `$11/$81`, edition 1, CRC `F46F28` | `029739f98752479bde0d60b1a72b8b762a38d023242e622c62e12c8c1e179df0` | 4 program + 2 data blocks |
| `dodcmd` | 6,901 B | module-local CMOC leaf stack only | `$21/$80`, edition 1, CRC `8C162B` | `9ab5ff77e14c0da5ce7f0956e09ef337bfe8a11e2b045fb3a5d1dd9524f98355` | 1 temporary program block |

The resident module is 10 bytes below the 32,768-byte four-block boundary.  Its
10,786-byte data request remains inside two 8K logical blocks (16,384 bytes).
The command overlay is 1,291 bytes below the one-block 8,192-byte boundary.
The 5,819-byte resident reduction is 10 bytes beyond the required 5,809-byte
reclamation.

### Validation and remaining live boundary

New overlay checks compile the exact command overlay against the resident game
API and verify command/bag/combat parity, `EXAMINE` framebuffer parity, rejected
ABI v1 mismatch without state mutation, retained-link ownership, balanced
link/unlink and missing-module behavior.  The complete Daggorath test set passed
after the extraction, including Combat (13), bag (13), EXAMINE (11), audio,
creature, heartbeat, presentation and lifecycle suites.  The MCP suite and its
TypeScript build also passed without source changes in MCP.

An isolated disposable EOU run restored `nos9_ready_v2` and mounted a copied
artifact floppy containing `dodcmd` and an overlay-only probe.  Its raw
keyboard launch did not yield a reliable Shell+ process-status observation, so
it is **not** evidence that the production `/d1/dodcmd` load/link/call/unlink
cycle has completed live.  The earlier ABI prototype did prove the same
`Sbrtn+Objct` call, unlink, retained residency and relink behavior (10 → 11 →
12), but the production overlay still needs one controlled guest-status run
before this extraction can be accepted as live-complete.  No canonical media
was used for that attempt.

### Final production-extraction acceptance

#### Proven

- Two clean builds produced byte-identical `dodgame` artifacts: 32,758 bytes,
  CRC `F46F28`, SHA-256
  `029739f98752479bde0d60b1a72b8b762a38d023242e622c62e12c8c1e179df0`.
  The resident program therefore uses four 8 KiB logical blocks rather than the
  committed Combat M1 baseline's five; its 10,786-byte data request remains two
  blocks. Normal graphical gameplay is consequently `4 program + 2 data + 1
  CoWin = 7/8` logical blocks.
- Two clean builds produced byte-identical `dodcmd` artifacts: 6,901 bytes,
  CRC `8C162B`, SHA-256
  `9ab5ff77e14c0da5ce7f0956e09ef337bfe8a11e2b045fb3a5d1dd9524f98355`.
  It is a reentrant/read-only `$21/$80` `Sbrtn+Objct` module and fits one 8 KiB
  temporary logical block.
- The independent live Level II ABI prototype established retained physical
  residency, `F$Link`, callable module execution, host-state persistence
  (`10 → 11 → 12`), `F$UnLink`, relink without a disk reread, and rejected-ABI
  `E$IllArg` handling without state corruption.
- Production overlay-host/unit tests pass, including retained-link ownership,
  balanced link/unlink, missing-module handling, command/bag/combat parity and
  EXAMINE framebuffer parity. The Combat M1 golden fixture traverses the
  extracted implementation; bag and EXAMINE suites, the relevant Daggorath
  regressions, MCP's 115 tests, and the TypeScript build have passed.
- In the recovered M6 disposable EOU environment, the exact production
  `dhbpack` and `dodgame` modules loaded through verified Shell+/MCPDONE status
  handling with guest status `000`. Canonical media and the canonical ready
  state remained unchanged.

#### Live production `/w` overlay-cycle acceptance pending

We have **not** observed one complete live production cycle:

```text
/w input → parser → F$Link dodcmd → command → F$UnLink → continuation → status
```

`dodgame` owns a private graphics `/w` input path, so inherited standard input
cannot drive its command loop. Timing-sensitive keyboard delivery was rejected
as unreliable. The bounded debugger-assisted attempt could not obtain a
trustworthy live runtime input/mapping observation before the application
returned to Term. No production input behavior was changed to make that test
possible. This is an **acceptance instrumentation limitation**, not an observed
`dodgame` or `dodcmd` defect.

Future graphical applications should prefer a deliberately designed diagnostic
or test seam over ad-hoc keyboard/debugger injection. Such a seam is deferred;
it must not alter normal production semantics merely to support acceptance.

### Scope status

This extraction does not begin Attract M1B and does not change the high-frequency
renderer, CoWin ownership, heartbeat VIRQ, audio recipes, parser semantics or
canonical media. It is ready for source review with the live `/w` observation
explicitly pending.

1. **Freeze the baseline.** Preserve the current module identity, map and behavioral
   suites as the comparison point.
2. **Build a disposable ABI proof.** A tiny CMOC host links a reentrant one-block
   `Sbrtn+Objct` module, passes a context pointer through an assembly shim, receives a
   result, unlinks it, and verifies absent/busy/cancellation paths. Test on EOU before
   applying it to Daggorath.
3. **Define a versioned ABI.** Include ABI version, operation, `Game *`, callback table,
   argument/result fields and explicit register preservation. No host symbol imports.
4. **Extract a measured command slice.** Start with Combat + bag + EXAMINE and private
   tables/helpers. Preserve general logic and tests; do not special-case attract.
5. **Prove block targets.** Require normal core <=4 program blocks and overlay <=1,
   with two data blocks and mapped CoWin. Verify no disk/FDC activity after preload.
6. **Validate lifecycle/timing.** Test normal commands, error/audio absence, heartbeat,
   cancellation, GShell launch, repeated link/unlink and missing-module behavior.
7. **Implement Attract M1B as a Chain phase.** Rebuild mappings and signal interception
   per the proven phase contract. Do not put Wizard/demo resources in the normal core.
8. **Revisit data modules and runtime pulls.** Only after maps show the next pressure,
   group large immutable resources by access phase and audit `printf`/32-bit helper use.

## Direct answers

1. **Do we need architectural work before Attract M1B?** Yes. The callable-module ABI
   is now proven; establish the four-block core next, and keep the attract sequence a
   separate Chain phase. The current 2,383-byte slack is not a safe unmeasured budget.
2. **Should `dodgame` remain the permanent owner of `Game` state?** Yes, throughout
   normal gameplay. Linked modules receive a temporary pointer. Chain boundaries use
   explicit reconstruction/handoff, never retained pointers.
3. **Which Level II mechanisms?** Combine `F$Chain` for coarse phases,
   `F$Link/F$UnLink` with `Sbrtn+Objct` for infrequent callable command code, the existing
   helper process for audio, and data modules only for sufficiently large/phase-specific
   immutable resources. Do not use a helper process for ordinary gameplay state.
4. **What should be extracted first?** A measured one-block command overlay containing
   Combat, bag and EXAMINE plus enough private helpers/tables to save at least 5,809
   primary-module bytes. Audio-only extraction is too small to cross the needed boundary.
5. **How many blocks remain free?** One during normal mapped graphical gameplay
   (`4 program + 2 data + 1 CoWin`); zero while the one-block command overlay is linked.
6. **How much can reside physically?** Potentially hundreds of kilobytes across program,
   subroutine, data and service modules on the 2 MB target, but there is no justified
   fixed guarantee. Kernel, graphics, other processes and fragmentation consume RAM;
   measure representative free memory and maintain explicit links for desired residency.
7. **CMOC implications?** CMOC emits relocatable LWTOOLS objects but no automatic
   dynamically linked C ABI. Use an assembly `Sbrtn+Objct` header and host/module shims,
   explicit register/stack/context rules, caller-owned state, no unresolved host symbols,
   and tightly controlled module-local runtime dependencies.
8. **Is manual GIME banking necessary?** No. It is redundant with Level II's DAT/module
   services and unsafe against scheduler/kernel ownership. The supported mechanisms meet
   the requirement.

## Unresolved risks

- The exact extraction boundary must be proven by linked artifacts; source symbol sums
  do not include duplicated runtime helpers and module packaging.
- A caller-provided callback table needs ABI versioning and reentrancy rules.
- Production preload ownership must follow the proven retained-reference pattern and
  verify zero FDC traffic during the resident heartbeat interval.
- One-block overlay failure needs graceful behavior and cleanup comparable to optional
  audio, while missing required gameplay code should fail explicitly rather than
  silently change semantics.
- Repeated link/unlink, Shift-BREAK during a module call, GShell launch and Chain between
  phases need live EOU acceptance.
- Future tables may outgrow one block or require two-block contiguity; their grouping
  should follow measured access phases rather than filenames.

## Integrity and scope

A disposable MAME run used copied media only under `/private/tmp/dod-overlay-abi/`.
The proof ended with the disposable MAME instance stopped. Read-only SHA-256
verification matched the established canonical baseline:

| Artifact | SHA-256 | Mode |
| --- | --- | ---: |
| `media/63SDC.VHD` | `db2f0f444073d3b88a3610a63ec9f7d2e2dd4299347181faa6b2b2aa9637de2c` | `0444` |
| `media/63SDC-MCP-DEV.VHD` | `4c6bdc0cd2f68aa57a9c75f75f15fe429f30926aa522917dd8d14aa1b9ae622e` | `0644` |
| `media/63EMU.DSK` | `9a51adb8656b9f003c7f822352c291487362f1b4c43aac1a16672d5d2bcb4c40` | `0664` |
| `MCP/states/coco3h/nos9_ready_v2.sta` | `a0f4ee094db471c596b9ecb9826922f80d982d2571fe7e72672a255ebca2effa` | `0644` |

The only repository change is this report. Build intermediates and prototype media
remain disposable under `/private/tmp/dod-level2-study/` and
`/private/tmp/dod-overlay-abi/`; they are not repository artifacts.

**READY FOR PRODUCTION OVERLAY EXTRACTION**

## Resident-core headroom pass — 2026-09-29

This pass starts from the accepted extraction artifact above; it does not alter the
overlay ABI, command ownership, `Game`, RNG, heartbeat/VIRQ, framebuffer or CoWin
ownership.  Its purpose is to replace the accidental ten-byte resident margin with a
measured margin before Attract M1B adds a separate chain phase.

### Measured baseline and attribution

A fresh build of commit `0fec21f` using the historical resident `-O0` setting produced
the accepted `$7FF6` / 32,758-byte `dodgame`, data request `$2A22` / 10,786 bytes and
the unchanged `$1AF5` / 6,901-byte `dodcmd`.  Its map ended at `$7FA4` before the
82-byte OS-9 packaging overhead, leaving only ten bytes before the four-block boundary.

The table below compares that exact source link with a disposable `-O2` resident link.
These are map-section code measurements; they are not source-line estimates.

| Resident contributor | `-O0` | `-O2` | Reduction | Ownership assessment |
| --- | ---: | ---: | ---: | --- |
| CMOC CRT and selected runtime | 3,074 | 3,074 | 0 | resident runtime; no extraction candidate |
| `main.c` lifecycle/heartbeat coordination | 4,341 | 3,341 | 1,000 | continuous resident core |
| `game.c` initialization, renderer and resident callbacks | 8,595 | 6,777 | 1,818 | core/renderer; retain resident |
| `creature.c` | 3,013 | 2,262 | 751 | high-frequency scheduler; retain resident |
| `input.c` | 294 | 179 | 115 | ordinary input; retain resident |
| `presentation.c` | 3,534 | 2,713 | 821 | exclusive graphics owner; retain resident |
| `window-path.c`, `os9.c`, logical renderer | 2,286 | 1,877 | 409 | graphics/OS wrappers; retain resident |
| native heartbeat client | 972 | 641 | 331 | authoritative coordination; retain resident |
| audio IPC/client/event | 2,861 | 1,964 | 897 | resident optional-audio facade; retained-load semantics require its persistent client state to stay here |
| overlay host/shim | 529 | 529 | 0 | required resident ABI boundary |
| **Total module reduction** | **32,758** | **26,616** | **6,142** | — |

The current command extraction has already moved the genuine low-frequency command
implementation and its private tables into `dodcmd`.  The remaining large code is
either continuous gameplay/presentation ownership or the persistent best-effort audio
client.  Moving that client into `dodcmd` would lose its start-once/disable-on-failure
state between temporary links, or require a new service/overlay architecture.  This
pass intentionally does neither.

This is deliberately a resident-build policy change, selected after the map
attribution rather than assumed at the start of the pass. The source-derived command
functionality remains in `dodcmd`, while all authoritative and high-frequency state
remains resident. `build_gameplay.py` now uses the measured `-O2` setting for
`dodgame`; the callable overlay already used `-O2` and its bytes did not change. The
overlay build test now retains a `<= 30,720` resident-size regression gate.

### Resulting map and artifact contract

Two clean builds of the revised command must be byte-identical.  The measured first
build is:

| Artifact | Size | Change | Data request | Mapping result |
| --- | ---: | ---: | ---: | --- |
| prior extracted `dodgame` | 32,758 B | baseline | 10,786 B | 4 program + 2 data blocks |
| optimized resident `dodgame` | 26,616 B | -6,142 B | 10,786 B | `$11/$81`, edition 1, CRC `859B22`, SHA-256 `28b5debc208221373d22c4400570f416812745527b707d977fc602245e21f994`; 4 program + 2 data blocks |
| `dodcmd` | 6,901 B | 0 B | module-local leaf stack | `$21/$80`, edition 1, CRC `8C162B`, SHA-256 `9ab5ff77e14c0da5ce7f0956e09ef337bfe8a11e2b045fb3a5d1dd9524f98355`; 1 temporary program block |

`dodgame` now has 6,152 bytes of raw headroom below 32,768, exceeding the requested
2,048-byte margin while retaining the same normal graphical map:

```text
normal gameplay:  4 program + 2 process-data + 1 CoWin = 7 / 8
command call:     4 program + 2 process-data + 1 CoWin + 1 dodcmd = 8 / 8
```

There is no new overlay, no new data module, no changed process-data allocation and no
claim that physical RAM is the limiting resource.  The pending live production `/w`
overlay-cycle acceptance remains pending exactly as documented above.

### CMOC 0.1.90 `-O2` production validation

The resident policy is intentionally `cmoc --os9 -O2`; it is not a generic compiler
assumption.  The installed compiler identifies itself as **CMOC 0.1.90**, and its own
`--help` says that `-O2` is the default optimization level while `-O0` compiles faster.
Its installed `NEWS` records a low-level optimizer and fixes to individual optimizer
passes, but does **not** define a GCC-like public pass or aliasing contract.  Therefore
the validation below describes observed CMOC output and does not claim unverified
strict-aliasing, volatile, or interprocedural-reordering semantics.

The generated `-O2` listings show only the expected local transformations at the
reviewed boundaries: redundant push/load removal, constant-load shortening,
load/compare-zero folding, branch inversion/removal, and simple address/arithmetic
simplification.  The current compiler also documents that an absolute-address load is
treated as volatile by the `storeLoad` optimization.  The resident heartbeat sample
still emits a fresh `LDB $FF22` on every `heartbeat_audio_bit()` call; it was not folded
or cached.  This matters because CMOC's historical NEWS also cautions that `volatile`
has not been a general language-level guarantee in every release.

Focused listing review compared the historical `-O0` link with the candidate `-O2`
link.  The explicit `asm {}` bodies in `os9.c`, `window-path.c`, and `audio/ipc.c`
remain the OS-9 boundary; `os_getstat`/`os_setstat` still save and restore `Y`/`U` and
issue their `os9` instructions, while graphics mapping/presentation retains every
`os_write`, `os_map_buffer`, `expand_chunk`, and progress-callback call.  The main-loop
listing still calls `game_overlay_command`, `native_heartbeat_rate`, and the optional
audio path in their source-defined order.  `overlay-host-shim.asm` is assembled source,
not optimized C: its `F$NMLoad`, `F$Link`, call, and `F$UnLink` sequence is unchanged.
The small C overlay host was already compiled at `-O2` before this resident policy
change, so the overlay ABI itself is not newly exposed to a different optimization
level.  The native heartbeat driver remains separate assembly.

No optimizer-sensitive defect was found in this review.  Externally changed heartbeat
state is obtained through the driver's atomic system-call interface; graphics state is
owned by the one foreground process; and optional audio failures are returned through
the already-tested client policy rather than changing game state.  Direct I/O paths use
explicit assembly where needed.  We did **not** enable CMOC's separate
`-fomit-frame-pointer` option.

Semantic validation used the exact `-O2` build.  All 20 Daggorath `test_*.py` scripts
passed, including the Combat M1 13/13 golden fixture and RNG assertions, Bag 13/13,
EXAMINE 11/11, overlay ABI/retained-load lifecycle, creature scheduling, native
heartbeat, presentation, audio protocol/client/service, and process-lifecycle checks.
The deterministic fixtures include exact combat/game-state assertions and the playback
suite's exact cached-frame comparisons; they exercise production rendering and command
logic rather than merely module loading.  Two fresh `dodgame` builds and their
unchanged `dodcmd` companion were byte-identical.  ToolShed `ident` accepted both
modules with good CRCs.

This establishes `-O2` as the measured resident production baseline.  The historical
`-O0` artifact above remains the comparison baseline; it is not erased or rewritten.
