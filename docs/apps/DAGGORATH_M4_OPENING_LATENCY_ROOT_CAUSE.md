# M4 opening latency: private root-cause measurements

This is analysis of the frozen M3/M4 implementation, following the [opening timing ledger](DAGGORATH_M4_OPENING_TIMING_ATTRIBUTION.md) and the [source-built cartridge storyboard](DAGGORATH_CARTRIDGE_STORYBOARD.md). All instrumentation and disk images were disposable under `/private/tmp/daggorath-m4-rootcause*`. No repository production or test source, canonical medium, or save state was changed. Each private run used the restored EOU Term, a byte-verified staged artifact, one public Shell+ launch, and deferred short clock output after graphical cleanup. Private MAME sessions were stopped. The repository's `MCP/Documents/` is absent in this checkout; the OS-9 ownership analysis below is tied to identified source and live behavior, not a guessed manual rule.

## A. The 16-second demo initialization

The target probe inserted one `os_clock()` sample at each `init_level()`/`maze()` boundary while retaining the three-block `dodintro` mapping. Raw relevant samples from the first full target run:

```text
TM24 066B 066D 0674 0A0B 0A2B 0A2C 0A2D 0A2D
TM32 0674 0675 083C 086C 0942 0A04 0A0B 0000
```

`game_init_demo()` measured 962 ticks (16.033 s) in this instrumented run, versus 960 ticks (16.000 s) in the earlier coarser probe. The two-tick difference is consistent with added markers and run variation. Percentages below use the 962-tick run and are approximate because each boundary has one-tick resolution.

| Source/port suboperation | Calls or deterministic work | Ticks / nominal time | Share of 962 ticks |
| --- | ---: | ---: | ---: |
| Clear 2,608-byte `Game`, set level/player fields | one clear | 2 / 0.033 s | 0.2% |
| Create initial OCB pool | 63 births; total objects after DEMDAT 66 | 7 / 0.117 s | 0.7% |
| `maze()` / source `DGNGEN` equivalent | 500 carved cells, 70 doors, 45 secret doors, final 21 RNG transitions | **919 / 15.317 s** | **95.5%** |
| Birth/place 23 CCBs | 23 successes, 44 candidate cells | 32 / 0.533 s | 3.3% |
| Attach source-selected objects to creatures | one pass over OCBs | 1 / 0.017 s | 0.1% |
| Birth/attach three DEMDAT bag objects | 3 births | 1 / 0.017 s | 0.1% |
| Health, recovery and burn initialization | one | 0 sampled / <1 tick | <0.1% |

The maze itself divides as follows:

| Maze phase | Calls/iterations | Ticks / nominal time | Share of 919 maze ticks |
| --- | ---: | ---: | ---: |
| Clear 1,024 cells, seed and choose starting cell | one clear, one candidate | 1 / 0.017 s | 0.1% |
| Carve 500 cells | 514 direction/distance selections, 1,527 tentative steps, 7,374 `cell()` calls, 1,028 RNG calls | **455 / 7.583 s** | **49.5%** |
| Fill surrounding wall bits | 1,024-cell scan, 3,024 `cell()` calls | 48 / 0.800 s | 5.2% |
| Place 70 ordinary doors | 253 candidate attempts, 630 RNG calls | 214 / 3.567 s | 23.3% |
| Place 45 secret doors | 239 candidate attempts, 575 RNG calls | 194 / 3.233 s | 21.1% |
| Source `DGEN90` final spin | 21 RNG calls | 7 / 0.117 s | 0.8% |

The iteration/RNG counts came from a **separate disposable host count build** of the same `game.c` control flow, because incrementing counters on every target RNG/cell call would contaminate the clock measurement. It produced the accepted level-two result: seed `75 65 19`, 66 objects and 23 creatures. Total initialization RNG calls were 2,344: 2 starting-cell, 1,028 carve, 630 ordinary-door, 575 secret-door, 21 final-spin, and 88 creature-placement calls. The source loops and constants correspond to `DGNGEN.ASM:DGEN10–DGEN90`, `NEWLVL.ASM:NEWLVX/NLVL30–NLVL44`, and `RANDOM.ASM:RANDOX` in the read-only reference checkout `/Volumes/SEDONA/Projects/daggorath-reference` at commit `a94326f00ebb16a106b540c58bc2ccf5f7b66dac`. `ONCE.ASM:GAME20–GAME40` places this source-semantic initialization after PREPARE and before the map; the original 6.442-second visible interval also includes its own drawing and therefore is not an isolated cartridge initialization benchmark.

The CMOC 0.1.90 `-O2` listing in the disposable build shows why this source-equivalent work is expensive on the 6809 target. `game_random()` is a 187-byte C routine with an eight-pass outer shift, an eight-pass bit/parity loop, and a three-byte seed loop per outer pass. It repeatedly spills locals and indexes `Game.seed` at offset 2576. The source cartridge `RANDOM.ASM:RANDOX` performs the equivalent bit shifts directly with 6809 `LSLA`, `LSRB` and `ROL` instructions. The generated maze listing also uses stack-checked C calls and signed 16-bit row/column tests, then calls `cell()` repeatedly for neighbor checks. This identifies **portable C instruction overhead inside authentic maze/RNG work**, not repeated level initialization or a hidden scheduler wait. It is a later surgical 6809/6309 optimization candidate; no code was optimized here.

## B. Wizard blank to intro entry

The original accepted public video gap was 14.035 s, of which normal intro entry through PREPARE transfer was 36 ticks / 0.600 s. The follow-up used a **private** Wizard probe that stayed below the same 24,576-byte, three-program-block boundary (24,572 bytes). To make room for seven clock marks and a deferred short output row, it removed only the private no-input run's cancellation/signal polling and returned-Term message; absolute Wizard deadlines, final graphics, handoff and phase-chain operations remained. This makes the syscall brackets useful but **not a byte-identical production timing run**.

The first private row crossed the EOU minute boundary during preflight:

```text
WZ 0D53 0D53 0D53 0D53 0109 010A 010A
```

Because this project previously observed EOU Clock2 RTC-refresh discontinuities, its apparent 454-tick preflight span is **not accepted as a clean wall-time measurement**. A confirmatory restored run was delayed at the Shell prompt so the measured boundary did not cross a minute:

```text
WZ   0340 0340 0340 0340 044B 044C 044C
TM00 055E 055F 055F 055F 0562 0574 0582 0624
```

| Bracket in no-rollover private run | Clock marks | Ticks / nominal time | What it includes |
| --- | --- | ---: | --- |
| Final blank flip → final absolute wait return | `WZ0→1` | 0 / <1 tick | The deadline had already elapsed in this run; this does not erase the source's nominal three-jiffy blank. |
| `screen_handoff` | `WZ1→2` | 0 / <1 tick | Process-local graphics mapping release. |
| Helper argument preparation to `F$NMLoad` entry | `WZ2→3` | 0 / <1 tick | Local path/parameter copy. |
| Target `F$NMLoad /d1/dodintro` | `WZ3→4` | **267 / 4.450 s** | Module preflight load and validation, not separated from internal disk I/O. |
| `F$UnLoad` retained preflight reference | `WZ4→5` | 1 / 0.017 s | Source name/type lookup, reference release. |
| Final preparation | `WZ5→6` | 0 / <1 tick | Type validation and entry to deferred probe output. |
| Deferred 39-byte probe output + `F$Chain` + target loading/replacement/startup | `WZ6→TM0` | **274 / 4.567 s** | Cannot isolate internal `F$Chain` or its disk reads with this bracket. |
| Intro entry → PREPARE transfer | `TM0→TM6` | 36 / 0.600 s | Independently matched prior probe. |

This private run's final blank→PREPARE total was about 9.63 s. It does **not** directly apportion the accepted public video's 14.035 s; the difference between runs is about 4.405 s, and the minute-crossing row is not a clean substitute. The major *kind* of work is clear: preflight loading and destructive chain/target startup. Their precise baseline shares, and the split of `F$Chain` between disk I/O, Level II remapping and CMOC startup, remain unmeasured.

`apps/daggorath/src/phase-chain.c` preflights with `F$NMLoad`, releases it with `F$UnLoad`, then calls destructive `F$Chain` by pathname. At zero references the inspected upstream Level II kernel's `funload.asm:FUnloadModule/FUnloadDltModMem` removes a program module from memory (official upstream checkout `/Volumes/SEDONA/Projects/nitros9-reference`, commit `f470fa52eb172b59b22c1b722074998cb42de9b1`). This makes a second target load during `F$Chain` plausible and is consistent with the two multi-second brackets. It does **not** prove which seconds inside the latter bracket were disk reads. The current preflight/release contract exists because installed EOU can condemn the caller on failed `F$Chain`; retaining a reference across that boundary previously leaked it. Removing preflight or retaining its reference is therefore not an approved latency fix.

## C. Heartbeat pack

The opening intro makes one `F$Load` of `/d1/dhbpack` after PREPARE. It measured 163 ticks / 2.717 s in the prior run and 162 ticks / 2.700 s in a later private run. The packed file is **740 bytes in three 256-byte disk sectors** on the disposable floppy: reentrant/read-only 6809 `DHeartbeat` device driver, 701 bytes, plus `dhb` SCF device descriptor, 39 bytes. `opening_heartbeat_modules_open()` uses a single `F$Load` because the previously tested separate installed-EOU loads hit `E$RAMFull`; it checks the returned `$E1` type. The actual native `/dhb` open, rate and enable happen only after the first dungeon transfer and separately cost about 3, ≤1 and 1 tick.

The **2.7 s belongs to the aggregate packed load**. There is no per-member syscall boundary, FDC-sector timing trace, or validated internal kernel/RBF timestamp here. Therefore individual driver/descriptor times and the disk-I/O-versus-module-validation split are **not measured**. File size and three-sector layout alone do not justify attributing the delay to a motor, disk transfer or CRC computation.

The pack is not needed to construct the source map, but is needed before heartbeat open. It cannot simply be loaded in Wizard with a process-owned retained reference: `F$Chain` destroys process-local state and the prior ownership investigation found that such references can leak on destructive failure. Releasing it before chain may discard the module. A system-owned/preloaded residency design could be studied separately, subject to Level II physical memory, DAT mapping and unload semantics. Loading it just before PREPARE *inside intro* would move 2.7 s from one visible gap to the other, not remove player-visible latency.

## D. Four-row prompt rendering

A second target probe placed marks around `primary_render()` itself:

```text
TM40 09E3 09E4 0A12 0000 0000 0000 0000 0000
```

The full pass was 47 ticks / 0.783 s: `memset` of the 1,024-byte, four-row text region was **1 tick / 0.017 s**, and the nested glyph loop was **46 ticks / 0.767 s**. The separate following `screen_present` remained about 15 ticks / 0.250 s, so this is not a CoWin transfer cost. The pass is called once for beat 012, not repeatedly by an opening loop.

At the lone-dot/underline prompt state, 126 of the 128 text cells are blank. `primary_render()` nevertheless visits all 128 cells and all seven glyph rows; the `code>=31` guard does not skip blank code 0. It calls `five()` up to 896 times and performs up to 4,480 bit-extraction loop iterations after clearing the same area. The `-O2` listing shows each `five()` call uses a separate 90-byte routine with shifting and stack work; each glyph-row store also recalculates font and framebuffer offsets through CMOC-generated arithmetic. The measured 46 ticks, source loop and listing together identify redundant **blank-glyph computation** as the dominant cause. Any future fast path must preserve the exact original four-row pixels, dot and underline; none was implemented.

## Ownership and correction opportunities

| Rank | Cost/operation | Category | Feasibility under current architecture |
| ---: | --- | --- | --- |
| 1 | ~15.3 s maze generation inside semantic initialization | Source-required, serial for current map; target C overhead | Optimize `game_random`, repeated `cell()`/neighbor arithmetic or maze loops only with exact state/RNG/maze regression. Candidate for surgical 6809 assembly; optional 6309 acceleration later. Moving the authoritative `Game` across `F$Chain` is not directly possible because process data is rebuilt. |
| 2 | ~4.45 s private-run `F$NMLoad` preflight plus ~4.57 s chain/output/target entry | EOU phase mechanism; not present on cartridge | Likely repeated module acquisition after balanced preflight release. Preserve destructive failure cleanup. Any caching/preload requires new ownership proof; do not remove preflight merely for speed. Exact accepted-video partition remains unresolved. |
| 3 | ~2.7 s heartbeat pack load | EOU-only but required before heartbeat open | Could overlap a legitimate visible interval only with a safe system/module ownership design. Wizard-owned preparation is not directly inherited through chain. |
| 4 | ~0.767 s blank-glyph loop | Source-visible output required, port implementation overhead | One-call semantic-preserving 6809/C fast-path candidate; no reason to move the text state across phases. |

The proposed `Wizard → prepare resources during source welcome dwell → F$Chain → PREPARE → already-prepared map` plan is **not directly feasible with the current state ownership**: authoritative `Game` is 2,608 bytes of process data and is intentionally rebuilt by the chained intro; Wizard's owned module references cannot simply survive a destructive chain. An explicit cross-phase serialized state or system-owned cache would be a distinct architectural change and must not compromise the one-free-DAT-slot, inherited CoWin, or failure-cleanup contracts. The cleaner immediate opportunities are source-equivalent performance improvements within intro and separately measured module lifecycle design work, not shortened waits or a canned map.

**Decision:** the 16-second initialization and 0.783-second text pass have identified target-side causes. The 2.7-second heartbeat-pack call is isolated but its internal disk/module split is not. The accepted public **14.035-second Wizard-to-PREPARE interval has no same-run operation partition** despite informative private brackets. This is **M4 OPENING LATENCY ROOT CAUSE PARTIAL**; no production timing change follows from this report.
