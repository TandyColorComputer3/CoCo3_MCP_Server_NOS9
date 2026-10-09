# Accepted PipeMan live validation

| Measurement | Accepted result |
| --- | ---: |
| Native generations | 221 |
| PB1 transitions | 221 |
| Foreground observations | 221 |
| Gameplay-style requests | 26 |
| Shutdown requests | 1 |
| Requests / ACKs | 27 / 27 |
| Maximum generation delta | 1 |
| Missed / duplicate / phase errors | 0 / 0 / 0 |
| Failed KICK signals / `E$USigP` | 0 / 0 |
| Independent maximum edge-to-observation latency | 2 ticks |

The exact 81-tick request-free gate passed. The first KICK occurred 84 VIRQ
ticks after enable: three ticks elapsed before the initial `$95` drain and 81
ticks were then accumulated. All 220 ordinary observations completed in one
source tick; the final shutdown generation completed in two.

The retained `final-counters.csv` and `pipe-81-gate.csv` are the compact raw
acceptance records. The driver's internal one-byte maximum-latency diagnostic
is not authoritative across `$95` counter drains because the drained counter
can wrap relative to an earlier edge timestamp. The independent rolling VIRQ
observer supplied the accepted 1-2 tick measurements.

Shutdown completed with heartbeat disabled, `waitArmed=0`,
`requestPending=0`, owner and active state cleared, children reaped, PipeMan
paths closed, and Shell+ restored.
