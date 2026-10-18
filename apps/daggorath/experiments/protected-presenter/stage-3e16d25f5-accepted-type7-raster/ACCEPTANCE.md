# Accepted boundaries

## D.25F.5 current run

- Exact `ppown25c` and `ppclient25c` modules were identified by SHA-256 and ToolShed CRC before execution and extracted byte-identically afterward. See `artifacts/` and `evidence/current-run/guest-logs/` in `PROVENANCE.json`.
- MAME 0.289 ample-289-u2, coco3h/HD6309E, 2 MB RAM and generated RGB `screen_config=1` were verified before the private run (`evidence/current-run/launch.json`). Only disposable media were mounted.
- The post-mount Shell+ lease was requalified. `pwd`, `date`, both loads, and both resident `ident -ms` commands completed with status 000 (`evidence/current-run/host-results.jsonl`; `screen-01.png`).
- The presenter was executed once. Its current-run parent PID was 3 and child PID 4. `ppparent25c.log` records type 7, successful activation and presentation PutBlk, both ACK sends, F$Wait returning child PID 4/status 0, graphics/IPC close success, and parent exit 0. `ppclient25c.log` records two 16-byte ACK reads and validations, shutdown acknowledgment, and client return intent 0. See the extracted files under `evidence/current-run/guest-logs/`.
- The original F$Fork B=4 request, startup signal, PipeMan framing, two transactions, and Select/DWEnd/Close cleanup are retained in `source/` and the exact binary. No guest change was made for the observer.
- Shell+ returned to a fresh nonce prompt and `os9_run` status 000, with positive graphics departure (`evidence/current-run/host-results.jsonl`, `observer-events.jsonl`, `screen-02.png`). F$Wait confirms collection of the matching child; no independent system-wide orphan inventory was made.

D.23's archived type-5 lifecycle is historical baseline evidence. It does not substitute for the current D.25F.5 result above.

## Raster classification

**PASS — TYPE-7 PUTBLK RASTER ACCEPTED** for rendered content and three-frame stability. See [RASTER_VALIDATION.md](RASTER_VALIDATION.md). No strict heartbeat or viewer scanout timing result is included.
