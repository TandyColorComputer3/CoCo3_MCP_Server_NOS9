# M4 opening timing attribution — partial

**Follow-up:** The operation-level figures below are supplemented by the [root-cause analysis](DAGGORATH_M4_OPENING_LATENCY_ROOT_CAUSE.md). That later private study partitions `game_init_demo()` and the four-row text pass, and brackets Wizard preflight and chain startup. The original 14.035-second public Wizard-to-PREPARE video interval still has no same-run syscall-level partition.

This report follows the [completed-frame visual audit](DAGGORATH_M4_VISUAL_AUDIT.md) and [source-built cartridge storyboard](DAGGORATH_CARTRIDGE_STORYBOARD.md). A disposable `dodintro` sampled the EOU 60 Hz `os_clock()` at 23 operation boundaries. The corrected private output was recovered in the **one additionally authorized Shell+ launch**. The probe changed no production or test source, canonical media, or save state. The private MAME session was stopped afterward.

The three output rows were:

```text
TM00 02CA 02CA 02CA 02CA 02CE 02DF 02EE 0391
TM08 0751 079D 07AC 084E 085F 0861 086C 089B
TM16 08AA 08AD 08AD 08AE 08AE 0962 0965 0000
```

The corrected private `dodintro` SHA-256 was `e47775d6dcc7d17197a15088c5500be1d4d3fb49babff4fcdb5e798e3bf17b0f`. The staged public launcher was unchanged at SHA-256 `cb950a95d2c2352fee61943600ccccfdf0e42f219c69551a606e6baecb72c7bb`; the Wizard was unchanged at `8dfe734d862f621c07b60db5badc62e3ec5282cc4fbe57ab8250957f625b3c3f`. The probe preserved the intro's three-block mapping. Zero sampled ticks means less than one tick, not zero work. The 162-tick map dwell and 180-tick review hold were measured exactly; samples were monotonic.

## Visible intervals

| Beat interval | Cartridge | EOU completed frames | Probe span | Reconciliation |
| --- | ---: | ---: | ---: | --- |
| 008 Wizard blank → 009 PREPARE | ~0.050 s | **14.035 s** | intro entry → PREPARE transfer **36 ticks / 0.600 s** | About **13.435 s** remains in the Wizard wait/handoff/preflight/chain/target startup group; individual operations are unmeasured. |
| 009 PREPARE → 010 full map | ~6.442 s | **20.543 s** | **1,214 ticks / 20.233 s** | Four operations account for the span; video/probe difference **0.310 s** comes from separate runs and visible versus call boundaries. |
| 011 fully black → 012 first dungeon/status | ~0.216 s *upper bound* | **1.201 s** | **75 ticks / 1.250 s** | Four operations account for the span; the 0.049 s cross-run/boundary difference has opposite sign. |

The source's ~0.216 s is an upper bound from last map frame to first dungeon frame, not an exact black-frame duration. The EOU excess against that bound is at least ~0.985 s. The first EOU status pixels precede active-heart startup.

## Operation ledger

“Movable” identifies a candidate for later architectural review, not approval to reorder a source beat.

| Operation | Phase | Calls | Sampled ticks / nominal time | Source-required? | EOU-only? | Movable? |
| --- | --- | ---: | ---: | --- | --- | --- |
| Final `wait_until(915)` after Wizard blank | 008→009 | 1 deadline | nominal 3 / 50 ms; **not sampled** | Yes | No | Preserve visible blank. |
| `screen_handoff` / GP unmap | 008→009 | 1 | **not sampled** | No | Yes | Must precede chain under current ownership. |
| Target `F$NMLoad` preflight | 008→009 | 1 | **not sampled** | No | Yes | Earlier preparation needs balanced reference proof. |
| Preflight `F$UnLoad` | 008→009 | 1 | **not sampled** | No | Yes | Must balance load. |
| `F$Chain`, target load/replacement/startup | 008→009 | 1 | **not sampled** | Phase order yes | Mechanism yes | Chain remains phase boundary; cost unknown. |
| Intro argument parse + signal interception, marks 0→3 | 008→009 | 1 each | 0 / <1 tick aggregate | No | Yes | Insignificant at this resolution. |
| Inherited CoWin `screen_adopt`, GP remap/heart buffer setup, 3→4 | 008→009 | 1 | **4 / 66.7 ms** | Ownership yes | Mechanism yes | Constrained by inherited path. |
| PREPARE logical render + retained copyright, 4→5 | 008→009 | 1 | **17 / 283.3 ms** | Yes | Renderer cost yes | Before transfer. |
| PREPARE `screen_present`, 5→6 | 008→009 | 1 | **15 / 250.0 ms** | Visible beat yes | CoWin transfer yes | Must finish for beat 009. |
| Heartbeat pack `F$Load /d1/dhbpack`, 6→7 | 009→010 | 1 | **163 / 2,716.7 ms** | No; native heartbeat starts after dungeon | Yes | Candidate for earlier preparation if DAT/reference rules permit. |
| `game_init_demo`, 7→8 | 009→010 | 1 | **960 / 16,000 ms** | Yes, source level-2 state | Target CPU duration yes | State precedes map; relocation needs semantic design. |
| `opening_render_map`, 8→9 | 009→010 | 1 | **76 / 1,266.7 ms** | Yes | Port render cost yes | Before transfer. |
| Map `screen_present`, 9→10 | 009→010 | 1 | **15 / 250.0 ms** | Visible beat yes | CoWin transfer yes | Must finish for beat 010. |
| Source map `dwell(162)`, 10→11 | after map | 162 one-jiffy sleeps | **162 / 2,700 ms** | Yes, two `WAITX` | No | Preserve after map. |
| Black clear + `screen_present`, 11→12 | transition | 1 | **17 / 283.3 ms** | Yes | CoWin transfer yes | Before black hold. |
| Source `dwell(2)`, 12→13 | 011→012 | 2 one-jiffy sleeps | **2 / 33.3 ms** | Yes, two `SYNC` | No | Preserve. |
| Prompt setup + dungeon/status `game_render_with_progress`, 13→14 | 011→012 | 1 | **11 / 183.3 ms** | Yes | Port render cost yes | Before transfer. |
| Four-row/lone-dot `primary_render`, 14→15 | 011→012 | 1 | **47 / 783.3 ms** | Text result yes | Port render cost yes | Optimize only after pixel equivalence proof. |
| Dungeon/status `screen_present`, 15→16 | 011→012 | 1 | **15 / 250.0 ms** | Visible beat yes | CoWin transfer yes | Must finish for beat 012. |
| `native_heartbeat_open`, 16→17 | after first dungeon transfer | 1 | **3 / 50.0 ms** | Active heart yes | Driver open yes | Outside black→first-status span. |
| Heartbeat rate update, 17→18 | after open | 1 | 0 / <1 tick | Yes | Control call yes | Before enable. |
| Heartbeat enable, 18→19 | after rate | 1 | **1 / 16.7 ms** | Yes | Control call yes | Preserve authority. |
| First heart reconciliation, 19→20 | after enable | 1 | 0 / <1 tick | Yes | Foreground presentation yes | Current generation only. |
| M4 review `dwell(180)`, 20→21 | after beat 012 | 180 one-jiffy sleeps | **180 / 3,000 ms** | M4 review hold only | Yes | Outside opening gaps. |
| Graphics close, 21→22 | cleanup | 1 | **3 / 50.0 ms** | Clean exit yes | EOU mechanism yes | After hold. |

PREPARE→map totals exactly `163 + 960 + 76 + 15 = 1,214` ticks / 20.233 s. The 16-second `game_init_demo` is the dominant measured cost: source-derived maze, OCB/CCB creation, creature placement/attachment, and DEMDAT equipment in `src/gameplay/game.c`. It is CPU work on the target, not an explicit wait. The 2.717-second pack load is EOU-only. The separate visible interval was 20.543 s; its 0.310-second difference is not assigned to a fifth operation.

Black→dungeon totals exactly `2 + 11 + 47 + 15 = 75` ticks / 1.250 s. `primary_render` in `src/gameplay/primary-text.c` scans all four 32-character rows and their bitmap rows, including blank glyphs. That is a plausible source of its 0.783-second cost, but no production correction is made here. Heartbeat open/rate/enable follows the first dungeon transfer, costing ~3, ≤1, and 1 tick respectively; it cannot cause the preceding black interval.

Intro entry→PREPARE transfer totals 36 ticks / 0.600 s. The visible Wizard-blank→PREPARE interval was 14.035 s, leaving approximately **13.435 s** across the nominal 0.050-second source wait, `screen_handoff`, `F$NMLoad` preflight, `F$UnLoad`, destructive `F$Chain`, target load/replacement and entry. A private Wizard timing variant changed `dodwiz` from three to four 8K program blocks and was discarded because it would alter the mapping being measured. The authorized rerun instrumented only `dodintro`. Calling the entire 13.435 s “`F$Chain` latency” would be unsupported.

## Wait accounting and next decision

There is **no** PREPARE→map explicit sleep, scheduler call, or credited-audio startup. `dwell(162)` begins after the map; `dwell(2)` follows the black transfer. The later 36-tick AUTTAB command dwell is not in this opening path. `dwell(180)` follows beat 012 and measured exactly 180 ticks. No stacked waits explain the long opening gaps.

The cartridge and selected EOU beat-012 frames both use the larger heart shape; EOU numerical phase was not captured on that exact frame. Opposite small/large phases do not explain the two-pixel visual residual. Heart geometry remains a separate acceptance item.

If another measurement is approved, first bracket Wizard handoff, preflight and chain without changing its three-block mapping. Then evaluate moving the measured EOU-only heartbeat-pack load into an existing visible wait if reference/DAT ownership permits. Separately investigate the 16-second source-derived initialization and 0.783-second text pass for safe performance improvements. Preserve exact gameplay state, phase order, CoWin ownership, heartbeat authority, M3 audio, and the source waits; do not shorten sleeps to chase timestamps.

**Conclusion:** PREPARE→map and black→dungeon are apportioned at operation level. Approximately **13.435 s** of the 008→009 path remains unpartitioned. This is **partial attribution**, not a basis for production timing changes.
