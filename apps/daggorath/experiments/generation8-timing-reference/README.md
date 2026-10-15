# Generation-8 timing reference

This directory preserves the Stage 3E.13C.4–3E.13D.4 **private diagnostic** evidence. It does not accept Variant B or change the strict 33.34 ms PB1-edge-to-complete-raster deadline. No production correction was tested.

## Independent raster results

The original `observer-events.jsonl` files in `evidence/stage-3e13c4/` contain the PB1 edge and first complete matching published frame for each session. Subtracting their emulated timestamps gives:

| Session | Private run directory | Generation-8 latency | Strict result |
| --- | --- | ---: | --- |
| 1 | `dag-mixed-scene-JtWs1B` | 34.185 ms | FAIL |
| 2 | `dag-mixed-scene-6shQTL` | 34.184 ms | FAIL |
| 3 | `dag-mixed-scene-ZklyY9` | 34.188 ms | FAIL |

Generation 7 was approximately 17.5 ms. All three generation-8 observations exceed 33.34 ms. The diagnostic breakpoint run in Stage 3E.13D.4 also observed a 34.187 ms generation-8 raster, but breakpoint-instrumented timing is **diagnostic only** and is not a fourth acceptance measurement.

## Broker calibration and timing

The Stage 3E.13D.4 tracer verified broker PID 4, module `dodhbp` at logical `$C000`, and the current generation-6 saved DAT image `[26, 62, 62, 62, 62, 22, 13, 24]`. Code occupied logical DAT slot 6, physical bank 13; its logical `$C000` base translated to physical `$1A000`. The changed data mappings since initialization were expected and did not invalidate the executable mapping. The tracer checked all ten breakpoint instructions against the exact linked broker artifact before arming. Examples: `service_heart` entry `$C9A1`/`$1A9A1` = `34 40`; its clock call `$C9B5`/`$1A9B5` = `17 12 70`; `F$CpyMem` at `$DC4A`/`$1BC4A` = `10 3F 1B`; snapshot call `$C9D5`/`$1A9D5` = `17 FF 9D`. The remaining six checks and their raw trace are in `diagnostics/stage-3e13d4/`.

| Boundary after native PB1 edge | Generation 7 | Generation 8 |
| --- | ---: | ---: |
| Broker current | +1.358 ms | Already current |
| `service_heart()` entry | +1.898 ms | +10.438 ms |
| `F$CpyMem` inside that service call | 1.693 ms | 1.685 ms |
| `os_clock()` inside that service call | 1.899 ms | 1.891 ms |
| First complete matching raster | +17.494 ms | +34.187 ms |

The first measured extra delay is **before the next `service_heart()` entry**. At the generation-8 edge, the broker was current; no `D.Proc` process switch was observed before service entry. Two other `os_clock()`/`F$CpyMem` calls occurred during that interval. Their call sites and the remaining broker-current work were not identified. Similar service-local clock and copy durations do not explain the extra delay.

## Findings and limits

- Variant B fails the unchanged strict deadline, and the generation-8 delay is reproducible.
- The exact internal cause remains unknown. Broker scheduling, status admission, and PipeMan have **not** been established as causes.
- The tracer records process-pointer writes but does not prove uninterrupted CPU execution between breakpoints. Execution breakpoints can perturb guest timing.
- No production correction was tested. Clean guest shutdown was not established in the bounded diagnostic run.
- The Stage 3E.13D.2 and D.3 private attempts stopped at calibration/observation boundaries; their source and result states are preserved separately. Do not use those failed attempts as timing acceptance evidence.
- All selected original evidence files were copied byte-for-byte. The original Stage 3E.13C.4 session-3 host workload result reports a bridge disconnect; the complete raster event log is preserved independently. No missing result was reconstructed.

See `artifact-manifest.json` for exact source paths and identities, `SHA256SUMS` for archive integrity, [diagnostics/calibration.md](diagnostics/calibration.md) for every instruction check, and [reproduction/README.md](reproduction/README.md) for the limited replay procedure.
