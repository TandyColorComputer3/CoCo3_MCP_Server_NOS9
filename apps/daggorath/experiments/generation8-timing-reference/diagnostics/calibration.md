# Generation-6 broker instruction calibration

The Stage 3E.13D.4 private tracer read broker PID 4's saved process DAT image at generation 6: `[26, 62, 62, 62, 62, 22, 13, 24]`. The module base was logical `$C000`. Its code slot 6 mapped physical bank 13 (`$1A000` base). The tracer bounded every physical read to the observed 2 MB RAM item and compared the following bytes against the exact `dodhbp` module with SHA-256 `d6e092562d7b2a69b55df428ae7acd03f5d8a8550576906c192ca7b08bf5a086` before installing breakpoints.

| Marker | Logical PC | Physical RAM | Linked instruction bytes |
| --- | ---: | ---: | --- |
| `service_in` | `$C9A1` | `$1A9A1` | `34 40` |
| `clock_call` | `$C9B5` | `$1A9B5` | `17 12 70` |
| `clock_in` | `$DC28` | `$1BC28` | `34 40` |
| `copy_call` | `$DC4A` | `$1BC4A` | `10 3F 1B` |
| `copy_return` | `$DC4D` | `$1BC4D` | `25 01` |
| `clock_return` | `$C9B8` | `$1A9B8` | `32 64` |
| `snapshot_call` | `$C9D5` | `$1A9D5` | `17 FF 9D` |
| `snapshot_wrapper` | `$C975` | `$1A975` | `34 40` |
| `native_snapshot_in` | `$D56F` | `$1B56F` | `34 40` |
| `native_snapshot_return` | `$C98F` | `$1A98F` | `32 64` |

The table records the successful breakpoint-installation gate and linked bytes. It does not imply that every marker was hit in both generations. The exact captured hit sequence, register values, `D.Proc` write events, and emulated timestamps are in `stage-3e13d4/state-final.json`.
