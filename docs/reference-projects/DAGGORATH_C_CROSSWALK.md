# Daggorath ASM → Windows C++ → Linux C++ → NitrOS-9 crosswalk

Research date: 2026-09-26. **Research only; no production code changes.**
This is a semantic navigation aid, not a claim that the complete C ports reproduce
the cartridge. Precedence for game semantics is **original 6809 source → original
cartridge runtime evidence → C-port interpretation → our inference**. Verified EOU
behavior remains primary for OS/platform integration, per the
[source-index rules](../source-index/README.md).

## 1. Identities, lineage and limits

| Reference | Pinned identity | Role |
|---|---|---|
| Original reconstruction, `/Volumes/SEDONA/Projects/daggorath-reference` | `a94326f00ebb16a106b540c58bc2ccf5f7b66dac` | MichaelSpencerJr/DungeonsOfDaggorath **contains ASM, not the Windows C conversion**. Recovered macros/source-listing limitations remain documented in [original archaeology](DAGGORATH.md). |
| Windows PC-Port 0.3.1 | `dodport_0.3.1.zip`, SHA-256 `8d1236f3b55ba106595a1c87ea7188ee60897a70a436b30cb5a31e0be3884862` | Actual archived source downloaded from the original project, inspected without running installer/binaries. Richard Hunerlach's C/C++ conversion; Dan Gendreau enhancements; Tim Lindner samples. |
| `/Volumes/SEDONA/Projects/daggorath-linux-reference` | `4ab53f4b4478df028d14a403fc03d9ff5266cde1` | gondur/dungeons-of-daggorath; single `create` commit. Repository describes Linux v0.5.1; bundled documents retain 0.2.2, 0.3 and 0.4.0 descriptions. Those are not independent releases verified by this checkout's history. |
| Current NitrOS-9 port | M4 working tree; `dodgame` 21,714 bytes / CRC AF5C54 | [M4 source/runtime evidence](../apps/DAGGORATH_GAMEPLAY_M4.md); existing uncommitted M4 work preserved. |

Primary external references: [PC-Port project/history](https://mspencer.net/daggorath/dodpcp.html),
[downloads](https://mspencer.net/daggorath/doddownload.html),
[Windows 0.3.1 archive](https://mspencer.net/daggorath/builds/dodport_0.3.1.zip),
[Linux pinned tree](https://github.com/gondur/dungeons-of-daggorath/tree/4ab53f4b4478df028d14a403fc03d9ff5266cde1).
The Windows archive is research scratch under `MCP/work/c-crosswalk/windows031/`,
not vendored source or a new external checkout. Recover it using the URL/hash above.

**Verified relationship:** Linux belongs to the Hunerlach Windows conversion
lineage; agreement between them is **one inherited interpretation**, not two
independent confirmations. Its `src/readme2.txt` describes the conversion from
6809, singleton classes and VC5 development; VC project files survive in the tree.
After CRLF normalization, Windows 0.3.1 and Linux have identical `parser.cpp`,
`rng.cpp`, `rng.h`, `dodgame.cpp`, `object.h`, `player.h`, `creature.h`, and
`parser.h`. Important function bodies also match verbatim: PGET, PDROP, PPULL,
PSTOW, PEXAM, PUSE, PINCAN, PCLIMB, DAMAGE, HUPDAT, BURNER, HSLOW; Object's
OFIND/FNDOBJ/OBJNAM/PAROBJ/OBIRTH/OCBFIL; Viewer's EXAMIN/PRTOBJ/VIEWER/STATUS;
Creature's CBIRTH/CFIND/CFIND2/CWALK/NEWLVL/CREGEN.

The later Linux snapshot changes ATTACK, CMOVE, maze options, initialization,
menus and configuration. This proves shared code and later modifications, **not**
the exact intermediate merge ancestry. Its one-commit history cannot attribute
every edit to a Windows release or Linux author. The project history identifies
Josh Albright's 0.4 Windows/Linux releases and a separate Stuart Cunningham Linux
0.3.1 branch; this pass does not assign gondur's import to an unverified exact
branch. No independent Windows 0.5 source snapshot was assumed.

`MCP/Documents/` and `DOCS_INDEX.md` are absent. Findings below are observations of
identified source and prior cartridge captures, not new manual/ABI assertions.
No new MAME run or C-port executable test was performed in this pass.

## 2. How to read the crosswalk

Path roots:

- **A** = [original ASM](/Volumes/SEDONA/Projects/daggorath-reference).
- **W** = Windows 0.3.1 archive `src/`.
- **L** = [Linux src](/Volumes/SEDONA/Projects/daggorath-linux-reference/src).
- **N** = [our src](../../apps/daggorath/src); `game.c` below means `gameplay/game.c`.

A file plus label/function is the durable locator; lines may move. W/L names in
separate columns were checked in both trees. “Same” means the named counterpart,
not identical behavior to ASM. Classifications apply to the stated edge/scope:

- **EXACT / VERY CLOSE (E):** narrowly identified data/algorithm; not whole-game equivalence.
- **SEMANTICALLY EQUIVALENT (S):** preserves the local operation with changed representation.
- **HOST-PLATFORM ADAPTATION (H):** OS, storage, input, graphics or scheduling replacement.
- **DELIBERATE ENHANCEMENT (D):** explicit optional new behavior.
- **BUG FIX / BEHAVIOR CHANGE (B):** a documented or demonstrated change between versions.
- **UNCERTAIN (U):** plausible correspondence without sufficient equivalence evidence.
- **CONFLICTS WITH ORIGINAL (C):** a concrete source-level disagreement, detailed below.

### Initialization, world and representation

| A: file : label/data | W: function/data | L: function/data | N: equivalent/status | Classification and scope |
|---|---|---|---|---|
| `ONCE.ASM: GAME, COMINI, GAME10..30`; `COMDAT.ASM: GAMDAT/DEMDAT` | `dodGame::COMINI/Restart`, `Player::setInitialObjects`, `Object::CreateAll` | Same, plus `IsDemo`, `RandomMaze` | `game_init`, importer `omx/cmt/cdb/odb` | S/H: initialization split across singleton objects; D: alternate starting inventory/random maze; C: level allocation below. N implements bounded initial state, not autoplay. |
| `DGNGEN.ASM: DGNGEN`, `LEVTAB`, `MAZLND` in `CD.ASM` | `Dungeon::DGNGEN`, `MAZLND[1024]`, `LEVTAB` | Same plus `SetLEVTABOrig/RandomMap`, vertical-feature overrides | `maze`, `Game.maze[1024]` | E: 32×32 byte-cell encoding. S: generation translation; D: random-map branch. N original level-zero seeds only. |
| `COMCRE.ASM: VFIND/VSUB00/VFTTAB`; `CRETUR.ASM: STEP/STEPOK/STPTAB` | `Dungeon::VFIND/VFINDsub/STEPOK`, `RowCol`, `STPTAB` | Same; random vertical table | `cell`, `dr/dc`, imported `vft`, `game_command` | S: row/column and packed wall directions; H: host indexes replace RAM pointers; D: random features. |
| `CD.ASM: PROW/PCOL/PDIR/LEVEL` | `Player::PROW/PCOL/PDIR`, `dodGame::LEVEL` | Same | `Game.row/col/dir`; level implicitly zero | E local bytes; U beyond current level-zero N scope. |
| `CD.ASM: OCB`, `OBIRTH.ASM: OBIRTX`, `DTABAS.ASM: ODBTAB/XXXTAB/GENXXX` | `dod.h: OCB/ODB/XDB`; `Object::OBIRTH/OCBFIL` | Same, optional shield/scroll table edits | packed `objects[72][14]`, `birth/fill` | S/H: C uses expanded classes; N retains original byte layout/address tokens. D: table overrides. |
| `CD.ASM: P.OCPTR/BAGPTR/PLHAND/PRHAND/PTORCH` | `OCB.P_OCPTR`, `Player::BAGPTR/PLHAND/PRHAND/PTORCH` (`int`, -1) | Same | `Word bag/hand/rightHand/torch`, `getword/putword/ocb`, zero sentinel | S only through explicit address↔index mapping; host layout is not ABI-compatible. |
| `COMCRE.ASM: FNDOBJ/OFIND/OFINDP/OFINDF` | `Object::FNDOBJ/OFIND` | Same | `on_floor` + ascending OCB scans | E/S: level/location/owner filter, allocation order. **No floor linked list** in any of these implementations. |
| `NEWLVL.ASM: NLVL40..44`, `CRETUR.ASM: CMOV10` | `Creature::NEWLVL/CMOVE`, `CCB.P_CCOBJ` | Same, optional ignored objects | initialization in `game_init`; no active creature pickup | S for owner/list association; D for ignored-object option; dynamic part unported. |

### Commands, names and inventory

| A: file : label/data | W | L | N | Classification and scope |
|---|---|---|---|---|
| `HUMAN.ASM: HUMAN`; `PARSER.ASM: PARSER/PARSE0/GETTOK`; `TOKEN.ASM` | `Player::PLAYER/HUMAN`, `Parser::PARSER/GETTOK`, packed CMDTAB | Same parser; host menu front end | `token/classify/game_command`, imported full vocabulary; bounded handlers | S: original token/prefix concepts retained. H: input transport/editor. D: pre-parser commands; N explicit EXIT extension. |
| `EXPAND.ASM: EXPAND/GETFIV`; `PARSER.ASM: PAROBJ/PARHND`; `DTABAS.ASM: ADJTAB/GENTAB` | `Parser::EXPAND/GETFIV/PARHND`, `Object::PAROBJ` | Same | import-time decoding; `classify/bag_command` | E/S: packed names, class versus adjective/type, unique prefixes. N is not a full runtime compressed-parser port. |
| `COMTXT.ASM: OBJNAM`; `STATUS.ASM: STATUS` | `Object::OBJNAM`, `Viewer::STATUS` | Same | `object_name/game_render_status` | S for name choice/hand identity; H for host TXB/GL glyphs. N exact tested status pixels. |
| `PGET.ASM: PPULL` | `Player::PPULL` | Same | `bag_command` PULL | S: first bag match, splice link, empty hand, clear active torch if pulled. N cartridge-matched M3/M4. |
| `PGET.ASM: PSTOW/PSTOW0` | `Player::PSTOW` | Same | `bag_command` STOW | S: prepend held object to bag, clear hand, preserve source owner/link semantics. |
| `PGET.ASM: PGET` | `Player::PGET` | Same | `bag_command` GET, `on_floor` | S: empty selected hand, original noun match, first floor match, owner 0→1, weight/HUPDAT. No convenience auto-stow. |
| `PGET.ASM: PDROP/WUPDAT/COMUPD` | `Player::PDROP` | Same | `bag_command` DROP + `health`, `game_message` | S within valid weights: clear hand, owner zero, set location/level, retain link, subtract class weight, status/redraw. The C word weight field preserves final wrap; host OCB indexes still require translation. |
| `PEXAM.ASM: PEXAM/EXAMIN/PRTOBJ/PCRLF` | `Player::PEXAM`, `Viewer::EXAMIN/PRTOBJ/PCRLF` | Same bodies | **Unported**; `object_name`, OCB/CCB state and font reusable | S for list traversal/layout intent; H for host text areas/highlight. Best M5 readability reference. |
| `PLOOK.ASM: PLOOK`, `PUPDAT.ASM: PUPDAT/PUPSUB` | `Player::PLOOK`, `Viewer::PUPDAT/PUPSUB` | Same | render/dirty loop; no EXAMINE display mode yet | H: dispatch/frame scheduling; future LOOK must leave EXAMINE using original behavior. |
| `PUSE.ASM: PUSE/USETAB` | `Player::PUSE` | Same | `game_command` torch subset only | S dispatch intent; other objects not implemented. Host sample waits are H, not timing evidence. |
| `PUSE.ASM: PUSE12`; `COMPLR.ASM: BURNER` | `Player::PUSE/BURNER`, `OCB.P_OCXX0..2` | Same | torch USE/stow and `game_tick` | S for active torch relationship; **B** five-second C fuel units versus original minute task, with cheat branch D. |
| `PUSE.ASM: UFL100/200/300/900` | `Player::PUSE` flask branches | Same | Unported | S Thews/Hale transition intent; C Abye arithmetic; H sample waits. |
| `PUSE.ASM: USC100/200`; `MAPPER.ASM: MAPPER` | `Player::PUSE`, `Viewer::MAPPER`, `showSeerMap` | Same plus scroll option | Unported | S map selection intent; H GL map/input mode; D optional extra/easier Vision scroll. |
| `PINCAN.ASM: PINCAN/PINC10/WINNER`; `PREVEA.ASM: PREVEA` | `Player::PINCAN/PREVEA` | Same PINCAN; changed PREVEA option | Unported | S ring/reveal decision aid; D cheats/scroll rule; H victory playback. |
| `PATTK.ASM: PATTK`; `DTABAS.ASM` ring/sword offense | `Player::PATTK`, `Object::ODBTAB/XXXTAB` | Same path plus instant regeneration | Object data imported; attacks unported | U full equivalence: charges, hit/damage, sound, loot and wizard branches inseparable; D infinite rings. |
| `CRETUR.ASM: SHIELD`; `DTABAS.ASM: XXXTAB` | `Creature::CMOVE` held-shield selection | Same plus `game.ShieldFix` table override | Shield state/name/floor rendering only | S selection intent; D/C optional swapped defenses intentionally depart from original. |

### Creatures, combat, life cycle and rendering

| A: file : label/data | W | L | N | Classification and scope |
|---|---|---|---|---|
| `CD.ASM: CCB/CCBLND`, `COMCRE.ASM: CBIRTH/CFIND/FNDCEL`, `COMDAT.ASM: CMTTAB` | `CCB`, `Creature::CBIRTH/CFIND/CFIND2`, `CDBTAB/CMXLND` | Same plus scroll population option | `creatures[32][17]`, `game_init/creature` | S: indexed host records vs packed original; initialization only in N. D population override. |
| `CRETUR.ASM: CMOVE/CMOV10..99/MOVTAB/CWALK` | `Creature::CMOVE/CWALK`, `Dungeon::STEPOK` | Same broad flow; `CreaturesIgnoreObjects` | **Unported movement/AI** | S decision structure: pickup, same-cell attack, axis sight, randomized relative walk. H deadlines/sample blocking; D option. Not certified tick/RNG-call equivalence. |
| `COMCRE.ASM: CREGEN`; `NEWLVL.ASM: NEWLVX` | `Creature::CREGEN/NEWLVL` | Same plus configuration affecting inputs | Only initial population/distribution | U dynamic equivalence; D regeneration scaling/instant regeneration; H scheduling. |
| `PATTK.ASM: ATTACK` | `Player::ATTACK` | `Player::ATTACK` corrected adjustment | Unported | **C in W; B in L**. Linux closer for ordinary range, still requires unsigned/wrap audit. |
| `PATTK.ASM: DAMAGE/SCAL16` | `Player::DAMAGE` | Same | Unported | U arithmetic equivalence: host intermediates differ. Player and creature share calculation; do not port as ordinary 16-bit C multiplication. |
| `CRETUR.ASM: CMOV20..30`, `PATTK.ASM` kill/loot branches | `Creature::CMOVE`, `Player::PATTK` | Same plus invulnerability/instant regen | Unported | S broad causal order; U complete effect/overflow/scheduler behavior. Death of a creature exposes its held objects; not a separate floor-inventory store. |
| `HUPDAT.ASM: HUPDAX/HUPD40/DEATH`; `COMPLR.ASM: HSLOW` | `Player::HUPDAT/HSLOW` | Same | `health/game_tick`; faint/dead flags, no full death animation | E quotient-plus-one insight; C recovery threshold; H 750-ms fades and C scheduler. |
| `COMMON.ASM: CLOCK/HEARTC/HEARTR/HEARTS` | `Scheduler::CLOCK`, `Player` heartbeat fields | Same conceptual PCM path | `audio/native_heartbeat.*`, native driver, phase query, `health` | H: C WAV/17-ms polling is not waveform preservation. N uses separately verified native VIRQ heartbeat, not host audio semantics. |
| `PCLIMB.ASM: PCLIMB`; `COMCRE.ASM: VFIND`; `NEWLVL.ASM` | `Player::PCLIMB`, `Creature::NEWLVL` | Same | Unported | S normal ladders/downward transitions; C post-wizard upward-hole exception; H preparation pause. |
| `PATTK.ASM: ENDGAM`, `PINCAN.ASM: WINNER` | `Player::PATTK/PINCAN`, `Viewer::ShowFade`, `dodGame::hasWon` | Same with surrounding options | Unported; standalone Wizard assets are not endgame | U until ring/object/level side effects traced together; H fade/restart control. |
| `RANDOM.ASM: RANDOX`, `CD.ASM: SEED` | `RNG::RANDOM/lsl/lsr/rol`, three-byte SEED | Identical; additional host random-maze seed generation | `game_random`, `Game.seed[3]` | E local 24-bit feedback algorithm. D initial seed changes; call order remains part of semantics. |
| `VIEWER.ASM: VIEWER/VIEW52/SETFAX/SETSCL`; `VOBJ.ASM: FWDOBJ` | `Viewer::VIEWER/SETFAD/SETSCL/DRAWIT`, FWDOBJ | Same control flow | `game_render/draw`, imported vectors | S visibility/order; C/H rasterization/scaling. N uses original logical framebuffer and tested two-pass object drawing. |
| `VIEWER.ASM` creature pass; `VCTLST.ASM: FWDCRE`; creature vector data | `Viewer::CMRDRW/PDRAW`, FWDCRE arrays | Same | No active creature rendering | S lookup/orientation cue; U raster equality. Never infer AI/render completion from initialized CCBs. |
| `VECTOR.ASM` vector plotting/scaling; `SWCHAR.ASM` font | `Viewer::drawVectorList/drawVector/plotPoint/drawCharacter`, `Coordinate` | Same base plus menus | `original/logical.c`, imports, `presentation.c` | H/C floating GL vs source raster; N separates 256×192 logic from 512×192 at (64,4). |
| `ONCE.ASM` Wizard; `MISC.ASM: WIZIX/WIZOX` | `Viewer::ShowFade/draw_fade`, scheduler fade loops | Same family | `playback.c`, `original/logical.c`, standalone `main.c` | H timing replacement; N cartridge-measured schedule, not host pauses. |
| `PZTAPE.ASM`, `COMMON.ASM: ZFLAG` | `Player::PZLOAD/PZSAVE`, `Scheduler::SAVE/LOAD` | Same family, options/menu paths | No game save/load feature; MAME states are separate | H file persistence replacing cassette; no layout interchange claim. |

## 3. Demonstrated divergences and hazards

These are source findings, not newly reproduced runtime failures. Do not silently
“fix” the original to match a PC port, or the original-derived NitrOS-9 code to
match a PC port's bug fix.

1. **Attack adjustment — C/B.** W `player.cpp:ATTACK` computes `pidx=T0-3` but
   multiplies **T0** by 10/25; L multiplies **pidx**. A `PATTK.ASM:ATTK20..30`
   subtracts 3 before the signed reward/penalty multiplication. At T0=3, original
   and L adjustment are zero, W adjustment is +30. The project's May 2006 history
   reports this conversion bug; the inspected diff confirms it. L fixes this
   specific error, not all host-width issues.
2. **Initial object level cycling — C.** A `ONCE.ASM:CINI44` increments B, compares
   with 5, and continues while `BLE`; W/L `Object::CreateAll` reset on `b>4`.
   Example allocation starting at level 4 with count 3 visits 4,5,4 in ASM, but
   4,4,4 in C. N follows the source `>5` boundary. Treat possible objects assigned
   to level 5 as original state, not an invitation to normalize it away.
3. **Faint recovery — C.** A `HUPDAT.ASM:HUPD40` uses `CMPA #4; BLE HUPD90`:
   recovery starts above 4. Both C versions recover at `HEARTR>=4` with sign-bit
   guard. Their quotient-minus-18 formula correctly reflects original
   increment-before-borrow division, but this does not validate the whole routine.
4. **Poison arithmetic — C.** A `PUSE.ASM:UFL300` uses `SCAL16` with coefficient
   102, i.e. floor(power×102/128), then 16-bit accumulation. W/L use
   `(short)((double)PPOW*0.8)`. For power 160 this produces 128 rather than 127.
   Truncation, signed conversion and wrap at high power require explicit tests.
5. **Fuel representation/cadence — B.** A `COMPLR.ASM:BURNER` decrements a byte
   once per minute. C widens fuel, uses five-second units and ceiling conversion
   to minutes for lighting; its comment explicitly seeks to reduce level-change
   timer loss. An “infinite torch” option bypasses later reductions, not a source
   rule. N keeps original minute semantics; do not import the C numeric fuel field.
6. **Upward holes — C despite documentation.** W/L `PCLIMB` permit HOLE_UP when
   `FRZFLG` is set. A `PCLIMB:PCLI10` permits only VF.LUP; `COMCRE:VFIND` has no
   freeze override. The retained README says earlier unrestricted hole climbing
   was fixed and describes this remaining post-wizard exception. It is not
   supported by the inspected normative assembly path; no new cartridge test of
   that endgame condition was performed here.
7. **Explicit options — D.** L `Object::Reset` swaps leather/bronze magical versus
   physical defense under `ShieldFix`; changes OMXTAB for VisionScroll;
   `Creature::Reset` changes corresponding population; `PREVEA` lowers that
   scroll's reveal requirement. `Dungeon` randomizes seeds and relocates starting
   cells/vertical features; `CMOVE` may skip picking up objects in the player's
   cell; `PATTK` may regenerate a creature immediately. These are visible branches,
   not hypothetical differences. Do not assume their defaults from stale readmes.
8. **Creature tempo — H/D.** C CDB movement/attack intervals are host milliseconds;
   `UpdateCreSpeed` scales them using float. Original CDB stores byte delays,
   and `CRETUR:CMOV98` schedules Q.TEN. Sample waits and configurable regeneration
   change effective cadence even with apparently similar constants.
9. **Demo/turn timing — documented B/U.** `src/readme2.txt` describes a 0.2.1
   turning-animation fix; `readme.txt` admits demo timing is imperfect. No earlier
   executable was run, so those historical reports are not new measured findings.
   Movement/turn pauses and fade loops are host-time implementations.

### Width/layout checklist for any adaptation

| Hazard | Evidence and required discipline |
|---|---|
| Byte vs word vs host int | `dod.h` defines unsigned char/short but uses host `int` for links, clocks and arithmetic. CMOC's target arithmetic must be checked independently; use explicit byte/word truncation and wider unsigned intermediates where source operations require them. |
| Borrow vs signed comparison | ATTACK uses a 16-bit subtract and carry test in ASM; C uses signed `int Dval` and `<0`. `(DP-DD)*4` does not reproduce modulo-65536 subtraction/shift for every input. Exhaustive/boundary tests are needed before combat porting. |
| Intermediate truncation | SCAL16 returns a word after each stage. C DAMAGE multiplies promoted ints, shifts, then casts later. High-value inputs can differ through missing intermediate wrap; copying it to a 16-bit-int compiler can also overflow much earlier. |
| Signed right shift | ASM ASR/rotate sequences specify sign/carry. C negative division truncation and implementation-dependent negative shifts cannot stand in for them. N `scale` explicitly handles negative rounding; PC `ScaleXf` divides by 127. |
| Packed state | Original OCB=14 and CCB=17 bytes (`CD.ASM`). Host classes expand timing and special fields and introduce padding. Never serialize host struct memory as cartridge state. N stores explicit packed arrays. |
| Links/sentinels | ASM links are 16-bit addresses, zero empty. C links are indexes, -1 empty, index0 valid. Creature lists are per-CCB **object** chains, not an invented creature linked list. Scheduler's TCB queues are a different structure. |
| Ownership | Owner0=floor, byte1=player, byteFF=creature in inspected paths. C decrement from zero relies on byte wrap. Do not turn FF into a signed ownership predicate without tracing the original test. |
| RNG | Three seed bytes, feedback E1, eight bit rounds. L/W implementation preserves carry explicitly; ordinary `rand()` is not a replacement. RandomMaze uses `srand(GetTickCount())`; under LINUX that macro is `time(NULL)`, a seed-resolution adaptation, not the core generator. |
| Array bounds | Byte row/col stepping wraps before source validation; int indexes must not index before checking sentinels. OCBPTR means address limit in ASM and count in C; N original-token conversion assumes a validated token. |
| Shared scratch/state | Globals OFINDP/OFINDF, parser scratch and RNG are mutable singleton state. A future cooperating process cannot safely share these operations without an ownership protocol. C classes are not proof of reentrancy. |
| Time | Host milliseconds, `next_time=curTime+delay`, sample-completion loops and 17-ms tick approximation must not replace source event/countdown semantics. N uses verified OS timing and bounded resident heartbeat lifecycle. |

The C source is valuable precisely because it exposes intent; it does not make
host-width arithmetic authoritative. No speculative production corrections were made.

## 4. Rendering, audio and input boundaries

### Rendering is not a pixel oracle

W/L `Viewer::VIEWER` retains useful ordering: wall features, creature, side peeks,
vertical features, floor objects with two DRAWIT passes, then forward-cell advance
until obstruction/range limit. That is a useful explanation of A `VIEWER.ASM`;
N's M4 original RAM/frame captures remain stronger evidence of actual pixels.

W/L `Coordinate` assumes a 4:3 host display, rounds selected width to multiples of
256, derives height and applies offsets. `Viewer::drawVectorList` uses floating
`Scalef` even in normal mode (cast endpoints back to integers); `Scalef` is generated
with repeated ×0.633, while a separate original-looking byte `Scale` table remains.
`ScaleXf/ScaleYf` divide by 127. `drawVector` is floating DDA; VECTOR mode turns
fade into brightness, HIRES increases sample density, normal mode plots expanded
host quads. Thus “NORMAL” is not evidence of original vector rounding, endpoint
coverage or framebuffer fade contents. Font drawing uses GL quads via
`drawCharacter/drawVectorListAQ`, not the original packed screen writes.

The status area's hand names and heart placement survive conceptually, but
`Scheduler::CLOCK` writes `< >`/`{ }` character pairs into `statArea` and host glyph
rendering draws them. Text buffers (`TXB`, `examArea`, `statArea`, `primaryArea`)
replace original screen descriptors. Creature/object vector lookup tables are
excellent label maps, not a substitute for original data or orientation tests.
`ShowFade`, `draw_fade`, `death_fade` and host pauses replace original animation
scheduling. N retains the logical 256×192 framebuffer and original fade frames,
with **512×192 at (64,4)** in the owned 640×200 screen. See the measured
[Wizard timing/presentation](../apps/DAGGORATH_WIZARD_M1.md) and
[M4 comparisons](../apps/DAGGORATH_GAMEPLAY_M4.md). Do not import GL aspect or
floating scaling into the NitrOS-9 presentation layer.

### Sound crosswalk

A [SOUNDS.ASM](/Volumes/SEDONA/Projects/daggorath-reference/SOUNDS.ASM) dispatches
semantic IDs through SNDTAB/ISOUND and procedural routines. W/L replace these
with preloaded WAVs in `Creature::LoadSounds`, `Object::LoadSounds`,
`Player::LoadSounds`, `Scheduler::LoadSounds` and `Viewer` fade code. The two C
versions are the same sample lineage, not independent recordings of cartridge
semantics. Source-event details remain in [audio research](../apps/DAGGORATH_AUDIO_RESEARCH.md).

| Original routine/event family | W/L replacement/caller | N status |
|---|---|---|
| SQUEAK, RATTLE, GROWL, BEOOP, KLANK, GRAWL, PSSST, KKLANK, PSSHT, SNARL, BDLBDL | `Creature::creSound[0..11]`, `LoadSounds`, CMOVE/CWALK | Semantic SQUEAK available in SSC service; no creature AI connected. Other creature effects unported. |
| GLUGLG, PHASER, WHOOP, CLANG, WHOOSH, CHUCK | `Object::objSound[0..5]`, PUSE/PATTK/PINCAN | Semantic PHASER/WHOOP implemented as enhanced SSC recipes; other object effects deferred. |
| KLINK, CLANK, THUD, BANG | player/creature WAV handles used in combat/movement | Gameplay sound integration not implied by existing three-effect service. |
| KABOOM/BUZZ, wizard transitions | `Viewer::ShowFade`/fade sample playback | Visual Wizard remains separate; no production Wizard buzz. |
| `COMMON.ASM:CLOCK` heartbeat output | `Scheduler::CLOCK`, two heart samples | Verified native 1-bit/VIRQ backend and phase-query integration, not sample playback. |

Classification: **H** for sample synthesis; **D** for stereo positioning and
replacement sample choices; **U** for equivalence of waveforms/durations. W/L often
busy-poll `Mix_Playing`, servicing CLOCK in effect loops; CLOCK itself contains
`while (Mix_Playing(hrtChannel)==1);`. Host offloaded playback therefore does **not**
mean game logic always continues asynchronously. C sound waits can delay creatures,
movement and animation. The retained project page describes emulator-derived
clips; no waveform measurements were performed here. Use source event/blocking
intent and our measured timing, not WAV duration, to design later N audio semantics.
The C host loop is unsuitable as a NitrOS-9 scheduler pattern.

### Parser and keyboard

The core is not a wholesale simplified replacement: W/L `Parser::PARSER`,
`GETTOK`, `EXPAND`, `GETFIV`, `PARHND`, `Object::PAROBJ` preserve packed vocabulary,
matching counts and many original labels/control paths. These files/functions are
identical across the compared C snapshots. That is high-value readability evidence,
not proof of every input edge case. The original remains the arbiter for ambiguous
prefixes and full-word matches.

Host differences are explicit:

- `Player::PLAYER/HUMAN` translates ASCII to internal five-bit letters and handles
  line editing; SDL events replace A keyboard IRQ/buffer scanning.
- `enhanced.cpp:PreTranslateCommand` adds SETOPT/SETCHEAT abbreviations and RESTART;
  retained readme documents RESTART as unabbreviated. Cheats include torch/ring,
  regeneration scaling, reveal, starting items and invulnerability.
- `OS_Link::handle_key_down` supplies QWERTY/Dvorak mapping and host Escape behavior.
  W Escape exits; L calls `main_menu`. L map-mode keys return to 3D and enqueue a
  space, explicitly commented as a hack. Arrow-key menu navigation is not evidence
  of original in-game movement shortcuts.
- `PZLOAD/PZSAVE` use host filenames/defaults and Scheduler host persistence rather
  than original cassette blocks. The menu/configuration system and options file
  are outside the cartridge command contract.

N's `gameplay/input.c` and main loop are deliberate EOU input/lifecycle adaptations;
`game_command` implements only the bounded milestone handlers, with original
vocabulary/abbreviation tests and explicit EXIT. Neither C host editor nor its
menus should silently become a NitrOS-9 game feature.

## 5. Upcoming-port value and M5 decision

| Unported subsystem | C-lineage value | What it helps with / gate before implementation |
|---|---|---|
| EXAMINE/LOOK | **HIGH-VALUE READABILITY REFERENCE** | Identical PEXAM/EXAMIN bodies expose floor scan, bag order, paired columns, active torch highlight and creature marker. Rebuild original text layout, not host TXB/GL widgets. |
| Creature state/lookup | **HIGH-VALUE READABILITY REFERENCE** | CFIND/CBIRTH/CCB fields and held-object chains clarify the packed blocks. Preserve original record widths and allocation/reuse order. |
| Creature movement/AI | **USEFUL WITH CAUTION** | CMOVE/CWALK make sight-line versus random-walk branches readable. Strip optional behavior; verify source RNG calls, delays, collision rules and sound blocking before turning tasks on. |
| Combat/encounters | **USEFUL WITH CAUTION** | PATTK/CMOVE explain common attack blocks, shield choice, loot release and wizard branching. W ATTACK is wrong; L correction is only one part of arithmetic validation. |
| Damage/recovery/faint | **USEFUL WITH CAUTION** | DAMAGE/HUPDAT/HSLOW reveal formula intent. Wrap, SCAL16 stages, threshold disagreement and fade/scheduler coupling require source-derived tests. |
| Death | **USEFUL WITH CAUTION** | HUPDAT→ShowFade makes orchestration readable. Actual death state, keyboard flushing, restart and original fade timing must be traced separately. |
| Level transitions | **USEFUL WITH CAUTION** | PCLIMB→NEWLVL→DGNGEN clarifies dependencies. Object persistence, creature redistribution, torch timing, upward-hole divergence and alternate maps prevent copying as one trusted function. |
| Endgame/victory | **USEFUL WITH CAUTION** | PATTK/PINCAN expose two wizard branches and ring conditions. Requires combat, level rebuild, inventory stripping/freeze and victory lifecycle; far larger than M5. |
| Host scheduler/render/audio/save system | **LOW VALUE / HEAVILY HOST-SPECIFIC** | Useful dependency names; unsuitable OS-9 timing, memory, pixel or persistence implementations. |

**EXAMINE remains the recommended smallest coherent M5**, subject to the following
explicit dependency boundary. It is not an AI milestone and not a modern inventory
window. The original graph is:

```text
HUMAN command dispatch → PEXAM → DSPMOD=EXAMIN → PUPDAT
  → EXAMIO / alternate-screen clear / TXTEXA descriptor
  → packed “IN THIS ROOM” header
  → CFIND (active creature at player location) → optional !CREATURE! marker
  → OFIND/FNDOBJ → PRTOBJ → OBJNAM → OUTSTR
  → separator / BACKPACK header
  → BAGPTR / P.OCPTR traversal → PTORCH inverse highlight → PRTOBJ
  → restore text routing
PLOOK → VIEWER display mode → PUPDAT
```

A `PEXAM.ASM` does not execute CMOVE, ATTACK, DAMAGE, sound, RNG, level transitions
or disk access. It **does** call CFIND; implement the original active-record lookup
against existing initialized CCBs, without enabling movement/AI. N's current helper
was written for initial population and should not be assumed to satisfy future
dead-slot reuse semantics. Reuse M4 floor filtering and M3 object naming without
creating a second inventory model. Preserve exact two-column/tab/CRLF behavior,
32-character separator and inverted active-torch name; test overflowing lists and
restoration of normal input/status regions. Existing N simple `text()` is not a
complete original TXTEXA/OUTCHR renderer: that is the principal new work.

A bounded acceptance gate should compare cartridge room/bag frames and state for
empty floor, one/multiple floor objects, linked bag order, active torch highlight,
and a read-only occupied room; LOOK must return to unchanged dungeon state.
Exercise GET/DROP/PULL/STOW while respecting original display-mode persistence,
heartbeat activity and resident/no-FDC constraints. No M5 code or tests were added.
Combat or general USE would require significantly more unported causal paths;
no clearly smaller, equally coherent alternative emerged.

## 6. Memory and future CPU/platform boundaries

The [measured M4 budget](../apps/DAGGORATH_GAMEPLAY_M4.md#build-identity-and-memory)
is the planning baseline, not the host classes' memory footprint:

| Item | M4 |
|---|---:|
| dodgame module | 21,714 bytes (+1,034 from M3) |
| Code / read-only data | 18,252 / 3,411 |
| Initialized writable data / BSS | 6 / 9,022 |
| Data request / extra reserved stack | 10,558 / 1,536 |
| Game / packed OCBs within it | 2,606 / 1,008 |
| Logical frame / input underlay | 6,144 / 224 |
| Separately mapped GP payload / system screen | 12,288 / 16,000 |

M4 accounts for three code, two data and two GP 8K slots. The possible remaining
slot is not a measured free-heap promise. Host C singleton classes (`dod.cpp`)
expand fields, keep graphics/sample assets resident and share public mutable state;
class separation alone neither saves address space nor provides reentrancy.

Useful *prospective* boundaries, inferred from the crosswalk:

- **Data modules:** immutable future creature/vector tables are natural candidates
  if linked-size growth warrants them. Mapping still consumes logical space; do not
  count physical RAM as free process addressing.
- **Reentrant code modules:** parser, pure math or name decoding could be reusable
  if state is explicit. C globals/carry/parser scratch must not leak into shared
  writable code. Current M4 growth alone does not justify a module split.
- **Cooperating processes:** existing audio service is an appropriate independent
  hardware owner. Future inventory/map windows could request snapshots from the
  authoritative game process; never give each process its own mutable game copy.
- **Overlays:** rare death/endgame or initialization code may eventually offer better
  savings than tiny EXAMINE. Loading from disk during supported heartbeat residency
  conflicts with the established no-FDC gate; this is a design constraint, not an
  approved overlay plan.
- **MMU-backed storage:** large immutable assets/caches may merit investigation after
  measurements. Do not replace the verified graphics mapping with a custom pager
  or infer an OS-9 ABI from desktop pointers.

Consult [memory/MMU](../source-index/MEMORY_MMU.md),
[modules](../source-index/MODULES.md), [C development](../source-index/C_DEVELOPMENT.md)
and [processes](../source-index/PROCESSES_SIGNALS.md) before designing these.
None was implemented or required by this research.

**6809/6309 architectural intent:** keep game semantics and ordinary platform code
CPU-neutral. Isolate future 6309 optimizations behind replaceable interfaces with
functionally equivalent 6809 implementations where practical. Validate both against
the same source-state/frame/timing contracts. Our currently verified HD6309 build
is not evidence that a complete 6809 build has already been tested. No dual-build
work was performed.

## 7. Licensing and provenance

This is a record of the supplied notices, not a legal clearance determination.
A includes `grant_of_license.png`, a grant naming Michael J. Spencer Jr.; see
[the prior provenance assessment](DAGGORATH.md). Both C distributions retain
Douglas J. Morgan/DynaMicro copyright notices and credit Hunerlach, Gendreau and
Lindner. L `license/license.txt` reproduces Morgan's broad reproduction/development
permission conditioned on efforts to preserve the game in its original form.
It explicitly says the included LGPL applies to SDL/SDL_Mixer, **not the PC-Port
project**. W's `src/readme2.txt` calls the project open source and requests
commented contributions to the maintainer; that phrase is not a standard permissive
license covering every later contribution. W archive `sdl/LICENSE.txt` is the library LGPL, while `readme.txt` and
source headers retain the game credits; these notices should
be retained with any prospective reuse rather than inferred from GitHub metadata.

Consequences for this project:

- Do not label the conversion MIT/GPL/LGPL merely because SDL is LGPL.
- Preserve original notices and identify exact original labels, C version/file,
  author credits and modifications if adapting code; provenance is required by our
  project rules even where the grant does not spell out a modern attribution clause.
- A game-level grant is not sufficient evidence of separately documented terms for
  all contributed C enhancements, WAV assets or later artwork. Clarify that scope
  before distributing directly copied conversion code/assets, especially enhanced
  versions. Reference-guided independent ASM-derived implementation is the prudent
  current path; this pass copied no code into production.
- No prohibition or blanket permission for all derivative redistribution is inferred
  beyond the actual notices. The exact preservation condition matters for enhanced
  gameplay options; obtain appropriate clarification for a distribution decision.

## 8. Reproduction and validation

Read-only local commands/method:

```sh
git -C /Volumes/SEDONA/Projects/daggorath-reference rev-parse HEAD
git -C /Volumes/SEDONA/Projects/daggorath-reference status --porcelain
git -C /Volumes/SEDONA/Projects/daggorath-linux-reference rev-parse HEAD
git -C /Volumes/SEDONA/Projects/daggorath-linux-reference log --all --oneline
git -C /Volumes/SEDONA/Projects/daggorath-linux-reference status --porcelain
curl -fL --max-time 45 https://mspencer.net/daggorath/builds/dodport_0.3.1.zip \
  -o MCP/work/c-crosswalk/dodport_0.3.1.zip
```

Python `zipfile` read only `.cpp/.h/.txt` members into ignored scratch; no archive
executable was run. `hashlib.sha256` pinned the archive. `Path.read_text` normalized
CRLF, `difflib.unified_diff` compared matching files, and brace-delimited function
comparisons checked the listed identical methods. Targeted `rg`, `sed`, and direct
ASM reads traced their callers and data. These textual comparisons are not binary
or exhaustive semantic equivalence proofs. Scratch paths are optional and recoverable;
no Windows source tree is part of the Git deliverable.

Validation is limited to this documentation task: before/after hashes cover
`apps/daggorath`, MCP production source/bridge and canonical media; reference
checkouts retain their pinned HEADs and clean status. Existing M4 working changes
were present before this pass and left untouched. Documentation links and
`git diff --check` were checked. No new game build, emulator run, media mount,
application source edit or commit was needed.

Final checks: **33 local links resolved**, no trailing whitespace in the new
document, and `git diff --check` passed. **149 existing application/MCP/media files**
retained their SHA-256 and permissions; both external reference HEADs were unchanged
and both worktrees remained clean. The only tracked-document edit is the navigation
row in this directory’s README; this crosswalk is new. No commit.
