# Dungeons of Daggorath Combat Milestone 1 Reference

Combat M1 is currently a reference-analysis checkpoint. No production combat code is
implemented by this pass. Its purpose is to resolve the command-10 `ATTACK RIGHT`
dependency that blocked the full attract loop in
[`DAGGORATH_ATTRACT_M1.md`](DAGGORATH_ATTRACT_M1.md).

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
