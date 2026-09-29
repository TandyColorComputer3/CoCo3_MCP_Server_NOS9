# Dungeons of Daggorath Combat Milestone 1 Reference

Combat M1 began as the reference-analysis checkpoint below and now includes the
bounded production implementation and validation recorded at the end of this report.
Its purpose is to resolve the command-10 `ATTACK RIGHT` dependency that blocked the
full attract loop in [`DAGGORATH_ATTRACT_M1.md`](DAGGORATH_ATTRACT_M1.md), while
retaining ordinary `ATTACK LEFT|RIGHT` behavior inside the same source-derived path.

The authoritative program source is the read-only checkout
`/Volumes/SEDONA/Projects/daggorath-reference` at commit
`a94326f00ebb16a106b540c58bc2ccf5f7b66dac`. The source-built cartridge used below has
SHA-256 `35e6a77354dcf1a3048f276824b7a0f9f759115fdd40603664cebfb3a7da6571`.

## Evidence vocabulary

- **Observed** means a value or control-flow point was recorded directly by the MAME
  debugger, or is present in the saved RAM range.
- **Source-confirmed** means the behavior is stated by an instruction sequence or data
  table in the original 6809 source.
- **Derived** means arithmetic or a record interpretation follows mechanically from
  observed values and source-confirmed layout/code.
- **Inferred** means a plausible interpretation lacks direct capture or a complete
  source proof. Inferences are kept out of the implementation contract.

## Safe capture method and reproducibility

Two independent cold starts used MAME 0.289's built-in debugger with conditional
execution breakpoints. The condition `w@0x20d == 0xd9b1` selected AUTTAB command 10.
Each run saved `0x0dff` bytes at `$0200..$0FFE` immediately on entry to `PATTK` and
again on entry to `PATT99`, and logged the same registers and explicit combat
breakpoints. The sessions ended with MAME's `quit` debugger command and reported
`Exited via the debugger` after 76 emulated seconds. No disk media was attached.

The complete pre-command RAM images were byte-identical:

```text
run 1 pre  f1dab5fc9d43b0cdec7bf69ecb7fb6a7f187c363aefce2e671a6e9846cfec093
run 2 pre  f1dab5fc9d43b0cdec7bf69ecb7fb6a7f187c363aefce2e671a6e9846cfec093
```

The complete images at `PATT99` were also byte-identical:

```text
run 1 post 639de8386fef3cd5399ccdc5ce82ea7b5cfdf41db5cd407b4d4053e3c529bccc
run 2 post 639de8386fef3cd5399ccdc5ce82ea7b5cfdf41db5cd407b4d4053e3c529bccc
```

Both 1,370-byte debugger logs were byte-identical as well. Thus the comparison covers
the entire captured game-RAM interval, not only selected fields, plus every logged
breakpoint/register value from `PATTK` through the hit, damage, kill and `PATT99`
paths. This proves reproducibility for this source-built cartridge and cold-start
autoplay condition. It does not prove that arbitrary start states produce the same
encounter.

The debugger script and raw captures remain disposable evidence under
`/private/tmp/dod-combat-m1`; they are not repository inputs.

### Rejected Lua instrumentation

Earlier experimental Lua memory taps are an explicit dead end. Six MAME processes
created macOS crash reports between 21:37 and 21:40 on 2026-09-28. Each terminated
with `EXC_BAD_ACCESS / SIGSEGV`; the faulting stack was in
`lua_gettop` called by `lua_engine::tap_helper` from an HD6309 write tap. Lua read or
write taps, `lua_engine::tap_helper`, and variants of that mechanism must not be reused
for Combat M1. The two debugger captures above completed without a crash.

## Command entry and dependency chain

The source command stream is `TOKEN.ASM:AUTTAB`; its tenth entry is
`ATM2 M$ATTK,M$RT` (`TOKEN.ASM:518-536`). `HUMAN.ASM:PLAY20..50` expands its tokens,
feeds them through the ordinary character/parser path, appends carriage return, then
stores the next AUTTAB pointer. `HUMAN.ASM:HMAN50..70` parses the primary command and
dispatches through `DISPAT`.

The minimum command path is:

```text
AUTTAB command 10: ATTACK RIGHT
  -> PLAYER / HUMAN / PARSER
  -> PATTK ($D2B8)
       PARHND -> right-hand OCB
       copy weapon offense -> PLRBLK
       charge attack energy to player damage
       weapon-class sound
       CFIND(player row,column)
       ATTACK($D3D7) -> one RANDOX($C4CF)
       live-torch darkness gate
       KLINK + hit indication
       DAMAGE($D40C)
       death cleanup + BANG + power absorption
       PATT99($D375) -> HUPDAT -> return
  -> HUMAN resets input state and prompts
  -> PLAYER stores next AUTTAB pointer and reschedules itself
```

Addresses come from the exact ROM symbol dump: `PATTK=$D2B8`, `PATT20=$D2F7`,
`PATT24=$D31F`, `PATT30=$D32C`, `PATT40=$D33A`, `PATT99=$D375`,
`ATTACK=$D3D7`, `DAMAGE=$D40C`, `CFIND=$CF82`, and `RANDOX=$C4CF`.

## Exact captured pre-state

The following table is the state at `PATTK`, before the command mutates combat data.

| State | Value | Evidence |
|---|---:|---|
| AUTTAB pointer | `$D9B1` | **Observed** at `$020D`; this selects command 10 in the built ROM. |
| dungeon level | `2` | **Observed** at `LEVEL=$0281`. |
| player row,column | `(9,22)` / `$0916` | **Observed** at `PROW=$0213`. |
| player direction | `0` | **Observed** at `PDIR=$0223`. |
| player power | `6048` / `$17A0` | **Observed** at `PPOW=$0217`. |
| player damage | `16` / `$0010` | **Observed** at `PDAM=$0221`. |
| left hand | `$0EA3` | **Observed** at `PLHAND=$021D`; not selected by this command. |
| right hand | `$0E87` | **Observed** at `PRHAND=$021F`. |
| active torch | `$0E95` | **Observed** at `PTORCH=$0224`. |
| general RNG seed | `$F194D4` | **Observed** at `SEED=$026B..$026D`. |
| level population row | `CMXPTR=$03B0` | **Observed** at `$0282`; source defines `CMXPTR` as the current `CMXLND` row. |

### Right-hand weapon

The 14 bytes at `$0E87` are identical before and after the command:

```text
00 00  00 00  3C 01  00 00 00  0D 04 00 00 28
```

Using `CD.ASM:P.OCPTR..P.OCPHO`, this is an owned type `$0D`, class `4` object
with magic offense `0` and physical offense `40`. `DTABAS.ASM:OBJXXX` identifies
that tuple as the level-zero iron sword (`T.SWO2`, `K.SWOR`, `0`, `40`). This is
**observed record data** and a **source-confirmed identity**. `PATTK.ASM:18-40` copies
its offense to the player attack block and emits `SNDOBJ + K.SWOR`, whose
`SOUNDS.ASM:SNDTAB` entry is `A$SWOR=16`, `WHOOSH`.

### Active torch

The 14 bytes at `$0E95` are also unchanged:

```text
00 00  00 00  3C 01  0E 07 00  0F 05 00 00 05
```

Type `$0F`, class `5` is the pine torch (`T.TOR4`, `K.TORC`) in
`DTABAS.ASM:OBJXXX`. `PTORCH` points to it and its type is not the dead-torch type.
Therefore `PATTK.ASM:PATT22` is bypassed. This command performs no secondary
darkness RNG draw.

### Selected creature

`CFIND` returned `$0506`. From `CCBLND=$03D4` and `CC.LEN=17`, that is CCB slot 18
(zero based). Its authoritative pre-command bytes were:

```text
02 C0  00 80 80 30  11 0D  00 00  00 00  FF 05 00 09 16
```

Decoded with `CD.ASM:P.CCPOW..P.CCCOL`:

| Field | Value |
|---|---:|
| power | `$02C0` = 704 |
| magic offense / defense | `0` / `128` |
| physical offense / defense | `128` / `48` |
| movement / attack delay | `17` / `13` tenth-second units |
| carried-object pointer | `0` |
| accumulated damage | `0` |
| in-use | `$FF` |
| type | `5` |
| direction | `0` |
| row,column | `(9,22)` |

`DTABAS.ASM:CREXXX` defines type 5 with exactly those attributes as `SGINT2`, Stone
Giant 2. The creature was already live and co-located with the player; command 10 did
not generate it. `COMCRE.ASM:CFIND` scans CCB order for matching row/column and a
nonzero `P.CCUSE`, explaining why this slot is selected.

## Exact combat resolution

### Attack energy cost

**Source-confirmed:** `PATTK.ASM:24-32` adds the weapon's two offense values, divides
that radix-7 percentage by eight, scales current power through `SCAL16`, and adds the
result to player damage before creature lookup.

**Derived from observed state:** `(0 + 40) >> 3 = 5`, then source `SCAL16`
computes `floor(6048 * 5 / 128) = 236`. Damage changes from `16` to `252` (`$00FC`).
The debugger observed `$00FC` before `CFIND`, and the post image retains it. Thus an
attack spends energy even if no creature is found or the attack later misses.

### Hit probability and RNG

**Source-confirmed:** `PATTK.ASM:ATTACK` starts index `T0=15`, computes four times
the defender's remaining power, repeatedly subtracts attacker power, converts the
remaining index to a reward or penalty, consumes one byte from the general RNG, then
tests `random + adjustment - 127` for a negative miss.

For this state:

```text
defender remaining power = 704 - 0 = 704
four times defender      = 2816
first subtraction        = 2816 - 6048, borrow
T0 remains               = 15
reward                   = (15 - 3) * 10 = 120 ($0078)
```

The debugger observed `$0078` stacked at `ATTK30`. `RANDOX` consumed seed
`F1 94 D4`, returned `A=$65` (101), and left seed `65 F1 94`. `RANDOM.ASM:RANDOX`
confirms that one call performs eight polynomial-feedback shifts across the 24-bit
seed and returns the new high byte. The final signed test value was:

```text
101 + 120 - 127 = 94 ($005E)
```

The debugger observed `D=$005E`, a nonnegative result, at `ATTK99`, followed by the
hit branch at `$D31F`. Exactly one general-purpose RNG call was consumed by the
player attack. The active pine torch prevented the additional darkness call.

### Hit, damage and death

The hit path emits `A$KLK2=18`, `KLINK`, and the source hit indication before
`DAMAGE`. `DAMAGE` applies magic and physical components separately using the same
radix-7 `SCAL16` routine:

```text
magic:   floor(floor(6048 * 0   / 128) * 128 / 128) = 0
physical first scale: floor(6048 * 40 / 128) = 1890
physical defense:     SCAL16(1890,48)         = 708 ($02C4)
```

The second physical result is 708 rather than the real-number result 708.75 because
the 6809 routine truncates its radix-7 multiplication. The debugger and RAM image
both observed creature damage change from `$0000` to `$02C4`. Since 708 exceeds
power 704, the source comparison takes the death path at `$D32C`.

### Kill and reward

The creature carried no objects, so the source drop loop has nothing to relink or
place. The remaining source-confirmed kill actions are visible in the RAM delta:

- `CMXPTR=$03B0`; its type-5 population byte at `$03B5` changes `6 -> 5`.
- CCB `$0506` damage changes `0 -> 708` and `P.CCUSE` changes `$FF -> $00`.
- `PATTK` requests a view update and emits `A$EXP0=21`, `BANG`.
- one eighth of creature power is absorbed: `704 >> 3 = 88` (`$58`). Player power
  changes `6048 -> 6136` (`$17F8`).
- type 5 is neither special wizard type 10 nor 11, so neither endgame branch runs.

The complete foreground sound sequence for command 10 is therefore
`WHOOSH -> KLINK -> BANG`. These are source event identities. Their SSC recipes are
the already established enhanced backend and are not part of the combat calculation.

## Post-state and return boundary

The saved post image was taken at entry to `PATT99`, before its `SWI HUPDAT`; it must
not be described as a capture after the heartbeat update. The relevant captured delta
is:

| Address/state | Before | At `PATT99` |
|---|---:|---:|
| `PPOW $0217` | `$17A0` (6048) | `$17F8` (6136) |
| `PPHO $021B` | `0` | `40` |
| `PDAM $0221` | `$0010` (16) | `$00FC` (252) |
| `SEED $026B..D` | `$F194D4` | `$65F194` |
| current type-5 population `$03B5` | `6` | `5` |
| creature damage `$0510` | `0` | `$02C4` (708) |
| creature in-use `$0512` | `$FF` | `0` |

**Source-confirmed subsequent control flow:** `PATT99` invokes `HUPDAT`. With the new
power and damage, `HUPDAT.ASM` computes the increment-before-borrow quotient used by
the existing port:

```text
HEARTR = floor((6136 * 64) / (6136 + 2 * 252)) + 1 - 19 = 41
```

This neither faints nor kills the player. Control returns through the shared
`ASRD7..ASRD` tail and `RTS`, then `HUMAN` clears the line buffer and emits the next
prompt. `PLAYER:PLAY50` subsequently stores its already-advanced token pointer and
reschedules at `Q.JIF`. This return sequence is established by source, not by a
post-`HUPDAT` debugger snapshot.

## State dependencies and limits of the capture

The attack result at command entry depends directly on the player attack block,
right-hand OCB, active torch, selected CCB, current level population row and 24-bit
general RNG seed. Earlier autoplay, creature scheduling and random activity are what
produce that exact pre-state. Combat M1 must use the existing authentic packed
OCB/CCB arrays and RNG stream rather than inject this outcome as a command-10 special
case.

The two captures establish one exact cold-start demo outcome. They do not establish:

- every hit-probability branch, miss, darkness miss or ring behavior;
- creature attacks against the player, shielding, fainting or player death;
- carried-object drop behavior with a nonempty creature object list;
- wizard type 10/11 endgame behavior;
- whether every scheduler interleaving reaches this same command-10 state from an
  arbitrary timing epoch;
- pixel or PCM timing for the three semantic sounds.

Those are outside Combat M1 unless required to preserve the connected source path.
No additional MAME run was needed: the two debugger captures plus original source
resolve the attract command's target, RNG, hit, damage, death, reward, events and
return behavior.

## Proposed minimum faithful implementation boundary

Production work should begin only after review of this evidence. The smallest faithful
Combat M1 slice is:

1. Extend the existing parser with ordinary source-compatible `ATTACK LEFT|RIGHT`
   hand selection; do not special-case autoplay command 10.
2. Operate directly on the established 14-byte OCBs, 17-byte CCBs, player fields and
   three-byte RNG. Keep record order and address-token translation at the existing
   boundary.
3. Port `PATTK` attack-energy expenditure, object-class event selection, ring checks,
   `CFIND`, `ATTACK`, live-torch darkness gating and `DAMAGE` with the original
   truncating radix-7 arithmetic.
4. Implement hit/miss indication and source semantic events through the existing audio
   client: weapon event, `KLINK` on a hit, and `BANG` on this kill. Do not alter SSC
   ownership, recipes, native heartbeat or Wizard buzz.
5. On death, preserve the complete connected cleanup contract: drop carried OCBs,
   decrement the current level/type population, clear `CCUSE`, request redraw, absorb
   one eighth power with the source cap, and retain the type-10/type-11 branch
   boundaries without implementing unrelated endgame presentation.
6. Run the existing health calculation after attack energy/reward so heartbeat cadence
   follows the new authoritative player power/damage. Do not change VIRQ or visual
   heartbeat timing.
7. Connect the existing creature scheduler's `combatPending` boundary only as needed
   to preserve state/order. Creature-against-player combat remains a separate tested
   extension unless the attract path demonstrably requires it before command 10.
8. Add exact fixture tests for the captured command-10 pre-state and assert the RNG
   transition, `WHOOSH/KLINK/BANG` event order, player and creature deltas, population
   decrement, final `HEARTR=41`, and next-command readiness.

The acceptance reference for the attract path is the exact transition documented
above: live SGINT2 slot 18 at `(9,22)`, seed `$F194D4`, iron sword attack, one RNG draw
to `$65F194`, hit result `$005E`, damage `$02C4`, kill, population `6 -> 5`, power
`$17A0 -> $17F8`, player damage `$0010 -> $00FC`, and the three semantic sound events.

## Production implementation

The implementation remains in the existing gameplay state layer. `game.c` adds a
general `ATTACK LEFT|RIGHT` path rather than an autoplay or command-10 branch. It uses
the port's existing packed 14-byte OCBs, 17-byte CCBs, 24-bit RNG, player fields and
health calculation. Address-token OCB links are translated only at the existing OCB
boundary. `GameCombat` reports the source-visible result and ordered semantic sound
events to the platform layer without placing OS-9 or SSC operations in game logic.

The implemented order follows `PATTK.ASM`:

1. select the requested hand and substitute `EMPHND` when it is empty;
2. copy magic/physical offense, charge attack energy with `SCAL16` truncation, and
   request the weapon-class sound;
3. consume ring charge where applicable, find the first live same-cell CCB, and run
   the original hit calculation;
4. consume the second RNG byte only for the source darkness gate;
5. request `KLINK`, apply separately truncated magic and physical damage, and take
   either the nonlethal return or death path;
6. follow the creature's packed carried-object list, drop those OCBs into its cell,
   release the CCB, request `BANG`, absorb one eighth of creature power with the
   original cap, and recalculate health/heartbeat rate.

The current level has no separately retained `CMXLND` regeneration matrix in the
bounded port. Its population is represented by live packed CCBs; releasing the killed
type-5 CCB changes the observable type-5 population from six to five. Level-transition
regeneration remains outside M1.

`main.c` sends `GameCombat.events` through the established semantic `dodaudio` client.
The service is started lazily at `/d1/dodaudio` with the existing `ssc-mame-fast`
profile. Each event is submitted and drained before the next, preserving the original
synchronous event order without moving waveform generation into the game process.
The native VIRQ heartbeat code and the 23-effect SSC catalog are unchanged.

Audio is an optional presentation service. `main.c` propagates the authoritative
post-command heartbeat rate before it calls the audio client. Failure to start the
service, validate its greeting, send or receive an event, or drain an effect is not a
game error. Startup failure marks audio unavailable for the rest of that process, so
later attacks do not repeat a failing fork. An established client that fails is
cancelled/closed and likewise disabled. Combat state, RNG state, parser continuation,
heartbeat propagation and normal exit do not depend on audio availability. Audio
shutdown errors during final cleanup are also presentation-only and do not replace the
game's exit status.

## Approved correction to the pre-Combat test contract

One existing `test_bag.py` expectation classified empty-hand `ATTACK LEFT` as invalid
and required rejection. That expectation contradicted the original source and was
changed only after explicit approval:

- `PATTK.ASM:11-14` selects the hand and substitutes `EMPHND` when its pointer is zero.
- `COMDAT.ASM:123-127` initializes `EMPHND` with class 4, magic offense 0 and physical
  offense 5.
- `PATTK.ASM:24-40` charges energy and emits `SNDOBJ + class`; `SOUNDS.ASM:80-90`
  maps class 4 to `WHOOSH`.

The corrected legacy test now requires success, unchanged initial state, zero energy
at power 160 (`(0 + 5) >> 3 == 0`), and no RNG consumption when no creature occupies
the cell. A new Combat M1 assertion additionally observes the class-4 `WHOOSH` event.
All unrelated bag assertions remain unchanged.

## Golden fixture and bounded branch coverage

The host C fixture mechanically constructs the documented command-10 state and runs
the production combat function. Its exact assertions pass:

| Result | Required and observed |
|---|---:|
| attack energy / player damage | `236` / `252` |
| RNG calls / transition | `1`, `$F194D4 -> $65F194` |
| signed hit result | `94` |
| physical damage | `708` |
| target / death | CCB 18 / killed and released |
| type-5 live population | `6 -> 5` |
| player power / heartbeat rate | `6136` / `41` |
| semantic events | `WHOOSH -> KLINK -> BANG` |

The same test executable also verifies parser continuation after the golden attack,
empty-hand class/energy/RNG behavior, a
left-hand ring with charge consumption and a guaranteed nonlethal hit, a weak miss,
the second darkness RNG draw and miss, packed carried-object dropping, and invalid
direction rejection. These branches use the same production function as the golden
fixture. No command-10 constant or fixture-specific production branch exists.

## Build provenance and module impact

The reproducible target command is generated by `build_gameplay.py` and invokes CMOC
with `--os9 -O0 --intermediate --verbose --add-os9-stack-space=1536`, the application
include paths, and the production gameplay, presentation, OS-9, heartbeat and audio
client sources. The resolved tools were:

```text
/usr/local/bin/cmoc   cmoc 0.1.90
/usr/local/bin/lwasm lwasm from lwtools 4.22
/usr/local/bin/lwlink lwlink from lwtools 4.22
```

CMOC compiled and drove the OS-9 link. Its generated/intermediate assembly used
`lwasm`; `lwlink` produced the module. Two clean builds were byte-identical with
SHA-256 `9d802e55458c7024bed1bb9df6bda0787c37b11fa461c6ab0a2496573f91ce2f`.
ToolShed identifies `dodgame` as an edition-1, re-entrant/read-only OS-9 6809 program:

```text
module size  $96B1 / 38,577 bytes
data size    $2A14 / 10,772 bytes
entry        $000D
CRC          90C7EF (Good)
```

The committed pre-Combat artifact rebuilt as 33,815 bytes with 10,765 bytes of data
and CRC `9D2CD2`. Combat M1 therefore adds 4,762 module bytes and seven requested data
bytes. The increase includes linking the already established audio client/event code;
it does not add a framebuffer or another large runtime allocation. The 64K Level II
process ceiling remains a constraint for subsequent milestones. The optional-audio
correction added eight bytes relative to the audited 38,569-byte build. The module
still uses five 8K program blocks and has 2,383 bytes before the 40,960-byte boundary.

## Validation and limitations

All 20 Daggorath host test scripts passed after the implementation, including 13
Combat M1 source-state checks and the corrected 13-test bag suite. The complete MCP
suite passed 115/115 and the TypeScript build passed. Both clean target builds were
byte-identical and ToolShed reported a good CRC.

The existing audio client suite now also proves the optional policy: the golden three
events retain `WHOOSH -> KLINK -> BANG` order when the service works; startup failure
is swallowed and not retried; and a send failure after successful initialization
cancels the client and disables later audio. The Combat fixture retains the exact
rate-41 state and accepts the next ordinary parser command. A source-order assertion
guards the critical `combat -> heartbeat propagation -> optional audio` sequence.

No MAME run was made for this implementation pass. The exact cartridge result was
already established by the two reproducible debugger captures, and the task forbids
reusing the crashing Lua tap path. Consequently the arithmetic/state transition and
semantic event sequence are verified, while live PCM timing and an EOU execution of
the new module remain future integration evidence.

M1 still excludes creature attacks against the player, complete shield/defense and
faint/death presentation, level regeneration, wizard/endgame branches, and Attract
M1B chaining. Its type-10/type-11 endgame effects are not implemented. Combat sound
events use the already accepted enhanced SSC recipes; they are semantic mappings, not
claims of cartridge-waveform identity.
