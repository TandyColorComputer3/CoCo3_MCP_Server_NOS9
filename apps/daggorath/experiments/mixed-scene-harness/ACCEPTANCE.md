# Stage 3E.13 validation checklist

Status at preparation: **BUILD VERIFIED; OBSERVER STATIC-CHECKED; LIVE RUN NOT
STARTED; MIXED-SCENE ACCEPTANCE NOT CLAIMED.**

## Preparation gates

- [x] Current repository baseline verified as `11ca734a9fbc91385f5bec7a6db550983cb87c31`.
- [x] Exact accepted `dodhbp` and `dhbpack` hashes verified before harness use.
- [x] Harness uses only existing broker/presenter declarations and fixed-frame client.
- [x] Two isolated clean builds produce byte-identical modules.
- [x] ToolShed validates module CRC and embedded executable identity.
- [x] Link map reports two program DAT blocks and one data DAT block (7,397-byte data request; 1,024-byte added stack reserve).
- [x] Static observer classifier/API checks pass.
- [x] Private config generator emits and verifies `coco3h.cfg` RGB value 1.
- [x] Host scripts pass syntax checks.
- [ ] Disposable boot/D1/VHD/state inputs supplied and identified.
- [ ] `msharness`, `dodhbp`, `dhbpack` staged and extracted byte-identically on disposable D1.
- [ ] Actual MAME `-version`, `coco3h`, HD6309E, 2 MB, and RGB config verified immediately before launch.
- [ ] Observer readiness, tap, frame callback, bridge connection verified live.

## Required live gates (not run)

1. Harmless private EOU boot/bridge control.
2. Workload startup and exact module identities.
3. Positive graphics lifecycle entry before raster classification.
4. At rates 46, 45, and 41, bounded deterministic status presentations with one outstanding client request.
5. Per native generation: PB1 timestamp, phase, first complete matching raster, partial/obsolete/blank classifications, and edge-to-raster deadline classification. Preserve strict 33.34 ms; indeterminate is not pass.
6. Status pixels outside x=304–335/y=156–162 are correct; no status operation writes into the heart rectangle.
7. Requests and validated ACKs exactly account for HELLO/start, rate, enable, snapshot, scene, stats, and shutdown operations; no pending work or error.
8. Observe normal workload cleanup, graphics departure, broker retirement, and Shell+ return independently. Do not infer one from another.
9. Record event-buffer overflow, callback errors, DAT/stack/resource state where supported.

The harness is a new test baseline. It does not retroactively validate or
reproduce Stage 3E.10T, does not establish cartridge fidelity, and does not
prove production gameplay acceptance.
