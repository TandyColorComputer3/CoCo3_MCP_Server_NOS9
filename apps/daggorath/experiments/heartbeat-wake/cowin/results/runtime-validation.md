# Runtime validation

Status: **RECONSTRUCTED — NOT YET ACCEPTED**.

## Heartbeat-only gate

The heartbeat-only derivative preserves the exact broker/driver code and moves
the first scene sender signal until after generation 220. One continuous live
run measured:

- 220 native generations;
- 220 PB1 transitions;
- 220 completed heart PutBlks;
- 220 independently observed raster transitions;
- exact alternating phase hashes `82AAA899` and `CCB5095D`;
- zero missed, duplicated, or phase-error generations;
- zero `$BC`;
- completion visible in the same or next completed video frame (maximum about
  16.7 ms, below two 60 Hz ticks).

This proves the reconstructed heartbeat-only path. It does not accept the
mapped-scene path.

## First mapped scene

The broker defined group 196/buffer 4 with 512 bytes, mapped it at `$C4A0`,
prepared the mapped payload, and submitted PutBlk packet:

`1B 2D C4 04 00 40 00 0C`

The raw `I$Write` result was `$BD` (189). Upstream `defs/os9.d`, Level II
windowing errors beginning at ORG 183, identifies `$BD` as `E$ICoord`.
Diagnostic byte 24 and the caller's captured return byte 62 both contain
`$BD`.

The linked control flow proves:

1. marker B2 executes inside `graphics_scene()` after `os_write()` returns;
2. the function returns `$BD` through B;
3. caller marker C1 executes and records `$BD`;
4. the caller's `if (e) return e` branch is taken;
5. marker C2, post-scene heartbeat snapshot, ACK construction, and ACK write
   are not reached.

There is no stack or relocation failure at this boundary: Y is `$0000`, the
captured stack is above `__stbot`, the return PC matches the linked caller
continuation, and the eight-entry DAT image is unchanged.

The working production strip setup uses DefGPB, one GetBlk to initialize the
buffer's graphics metadata, then `SS.MpGPB`. This POC omitted the GetBlk step.
The smallest proposed correction is the equivalent one-time 512x8 GetBlk
before mapping buffer 196/4. It is not implemented in this checkpoint.
