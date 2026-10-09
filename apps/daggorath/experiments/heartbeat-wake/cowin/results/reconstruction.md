# Reconstruction record

- Source base: committed accepted `pipeman` experiment at
  `c429482ec33d002944213a89ceb7242013bee478`.
- CoWin changes: replayed from the recorded private session in their original
  order, including heart/status GP buffers, persistent mapped scene buffer,
  scene request protocol, and memory-only boundary diagnostics.
- Toolchain: CMOC 0.1.90; lwtools/lwasm 4.22; ToolShed 2.2.
- Historical direct-file comparison: unavailable because the private files
  were deleted with `/private/tmp`.
- Hash comparison: reconstructed `wcowin-map3` and `kickchild` match the
  historical build record exactly. The deleted historical file was unavailable
  for a direct byte comparison.
- Two clean baseline builds, two clean heartbeat-only builds, and two clean
  return-diagnostic builds were byte-identical within each build mode.
- Baseline heartbeat gate: 220 native generations, 220 PB1 transitions, 220
  completed heart PutBlks, and 220 independently observed raster transitions;
  zero misses, duplicates, phase errors, or `$BC`; maximum observation delay
  one completed frame (about 16.7 ms).
- First scene request: group 196/buffer 4, 512-byte mapping at `$C4A0`, PutBlk
  packet `1B 2D C4 04 00 40 00 0C` (destination 64,12). `I$Write` returned
  `$BD` / 189 / `E$ICoord`.
- Return diagnostic: marker B2 in `graphics_scene`, followed by caller marker
  C1 and returned result `$BD`; caller marker C2 was never written. The caller
  therefore entered its normal error return before heartbeat reconciliation or
  ACK construction.
- Return context: Y=`$0000`, U=`$1E52`, S=`$1E4E`, caller return PC=`$E641`,
  saved caller U=`$1EA5`, `__stbot=$1DF0`, `__mtop=$0350`, CC=`$81`.
- Active process DAT image before and after the scene call was unchanged:
  `07 3E 3E 3E 3E 3E 18 10`.
- Source comparison: the known-working production mapped-buffer path performs
  one GetBlk to establish GP geometry metadata before `SS.MpGPB`. The
  reconstructed POC performed DefGPB then `SS.MpGPB` without that one-time
  initialization. Adding the corresponding one-time 512x8 GetBlk before map is
  the smallest proposed experiment correction; it is deliberately unimplemented.

Runtime status: **RECONSTRUCTED — NOT YET ACCEPTED**.
