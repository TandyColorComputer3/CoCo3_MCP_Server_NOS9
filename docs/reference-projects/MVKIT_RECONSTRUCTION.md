# MVKit reconstruction evidence — no reconstruction performed

[Navigation](README.md) · [Application patterns](MVEDIT_MVDRAW.md)

## Recovery result

**A substantial MVKit source snapshot survives in mvdraw Git history.** Current mvdraw and mvedit HEADs have no tracked `mvkit/` tree. Neither has a tracked `.gitmodules` or gitlink entry. Both current Makefiles instead fetch `jamieleecho/xmastree` and move its `mvkit/` subdirectory into the application tree at build time. This is build-time vendoring, not a pinned submodule or package dependency. The availability of that remote was not assumed or tested.

| Evidence | Finding |
|---|---|
| `git log --all -- mvkit` in mvdraw | `03496b9` initial checkin; `47b116f` working-functions changes; `c8f82da` removes the vendored tree |
| Last snapshot before removal | **`47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2`** |
| Removal commit | `c8f82dafc6f9aec465676d97d0357577fa6e2c93` |
| Historical tree | 97 tracked files: 39 `.c` (including tutorials), 16 `.h`, Makefile/app.mk, Doxygen configuration, README/USING, seven tutorial directories and assets |
| Framework declaration | [`mvkit/include/mvkit/mv_version.h`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/mvkit/include/mvkit/mv_version.h): 0.1.0 |
| mvedit reachable history | No tracked MVKit paths found |
| Historical licensing evidence | Root [`LICENSE`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/LICENSE) is MIT, copyright 2026 Jamie Cho; preserve it with any later reuse |

The checkouts are non-shallow, but this result covers **locally reachable Git history**, not deleted/unfetched remote refs. No checkout, extraction into the project, or MVKit implementation was made. Read individual historical files with `git show 47b116f:mvkit/<path>`.

## Recovery is not the same as build completeness

[`mvkit/Makefile`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/mvkit/Makefile) lists `src/mv_document_close.o`; [`mvkit/include/mvkit/mv_document.h`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/mvkit/include/mvkit/mv_document.h) declares `mv_document_close`, and current mvdraw calls it. **`mvkit/src/mv_document_close.c` is absent from the recovered tree**, and no path history for that file was found in the available refs. This is a concrete missing implementation, not merely a missing dependency installation. Do not claim that recovering the snapshot alone yields a buildable library.

The seven guides, headers and other document routines preserve enough contract evidence to investigate this gap later. Any newly written replacement would be a new implementation, not recovered original source. cmoc_os9 and asset converters are external dependencies; no test proves this snapshot works against the frozen EOU modules or newer CMOC.

## Recovered API and behavior

| Layer | Exact historical source | Observed contract and limits |
|---|---|---|
| App lifecycle | [`mvkit/include/mvkit/mv_app.h`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/mvkit/include/mvkit/mv_app.h), [`mvkit/src/mv_app_run.c`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/mvkit/src/mv_app_run.c) | `mv_app_run_typed`; `mv_app_run`/`mv_app_run_with_scrollbars` macros choose WT_FWIN/WT_FSWIN; pre_init, init, menu refresh, menu actions and application-event callbacks. Loop runs until process exit. |
| Events | [`mvkit/include/mvkit/mv_event.h`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/mvkit/include/mvkit/mv_event.h), `mv_app_run.c` | KeyPress/MouseClick payloads; intercept handler records signal; loop arms mouse signal 10 and key signal 11, sleeps, reads a key or mouse record. Menus/control region dispatch separately from content. This is a particular implementation, not a universal signal allocation policy. |
| Menus/windows | [`mvkit/include/mvkit/mv_menu.h`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/mvkit/include/mvkit/mv_menu.h) | Macro DSL creates cgfx menu/window descriptors; `mv_set_menus_sized` supports explicit minimum dimensions; action table terminates with negative menu id catch-all. Default MN_CLOS exits. |
| Views | [`mvkit/include/mvkit/mv_view.h`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/mvkit/include/mvkit/mv_view.h), [`mvkit/src/mv_view.c`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/mvkit/src/mv_view.c) | MVView holds rectangle, visibility and draw/click function pointers. Concrete view embeds it first. **No view hierarchy or automatic event routing**; app dispatches in explicit order. |
| Document | [`mvkit/include/mvkit/mv_document.h`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/mvkit/include/mvkit/mv_document.h) and `mv_document_*.c` | Opaque model, new/open/save callbacks, filename/extension, file-backed state, dirty tracking and undo manager. Missing close implementation noted above. |
| Undo | [`mvkit/include/mvkit/mv_undo_manager.h`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/mvkit/include/mvkit/mv_undo_manager.h) and split `mv_undo_manager_*.c` | Callback/context records, save marker and undo bookkeeping. It is not automatically a model snapshot or unlimited undo storage. |
| Images/toolbars | [`mvkit/src/mv_image.c`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/mvkit/src/mv_image.c), [`mvkit/src/mv_image_grid.c`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/mvkit/src/mv_image_grid.c) | Resource image loading plus selectable MVView-based image grids. The app controls tools and callback behavior. |
| Dialogs | [`mvkit/src/mv_file_dialog.c`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/mvkit/src/mv_file_dialog.c), [`mvkit/src/mv_app_message_box.c`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/mvkit/src/mv_app_message_box.c) | Modal file/message dialogs; concrete cgfx/window assumptions remain. |
| Theme | [`mvkit/include/mvkit/mv_theme.h`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/mvkit/include/mvkit/mv_theme.h), [`mvkit/src/mv_theme_chrome_ordered.c`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/mvkit/src/mv_theme_chrome_ordered.c) | Palette/chrome choices are separate from much of the document/event plumbing, but code is still coupled to cgfx calls. |
| Build | [`mvkit/app.mk`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/mvkit/app.mk), [`mvkit/Makefile`](https://github.com/jamieleecho/mvdraw/blob/47b116fc8af27cf83f8cc1e3aa7f85c3fbe288b2/mvkit/Makefile) | CMOC OS9 compilation, lwar library, libc/libcgfx linkage, PNG conversion, AIF generation and ToolShed packaging. |

The framework initializes echo/cursor/text/scaling/mouse options and calls `_cgfx_ss_wnset`. Its event loop exits through `exit(0)` for default close; the inspected code does **not** constitute a general save/restore-terminal cleanup abstraction. A future reusable runtime must audit errors, signal handling and teardown rather than copy the loop blindly.

## What can be recovered versus inferred

- **Directly recoverable:** the 97 tracked files, their exact historical bytes and authorship context; public structs/signatures, event dispatcher, theme/view/document/undo code, build tooling and tutorials.
- **Inferable from surviving apps:** how to supply model/view callbacks, implement scroll handlers, render custom canvases, handle file prompts and budget memory. Current mvedit provides evolved text/input optimizations beyond the historical framework.
- **Not established:** a complete last version of the deleted standalone project, missing close implementation, external cmoc_os9 internals, full EOU compatibility, and exact build reproducibility. No framework binary was built or matched to a running guest.

## Useful concepts independent of visual style

Retain as candidates: lifecycle callbacks; typed events; explicit view bounds/dispatch; model/view separation; command/menu enable state; document save/dirty/undo bookkeeping; resource naming and packaging. These can support a different visual renderer or a game-like full-screen application.

Keep replaceable: palette/chrome drawing, fixed cell sizing, framed-window defaults, modal dialog appearance, image-grid presentation and cgfx-specific rendering calls. “Theme” alone does not remove these dependencies. The evidence supports separating application plumbing from appearance; it does not establish that MVKit already has a backend-independent renderer. Wizard Milestone 1 does not need MVKit to create its OS-9 graphics lifecycle.

Cross-reference [graphics/windowing](../source-index/GRAPHICS_WINDOWING.md), [SCF/VTIO](../source-index/SCF_VTIO.md), [signals](../source-index/PROCESSES_SIGNALS.md), [C ABI](../source-index/C_DEVELOPMENT.md) and [upstream/EOU differences](../source-index/MODERNIZATION_NOTES.md). cgfx wrappers and BASIC09's GFX2 module are distinct interfaces to related graphics/window services; no `RUN GFX2` dependency was found in these C apps.
