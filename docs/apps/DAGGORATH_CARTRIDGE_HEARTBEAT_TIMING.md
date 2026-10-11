# Daggorath original-cartridge heartbeat timing reference

**Status (2026-10-11): ACCEPTED MEASUREMENT REFERENCE.** This document records
the Stage 3E.0 read-only timing study. It does not change C6 production code,
tests, or acceptance thresholds. The cartridge remains the behavioral oracle.

Evidence labels used below:

- **SOURCE VERIFIED**: established by the identified cartridge source and
  disassembly.
- **TARGET VERIFIED**: directly verified in the identified MAME target.
- **MEASURED**: derived from the retained event, audio, memory, and completed
  raster observations.
- **INFERRED**: a bounded interpretation of measured data.
- **UNRESOLVED**: not established by this study.

## Cartridge and target identity

| Item | Identity | Classification |
| --- | --- | --- |
| Cartridge ROM | 8,192 bytes; SHA-256 `35e6a77354dcf1a3048f276824b7a0f9f759115fdd40603664cebfb3a7da6571` | **TARGET VERIFIED** source-built artifact |
| Source | `daggorath-reference` commit `a94326f00ebb16a106b540c58bc2ccf5f7b66dac` | **SOURCE VERIFIED** |
| MAME | 0.289, Ample build `ample-289-u2` | **TARGET VERIFIED** |
| Machine | `coco3h`, 2 MiB, RGB output, 640x239 completed bitmap | **TARGET VERIFIED** |
| Execution | Original CoCo cartridge display path through the CoCo 3 compatibility configuration | **TARGET VERIFIED** |

The ROM is byte-verified against the identified source build. It has **not**
been independently authenticated against a retail cartridge dump and must not
be described as a verified retail image. The ROM is intentionally not retained
in this repository.

## Original heartbeat implementation

**SOURCE VERIFIED:** `COMMON.ASM:CLOCK/CLK30/CLK32` owns the heartbeat
countdown, sound edge, phase, and graphics update. `COMTXT.ASM:TXTDPB/DPB10`
deposits the heart glyph, and `PLOOK.ASM:INIVUX` enables the initial visible
heartbeat.

| Symbol | Address | Role |
| --- | ---: | --- |
| `HEARTF` | `$02AD` | Visual-heart enable |
| `HEARTC` | `$02AE` | Active countdown |
| `HEARTR` | `$02AF` | Countdown reload/rate |
| `HEARTS` | `$02B0` | Current visual phase |
| `HBEATF` | `$02B1` | Heartbeat processing gate |
| `CLOCK` | `$C27D` | Periodic clock entry |
| `CLK30` | `$C2A9` | Countdown/edge path |
| `CLK32` | `$C2D1` | Visual phase and glyph update |
| `TXTDPB` | `$CA17` | Two-page glyph deposit |

On every video jiffy, `CLK30` decrements `HEARTC`. At zero it reloads from
`HEARTR`, changes PIA PB1 bit 1, complements `HEARTS`, and reaches `CLK32`,
which deposits the selected heart glyph into both display pages. The sound and
graphics are sequential effects of the same interrupt-driven logical event;
they are not independently scheduled timers.

The seven-row glyph update writes 28 bytes: two bytes per row on each of two
pages. Page-zero heart bytes begin at `$230F/$2310`; page-one bytes begin at
`$3B0F/$3B10`; successive rows are 32 bytes apart.

## Heartbeat cadence

**MEASURED:** the MAME video period was `16.688153335 ms`, or
`59.9227476 Hz`. `HEARTR` is a count of those actual video jiffies.

| State | `HEARTR` | Measured transition interval |
| --- | ---: | ---: |
| Normal | 46 | 767.655 ms |
| Precombat | 45 | 750.967 ms |
| Accelerated after `!!!` | 41 | 684.214 ms |

**SOURCE VERIFIED:** a rate update changes `HEARTR` without resetting the
current `HEARTC` remainder.

## Audio event and measurement

The timing boundaries were:

- T0: logical event, observed when `HEARTC` reached zero;
- T1: first sound instruction, `LDB P.PIIOB,X` at `$C2B5`;
- T2: PB1 output write, `STB P.PIIOB,X` at `$C2B9`;
- T3: first accepted mixed-waveform sample after T2;
- T4: the following heartbeat edge, because PB1 holds its new state rather
  than ending with a separate heartbeat pulse instruction.

**MEASURED:** T2 occurred approximately `25.702 microseconds` after T0. MAME
captured mono signed 16-bit PCM at 48 kHz. T3 was the first post-T2 sample at
least 1,024 amplitude units from the stable pre-edge median. A sample was
accepted only when its 32-sample pre-edge window varied by no more than 512
units. This yielded 92/100 isolated waveform onsets in the idle sample and
91/100 in the dungeon-stress sample. Concurrent effects made the remaining
onsets ambiguous; no onset was fabricated.

The captured waveform represents MAME's mixed electrical output. Physical
speaker and acoustic propagation latency are **UNRESOLVED**.

## Visual event and measurement

The visual timing boundaries were:

- V0: logical heartbeat event;
- V1: first heart framebuffer write;
- V2: final heart framebuffer write;
- V3: first completed MAME raster containing the new phase;
- V4: first completed raster containing the entire correct heart.

Read-only memory taps captured V0 through V2. Same-callback MAME snapshots
captured V3/V4. Frame-notifier `screen:pixel()` was rejected because it exposed
the preceding render update on this legacy VDG path. The accepted method found
exactly two 32x7 completed-raster hashes:

- large: `e5750a0a9ca6a2786489714f683ae3269c9090f7ffad25779330d799c470c662`;
- small: `6218df7b8da8cacdafbeb27444912d5b89cc038f68e35f3c67c6d7128e98009d`.

V1 followed PB1 by approximately `0.1766 ms`; V2 completed approximately
`0.9901 ms` after T0. The first completed output raster followed PB1 by about
`16.19 ms`.

## Accepted results

### Relatively idle sample

One hundred consecutive events were analyzed.

| Sound onset to complete raster | Result |
| --- | ---: |
| Minimum | 15.786 ms |
| Median | 16.140 ms |
| P95 | 16.151 ms |
| Maximum | 16.154 ms |
| Standard deviation | 0.037 ms |

### Dungeon-rendering sample

The first 100 autoplay events covered initial dungeon activity, inventory and
torch commands, `LOOK`, movement redraws, Giant appearance and approach, and
combat preparation. Twenty-three events coincided with substantial
framebuffer activity.

| Sound onset to complete raster | Result |
| --- | ---: |
| Median | 16.141 ms |
| Maximum | 16.181 ms |

### Combat and accelerated heartbeat

The captured cartridge sequence measured approximately 18 completed frames
from `ATTACK RIGHT` to `!!!` and approximately 96 frames from `!!!` to the next
prompt. During the accelerated `HEARTR=41` interval:

| Measurement | Result |
| --- | ---: |
| Cadence median | 684.215 ms |
| Sound onset to complete raster median | 16.142 ms |
| Sound onset to complete raster maximum | 16.156 ms |

Concurrent combat audio made two of five combat waveform onsets ambiguous,
but every corresponding PB1 and visual transition remained observable.

## Jitter, drift, and raster integrity

**MEASURED:** 256 logical heartbeat events were correlated across the two
runs. Two consecutive completed snapshots were checked after every event.

| Observation | Count/result |
| --- | ---: |
| Missing visual phases | 0 |
| Duplicate visual phases | 0 |
| Obsolete visual phases | 0 |
| Extra raster transitions | 0 |
| Partial completed-heart frames | 0 |
| Accumulating visual drift | None observed |

The stable approximately one-frame offset is latency, not accumulating drift.
No completed raster exhibited the partial-heart class directly observed in C6
Stage 3C.15. This is evidence for the measured sequences; it does not prove
that every possible cartridge state is tear-free.

## Relationship to C6

The cartridge study measures completed raster output. C6 internal PutBlk
completion is not equivalent to that boundary. The retained C6 engineering
deadline remains two source ticks, approximately `33.34 ms`, until the user
explicitly approves another numerical requirement.

The cartridge reference supports a typical target near one completed frame.
It does not make obsolete phases, lost generations, permanent stalls, or
accumulating drift acceptable. See
[DAGGORATH_C6_ARCHITECTURE_DECISIONS.md](DAGGORATH_C6_ARCHITECTURE_DECISIONS.md)
for the approved stock-EOU direction and provisional timing discussion.

## Reproduction and evidence

The preserved evidence is in
[`apps/daggorath/experiments/cartridge-heartbeat-reference/`](../../apps/daggorath/experiments/cartridge-heartbeat-reference/README.md).
It includes the unmodified observer, analyzer, summary, numerical CSV ledgers,
source identity, original manifest, and representative rasters.

The original environment used `/private/tmp/c6-stage3e0`. `observer.lua`
accepts `STAGE3E_ROOT`, while `analyze.py` retains its original absolute root.
The scripts were preserved without portability edits. Full audio reanalysis
requires the excluded WAVs listed in the experiment README.

The original 549-entry manifest has one explicitly approved integrity
exception: mutable `cfg/coco3h.cfg` was rewritten after the run. The other 548
entries verify. The original manifest remains unchanged, and the current
post-run file is preserved under a distinct identity. This prevents a claim
of complete byte-for-byte environment reproduction.

## Measurement limitations

- The source-built ROM is not an independently authenticated retail dump.
- Some mixed waveform onsets are unresolved during concurrent sound effects.
- Physical acoustic latency was not measured.
- The runs cover the accepted storyboard path, not every cartridge state.
- Zero observed tearing in 256 events is not a universal tear-free proof.
- Human perceptibility requires controlled human review.

Cross-references:

- [DAGGORATH_CARTRIDGE_STORYBOARD.md](DAGGORATH_CARTRIDGE_STORYBOARD.md)
- [DAGGORATH_C6_HEARTBEAT_ARCHITECTURE.md](DAGGORATH_C6_HEARTBEAT_ARCHITECTURE.md)
- [DAGGORATH_C6_ARCHITECTURE_DECISIONS.md](DAGGORATH_C6_ARCHITECTURE_DECISIONS.md)
- [NITROS9_EOU_LEVEL2_LESSONS.md](../technical/NITROS9_EOU_LEVEL2_LESSONS.md)
