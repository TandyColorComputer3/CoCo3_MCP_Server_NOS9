# Daggorath Attract Compatibility C4: PREPARE to the lone-dot dungeon

**Status:** source-derived C4 review candidate; C3 is commit `0ab3c022783fd2a11df0f1f73ac66dbcc10e7d55`; C4 is intentionally uncommitted.

## Cartridge source boundary

C4 starts after the accepted C3 `MISC.ASM:PREPAX` presentation.  The recovered
cartridge path is:

```text
ONCE.ASM:GAME20 -> NEWLVL.ASM:NEWLVX
  -> DGNGEN.ASM:DGNGEN
  -> COMCRE.ASM:CBIRTH (CMTTAB types in descending source order)
  -> NEWLVL object-to-creature attachment
ONCE.ASM:GAME30 -> COMOBJ.ASM:OBIRTH/OCBFIL (DEMDAT)
ONCE.ASM:GAME40 -> MAPPER.ASM:MAPPER -> PUPDAT
  -> MISC.ASM:WAITX (81) -> WAITX (81) -> SYNC -> SYNC
ONCE.ASM:GAME50 -> PLOOK.ASM:INIVUX/PLOOK -> MISC.ASM:PROMPX
```

The compatibility core owns this path in `src/compat/c4.c`; it does not call
the modern `game_init_demo()` runner.  Its initializer is deliberately
source-structured and uses the same recovered `LVLTAB`, `OMXTAB`, `CMTTAB`,
`ODBTAB`, `CDBTAB` and `XXXTAB` data plus the 43-byte original-derived RANDOM
leaf already used on the target.  The modern initializer remains only an
independent, previously established semantic oracle.

## Deterministic state oracle

For source demo second 21 the compatibility initializer and the accepted
source model compare byte for byte across all 2,608 packed `Game` bytes.

| Item | C4 result |
| --- | --- |
| Complete packed state | 2,608 / 2,608 equal; SHA-256 `d8f29331f6383203e5a5cc6b2e0b0498f962a9d73ef6c34c6d0df6aca50d9b33` |
| RNG | `75 65 19`; 2,344 source transitions |
| Maze | 1,024 / 1,024 equal |
| Objects | 72 x 14-byte table equal; 66 live allocations |
| CCBs | 32 x 17-byte table equal; 23 active allocations |
| Player | level 2, `(12,22)`, north, power 6048 |
| DEMDAT | sword, torch and leather shield exact |

The compatibility process also mirrors the original-layout CCB, maze and OCB
tables at `$03D4`, `$05F4` and `$0B15` relative to the cartridge address
domain, together with the source player, inventory, light, RNG, autoplay and
map fields.  The complete 3,333-byte compatibility image is retained as
`test/fixtures/compat-c4/state.bin` (SHA-256
`6c9531c3c0d888785e5204b17e4b168e365a78f395cafe6fb4031bec5fec01e6`);
its port-owned unused/scratch bytes are zero rather than being claimed as a
cartridge RAM capture.  Consequently the packed semantic state and named
original-layout fields/tables are the authoritative equality checks.

## Map and first dungeon

`dag_compat_c4_map` is the `MAPPER/MAPP10/MARK4` closure: each 32 x 32 maze
cell occupies one byte by six scanlines, followed in source order by visible
level objects, active CCBs and the player marker.  Level two has no VFTTAB
vertical-feature entries.  Its complete 6,144-byte logical frame equals the
independent source MAPPER oracle:

```text
SHA-256 3646378c9ee7d845f2cd1a3489b1570cc7f5b7e550eb3f6ceef3704c65a5ec68
```

At the first dungeon boundary regular and magic light are both zero.
`VIEWER -> SETFAD -> VCTLST` therefore rejects all vector lists before pixel
generation.  The correct cartridge viewport is black; no cached scene is
replayed and no modern logical dungeon renderer is called.  C4 retains the C1
original vector engine for the first later lit view, while using the same
visibility result here. `INIVUX` supplies the inverse EMPTY/heart/EMPTY status
strip.  C2 then performs `CLRPRI`, `PROMPX` CR and dot.  Dungeon/status and the
dot are separate presentation boundaries, with the source `PUPDAT` visibility
SYNC between them.

| Logical oracle | Bytes | SHA-256 |
| --- | ---: | --- |
| initial dark dungeon/status before prompt | 6,144 | `458f0d9799e59e04143d3a532b2f9589b4e6accc88caf280e68b0a9ee046e157` |
| stable lone-dot frame | 6,144 | `d8c0d60b1b6cf2f0bbbc6cf5152d161ed5b52f49316eca5be5d72e139cf89a8d` |

The four primary rows are otherwise blank.  No `OK`, Wizard or map bytes
remain.

## Target timing

The clean RGB `coco3h`, 2 MB restored-EOU run used staged module SHA-256
`12c680aadd0bd37189b666113223c3e522aeacb3052c3462995eca007d9cdb55`.
Completed-frame sampling brackets scan-out boundaries, while the unit test
checks the exact source waits.

| Boundary | Cartridge | C4 | Delta | Status |
| --- | ---: | ---: | ---: | --- |
| near-black -> PREPARE | about 0.050 s | <=0.130 s (retained C3) | <=+0.080 s | bounded adapter difference |
| PREPARE -> first meaningful map | about 6.442 s | 6.271 s | -0.171 s | inside semantic envelope |
| PREPARE -> stable full map | about 6.442 s | 6.507 s | +0.065 s | MATCH within transfer/observer bound |
| stable map dwell | 162 ticks / 2.700 s | exactly 81+81 source ticks; 2.984 s visible bound | +0.284 s | source wait exact; transfer/scan-out bound |
| map -> full black | one presentation boundary | <=0.116 s sampled | — | MATCH |
| full black -> dungeon/status | about 0.216 s | 0.254 s sampled | +0.038 s | bounded full-frame adapter cost |
| dungeon/status -> lone dot | one source PUPDAT SYNC then prompt | 0.136 s sampled; one exact jiffy plus one cell transfer | — | source boundary retained |

The initialization and map work occurs after PREPARE, as it does in `GAME20`.
No pre-PREPARE preload hides it.  The selected original-derived initializer
fits within the cartridge choreography interval without artificial sleeps.

## Level II budget

| Artifact | Program | CRC | SHA-256 | Data request |
| --- | ---: | --- | --- | ---: |
| `dodcinit` | 12,756 (`$31D4`) | `9E475B` | `12c680aadd0bd37189b666113223c3e522aeacb3052c3462995eca007d9cdb55` | 13,932 (`$366C`) |

This is two program blocks, two data blocks and one CoWin mapping: **5/8 DAT
blocks**.  Process data includes the 3,333-byte original-layout state, 6,144-byte
logical frame, 2,608-byte packed semantic state, 1,536-byte stack and 311 bytes
of globals/alignment.  Presenter scratch remains in the one 2,048-byte mapped
CoWin strip.  A third program or data block is not consumed.

## Review evidence

- [continuous C4 run](assets/daggorath-attract-compat-c4/c4-opening.mp4)
- [original | C4 map](assets/daggorath-attract-compat-c4/pairs/map-original-eou.png)
- [original | C4 black transition](assets/daggorath-attract-compat-c4/pairs/black-original-eou.png)
- [original | C4 initial dungeon](assets/daggorath-attract-compat-c4/pairs/initial-dungeon-original-eou.png)
- [original | C4 lone-dot dungeon](assets/daggorath-attract-compat-c4/pairs/lone-dot-original-eou.png)

The original side contains capture color/fringing; exact acceptance is against
the source-derived one-bit logical oracles above.  The compatibility review
executable returns after its three-second review hold and does not enter
EXAMINE or AUTTAB.

## C5 connected dependency closure

C5 should bring across one connected execution graph rather than another set
of hand-authored frames:

```text
HUMAN.ASM:PLAY20/PLAY30 -> TOKEN.ASM:AUTTAB -> parser/command handlers
  -> COMPLR.ASM:SCHED/CLOCK queue ordering
  -> PLAYER/PUPDAT/LUKNEW presentation requests
  -> CRETUR.ASM:CMOVE/CWALK/CMOV20
  -> VIEWER/VCTLST (the existing C1 PIC vector engine)
```

The bounded first closure ends at the recognizable Giant: EXAMINE, torch
pull/use, first MOVE, shield/sword changes, source scheduler promotions and
the successive Giant redraws.  Parser state, OCB/CCB state and scheduler
timers should live in the existing compatibility state; OS-9 adapters should
remain limited to presentation, asynchronous semantic audio and interruption.
No C5 code is included in C4.
