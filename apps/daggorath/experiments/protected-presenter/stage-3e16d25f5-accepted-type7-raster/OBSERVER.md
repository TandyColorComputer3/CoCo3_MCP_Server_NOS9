# Observer identity, qualification, and limits

The exact Lua used for D.25F.5 is `observer/accepted/observer.lua`, SHA-256 `e7209733f15a0d798bf7b89b04069ccb61cc4ef5c2c93e93316500e28e2aaa91`. `observer/OBSERVER_DIFF.patch` is its exact diff from the unqualified D.25F.4 candidate. The D.25F.3 baseline and D.25F.4 candidate are preserved separately under `observer/` so the correction sequence is inspectable. `observer/fixture-controls/` holds D.25E's Python transliteration checks; those are independent calibration controls, **not** tests of the final Lua. The final Lua has its own startup assertions, executed in MAME before the presenter command. The bridge readiness field `classifierSelfTests=39` is a completion marker returned by that function, not a measured count of independent assertions.

## Correction sequence

1. The frame notifier handle remains globally retained for the session (D.25D lifetime correction).
2. `mark_array` forces actual JSON arrays for numeric palette/band fields; the live `type7-array-probe.json` verifies serialization.
3. Fixture profile requires independent calibration markers. Presenter profile does not borrow those fixture-only markers.
4. D.25E fixture evidence calibrates snapshot X'=X, Y'=Y+22, 640×239, 611,840 host-endian pixel bytes. The presenter uses that independently verified transform.
5. Exact 32×8, 256-pixel comparisons enforce RGB band order 000000, 555555, AAAAAA, FFFFFF. Four distinct colors alone are insufficient.
6. The first active callback is sampled, then every eighth active callback. No host timer declares the guest's `os_sleep(180)` finished.
7. On an exact candidate match, the next two successive callback frames are captured. A mismatch resets the stability sequence.
8. Snapshot attempts cap at 512; the last coarse sample would be active callback 4088. The executed source comment rounds that to 4096; 4088 is the exact schedule. Event history separately caps at 256 and retains essential first/matching/stable records and counters. The run had 59 attempts, 0 omitted records, and no overflow.
9. The first full raw snapshot (611,840 bytes), early/later/final crops, and three matching raw crops are archived. Each crop is 1024 bytes; `raster-summary.json` gives independent SHA-256 and pixel comparisons.
10. Rejection reasons are counted; all 56 pre-presentation rejections were `band_color_or_uniformity_mismatch` on blank destination crops.

The first later sample is labeled *provisional*: it was taken after 3.0 emulated seconds, without an instruction-level guest sleep-return observation. Coarse sampling may miss a short transient between sampled frames. The three-frame criterion applies only after an exact candidate is observed. `video:snapshot_pixels()` is rendered MAME output, not a direct host-viewer scanout probe.

`host/private-host.mjs` and `host/requalify.mjs` are exact private host helpers from the accepted run. `host/used-private-config.json` is evidence of the resolved private input paths, not a portable configuration: its `/private/tmp` paths and the host helper's guarded run root must be adapted and revalidated for a new private run. No ROM, media, or saved state is stored here. The host imports the repository MCP runtime; it is not a standalone emulator distribution. See `evidence/current-run/launch.json`, `type7-array-probe.json`, `ready-state-staging.json`, and `host-results.jsonl` for the actual preflight and execution record.
