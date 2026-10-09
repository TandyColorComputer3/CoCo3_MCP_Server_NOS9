# Accepted live results

| Experiment | Native | PB1 | Observed | Requests | Maximum delta | Errors |
| --- | ---: | ---: | ---: | ---: | ---: | --- |
| ARM / `S$Wake` | 220 | 220 | 220 | n/a | 1 | none |
| ARM / `S$Wake` / KICK | 220 | 220 | 220 | 26/26 claimed once | 1 | none |

For ARM/KICK, heartbeat notifications were 218 attempted and 218 successful.
KICK notifications were 24 attempted and 24 successful; two KICKs found the
receiver active and required no signal. There were no misses, duplicates,
phase errors, collisions, request errors, failed signals, or `E$USigP`.

The PipeMan experiment is excluded from this table because it did not pass its
first 81-tick gate. See the repository README for the exact limitation.
