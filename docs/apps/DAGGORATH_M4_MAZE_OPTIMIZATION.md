# M4 source-equivalent maze-generation candidate

The selected change replaces only the production `game_random(Game *)` leaf with
a 43-byte relocatable 6809 routine adapted from the original cartridge's
`RANDOM.ASM:RANDOX`. The original parity mask `$E1`, eight feedback rounds,
and three-byte rotate order are retained. Maze carving, ordinary and secret
door placement, CCB/OCB construction, level data, and source wait placement
remain unchanged. Host tests continue to compile the C reference. This is
not a 6309-specific path. The read-only cartridge source checkout was
`/Volumes/SEDONA/Projects/daggorath-reference` at
`a94326f00ebb16a106b540c58bc2ccf5f7b66dac`, specifically
`DGNGEN.ASM:DGEN10–DGEN90`, `RANDOM.ASM:RANDOX`, and `NEWLVL.ASM`.

## Exact-state oracle

`apps/daggorath/test_maze_oracle.py` checks the complete 2,608-byte host
`Game` image, final seed, player level/position/orientation, OCB/CCB counts,
and exact RANDOM call count for normal level 0 and demo level 2 with final
spin values 0, 21, and 37. Demo level 2 with spin 21 additionally has
independent original GAME40 maze, OCB and CCB hashes in `test_demo_init.py`.
Its oracle is 2,344 RNG calls, final seed `75 65 19`, 66 OCBs, 23 CCBs,
player `(12,22)`, level 2. The normal level-0/spin-0 fixture makes 2,470
RNG calls. `Game.seed` remains at byte offset 2,576.

A disposable EOU target program independently compared the assembly routine
with the C reference over 2,048 consecutive calls across 128 varied seeds,
then compared all 2,579 source-packed maze/OCB/CCB/seed bytes after both
demo level-2/spin-21 and normal level-0/spin-0 initialization. A private
counting wrapper around the same assembly leaf measured 2,344 and 2,470
calls respectively. Both target runs printed `MAZE PASS`. This target proof
used only disposable modules/media; it is not an added routine runtime test
in the normal host suite.

## Measured candidates

All three target runs used the same disposable `dodintro` clock-marker layout
on restored EOU `coco3h`, 2 MB, RGB. The production-selected build has no
clock markers. One tick is nominally 1/60 s. Module sizes below are from
the **instrumented** comparison builds; function bytes come from the lwasm
listings. The `maze()` C body is otherwise identical in all three.

| Implementation | Maze ticks | Nominal time | Speedup | RNG routine bytes | Exact state | Init RNG calls |
| --- | ---: | ---: | ---: | ---: | --- | ---: |
| CMOC 0.1.90 `-O2` baseline | 919 | 15.317 s | 1.00× | 187 | oracle | 2,344 |
| CMOC-friendly C parity fold and fixed three-byte shift | 370 | 6.167 s | 2.48× | 162 | PASS, five host fixtures | 2,344 |
| Original-structured 6809 assembly | 273 | 4.550 s | 3.37× | 43 | PASS, host and EOU packed-state proofs | 2,344 |

The corresponding full `game_init_demo()` timings were 962, 391, and 291
ticks (16.033, 6.517, and 4.850 s). Instrumented `dodintro` sizes were
23,696, 23,671, and 23,552 bytes. The final uninstrumented assembly build
is 22,568 bytes, data request 10,924 bytes, CRC `1C53C0`.

| Maze phase (ticks) | Baseline | Optimized C | 6809 assembly |
| --- | ---: | ---: | ---: |
| Clear/seed/starting cell | 1 | 1 | 1 |
| Carve 500 cells | 455 | 204 | 160 |
| Fill surrounding wall bits | 48 | 48 | 48 |
| 70 ordinary doors | 214 | 60 | 33 |
| 45 secret doors | 194 | 55 | 30 |
| Final 21-transition spin | 7 | 2 | 1 |

The unchanged 48-tick wall pass and the large reductions in RNG-heavy carve
and door phases attribute the speedup to the RNG leaf rather than a changed
maze search, fewer retries, or skipped work.

The baseline generated `game_random()` has two nested eight-count loops plus
a three-byte indexed loop. It repeatedly spills byte locals to the U stack,
computes `Game.seed` at offset 2,576, and calls stack checking. The C
candidate removes the eight-step parity loop but still uses 162 bytes of
generated code with stack temporaries, byte shifts, and repeated seed loads.
The 6809 leaf retains the original `LSLA`/`LSRB`/`ROL` structure, takes the
Game pointer from CMOC's `2,S` argument, preserves X/Y/U (including resident
CMOC Y), returns the byte in B, and writes only caller-owned seed bytes. It
uses no absolute address or 6309 instruction. Its additional 97-tick (~1.62
s) improvement over C, smaller module footprint, and direct source tracing
justify the bounded assembly despite C's simpler maintenance. A 6309 path
was not pursued: the mandatory 6809 implementation already removes most of
the latency and no remaining 6309-specific hotspot was established.
Porting all of `DGNGEN.ASM` would also require replacing its absolute
`MAZLND` references and SWI-mediated RANDOM/NEGRAM calls with caller-owned
`Game` access and CMOC-compatible entry/return rules. That is feasible in
principle but substantially broader and riskier than this measured leaf
replacement; the C maze body remains source-equivalent and is left intact.

## Visible opening effect and scope

The accepted prior completed-frame PREPARE→full-map interval was 20.543 s.
A completed-frame capture of the byte-verified selected production build at
about 59.92 fps first showed PREPARE at 61.805 s and the full map at 70.965
s, an interval of about **9.16 s**. The ~11.38 s reduction is consistent with
the measured 646-tick (10.77 s) maze reduction plus capture/run variation.
The remaining visible interval includes the separately measured ~2.7 s
heartbeat-pack load, map construction/transfer, and other existing work.
The Wizard-blank→PREPARE handoff is a separate unresolved latency issue; this
maze change does not shorten it. No four-row text path or opening wait changed.

All 39 Daggorath `test_*.py` scripts passed. Two clean builds of `dodintro`,
`dodgame`, `dodcmd`, `dodsched`, and `doddemo` were byte-identical; ToolShed
reported good OS-9 module CRCs. `dodgame` is 27,287 bytes (below 30,720),
`dodcmd` 7,004 bytes, `dodsched` exactly 8,192 bytes, and `doddemo` 23,905
bytes. Process data requests remain within two 8K blocks. The selected
routine changes no public ABI or packed `Game` offset.

## Pre-M5 checkpoint verification

Fresh validation of the combined M3/M4/maze tree passed all 39 Daggorath
`test_*.py` scripts, MCP 115/115, TypeScript build, whitespace checks, and
141 local documentation links. Two new clean builds of every module below
were byte-identical and passed ToolShed CRC validation. No production source
was changed during checkpoint separation. The pre-maze M3 staged runner and
M4 staged opening were also independently built/tested before their commits.

| Module | Bytes | Data request | CRC | SHA-256 |
| --- | ---: | ---: | --- | --- |
| `dodgame` | 27287 | 11109 | `8BB638` | `d9b7e4b98b3bd6631bd0b5dacd4fda34465eaeda6de00ea8532564a2d806956a` |
| `dodcmd` | 7004 | 0 | `C3B942` | `fbf2424f991afe7610a815113b1084372afa495272587be187b9a1872726b946` |
| `dodsched` | 8192 | 0 | `DA8721` | `82deac540cd172f9fdd99d26b2a6989c3d8193265ad60b8b2ded35d594bff6b0` |
| `doddemo` | 23905 | 10896 | `9A932C` | `e6d16db714768a8708ece38d75a9580b6e797cc877dd18681769d16630b2b0ab` |
| `daggorath` | 863 | 1573 | `7CE837` | `cb950a95d2c2352fee61943600ccccfdf0e42f219c69551a606e6baecb72c7bb` |
| `dodwiz` | 24562 | 7855 | `A25DCA` | `8dfe734d862f621c07b60db5badc62e3ec5282cc4fbe57ab8250957f625b3c3f` |
| `dodintro` | 22568 | 10924 | `1C53C0` | `d7753469c39621742d693da30b90b7bc139c1f393069cef59b55d8a187fff8a6` |
| `dodaudio` | 10086 | 2142 | `CD93E9` | `f6a87a81b692ed92a44be1f9431895e3259c173bf96dace006092514c89d5ca5` |

`dodgame` uses four 8K program blocks and two data blocks; CoWin leaves
one slot for alternating `dodcmd`/`dodsched`. `dodsched` has no growth
headroom. Callable modules request no private data; their state remains in
the host. `dodintro` and `doddemo` each occupy three program and two data
blocks. `dodwiz` occupies three program and one data block.

Canonical SHA-256 checks matched the established baselines for `63SDC.VHD`,
`63SDC-MCP-DEV.VHD`, `63EMU.DSK`, and `nos9_ready_v2.sta`; `63SDC.VHD`
remained mode 0444. No private MAME/observer process remained. The normal
configured MCP connector was left running; it reported no MAME or bridge.
`cfg/`, `snap/`, disposable disks, scripts and raw captures were excluded.

These checkpoints preserve known visual debt and do not claim M3's 33.34 ms
latency gate or the future Giant sequence has passed. Opening debt is listed
in [the M4 report](DAGGORATH_CARTRIDGE_OPENING_M4.md#opening-polish-debt-retained-for-m5).
