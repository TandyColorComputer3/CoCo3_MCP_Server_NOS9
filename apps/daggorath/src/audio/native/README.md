# Native heartbeat device service

Candidate production driver for the verified resident CoCo 3 EOU target.
See `docs/apps/DAGGORATH_AUDIO_M3.md` for current live acceptance status.

The driver owns its static data and a one-video-tick VIRQ. The game owns an open
`/dhb` path through `native_heartbeat.h`; `dodaudio` continues to own SSC transient
effects independently. No application receives a kernel pointer or touches PB1.

## Private ABI, version 1

| Operation | SetStat | Registers / behavior |
|---|---|---|
| Claim | `$90` | Exclusive system-path ownership; disabled, countdown 1, rate byte 0, output low |
| Release | `$91` | Disable, remove VIRQ then IRQ, restore acquired PB1/DDR bit |
| Rate | `$92` | X must be 0..255; update only rate, preserve countdown and enable |
| Enable/resume | `$93` | Enable without resetting countdown or phase |
| Disable/freeze | `$94` | Freeze countdown and hold output phase |
| Query | GetStat `$90` | A active; X high=rate, low=countdown; Y high=enabled, low=fault |

Rate/countdown byte zero represents 256 decrements. Acquisition's initial 1
comes from initial cleared HEARTC plus INIVUX's increment. Acquisition does not
pretend that later disable/resume is a fresh game initialization. Unknown
operations return E$UnkSvc, duplicate/foreign claims E$DevBsy, invalid rate or
unsupported PIA configuration 187. Query does not disclose driver addresses.

All private control SetStats require the owning system path after claim. Kernel final path close
and device Term supply cleanup for process death. Explicit release stops the
service even if duplicated references remain. **I$Dup retains references.** Upstream `level1/modules/kernel/ffork.asm`,
Level-II branch, copies only paths 0..2. An ordinary device path above stderr is
not automatically inherited by F$Fork. Do not duplicate `/dhb` onto stdio; if a
reference is deliberately inherited, forced parent death does not imply
last-reference teardown. Helpers are preloaded before activation to avoid
active floppy loading, independently of path inheritance. No undocumented lease or
process-liveness scan runs in interrupt context.

The callback acknowledges its VIRQ, freezes if disabled, decrements the byte,
and on zero toggles only `$FF22` bit 1 and reloads the current rate. A bounded
saved-CC bracket protects the state/PIA operation. There are no OS calls in the
callback. Unsupported PIA control-bank/interrupt configuration freezes it with
fault 187 rather than writing a DDR register as port data.

Normal shutdown restores only the saved PB1 and its DDR bit, merging current
other bits. It does not restore a stale video byte or switch the audio mux.
Disable is **not** shutdown: the source holds its level when disabled. A failed
kernel deregistration keeps Term/storage alive and sleeps/retries in process
context; IOMan ignores a Term error return before releasing storage, so returning
with a live callback is forbidden.

The timing guarantee excludes active rb1773 HALT floppy I/O. This must remain a
full-game integration gate. No system driver changes are supplied here.

Build with `python3 apps/daggorath/build_heartbeat.py --out OUTPUT`; `dhbpack`
contains driver and descriptor together for the verified EOU loading workflow.
Use a fresh disposable artifact disk and cold boot after modifying that disk.
Do not install this by editing the canonical VHD or restoring stale disk caches.
