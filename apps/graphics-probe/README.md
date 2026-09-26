# Graphics execution lifecycle probe

`gfxprobe` is a disposable native OS-9 program for testing `os9_run` graphics
policy. It is not an application or a Daggorath implementation.

It opens its own `/w`, defines type 5 with 80×25 cells, disables coordinate scaling,
sets black/white on that new screen, draws a 640×200 outline and centered 256×192
rectangle/diagonals, selects the window for approximately 600 ticks, then selects
stdout Term, ends and closes only its window. Public query checks require type 5
and 80×25; the live pixel extent is verified separately.

- `gfxprobe`: normal completion, status 000.
- `gfxprobe cancel`: sends itself signal 3 after 180 polls; intercepted flag leads
  through the same cleanup, status 003. This exercises the signal path without
  concurrent MCP keyboard injection.
- `gfxprobe error`: deliberate error after 180 polls; same cleanup, status 187.

No files are written. No font, palette or dimensions on Term are changed. Signal 2
and 3 handlers only set a flag; signal 0/forced termination cannot guarantee cleanup.
The caller must provide the verified Term as stdout.

Build (from repository root):

```sh
python3 apps/graphics-probe/build.py --out /private/tmp/graphics-execution/build \
  --cmoc /usr/local/bin/cmoc --lwasm /usr/local/bin/lwasm --lwlink /usr/local/bin/lwlink \
  --os9 /Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9
python3 apps/graphics-probe/test.py
```

The recipe derives from `apps/window-manager/build.py`; OS-9 wrappers retain the
Window Manager's verified CMOC ABI, signal handling and F$Sleep convention. Public
window packets follow upstream `level2/coco3/modules/cowin.asm` L0027 and
`level2/cmds/grfdrv.asm` L086A.25 at commit
`f470fa52eb172b59b22c1b722074998cb42de9b1`. The NitrOS-9 Project's
[windowing manual](https://sourceforge.net/p/nitros9/wiki/The_NitrOS-9_Windowing_System/)
defines the public operations. The older manual's 192-line table alone does not
establish EOU's 200-line mode; the identified implementation and live outline do.

Use only the [disposable staging workflow](../../docs/architecture/NITROS9_ARTIFACT_STAGING.md).
See [execution evidence](../../docs/architecture/NITROS9_GRAPHICS_EXECUTION.md).
