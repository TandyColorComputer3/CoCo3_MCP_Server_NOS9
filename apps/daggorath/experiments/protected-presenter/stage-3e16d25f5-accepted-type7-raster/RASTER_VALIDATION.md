# Current-run raster validation

The D.25E independent fixture established a rendered 640×239, 611,840-byte snapshot and X'=X, Y'=Y+22 mapping (`evidence/independent-calibration/raster-verification.json`). Thus the presenter's logical X=64–95, Y=64–71 crop is snapshot X=64–95, Y=86–93. The source writes eight copies of the packed row `00 00 55 55 AA AA FF FF`, and palette values `[0,7,56,63]` decode to four eight-pixel grayscale bands per row.

| Event | Frame callback | Emulated time (s) | Raw crop result |
| --- | ---: | ---: | --- |
| First graphics-active | 7399 | 194.74928442321 | Destination blank; activation pattern separately present at Y32 |
| First provisional later | 7582 | 197.80321648343 | Destination blank; not proof of sleep return |
| Final mismatch before match | 7838 | 202.07538373706 | Destination blank |
| First exact match | 7846 | 202.20888896374 | 256/256 pixels correct |
| Second successive frame | 7847 | 202.22557711707 | 256/256 pixels correct |
| Third successive frame | 7848 | 202.24226527041 | 256/256 pixels correct |

The three accepted crops are three distinct frame-callback samples. Each archived 1024-byte raw crop has SHA-256 `a312dd3f7965c39c62bc737d0abf9e9020484bd7ad3597504bf940c2141f41a4`. Each row has exactly eight pixels at each RGB value, in order: `000000`, `555555`, `AAAAAA`, `FFFFFF`. The first full raw snapshot and its activation crop are archived for independent reinspection. `evidence/current-run/raster-summary.json` contains dimensions, timestamps, per-sample checksums, band classifications and pixel counts; `host-results.jsonl` retains all 59 attempt timestamps; `observer-events.jsonl` retains the bounded transition ledger. The active graphics interval was emulated 194.74928442321–257.53011726765 s.

All 56 rejected snapshots preceded the first exact match; none establishes a bad post-PutBlk raster. The observer made 59 acquisitions, recorded no callback error, and retained required evidence without event overflow. Its source checked full pixel-buffer length before cropping, and the archived first full frame independently confirms the activation pattern. Type-7 mode is also recorded as `SCREEN_TYPE=07` in the parent log.

This is rendered content/stability evidence. It does not measure PB1-to-raster latency, physical host-viewer publication, or whether a scene/status draw can overwrite the heartbeat rectangle.
