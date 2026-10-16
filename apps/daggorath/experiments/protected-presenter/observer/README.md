# Original observers missing

The display ledger, heartbeat/raster observer, Select tracer, and readiness
controls for this experiment were not recovered. Prior lessons include:

- GIME register 0 alone does not establish graphics ownership; inspect mode
  registers 8/9 and completed video frames.
- For physical RAM, validate offsets against the 2 MB RAM save-item length and
  read through `ram:read_block`; debugger `b@` reads are not physical RAM.
- `emu.item()` takes exactly one item-name argument in this installed MAME.
- Audio block arrays need the correct serialization markers.
- Frame callbacks must remain registered and their execution verified.

These are recovery notes, not a working observer. An older observer from the
mixed-scene harness is not a calibrated replacement.
