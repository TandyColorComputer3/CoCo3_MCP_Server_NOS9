# Cartridge Opening Fidelity M4 — destructive chain ownership

**Checkpoint:** the public opening reaches beat 012. The later
[eleven-beat visual audit](DAGGORATH_M4_VISUAL_AUDIT.md) supersedes the
historical partial-capture tables below: 002–007 MATCH, 008–011 WRONG TIMING,
012 PARTIAL. Functional progression is accepted; timing, heart raster and
opening sound fidelity are not declared complete.

## Clock discontinuity and public-launch acceptance (2026-10-02)

The intermittent public-launch `187` came from the application's Wizard
`sample_clock()` guard, **not** from an OS-9 syscall. A private build changing
only that guard's return from 187 to 188 changed the child status to 188.
Earlier byte-identical public builds also completed normally, so the status
alone did not establish a phase-chain, graphics, heartbeat, or Wizard-vector
regression. [The prior independent clock investigation](DAGGORATH_AUDIO_RESEARCH.md#15-status-187-follow-up-wizard-rejects-a-restored-wall-clock-jump)
measured a `3596 → 1575` apparent 1,579-tick jump and traced EOU Clock2's
minute refresh to the RTC calendar after a restored state.

`os_clock()` copies EOU physical-block-0 `D.Time` through `D.Tick` (`$28..$2E`)
via `F$CpyMem`, checks the second/countdown ranges, and returns an unsigned
16-bit tick-of-minute: `second*60 + 60 - D.Tick`. `sample_clock()` computes
`(now - previous) mod 3600`. Installed EOU
`/dd/SOURCECODE/ASM/NITROS9/CLOCKS/clock.asm:SvcVIRQ` calls
`clock2_messemu.asm:GetTime` when seconds reach 60; the latter refreshes the
calendar from the Disto RTC. The seven-byte copy is not established as atomic
against an IRQ, but no torn/out-of-range packet was observed in the 9,600-read
target probe. That probe did see a **1,021-tick maximum** across a minute
refresh. The old `delta > 300` return was a port-side sanity guard against
treating a clock adjustment or stall as animation time; fatal termination was
not a cartridge behavior requirement.

The current nonfatal private Wizard probe changed no normal timing operation
and deferred diagnostic printing until after the timed phase. Its first fresh
restored public run sampled 302 times with no large delta (`max=28`). A second
run sampled 303 times and captured this coherent EOU calendar refresh:

| Recorded run | Samples | Delta 0 | Delta 1 | Delta 2–10 | Delta 11–300 | Delta >300 | Maximum |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| First fresh restore | 302 | 0 | 267 | 1 | 34 | 0 | 28 |
| Later no-restore launch | 303 | 0 | 267 | 1 | 34 | 1 | 421 |
| Further fresh restore | 302 | 0 | 267 | 1 | 34 | 0 | 28 |

| Previous | Current | Computed delta | Initial packet from that run | Raw refreshed packet |
| --- | --- | ---: | --- | --- |
| 3599 | 420 | **421** | `126 10 2 14 45 53 52`; the immediately preceding packet was not saved | `126 10 2 14 46 7 60` |

The large apparent delta occurred while the Wizard continued, not after a
seven-second foreground stall. A further recorded diagnostic launch sampled
302 times without a large delta (`max=28`). The earlier no-tone and
cold-boot/restore controls in the
linked investigation independently establish that audio is unnecessary and
that restored RTC/calendar state can diverge. The current target-side
arithmetic probe passed ordinary increment, low/higher-byte rollover,
minute wrap, and both measured jumps (`3599→420=421`,
`3596→1575=1579`). Inspection of both the diagnostic `-O0` and the accepted
production `-O2` CMOC 0.1.90 listings found `CMPD`/`LBHS` followed by
`SUBD`, or `LDD #$0E10; SUBD; ADDD` for wrap, and `CMPD #$012C; LBHI` for
the guard. The `-O2` branch writes the new `previous` and returns zero without
adding to `elapsed`. Thus the target arithmetic implements the intended unsigned
modulo calculation; the input clock is discontinuous.

The bounded correction preserves the `>300` detection but sets `previous=now`
and returns success without adding that delta to `elapsed`. It also treats an
actual longer-than-five-second interruption as a new baseline rather than
playing elapsed animation in a burst. Ordinary deltas and minute wrap retain
their prior behavior. A focused host test exercises those cases and recovery
after both measured jumps. No phase-chain or presentation code changed for
this correction.

Two clean builds produced byte-identical `dodwiz`: **24,562 bytes**, data
request **7,855**, CRC **`A25DCA`**, SHA-256
`8dfe734d862f621c07b60db5badc62e3ec5282cc4fbe57ab8250957f625b3c3f`.
The executable was extracted back from disposable acceptance media with the
same hash. **Ten fresh `nos9_ready_v2` public `/d1/daggorath` launches** used
that artifact, without manual module preload; each returned Shell+ status
`000` after the opening. The post-run `mdir -e` export contained no retained
`daggorath`, `dodwiz`, `dodintro`, `DHeartbeat`, or `dhb` entry.

A separate public completed-frame recording captured **5,545 frames at about
59.92 fps**. It shows beats 002–012 from one run, including short Wizard/fade
states, copyright persistence through PREPARE, map replacement, black
map-to-dungeon transition, and the first dot-only dungeon/status frame. These
recording timestamps include shell restore/module loading and are not source
logical-jiffy measurements. The raw 2.4 GiB AVI remains disposable; the small
selected frames are retained below. This capture is evidence collection, not
an assertion that every visual/timing comparison now matches the cartridge.

| Beat | EOU completed-frame sample, seconds from recording start |
| --- | --- |
| 002 | [copyright and first strokes](assets/daggorath-opening-m4/clock-capture/002-t31.5.png), 31.5 s |
| 003 | [Wizard emerging](assets/daggorath-opening-m4/clock-capture/003-t35.5.png), 35.5 s |
| 004 | [Wizard complete](assets/daggorath-opening-m4/clock-capture/004-t37.5.png), 37.5 s |
| 005 | [welcome](assets/daggorath-opening-m4/clock-capture/005-t39.5.png), 39.5 s |
| 006 | [welcome cleared](assets/daggorath-opening-m4/clock-capture/006-t41.5.png), 41.5 s |
| 007 | [Wizard fade](assets/daggorath-opening-m4/clock-capture/007-t44.5.png), 44.5 s |
| 008 | [black viewport, copyright retained](assets/daggorath-opening-m4/clock-capture/008-t48.5.png), 48.5 s |
| 009 | [PREPARE and copyright](assets/daggorath-opening-m4/clock-capture/009-t64.5.png), 64.5 s |
| 010 | [map](assets/daggorath-opening-m4/clock-capture/010-t83.5.png), 83.5 s |
| 011 | [black transition](assets/daggorath-opening-m4/clock-capture/011-t86.5.png), 86.5 s |
| 012 | [initial dungeon/status and lone dot](assets/daggorath-opening-m4/clock-capture/012-t87.5.png), 87.5 s |


## Continued visual check: copyright survives until the map

The earlier description of beat 008 as having no copyright was wrong. The
committed cartridge captures for beats 006, 007, 008, and 009 have **identical
512×8 status-strip pixels** (the aligned binary masks have the same SHA-256
prefix `5da873e61ad177a2`). The viewport becomes nearly black, but the
copyright strip remains. The map at beat 010 replaces it. This agrees with
`MISC.ASM:WIZOX`, which clears the primary text area, `CLEAR.ASM:ZFLOPX`,
which clears the selected graphics VDB, and `MISC.ASM:PREPAX`, which writes
the EXAMINE area without calling `CLRSTS`. `COMDAT.ASM:STSVDB` locates the
status strip at logical scanlines 152–159. The user's live cartridge
observation was decisive in correcting the interpretation of the small frame.

The current EOU opening retains those status bytes through Wizard fade,
near-black viewport transition, and PREPARE. In one continuous, byte-verified
`/d1/daggorath` run, sampled EOU status masks from Wizard through PREPARE
were identical (SHA-256 prefix `df712c2b6f3ceebd`, after the two-scanline
capture offset); the map changed that strip. `wizard_copyright` draws the
same source-derived status material into PREPARE's logical frame, and a focused
test guards its persistence and map replacement. The original and EOU masks
are not claimed pixel-identical: cartridge color/capture and EOU type-5 RGB
monochrome differ.

The opening's native heartbeat dependency also exposed an EOU module-loading
boundary. A clean restored shell has no `/dhb`. Separate driver/descriptor
loads let the later `I$Open` return raw 237 (`E$RAMFull`), while a disposable
Term probe showed that `F$Load` of the existing concatenated `dhbpack` permits
`I$Open`, `I$Close`, and automatic final module removal. The opening now uses
that same pack before the map, without a manual shell `load`. Its wrapper
must restore CMOC's U/Y context before storing `F$Load`'s returned type; a
disposable stage-coded run caught an initial 187 type-check failure from the
incorrect ordering. The corrected continuous run reached the map and initial
dark dungeon/prompt, returned to Term without an error line, and `mdir`
showed no remaining `DHeartbeat` or `dhb` module. This is opening-path
evidence, not final acceptance of all visual beats or heartbeat latency.

The byte-verified `dodintro` in that run is 22,712 bytes (data request
10,924, CRC `E12D49`, SHA-256
`19c85fe14fda2c43b2a1faa1d29d4973fbba92d5fef4293f98771f1029849e5f`).
The 740-byte `dhbpack` SHA-256 is
`42af881270cfdad9b2323ac37380c4d7c890f897ae1acfcb5eb6bd3aadc72b80`.
`build_opening.py` now produces that pack alongside the public phase modules.
These files were staged on a disposable floppy and extracted back with matching
hashes. No shell module preload occurred during the public run.

### Current visual pairs, still under review

Each pair is **ORIGINAL | EOU**, aligned on the 512×192 logical game area.
The EOU column is from the same continuous restored-state public launch;
source images are from the committed cartridge storyboard. The monochrome
type-5 RGB display cannot reproduce the cartridge capture's artifact colors.
The pairs are review evidence, not an assertion of timing or final M4 fidelity.

| Beat | Pair | Current observation |
| --- | --- | --- |
| 002 | [first strokes](assets/daggorath-opening-m4/002-first-strokes.png) | Early Wizard dots and copyright appear; the sampled fade phase is later than the source frame. |
| 004 | [complete Wizard](assets/daggorath-opening-m4/004-wizard-complete.png) | Main vector geometry aligns; cartridge artifact colors differ. |
| 005 | [welcome](assets/daggorath-opening-m4/005-welcome.png) | Both source message rows appear with Wizard and copyright. |
| 007 | [fade](assets/daggorath-opening-m4/007-fade.png) | Sparse Wizard pixels remain over persistent copyright; exact temporal phase is not matched by this sample. |
| 008 | [near-black viewport](assets/daggorath-opening-m4/008-near-black.png) | EOU sample is later in the fade than the cartridge frame. The status strip remains in both. The original's remaining 50 upper-field bright pixels best overlap generated fade 28 (22 pixels), a phase that `dodwiz` schedules but that the coarse live capture missed. |
| 009 | [PREPARE](assets/daggorath-opening-m4/009-prepare.png) | Source cell and persistent copyright strip align after the accepted two-scanline capture offset. |
| 010 | [map](assets/daggorath-opening-m4/010-map.png) | Source-derived map replaces the copyright/status material. Aligned binary masks differ at 328 of 98,304 pixels at a grayscale threshold of 80. |
| 012 | [dark dungeon/prompt](assets/daggorath-opening-m4/012-dungeon-prompt.png) | Empty hands, status/heart and the lone dot with cursor are visible; no `OK` or stale map text. |

The present MCP screenshot cadence missed the short exact phases for beats
003, 006 and 011. Host PNG timestamps include observer delays, so they are
**not** emulated-jiffy timing measurements. The source Wizard deadline table,
`MISC.ASM:WAITX`, and the intro's explicit 162-jiffy map dwell document the
intended logical waits; a frame-numbered live comparison is still needed for
the precise transient gates. No beat 013 command was dispatched. The current
opening is therefore **not yet marked M4 ready for human-review acceptance**.

## Earlier phase-controller checkpoint (historical)

The following sections record the earlier bounded phase-controller proof as
it stood when execution was deliberately stopped at PREPARE. The continued
visual run and newer artifact figures above supersede that checkpoint's
"not yet reached" entries; its failure-cleanup evidence remains applicable.

The committed [cartridge storyboard](DAGGORATH_CARTRIDGE_STORYBOARD.md) remains the oracle. Beat 001's cartridge reset-video transient is an **INTENTIONAL PLATFORM DIFFERENCE — NitrOS-9 launch boundary**; beat 002 is the first required EOU visual match. This checkpoint proved the public phase lifecycle only, through PREPARE. It was **not** visual or audible acceptance of later beats. M4 code and the earlier M3/audio work remain uncommitted.

## Continuous public boundary proof

From a fresh `nos9_ready_v2` Term state on `coco3h` with 2 MB and RGB, one `/d1/daggorath` command displayed the Wizard/copyright frame, then the black handoff, then `PREPARE!`. No `load dodwiz` or `load dodintro` command was issued and Shell+ did not reappear between phases. The exact staged modules were extracted and byte-compared with the host builds. The disposable disk's three executable files had owner/public execute attributes set before launch. MAME was stopped at PREPARE, before judging the map or dungeon.

The public controller uses one process identity and two `F$Chain` replacements. The corrected helper preflights a NUL-terminated absolute executable path with `F$NMLoad`, checks its returned type, and **releases that reference before** calling `F$Chain` with the same NUL path, type `$11`, one optional data page, the exact parameter byte count and a parameter copy in caller process data. The launcher passes eight bytes (`opening` plus CR); Wizard passes the decimal inherited graphics-path number plus CR. No arriving phase calls `F$UnLoad`. The chain syscall obtains the new primary module link itself. A failed preflight returns its raw error while the caller is intact; a later chain failure can condemn the caller.

Wizard's `screen_handoff` relinquishes only its process-local `SS.MpGPB` mapping; the inherited graphics path and CoWin GP buffer remain owned by the same process across `F$Chain`. Intro remaps GP 196/1. Wizard no longer defines the heartbeat phase buffers it never uses, avoiding a duplicate definition on intro adoption. This is a deliberate single-window handoff, not an accidental window close masquerading as the black beat. Current Wizard and intro programs each occupy three 8K program blocks; their respective data requests are 7,855 bytes (one block) and 10,923 bytes (two blocks). Intro plus CoWin and GP mapping is 3 + 2 + 1 + 1 = 7/8 logical blocks. The preflight nonmapping reference is released before either handoff.

### Historical statuses and their evidentiary limits

Installed EOU definitions identify **221 = E$MNF (Module Not Found)**, **194 = E$BadBuf (bad/undefined buffer number)** and **234 = E$NEMod (Non-existent Module)**. The old final 221 occurred before Wizard on a direct `F$Chain` attempt using a high-bit-terminated absolute pathname. Level II source shows `F$Chain` tries `F$SLink` then `F$Load`; no syscall-boundary capture distinguished which returned 221 in that old run. A later controlled probe directly showed `F$NMLoad` returning **215 = E$BPNam** with a high-bit pathname; `F$NMLoad` reaches `I$Open`. The corrected chain passes the NUL-terminated path to both preflight and the kernel's own load fallback. A disposable staging failure was also traced to absent execute attributes on the test disk, corrected only there.

The old final 194 appeared after Wizard and before PREPARE. Source review found Wizard unnecessarily defining GP 196/2, while intro adoption defined it again; CoWin reports E$BadBuf for an existing GP buffer. Removing Wizard's unused heart-buffer definitions allowed the continuous run to reach PREPARE. This is a cause supported by the source difference and before/after live behavior, though that old run did not capture the return at the `DefGPB` syscall itself. The older final 234 came from a separately preloaded, superseded build. No syscall-boundary trace survives for it; **its exact origin remains unattributed and need not be reconstructed for M4**. The new controlled failure experiment below also returns 234, but does not retroactively establish the old run's cause.

## Superseded preload-helper failure cleanup proof — failed

A disposable `phasefail` program linked the **current production `phase-chain.c`** unchanged for the initial experiment. Its 27-byte `$21/$80` `phtarget` module was staged on a private executable disk and byte-verified. `dod_chain("/d1/phtarget", "x\r", 2)` successfully acquired a retained `F$NMLoad` reference. The helper then asked `F$Chain` for a `$11` program; the guest exited with **234 / E$NEMod**. Shell+ remained usable. The authoritative upstream Level II `kernel/fchain.asm` checks the linked/loaded target's type and, on error, marks the process condemned. `level2/modules/kernel/ffmodul.asm` rejects a directory lookup whose first character is `/`.

The first three failed launches left `phtarget` module-directory use counts **1 → 2 → 3**, captured through `mdir -e` to disposable files. This is a cumulative retained-reference leak, not merely a returned error. I then made the narrow helper correction that passes only the basename to `F$UnLoad` if a chain call returns. The controlled failure still produced counts **1 → 2**. A disposable caller that would return intentional status **042** after `dod_chain()` instead exited with the kernel's **234**, proving that this failure does **not** return to the caller-side cleanup branch on this EOU path. The basename correction is source-correct for a kernel variant that returns an error, but it cannot repair this installed-kernel failure.

Two other classes did return normally: missing `/d1/absentmod` produced **216 / E$PNNF** before acquiring a retained target reference, and invalid parameters produced the helper's **187 / E$IllArg** before attempting a load. The caller's inherited stdout write succeeded in both cases, and subsequent Shell+ commands worked. They create no `phtarget` reference. This experiment establishes a **current contract limitation**: preloading with `F$NMLoad` can leave a physical module reference if the later `F$Chain` fails and condemns the process. Do not describe failed-chain cleanup as proven or leak-free.

## Corrected ownership and installed Level II path

The identified upstream source is `nitros9-reference/level1/modules/kernel/fchain.asm`'s **Level II** branch (`ELSE`), plus `level1/modules/ioman.asm:FNMLoad/FLoad` and `level2/modules/kernel/ffmodul.asm`. `FChainProcess` copies the descriptor, clears signals, unlinks the old `P$PModul`, and marks unused DAT entries free **before** `FChainEverything` searches for the target. That routine first uses `F$SLink` in the new DAT image and falls back to `F$Load` with the pathname. The linked or loaded pointer becomes the new descriptor's `P$PModul`; the new data request is allocated and the process register image replaced. The success path activates that process and never returns to old C. A type or memory failure goes to `FChainTarget`, which calls `F$Exit` with the error. The kernel's new primary link is distinct from any `F$NMLoad` reference created by application code. The old helper's latter reference was not consumed or transferred by `F$Chain`, explaining the measured leak.

The corrected helper uses `F$NMLoad` only as recoverable preflight: it obtains the target's actual type, calls `F$UnLoad` with that type and the module basename, checks release success, then rejects non-`$11` types before the destructive boundary. This makes a wrong-type module return **187 / E$IllArg** without calling `F$Chain`. The target's executable path remains a normal NUL-terminated pathname for `F$Chain` itself; its `F$SLink` miss can safely fall back to `F$Load` without relying on an application-held reference. The parameter limit, nonnull pointer, copied process-data lifetime, and caller-side graphics handoff are checked/established before chain. This preflight is not an atomic guarantee against media or kernel failures after it: such a failure still terminates the process, but has no caller-held module reference to leak.

A byte-verified disposable direct-chain probe with a **NUL pathname and no pre-load** targeted the same `$21/$80` `phtarget`. The guest returned **234 / E$NEMod** at the kernel's type check. Two further identical failed runs left `phtarget` **absent** from all three captured `mdir -e` module directories (`count1`, `count2`, `count3`), rather than climbing 1→2→3 as with the old helper. The revised helper's byte-verified private caller rejected the same target with 187; after repeated runs `phtarget` was likewise absent from `newcount1` and `newcount2`. A missing pathname still returned 216. These counts support the absence of a caller-owned retained reference on the deliberate destructive path and the preflight-return path; they do not make arbitrary kernel failures recoverable.

The exact final type-aware-release build was staged byte-for-byte on disposable media. Two fresh `nos9_ready_v2` launches of `/d1/daggorath`, without manual load or a stale resident phase, showed Wizard/copyright, black handoff and PREPARE. Each run was stopped at PREPARE. The revised helper's generated assembly places `F$NMLoad`, the `F$UnLoad` preflight release call, and `F$Chain` in that order; `Y` is the exact parameter length and `U` points into caller process data. No `dod_chain_arrived` call remains in Wizard or intro.

A disposable caller linked the same corrected `phase-chain.c` and chained to a byte-verified `$11/$81` `phprog` with sufficient data request. Its successful target returned intentional status **042**. Three attempts produced 042, 216, 042 (the middle preflight returned path-not-found, with Shell+ still usable); `mdir -e` snapshots after each attempt contained **no** `phprog` or `phhelper` entry. The intermittent 216 in this private disk run was not attributed and is not presented as a production-chain success. The two 042 runs prove the normal helper's successful chain reaches the target without leaving a caller-held reference; the unchanged module-directory absence across all snapshots rules out accumulating use counts in this private repeated-run test. The production Wizard→intro proof is separate and was not allowed to continue past PREPARE solely to obtain a shell-side module listing.

## Controlled chain evidence

Level II `F$Chain` replaces the current module and process data and preserves open paths. The upstream `level1/modules/kernel/fchain.asm` Level II branch unlinks the old primary module before copying the U/Y parameter area. A disposable `/d1/chainlarge` → `/d1/retlarge` probe returned the target's intentional status **042** after validating the exact `opening` argument. A target with insufficient data-space headroom instead returned **223** during `F$Mem`; that separate probe does not diagnose the public chain. The production helper copies all parameter bytes to process data and uses the exact byte count.

The direct, independently preloaded Wizard run displayed its source-derived frame and copyright. Its `sound_event` remains a silent stub; the storyboard records fade buzz and `KABOOM` during beats 002–007. Thus even a completed graphics chain would still require an audible-fidelity decision and verification before this milestone could be called a match.

## Beat gate

| Beat | Cartridge | Current EOU evidence | Status |
| --- | --- | --- | --- |
| 001 | reset/first video transient | Term/Shell+ is the NitrOS-9 launch boundary | INTENTIONAL PLATFORM DIFFERENCE |
| 002 | copyright and first Wizard strokes | continuous live frame captured, but not at the exact early fade phase | PARTIAL; exact phase capture pending |
| 003–007 | emerging/complete Wizard, welcome, clear, fade | complete Wizard, welcome and fade captured; short emergence/clear phases missed by the observer | PARTIAL; sound fidelity deferred |
| 008 | near-black viewport with copyright retained | persistent status verified against original pixels; exact fade phase missed | PARTIAL; frame-numbered timing pending |
| 009 | PREPARE with copyright | source-cell placement and persistent strip captured | PARTIAL; timing pending |
| 010 | full map replaces copyright | source-derived renderer captured in the continuous run | PARTIAL; final geometry review pending |
| 011 | black before dungeon | explicit clear and two-jiffy hold in the current phase; observer missed this short frame | PHASE IMPLEMENTED; live frame pending |
| 012 | first dungeon, four-row lone dot | live dark frame, empty hands and lone dot captured; no `OK` | PARTIAL; final comparison pending |

The paired frames above are preliminary visual-review evidence. The
[original numbered frames](assets/daggorath-cartridge-storyboard/frame-manifest.json)
remain the reference. The current intro runner stops after beat 012 and does
not dispatch AUTTAB command 1.

## Implementation and checks so far

The uncommitted M4 work adds a public launcher, `F$Chain` helper, graphics-path handoff/adoption, a separate `dodintro` opening phase, source-based full-map drawing, and persistent four-row primary-text state. The byte-verified final ownership build is module/CRC valid: `daggorath` **863 bytes**, `dodwiz` **24,533 bytes** with data request **7,855**, and `dodintro` **22,403 bytes** with data request **10,923**. The live Wizard and PREPARE frames prove both phase entries; the 7/8 mapping count remains a source/module-size calculation rather than a captured DAT image for this run.

`test_opening_m4.py` passed **2/2** and the approved narrow source-incorrect update to `test_phase_chain_m4.py` passed **1/1**. Full Daggorath/MCP regression, paired frame review, timing comparison, cleanup after normal graphical completion, and beat-012 visual gate have **not** run in this bounded task. MAME was stopped at PREPARE. Canonical media and `nos9_ready_v2` retain their recorded hashes; the stock VHD remains read-only.

The phase controller's reusable contract is now: validate length and storage; preflight target existence/type; release the preflight reference; call `F$Chain` with a NUL pathname and process-data parameters; let the arriving phase own its inherited graphics path and rebuild local state. Successful chain never returns, and a failed chain may terminate the process with the raw kernel error. No application cleanup depends on instructions after it. No manual Term `load` step was part of either successful public run.


## Opening polish debt retained for M5

M5 begins at beat 013; these opening issues remain tracked and must not be
silently reclassified as matches:

- Final Wizard dissolve to PREPARE retains substantial EOU-only latency. The
  original 14.035-second public interval has no complete same-run partition;
  private preflight and chain/startup brackets identify substantial loading work.
- PREPARE to map was 20.543 seconds before the separately measured maze RNG
  optimization and about 9.16 seconds afterward, versus 6.442 seconds in the
  cartridge reference. No source wait was shortened.
- Heartbeat-pack acquisition costs about 2.7 seconds. Moving preparation needs
  an explicit cross-phase ownership proof.
- Black to first dungeon is slower than the cartridge; the four-row blank-cell
  glyph loop is a measured optimization opportunity, not an implemented fix.
- The selected beat-012 large heart differs by two pixels. Review correctly
  matched dynamic phases before changing geometry.
- Wizard fade buzz and KABOOM still require opening-audio fidelity work.

The historical measurements and causal limits remain in the
[root-cause report](DAGGORATH_M4_OPENING_LATENCY_ROOT_CAUSE.md).
