# Window Manager / Window Profile feasibility

Research date: 2026-09-26. **Source archaeology, not an application or a live API certification.** No guest operation, media change or reference-repository change was made.

## Evidence and scope

Start with the [source index](../source-index/README.md), [windowing](../source-index/GRAPHICS_WINDOWING.md), [SCF/VTIO](../source-index/SCF_VTIO.md), [runtime observations](../source-index/RUNTIME_MATCHES.md) and [upstream crosswalk](../source-index/EOU_UPSTREAM_CROSSWALK.md). `MCP/Documents/` and `DOCS_INDEX.md` are absent in this checkout; consequently no manual-page confirmation is claimed. The API descriptions below are verified against implementations, and still need a small runtime compatibility probe before application work.

Source shorthand used throughout:

- **CW**: official upstream [level2/coco3/modules/cowin.asm](/Volumes/SEDONA/Projects/nitros9-reference/level2/coco3/modules/cowin.asm), commit `f470fa52eb172b59b22c1b722074998cb42de9b1`. Compare EOU `/dd/SOURCECODE/ASM/NITROS9/SCF/cowin_beta6.asm`, especially `GetStt`, `L0A9A`, `L0AD5`, `L0AF4`.
- **G2**: EOU `/dd/SOURCECODE/ASM/BASIC09/gfx2_ver1.asm`, `FuncTbl` and named handler comments. GFX2 is a BASIC09 wrapper; a C/assembly application can use the underlying path operations without loading a BASIC09 wrapper.
- **WI**: EOU `/dd/SOURCECODE/ASM/VEFIO-WINFO/winfo.asm`, `winfodefs`, `windowinfo.b09`. WInfo source declares edition 3. The demonstration credits Ron Lammardo (1987) and an EOU edition by L. Curtis Boyle (December 2023). Upstream historical [archive/utils/winfo/winfo.doc](/Volumes/SEDONA/Projects/nitros9-reference/archive/utils/winfo/winfo.doc) documents earlier version fragility.
- **CONTROL**: EOU `/dd/SOURCECODE/C/CONTROL/control.c`, especially `Palette`, `Font`, `FColor`, `BColor` call sites.
- **GS**: EOU `/dd/SOURCECODE/ASM/GSHELL/gshell_ver1_0_1.asm`, `TNDYSLCT`, `SELCNTRL`, `EXCOPOPR`, `ICONAIF`, `EXECAIF`, and `ID.*` AIF fields.

Previous observations identify resident CoWin edition 2, VTIO edition 4, GrfDrv edition 14 and SCF edition 18. They do **not** prove exact source/binary equality. The GFX2 and GShell observations in the runtime index are disk-module observations. Upstream-only services cannot automatically be assumed present in EOU.

## What can be queried and changed

For application-level `I$GetStt`, A supplies the open path and B the SS function; the internal CoWin dispatcher has a different entry convention. Do not copy its internal register convention into an application wrapper.

| Capability | Verified mechanism and evidence | Boundary / application policy |
|---|---|---|
| Screen type | `I$GetStt SS.ScTyp`: returned A; CW `L0AD5`, G2 `L043F` | CoWin returns 1 for 40-column hardware text, 2 for 80-column hardware text; graphics converts its internal type by adding 4. Treat unsupported/VDG services separately, not as another known CoWin mode. |
| Current working dimensions | `I$GetStt SS.ScSiz`: X columns, Y rows; CW `L0A9A`, G2 WINFO | Explicitly the current **CWArea**, not original DWSet extent or pixel resolution. A graphics window also has character-cell dimensions. |
| Foreground/background/border | `I$GetStt SS.FBRgs`: A foreground, B background, X border; CW `L0AF4` | Fore/background are masked palette-register indices. Border comes from the screen table. Do not label these as independent RGB values. |
| Palette | `I$GetStt SS.Palet`: caller X points to a 16-byte buffer; CW `L0AA7` uses `F$Move` | Screen palette is shared by windows on that screen. `SS.DfPal` concerns default palettes, not a private application palette. |
| Set foreground/background | G2 Color `L0589` emits ESC `$32` / ESC `$33` via `I$Write`; CONTROL uses `FColor`/`BColor` | Validate indices for mode; these affect drawing state, not a guaranteed recoloring of all existing content. |
| Set border | G2 Border `L05C1`, ESC `$34` | Screen-scoped effect. Exclude from automatic rollback across unrelated windows unless ownership is known. |
| Set palette | G2 Palette and CONTROL `Palette(PATH,p,palval)` | Affects other content using the same registers; not an initial safe per-window preference. |
| Set working area | G2 CWArea, ESC `$25`, four byte parameters for position/size | Does not independently resize a physical screen. Geometry changes can invalidate the target application's layout. |
| Define window/screen | G2 DWSet, ESC `$20`; upstream [wcreate.asm](/Volumes/SEDONA/Projects/nitros9-reference/level2/cmds/wcreate.asm) | Creation has ownership, screen-type and descriptor constraints. Use the exact mode-specific packet from the source, not a guessed fixed argument count. |
| Select display | G2 Select, ESC `$21` | Selects the target display/window; it does not transfer another process's standard input or move a running shell. |
| End window | G2 DWEnd, ESC `$24` | Not equivalent to killing its owner or closing all references. An arbitrary shell's window must not be destroyed by a profile utility. |
| Overlay lifecycle | G2 OWSet / OWEnd, ESC `$22` / `$23` | Useful for application-owned dialogs; not global window enumeration. |
| Set font | G2 Font `L0520`, ESC `$3A`, group and buffer identifiers; CONTROL uses `Font(PATH,GRP_FONT,FNT_S8X8)` | Font resource must exist. The group/buffer identity is not a self-contained portable font file. |
| Query font and extended state | WI reads system/driver/window structures using `F$CpyMem`, returns `WI$FntGr`, `WI$FntBf` and other fields | No public font query was found in the inspected CW GetStt dispatcher. Private WInfo offsets require exact target verification; report “unknown” rather than fabricate a default. |
| Multi-Vue frame/menu | `I$SetStt SS.WnSet`, `SS.SBar`, `SS.UMBar`; `I$GetStt SS.MnSel`; G2 named handlers | These are UI facilities, not replacements for DWSet/CWArea or a global manager API. |
| Mouse | G2 GetMouse/SetMouse paths use `SS.Mouse` | Use the matching structure/defs; do not assume a generic host mouse event layout. |
| More screen geometry | CW `SS.ScInf` returns mapped-screen block/offset and current working rectangle information | Source supports this, but availability/ABI on the frozen EOU target needs verification. Not required by Milestone 1; physical memory mapping is unnecessary for color profiles. |

G2 WINFO (`L043F`) is a particularly small example: it combines three GetStats to return type, columns, rows, foreground, background and border. It is **not** the same routine as the separate WInfo module, whose larger result includes private data.

## Enumerating useful windows

A device descriptor, attached device, active window, selected display and process using a path are different things. No stable public “enumerate every live window with its owners” operation was established in the inspected API.

WI is the strongest existing inspection example: it accepts a named device, uses `I$Attach`/`I$Detach`, copies system structures and distinguishes not-in-device-table, not-VTIO and not-activated states. `windowinfo.b09` demonstrates these outcomes and extended state decoding. This is evidence for a **target-specific inspector**, not a portable management API. Attach can initialize a device; blindly opening `/w` is especially unsuitable for enumeration because CW `SS.Open` / `L0B4B` allocates a free window and forms its numbered name.

Proposed progression:

1. Start with an already-open path inherited by the utility (or explicitly supplied by a cooperating launcher). Query that one target without discovering/creating others.
2. Add an explicit list of candidate descriptors, distinguishing configured names from verified live windows. Never assume every `/w1` through `/w15` exists or is idle.
3. Only if global enumeration is needed, isolate a version-checked WInfo adapter, verify its structure against the installed modules, and retain “unknown/inaccessible” results. Do not embed private offsets throughout the UI.

## Creation, closure and restoration

Upstream [wcreate.hp](/Volumes/SEDONA/Projects/nitros9-reference/level2/sys/wcreate.hp) documents `wcreate /wX [-s=type] xpos ypos xsiz ysiz fcol bcol [<bord>]`; `wcreate.asm` shows `I$Attach`, `I$Open`, packet construction and cleanup. This is a concrete creation example, not proof that every invocation on an existing occupied window is safe.

The proposed application should keep an ownership ledger: paths it opened, windows/screens it created, original queryable settings and active profile. Close owned paths with `I$Close`; end only owned windows after their users have exited. Preserve the controlling terminal and provide cancellation/error cleanup. Do not force a foreign process's window into a new mode, destroy its overlays or infer ownership from its `/wN` name.

A settings snapshot is not a screen-content or process snapshot. Public queries do not establish a complete inverse of every GFX2 operation: font, original DWSet area, palette sharing, overlay stack and application-specific redraw state need separate treatment. Restore only captured, supported fields; do not describe a partial profile as a full restore.

## Profile model — proposal, not an existing API

A named, versioned application-owned file can contain a target role, expected mode, cell dimensions as constraints, foreground/background and optional explicit font resource requirements. Use ordinary RBF file operations, following the file patterns indexed in [C development](../source-index/C_DEVELOPMENT.md) and MVKit's document callbacks. Profile persistence would write user files during a later implementation; no profile was written to EOU in this task.

For one window: query → validate capabilities and expected mode → capture original supported fields → apply → query back → expose revert. Initial format should distinguish **observed**, **requested** and **unknown** values. Reject incompatible profiles rather than silently converting screens.

For multiple windows: describe roles and creation order, not persistent numeric `/wN` identities. Allocate owned resources, resolve roles to actual paths, then launch cooperating clients. Partial failure needs reverse-order cleanup. This is a later session-launcher feature; no OS-wide atomic window transaction was found. Shared palette/border conflicts need explicit resolution.

## MVKit and GShell integration

[MVKit reconstruction](../reference-projects/MVKIT_RECONSTRUCTION.md) identifies a substantial historical snapshot at mvdraw commit `47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2`. It offers event/menu dispatch, small views, dirty state and document callbacks. Use those ideas for profile forms and dialogs. Keep capability probing, ownership, raw window queries and profile application in an independent window-service layer. This allows a different visual style and avoids encoding private kernel knowledge in widgets.

Recovery is not build completion: `mv_document_close.c` is missing, dependencies are not fully pinned, views do not automatically route a hierarchy, and generic terminal restoration is not demonstrated. [mvedit/mvdraw](../reference-projects/MVEDIT_MVDRAW.md) provide concrete application patterns, not a certified reusable SDK.

GS provides verified AIF/icon launch: `ICONAIF` → `EXECAIF`; fields include module, parameters, memory size, type, dimensions and colors. Use an application icon/AIF or shell launch first. The inspected `TNDYSLCT` menu dispatch is fixed: `SELCNTRL` launches the named `control` program through `EXCOPOPR`. A configurable arbitrary Utilities-menu registration mechanism was **not** found. Do not replace `control` or promise a menu entry without a separate GShell-specific change and verification.

## Recommended Milestone 1

**One-path inspector and color profile utility**, initially text-driven or a small independent UI:

1. Operate on its own inherited/cooperatively supplied active window path. Show path, screen type, current cell dimensions, foreground/background/border and query errors; show unsupported font/original-geometry fields explicitly as unknown.
2. Allow foreground/background changes with mode validation and an immediately available revert. Keep border, palettes, fonts and dimensions informational initially.
3. Save/load one named profile containing only those editable properties and expected mode. Validate the entire file before applying it.
4. Query back the two colors; on normal/error exit restore the launch terminal according to an explicit “keep changes” choice. Do not promise repaint of historical content.
5. Test invalid paths, unsupported queries, malformed profiles, mode mismatch, partial write failure and cancellation on disposable development media. Verify no unrelated window/palette changes.

This satisfies useful inspection and persistent safe customization while leaving global enumeration, foreign-window destruction and multi-window sessions to later milestones. A live API probe is the first prerequisite, especially public-query behavior on hardware text versus graphics windows.
