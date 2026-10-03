# M4 opening visual audit: cartridge beats 002–012

This is an evidence and analysis checkpoint, not a presentation change. The oracle is the [source-built cartridge storyboard](DAGGORATH_CARTRIDGE_STORYBOARD.md) and its numbered original frames. The EOU side of every pair comes from **one continuous public `/d1/daggorath` launch**, not a separate `dodwiz` run. The completed-frame AVI has 5,545 frames at approximately 59.92 fps over 92.536 seconds; the raw recording remains disposable at `/private/tmp/daggorath-m4-clock/capture/opening.avi`. The `dodwiz` module in that run was byte-verified: 24,562 bytes, CRC `A25DCA`, SHA-256 `8dfe734d862f621c07b60db5badc62e3ec5282cc4fbe57ab8250957f625b3c3f`.

Each retained PNG is a horizontal `ORIGINAL | CURRENT EOU` join of two **unaltered 640×236 raster frames**. Neither half was independently scaled, shifted, recolored, or aligned. The EOU capture has a consistent two-scanline downward offset relative to the cartridge captures and a black exterior where the original has a white video border. These are recorded capture/platform differences; measurements below account for the offset without modifying the evidence images. The cartridge's NTSC/artifact-color fringes extend about two pixels beyond the EOU monochrome core in several glyphs and vectors. A binary pixel overlap is supporting evidence, not an audio or human perceptual match.

## Paired beat review

The single primary status in each row is for the *selected visible beat*. Timing shortcomings can still be recorded as secondary findings when the image matches. EOU times are from the continuous recording start; original representative times come from the storyboard frame manifest. A pair is linked in every row.

| Beat | Pair | Original / selected EOU time | Primary status | Observation |
| --- | --- | ---: | --- | --- |
| 002 copyright / first strokes | [ORIGINAL | EOU](assets/daggorath-opening-m4/visual-audit/002-original-current-eou.png) | 2.1 / 31.8 s | MATCH | Sparse first Wizard strokes and copyright are both present. The original exterior is white, EOU exterior black; source stroke cores align after the capture offset. |
| 003 Wizard emerging | [ORIGINAL | EOU](assets/daggorath-opening-m4/visual-audit/003-original-current-eou.png) | 4.1 / 34.3 s | MATCH | Same emerging crescent/body stage and persistent copyright. Minor fringe/color differences are inherent to the capture modes. |
| 004 Wizard complete | [ORIGINAL | EOU](assets/daggorath-opening-m4/visual-audit/004-original-current-eou.png) | 6.7 / 37.5 s | MATCH | Complete Wizard bounds and copyright match in the monochrome core. |
| 005 two welcome lines | [ORIGINAL | EOU](assets/daggorath-opening-m4/visual-audit/005-original-current-eou.png) | 8.4 / 39.5 s | MATCH | Both lines overlay the Wizard. First line has no prefix dot; second begins with its three literal dots. Two row baselines align after the capture offset. |
| 006 welcome cleared | [ORIGINAL | EOU](assets/daggorath-opening-m4/visual-audit/006-original-current-eou.png) | 10.9 / 41.5 s | MATCH | Both lower text rows clear; Wizard and copyright remain. |
| 007 Wizard fade | [ORIGINAL | EOU](assets/daggorath-opening-m4/visual-audit/007-original-current-eou.png) | 13.4 / 43.6 s | MATCH | Sparse late Wizard strokes remain in both, with copyright unchanged. |
| 008 near-black handoff | [ORIGINAL | EOU](assets/daggorath-opening-m4/visual-audit/008-original-current-eou.png) | 16.6 / 47.3 s | WRONG TIMING | The main field is nearly black with a few fading strokes, and the copyright strip remains. EOU then holds its black/copyright handoff far longer than the cartridge before PREPARE. |
| 009 PREPARE | [ORIGINAL | EOU](assets/daggorath-opening-m4/visual-audit/009-original-current-eou.png) | 17.4 / 63.5 s | WRONG TIMING | `PREPARE!` is at the source cell and copyright persists, but its first appearance follows a roughly 14-second EOU blank handoff and its subsequent map transition is substantially later. |
| 010 full map | [ORIGINAL | EOU](assets/daggorath-opening-m4/visual-audit/010-original-current-eou.png) | 24.0 / 83.5 s | WRONG TIMING | Full map geometry is very close, and it replaces the old copyright strip as required. It appears late because of the prior PREPARE-to-map interval. |
| 011 map-to-dungeon | [ORIGINAL | EOU](assets/daggorath-opening-m4/visual-audit/011-original-current-eou.png) | 26.1 / 85.0 s | WRONG TIMING | **The original selected still is the last full map**, despite its `black-before-dungeon` filename. The closest EOU still is likewise its last full map. The subsequent EOU black interval is substantially longer; this pair alone does not show a black cartridge frame. |
| 012 first dungeon / lone dot | [ORIGINAL | EOU](assets/daggorath-opening-m4/visual-audit/012-original-current-eou.png) | 26.6 / 87.0 s | PARTIAL | Dark field, empty-hand status, active large heart, four otherwise blank primary rows, lone dot and underline cursor are present; no `OK`, Wizard, or map residue. Two dark heart pixels differ at the selected phase after offset correction, so exact heart raster identity is unproven. Its arrival is also late. |

**Primary counts:** MATCH 6; PARTIAL 1; MISSING 0; EXTRA 0; WRONG GEOMETRY 0; WRONG TIMING 4; INTENTIONAL PLATFORM DIFFERENCE 0. The white-versus-black exterior, artifact colors, and uniform two-scanline capture offset are documented *secondary* platform differences across the sequence, not eleven duplicate primary classifications. No beat was marked MATCH merely because its source routine executed.

## Geometry and persistence checks

Coordinates in this table are inclusive bright-pixel bounds in the original physical 640×236 capture followed by the EOU capture. The EOU `+2` in Y is consistent across independent elements; the paired images were not adjusted. The original edge-color fringes explain many two-pixel X differences without moving the logical element.

| Feature | Original physical bounds | EOU physical bounds | Finding |
| --- | --- | --- | --- |
| Complete Wizard bright core, beat 004 | x226–395, y44–156 | x228–393, y46–158 | Same vector envelope after accounting for artifact fringe and uniform +2 Y capture offset. |
| Copyright/status strip, beats 002–009 | x64–575, y176–183 | x64–575, y178–185 | Retained continuously through beat 009. The EOU strip crop is byte-identical across these eight selected frames. |
| Welcome glyphs, beat 005 | x68–553, y192–206 | x68–551, y194–208 | Row glyph groups at original y192–198 and 200–206; EOU rows +2. |
| `PREPARE!`, beat 009 | x256–377, y96–102 | x258–375, y98–104 | Source logical `(96,72)` placement survives; original artifact fringe adds apparent width. |
| Map field, beat 010 | active x64–575, y24–175 | active x64–575, y26–177 | 42,114 EOU bright map pixels all fall within the original bright mask after offset correction; binary Dice similarity 0.994 for the active field. |
| First dungeon status, beat 012 | y176–183 | y178–185 | Status line, `EMPTY` hand labels, divider and heart are present. The dark upper field does not, by itself, establish lit perspective geometry. |
| Primary transcript, beat 012 | logical y160/168/176/184; physical first row y184–191 | same logical rows, physical first row y186–193 | Four 32-character rows exist; all have no words. Only row 1 contains the dot and transient underline cursor. No fifth row or generic `OK`. |

For the copyright strip, the selected original crop hash is constant across 002–009 and changes at 010; the selected EOU crop likewise stays constant through 009 and changes at 010. This confirms persistence through the welcome clear, Wizard fade, black handoff, and PREPARE, and replacement by the map. Beat 011's selected original still remains a map; the source event sequence and adjacent completed EOU frames establish the intervening clear separately.

The selected beat-012 EOU frame was chosen to match the original's **large heart phase**. In a binary comparison of the heart core after correcting only the measured capture offset, both have a 52-pixel dark pattern within the bright status track, with two EOU dark pixels extending into the last compared row where the original has bright pixels. This is too small to justify moving the heartbeat or changing its timing by eye. It warrants source/phase/scanout attribution before any raster correction. The dot and cursor positions agree after accounting for artifact fringe and the two-scanline capture offset.

## Timing

These selected-still offsets are normalized independently to beat 002 (original 2.1 s; EOU 31.8 s). The EOU frame clock resolves about 16.7 ms; selected analogous stills introduce additional subjective uncertainty. The early roughly 0.5–1.4-second offsets should not be interpreted as a proved standalone wait defect. The large later gaps are also supported by independently detected first/last visible transitions.

| Beat | Original from 002 | EOU from 002 | EOU − original | Dwell / transition assessment |
| --- | ---: | ---: | ---: | --- |
| 002 | 0.0 s | 0.0 s | 0.0 s | First selected cartridge-controlled beat. |
| 003 | 2.0 | 2.5 | +0.5 | Corresponding Wizard construction stage found. |
| 004 | 4.6 | 5.7 | +1.1 | Complete Wizard found; selected-frame offset alone does not prove a distinct choreography failure. |
| 005 | 6.3 | 7.7 | +1.4 | Both lines present. |
| 006 | 8.8 | 9.7 | +0.9 | Clear follows welcome. |
| 007 | 11.3 | 11.8 | +0.5 | Fade visibly underway. |
| 008 | 14.5 | 15.5 | +1.0 | Near-black content matches; following blank dwell diverges. |
| 009 | 15.3 | 31.7 | +16.4 | PREPARE appears after prolonged EOU handoff. |
| 010 | 21.9 | 51.7 | +29.8 | Full map onset inherits prior handoff and PREPARE-to-map delay. |
| 011 | 24.0 | 53.2 | +29.2 | Last full-map still; clear follows. |
| 012 | 24.5 | 55.2 | +30.7 | Dungeon/status arrival also delayed after map clear. |

The source-correlated cartridge transition observations are: final Wizard blank **16.920 s**, PREPARE first visible **16.970 s** (0.050 s later), full map first visible **23.412 s** (6.442 s after PREPARE), and first dungeon **26.316 s**. In the EOU completed-frame sequence, final Wizard blank is **47.773 s**, PREPARE first visible **61.808 s** (**14.035 s** later), full map **82.351 s** (**20.543 s** after PREPARE), map clear begins **85.221 s**, full black arrives **85.338 s**, and dungeon/status first appears **86.539 s** (**1.201 s** after full black). The original last-map reference at 26.1 s and dungeon at 26.316 s bound its corresponding clear to roughly 0.216 s or less; that is a bound from source evidence, not an independently timed first-black frame. The EOU's long transitions materially change the recognizable opening rhythm. Their exact underlying OS-9 load/render/wait attribution is **not established by this visual audit**.

## Ordered findings for a subsequent correction pass

1. **Beat 008 → 009, first actionable divergence:** explain the 14.035-second EOU black/copyright handoff versus the cartridge's 0.050-second final-blank-to-PREPARE handoff. Preserve copyright until the map. Do not shorten a legitimate source wait without identifying which phase or load occupies the interval.
2. **Beat 009 → 010:** attribute the 20.543-second EOU PREPARE-to-full-map interval versus the cartridge's 6.442 seconds. The source interval includes level creation and map drawing, so a naked PREPARE delay is not the assumed cause.
3. **Beat 011 → 012:** attribute the 1.201-second fully black EOU interval before status/dungeon versus the source reference bound of about 0.216 seconds. Retain the actual black transition.
4. **Beat 012:** verify the two-pixel selected-phase heart residual against source glyphs and frame scanout before changing placement or cached images. The dark initial view cannot establish lit dungeon perspective geometry; audit that at the later matching lit beats.

The earliest *literal* image difference is the platform/capture border at beat 002. The first **actionable storyboard divergence** is the prolonged black handoff after beat 008. This audit makes no production or test change and gives no instruction to repair these findings without a separate implementation decision.
