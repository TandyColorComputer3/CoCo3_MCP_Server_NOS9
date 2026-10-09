# Single-client CoWin heartbeat POC acceptance

This private experiment removes the second PipeMan reply reader. One persistent
priority-128 `kickchild` sends and validates 26 gameplay ACKs, waits until native
generation 220, then sends and validates the shutdown ACK on the same endpoints.
The priority-192 broker remains the sole reply writer and graphics owner.

The accepted clean run reached 221 native/PB1 generations because one edge landed
before atomic freeze. The broker completed the corresponding heart PutBlk before
advancing shown generation, ending at 221/221 with phase 1. All 27 requests were
claimed and ACKed exactly once. External Level II inspection found only PID 4 as
the reply reader and PID 3 as the reply writer; no PID 7 existed.

`evidence/validation.json` is the compact result. The CSV files are the curated
frame interval for the clean run. This remains a private POC; it does not change
production C6 or accepted checkpoints.

Run `./rebuild.sh` from this directory to rebuild from the complete preserved
source tree two levels above. `SHA256SUMS` records the accepted source and
artifact identities. The broker's live DAT image was
`0010:333E:333E:333E:333E:333E:0018:0007`, representing one program block, one
mapped CoWin buffer block, and one process-data block (3/8 occupied blocks).

The maximum edge-to-observed-raster latency was one completed source frame
(about 16.7 ms). This measured workload does not establish a universal
real-time guarantee under arbitrary Level II workloads. Production C6 gameplay
and cartridge choreography remain unaccepted.
