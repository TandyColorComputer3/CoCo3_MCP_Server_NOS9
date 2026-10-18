# D.24: 320×200×16 graphics feasibility

## Finding

The EOU-published BASIC09 preview shows a `wcreate` example using `-s=8` for
320×200 / 16-color operation. The official EOU documentation index identifies
the expanded GFX2 documentation and BASIC09 manual preview ([EOU
documentation](https://lcurtisboyle.com/nitros9/nitros9_docs.html)). However,
the published Windowing System Manual's screen-type table lists code 08 as
320×192 / 16 colors (with 32,000 bytes), while the EOU BASIC09 preview states
320×200. These references disagree on vertical resolution; the table's memory
entry also does not equal 320×192×4bpp. This cannot be resolved from installed
binaries in this read-only task. The manual identifies code 08 as the 16-color
type and documents screen-type-dependent buffers, `PutBlk`, palette registers,
and window operations ([Windowing System manual](https://sourceforge.net/p/nitros9/wiki/The_NitrOS-9_Windowing_System/)).

This is documented capability, not verified behavior of the exact installed
EOU binaries: local manuals are absent, and D.23 exercised type 5 only.
Installed EOU GFX2 Edition 5 documentation/source and a live type-8 control
were not available in this no-build/no-MAME task. Published interface
references identify DWSet `$1B20` with parameters STY, x/y origin, x/y size,
and foreground/background/border palette-register numbers; Palette is
`$1B31`; GPLoad is `$1B2B`; PutBlk is `$1B2D`. GPLoad supplies group/buffer,
source screen type, dimensions, byte length, and packed data. PutBlk supplies
group/buffer and destination x/y, using dimensions retained by the buffer.
Published manual text says the driver converts source-buffer and screen types
and clips at the window boundary; it does not document an interior protected
area mask. These are published descriptions, not verified packets for the
installed edition. Confirm command semantics and resolution against the exact
EOU manual/runtime before implementation.
The published screen type is the correct window-level configuration to
investigate, but the underlying CoCo 3 GIME mode/register state is not
established by these references or by the type-5 D.23 run.

## Geometry and protected heart

The proposed heart rectangle is X=304–335, Y=156–162. On a 320-pixel-wide
native raster, X=320–335 is outside the visible screen; the whole 32-pixel
rectangle cannot be interpreted as a literal native-pixel rectangle. Its Y
range fits within either 192- or 200-row candidate. The D.23 parent ran the
tested type-5 graphics environment; neither that run nor its source coordinates
prove type-8 geometry.

If GFX2 scaling maps a larger logical coordinate space into 320 physical
pixels, the heart's projected X interval may fit, but that mapping and pixel
dimensions are unmeasured. The protected area must first be defined in the
chosen mode's actual coordinate system. This is a blocking geometry question
for D.25, not a reason to silently shift or clip the rectangle.

No evidence shows that CoWin/GrfDrv `PutBlk` automatically excludes an
interior rectangle. Window-edge clipping is not an interior keep-out region.
The defensible future strategy is for the single presenter owner to split any
status/scene transfer that overlaps the protected heart into non-overlapping
rectangles, or use separate source blocks covering exactly the same
non-heart pixels. Before use, prove source format, byte alignment, coordinates,
and complete status coverage in the selected mode. Do not let gameplay or
interrupt code write directly to the framebuffer.

## Pixels, buffers, palette, and selection

Published GFX2 documentation distinguishes graphics-buffer format by screen
type. The Windowing Manual describes the 16-color packed pattern as four bits
per pixel: 32×8 pixels, 16 bytes per row, 128 bytes total. PutBlk uses the
selected graphics buffer's dimensions/format. The manual describes 16
palette registers and a six-bit RGB color value (two bits per component), and
notes palette state is shared by windows on a screen. DWSet selects foreground,
background, and border palette register indices; Palette changes register
values. These are documentation-derived requirements; the exact installed-
driver acceptance of a type-8 buffer and palette was not tested. Whether
CoWin's buffer creation allocates shared/global storage or process-addressable
mapped storage for this type, how much DAT space mapping consumes, and what
unmap/kill sequence is required are not resolved by D.23.

D.23 source provides a proven type-5 path only: parent creates/selects the
window, sets scaling, configures palette, defines a GP buffer, maps it, and
submits PutBlk. It closes by unmapping/killing the buffer, selecting the text
path, sending DWEnd, and closing the window. Do not copy the type-5 buffer
format or dimensions into type 8 without checking the installed GFX2 contract.

## Memory and DAT assessment

- D.23 parent module: 7,282 bytes (less than one 8 KiB block), data request
  53 bytes. Child module: 7,637 bytes, data request 217 bytes. Exact allocation
  rounding, stack high-water, and full process DAT images were not measured by
  the D.23 acceptance run.
- A 320×200×4bpp linear image is 32,000 bytes; a 320×192×4bpp image is 30,720
  bytes. Those are arithmetic image sizes, not proof that CoWin maps a linear
  framebuffer into the process. The old manual table itself lists 32,000 bytes
  for its 320×192 type-8 entry; that discrepancy remains unresolved.
- CoCo 3 physical RAM size does not expand a process's eight DAT slots. A
  conservative hypothetical mapping of one program block, one data/stack
  block, four screen blocks, and four full-size staging-buffer blocks would
  need ten slots and would not fit. This is not an asserted type-8 CoWin
  allocation scheme. Actual screen mapping, GPB mapping, DAT-slot usage, and
  8 KiB boundary behavior must be measured or verified from installed EOU
  source before choosing buffers.
- D.23's child F$Fork B=4 request and one-outstanding-client lifecycle are
  accepted constraints for the next control. Do not assume the type-5 parent
  DAT accounting proves type-8 feasibility. Keep staging minimal and map only
  the required buffer extent if the installed interface permits it.
- Stack high-water remains unmeasured. The next POC must record module/data
  allocation and DAT use, and must not treat module data request as stack
  measurement.

## D.25 proposal (one bounded POC)

Create one disposable parent variant from the preserved D.23 parent and keep
`ppclient3` byte-identical. Preserve B=4, startup signal, PipeMan frame format,
both ACK exchanges, F$Wait, and cleanup. Change only parent graphics setup to
the installed-EOU-verified 320×200×16 mode (or revise geometry if the installed
edition confirms 320×192), after confirming its exact window type/opcode, pixel
coordinates, buffer format, and palette contract. Have the parent submit one
small known-pattern PutBlk in an area proven outside the coordinate-correct
protected-heart rectangle. Observe the completed raster. Then run the same
shutdown, wait, graphics close, text restore, and Shell+ status checks as D.23.
No heartbeat/gameplay workload.

Do not start the POC until the coordinate mismatch, type-8 buffer mapping,
palette setup, and DAT footprint are resolved. Lifecycle/IPC topology need
not change; parent graphics setup does.
