# Stage 3E.16D.25F.5 — private type-7 raster observation

Classification: **PASS — TYPE-7 PUTBLK RASTER ACCEPTED**. This is content and three-frame stability acceptance only; no DHeartbeat deadline measurement occurred.

## Identity and provenance

- Repository main at f824f43f350d60521e1c6e0daec59e54a511f8a3. Pre-existing status unchanged; no repository writes, commit, or push.
- D.25E, D.25F.3, and D.25F.4 manifests: 118/118, 62/62, 16/16 verified.
- ppown25c: 7557 bytes, ToolShed CRC 007195 Good, SHA-256 5e8f9bfb2f84002490ce0c1edc5aa08e653814390f174f278b5ddf26e95a682e.
- ppclient25c: 7672 bytes, ToolShed CRC 6DF5D0 Good, SHA-256 b3e59eeaa0af91eee5d592693c79b91e39e49b01ac5ec5cee9438746ea0c249d.
- Baseline D.25F.3 observer SHA-256 f9ded0bcc8df21dd91daf9062a57b8e5340b5fcc43a4f376b92e721e833314f1; unqualified D.25F.4 candidate 12a39308121dcb2920e7c82391c2fd66e4a12ac03eff764259b644744efb5d96; D.25F.5 observer e7209733f15a0d798bf7b89b04069ccb61cc4ef5c2c93e93316500e28e2aaa91. Exact candidate-to-final diff: evidence/OBSERVER_DIFF.patch.
- Extracted guest executables match host input SHA-256 values byte for byte. Type-7 file attributes and reentrant module CRCs verified.

## Observer qualification and bounds

- Actual observer Lua initialized in MAME. All startup assertions passed (readiness marker 39); frame notifier installed, bridge JSON arrays qualified, physical RAM item 2,097,152 bytes.
- Fixture profile retains independent markers. Presenter profile requires 640×239, 611,840-byte 32-bit snapshots, calibrated (X,Y+22) crop and exact 256 pixels. It does not require fixture markers.
- Synthetic delayed control placed the first exact match at active callback 304, then captured callbacks 305 and 306. Wrong bands/colors/shift/geometry/length, interrupted stability, and capped history were rejected or handled.
- Sampling: first active callback, then each eighth active callback; max 512 snapshots with the last coarse sample at active callback 4088; after first match, next two successive callbacks. The executed source comment rounds this span to 4096; the exact last sample is callback 4088. In this run only 59 acquisitions occurred. Retained raw payload is one 611,840-byte initial snapshot plus a bounded number of 1024-byte crops; metadata and the event ledger are capped separately. The 3-second later label is provisional and does not claim the guest F$Sleep returned.
- D.25F.3 overflow arose from recording each of 1765 early active callbacks into a cap of 1800. This observer records summaries and essential transitions; omitted diagnostic records 0, overflow false.

## Private MAME and guest result

- MAME 0.289 ample-289-u2, coco3h/HD6309E, 2 MB, private RGB screen_config=1, private boot floppy/VHD/ready state/D1. Launch metadata records exact paths and argv. D1 mounted on flop2. Post-mount Shell+ lease, echo challenge, and release passed. pwd/date, both load commands, and resident ident commands all returned 000.
- One invalid `os9_run` preflight call requested 140000 ms; the API rejected it before command submission (elapsed 0, phase preflight). The accepted 120000 ms call executed ppown25c exactly once, completed with status 000, display departure, console return, and fresh Shell+ prompt.
- Positive graphics interval: emulated 194.74928442321 to 257.53011726765 seconds, 3762 active callbacks. Activation marker at Y32 matched all 256 expected pixels in the first full snapshot.
- 59 snapshot attempts and successful acquisitions: 56 pre-presentation rejections (blank destination), 3 exact matches. First active destination crop at sequence 7399/time 194.74928442321 blank; first provisional later crop at sequence 7582/time 197.80321648343 blank; final pre-match crop at sequence 7838/time 202.07538373706 blank. First exact match at sequence 7846/time 202.20888896374; stable matches at sequences 7847/time 202.22557711707 and 7848/time 202.24226527041. These are successive completed-frame callbacks. Each of the three raw 1024-byte crops independently matches all 256 expected pixels: RGB 000000, 555555, AAAAAA, FFFFFF in four left-to-right eight-pixel bands across eight rows. Each crop SHA-256 is a312dd3f7965c39c62bc737d0abf9e9020484bd7ad3597504bf940c2141f41a4.
- Parent PID 3, child PID 4. Parent reports type 7, activation PutBlk success, presentation PutBlk success, both ACK sends success, F$Wait PID 4/status 0, graphics and pipe close success, parent exit 0. Child logs both 16-byte ACK reads and validation, shutdown ACK, and return intent 0. Shell+ `os9_run` reports status 000. See extracted byte-exact guest logs and hashes in SHA256SUMS.
- No observer exception, lost required evidence, guest timeout, or graphics mismatch. MAME stopped and private host exited; canonical media never mounted as writable targets.

## Limits and next action

`video:snapshot_pixels()` is rendered MAME output, not proof of a physical host viewer scanout. This test includes no native heartbeat, PB1 edge, audio, gameplay, or 33.34 ms timing result. The first later sample is a provisional time label, not a measured post-sleep instruction boundary. The separate source and guest logs establish the request/ACK lifecycle.

Recommended next action: preserve this qualified private observer and its manifest-verified raster/lifecycle evidence as a reviewed Daggorath experiment checkpoint.
