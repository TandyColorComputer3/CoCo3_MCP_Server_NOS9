# Accepted live results

| Experiment | Native | PB1 | Observed | Requests | Maximum delta | Errors |
| --- | ---: | ---: | ---: | ---: | ---: | --- |
| ARM / `S$Wake` | 220 | 220 | 220 | n/a | 1 | none |
| ARM / `S$Wake` / KICK | 220 | 220 | 220 | 26/26 claimed once | 1 | none |
| PipeMan transport | 221 | 221 | 221 | 27/27 ACKed once | 1 | none |

For ARM/KICK, heartbeat notifications were 218 attempted and 218 successful.
KICK notifications were 24 attempted and 24 successful; two KICKs found the
receiver active and required no signal. There were no misses, duplicates,
phase errors, collisions, request errors, failed signals, or `E$USigP`.

The accepted PipeMan result includes the exact 81-tick request-free gate,
fragmented request/ACK transfers, and atomic freeze/final reconciliation.
It remains a wake/transport result; CoWin presentation and production C6 are
not accepted by this checkpoint.
