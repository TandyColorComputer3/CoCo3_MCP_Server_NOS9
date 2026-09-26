# Window Manager — Milestones 1–2

`wmview` is a native NitrOS-9 Level II window inspector, color-profile utility
and reversible color demonstration. It uses public path APIs; it does not enumerate windows or
read private driver structures.

## Build and test

Requires CMOC with OS-9 support, LWTOOLS, ToolShed `os9`, Python 3, and a host C
compiler for lifecycle tests. Tested tool versions and API provenance are in
[the milestone report](../../docs/apps/WINDOW_MANAGER_M1.md).

```sh
python3 apps/window-manager/test.py
python3 apps/window-manager/build.py --out /private/tmp/wmview-build \
  --cmoc /usr/local/bin/cmoc --lwasm /usr/local/bin/lwasm \
  --lwlink /usr/local/bin/lwlink \
  --os9 /Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9
```

The output directory contains the module, exact commands, tool/source hashes,
ToolShed identification, intermediates and map. Generated artifacts belong outside
this source directory. Stage using the existing fresh artifact floppy workflow;
cold boot with that image attached. Do not install into the canonical VHD or
restore a state associated with different media bytes.

At the verified shell, load the module from the artifact floppy, then run:

```text
load /d1/wmview
wmview
wmview demo
```

The default command only inspects stdout path 1. `demo` temporarily swaps drawing
foreground/background and changes the border, then restores and queries all three.
Use only on an idle, foreground hardware-text window you control. The border is
screen-scoped; there is no lock against another process changing shared settings.
Old printed text is not repainted. Abort/interrupt signals request cleanup.

Diagnostic modes `fault` and `cancel` deliberately trigger OS-9 error 201 and a
self-directed signal 3 after changing colors. They return those nonzero statuses
**after** verified restoration. They are useful for testing, not everyday operation.

## Layers

- `src/window.h`: window information and color-value representation.
- `src/window.c`: public queries, validated test colors, setting and restoration.
- `src/ui.c`: text presentation.
- `src/main.c`: lifecycle, explicit modes and single cleanup path.
- `src/os9.c`, `platform.h`: narrow syscall/signal boundary.
- `src/module.asm`: explicit module name, edition and revision.
- `src/profile.c`, `persistence.c`, `fileio.c`: profile model and exclusive file I/O.
- `src/m2.c`: profile commands and commit/rollback lifecycle.
- `src/font-catalog.c`, `font-data.h`: frozen read-only font reference.
- `test/lifecycle.c`, `test/profiles.c`: host mock-boundary lifecycle/profile tests.

No font selection, window creation, raw hardware access or MVKit dependency
are included. Signal 0, emulator termination, a broken device and concurrent writers
cannot be given an unconditional restoration guarantee.

## Milestone 2 profiles

See [the M2 report](../../docs/apps/WINDOW_MANAGER_M2.md) for the versioned file
format, lifecycle and live evidence. `wmview help` lists commands. On a disposable
writable RBF floppy, for example:

```text
wmview capture entry /d1/entry
wmview create night 1 0 /d1/night
wmview show /d1/night
wmview preview /d1/night
wmview apply /d1/night /d1/undo
wmview revert /d1/undo
wmview caps
wmview fonts
```

Apply first creates a NEW undo file; it refuses existing destinations. Successful
apply/revert keeps verified foreground/background values. Errors and handled
cancellation roll back entry values. Preview always restores. Profiles never change
border, palette, font, geometry or cursor options. The font catalog is a frozen
file-based reference, not enumeration of currently loaded buffers.

Do not restore an older emulator state after writing profiles to the session disk.
Use a cold boot or a complete byte-matched media/checkpoint set. Do not direct these
examples at a canonical development VHD.
