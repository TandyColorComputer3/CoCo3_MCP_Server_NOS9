# mvedit and mvdraw: surviving application patterns

[Navigation](README.md) · [MVKit history](MVKIT_RECONSTRUCTION.md)

Snapshots: mvedit `9fd15c476aed9cdc67d2d628309470e67951f174`; mvdraw `e3994af467556ca00d8ebe18de546a28e949f5b0`. These are source observations, not live EOU application tests.

## Shared build and skeleton

[Makefile](/Volumes/SEDONA/Projects/mvedit-reference/Makefile) and [Makefile](/Volumes/SEDONA/Projects/mvdraw-reference/Makefile) select CMOC/OS9, cmoc_os9 commit `14b8f6bc983a1c694d36e3890f34b16c06a2af20`, source lists and a supplied 6809 Level II v3.3.0 base disk. They include `mvkit/app.mk` with `-include`, so missing build rules may not fail immediately at the include. Both bootstrap MVKit from an **unpinned** xmastree clone; mvedit checks whether `mvkit/` exists, whereas mvdraw's recipe clones/moves it each time the target runs. These recipes modify files and were not executed.

Historical `app.mk` (linked in the MVKit report) compiles with `--os9`, cmoc_os9 headers and `--add-os9-stack-space=2048`, links `-lmvkit -lc -lcgfx`, generates launcher AIF with CR line endings, converts icons/images, copies a base disk into `build/`, then writes files/attributes using ToolShed. Its run target uses host MAME `coco3` with an FDC option and `dos` autoboot. That is an example build/run recipe, **not our canonical EOU configuration**.

| Aspect | mvedit | mvdraw |
|---|---|---|
| Application wiring | [mvedit.c](/Volumes/SEDONA/Projects/mvedit-reference/mvedit.c) | [mvdraw.c](/Volumes/SEDONA/Projects/mvdraw-reference/mvdraw.c) |
| Model | [textdoc.h](/Volumes/SEDONA/Projects/mvedit-reference/textdoc.h), [textdoc.c](/Volumes/SEDONA/Projects/mvedit-reference/textdoc.c): lines, insert/delete/load/save | [drawing.h](/Volumes/SEDONA/Projects/mvdraw-reference/drawing.h), [drawing.c](/Volumes/SEDONA/Projects/mvdraw-reference/drawing.c): shapes, order, serialization and undo records |
| Content view | [text_view.h](/Volumes/SEDONA/Projects/mvedit-reference/text_view.h), [text_view.c](/Volumes/SEDONA/Projects/mvedit-reference/text_view.c): cursor/selection/clipboard/scroll | [draw_view.h](/Volumes/SEDONA/Projects/mvdraw-reference/draw_view.h), [draw_view.c](/Volumes/SEDONA/Projects/mvdraw-reference/draw_view.c): hit tests, selection/resize/move, preview |
| App entry | `mv_app_run_with_scrollbars` | `mv_app_run` |
| Window declaration | WT_FSWIN, minimum 80×25 cells; 78×23 content, bottom row used for status | Framed canvas with tool/pattern/color/logic grids |
| Document ownership | App implements file/dirty/optional snapshot lifecycle; view has will_change/did_change notifications | MVDocument wraps Drawing; MVUndoItem actions for add/remove/move/restore |
| Data allocation | MEM_SIZE=120 pages in AIF | MEM_SIZE=96 pages in AIF |

Both Makefiles label screen type 5 “640×192” while declaring 25-character-row windows. Do not turn these comments into a guarantee of physical screen height: current upstream GrfDrv contains 192/200 mode selections. Future code must query and verify the actual EOU screen dimensions. The apps' framed working areas cannot be assumed to satisfy the wizard's full 640×200 target.

## Menus, events, mouse and scrolling

The common pattern is theme/model/resource setup → window setup → menu enable-state refresh → menu/content/key dispatch. Applications explicitly offer clicks to views; MVView does not route a tree of views automatically.

**mvedit:** `unhandled_menu` routes MN_USCRL/D.../L.../R... to `text_view_scroll`; `update_scrollbars` manages thumb values. The view converts mouse position to document cells, extends selections during drag and auto-scrolls at boundaries. `pure_scroll_repaint`/`vscroll_repaint` use cgfx delete/insert-line operations for short vertical shifts; horizontal/large changes repaint. Keyboard batching suppresses repeated drawing. The app's `_gs_rdy != -1` check is tied to its cmoc_os9 wrapper convention, not a universal numeric SS.Ready contract.

**mvdraw:** app dispatches tool grids before the canvas. `clip_to_canvas`/`clip_reset` bracket drawing with `_cgfx_cwarea`; cell-based working-area coordinates differ from pixel drawing/mouse coordinates. `stroke_geometry` calls `_cgfx_setdptr`, box/bar/circle/ellipse/line; logic/pattern/color are explicit. Drag loops read `_cgfx_gs_mouse`, erase/redraw XOR rubber bands and `Flush`, then repaint on release. These loops are examples to profile for cooperative behavior, not proof of bounded CPU use.

## Source beats stale feature summaries

Current mvedit `mvedit.c` has **ENABLE_UNDO=0**; current `text_view.c` maps left to `$08` and delete/backspace to ESC/BREAK `$05`, and the application changes interrupt/abort option bytes accordingly. Its README still advertises undo and a different left/erase explanation. Consult the actual revision and call sites. Restoration of changed terminal options must be explicitly handled in a reusable shell-launched application.

MVdraw README also gives conflicting shortcut summaries; `handle_key_event` is the concrete implementation to follow. Both applications depend on absent build-time framework/library material; screenshots/readmes are not proof the current checkout builds unchanged.

## Patterns worth adapting

- Keep model serialization separate from input/rendering, with explicit dirty/undo notifications.
- Use one coordinate conversion boundary and restore clipping state before menus or mouse reads. This directly informs Daggorath's logical viewport, although its y=4 offset is not an 8-pixel cell origin.
- Prefer localized repaint/batched output and measured redraw cost over unbounded whole-screen repaint on every input event.
- Make code/data/stack/resource budgets visible to builds; installed 2 MB machine RAM does not remove per-process address-space constraints. Verify actual allocation requirements against the selected OS/CPU/module evidence.
- Reuse lifecycle/commands independently of palette/chrome styling; document which cgfx and compiler ABI each adaptation assumes.

Knowledge pointers: `/dd/SOURCECODE/C/CONTROL`, `C/COCOTHELLO`, `ASM/GSHELL`, `BASIC09/GFX5`, `BASIC09/GUIB30` through [graphics index](../source-index/GRAPHICS_WINDOWING.md); compiler-wrapper cautions in [C development](../source-index/C_DEVELOPMENT.md). Current upstream [level2/coco3/modules/cowin.asm](/Volumes/SEDONA/Projects/nitros9-reference/level2/coco3/modules/cowin.asm) and [level2/cmds/grfdrv.asm](/Volumes/SEDONA/Projects/nitros9-reference/level2/cmds/grfdrv.asm) explain service implementations, but the frozen EOU CoWin/GrfDrv fingerprints govern compatibility. No framework or application was reconstructed, built or installed.
