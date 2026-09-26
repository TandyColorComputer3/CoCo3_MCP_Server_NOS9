# NitrOS-9 development artifact staging

2026-09-26. This milestone implements **host-side preparation of a fresh artifact floppy**, using the existing MCP launch/mount/readiness/run tools. It does not install into `/dd`, implement the Window Manager, or introduce a guest transfer daemon.

## Decision and safety boundary

Use a **new disposable OS-9 floppy for each build**, attach it as `flop2` before booting OS-9, cold-boot disposable copies of the canonical EOU boot floppy/VHD, and load the program from `/d1`. No existing guest destination is replaced. Canonical media never enter the experimental emulator session.

The new command is `npm --prefix MCP run stage -- ...` (after building), backed by [os9-stage.ts](../../MCP/src/os9-stage.ts) and [os9-stage-cli.ts](../../MCP/src/os9-stage-cli.ts). It returns JSON and prepares media only. The name `stage` deliberately does not mean “installed into the running OS.” A public MCP installation tool would imply lifecycle guarantees that this small host preparer does not provide.

The hardware configuration remains the frozen `coco3h`, 2M, RGB, MPI empty/SSC/empty/SCII+RTC configuration. No boot configuration was changed. MAME 0.289 `-listmedia` with those slots verified separate `flop1`, `flop2`, `hard1`, `hard2` devices. Live `load /d1/stgprobe` established the second floppy's guest mapping for this EOU target; do not generalize that mapping to unrelated boot configurations.

## Evidence and filesystem model

Start with [RBF evidence](../source-index/RBF.md), [module evidence](../source-index/MODULES.md), [live boot](NITROS9_LIVE_BOOT.md) and [os9_run](NITROS9_RUN.md). The manuals directory and DOCS_INDEX named by AGENTS.md are absent in this checkout; the following implementation sources were inspected instead:

- `/dd/SOURCECODE/ASM/NITROS9/RBF/rbf_postbeta6.asm`: `PD.BUF` sector buffers, `PD.SMF` buffer flags, `RdFlDscr` returning an already buffered descriptor, `L1237` flushing buffered sectors, and path-descriptor segment fields at `L10FD`.
- Official upstream `/Volumes/SEDONA/Projects/nitros9-reference/defs/os9.d`: `M$ID`, `M$Size`, `M$Name`, `M$Type`, `M$Revs`, `M$Exec`, `M$Mem`, `I$WritLn`, `F$Exit`. Commit `f470fa52eb172b59b22c1b722074998cb42de9b1`.
- ToolShed 2.2 `os9/os9ident.c`, `ident_os9`: module metadata/CRC reporting. A zero process exit status alone does not prove the CRC was good; the preparer explicitly requires `(Good)` for the expected CRC.

**A MAME state and backing files are separate objects.** Restoring RAM/device state restores the guest's old in-memory filesystem assumptions, not the old VHD bytes. Stopping MAME before a host copy prevents concurrent writers but does not make a *subsequent old-state restore* consistent. An idle shell is not a certificate that every RBF/device cache is empty. Module-name caches are another reason not to reuse a running session for a changed binary of the same name.

### Compared workflows

| Method | Experiment / evidence | Decision |
|---|---|---|
| A. Stop → host ToolShed copy → cold boot | Added `/stage-note` only to a disposable VHD clone while MAME was stopped. Cold boot read the note successfully. The separately staged native module also loaded and ran. | Supported when media are exclusive to the session and no old state is restored. Use private clones rather than repeatedly changing the canonical development VHD. |
| B. Stop → host copy → restore an older state | Deliberately tested on that clone: restored the pre-copy `staging_ready` state, then `list /dd/stage-note` returned status `000` and showed the newly added note. Clone hash did not change during this read-only experiment. | **Not supported.** This limited success is not a cache-coherency proof. Open files, dirty buffers, allocation maps and other metadata were not exhaustively tested; no corruption was claimed or deliberately induced. Cold-boot afterward. |
| C. Guest-side transfer while running | Guest RBF operations avoid a simultaneous external filesystem writer. A future receiver, or guest copy from a prepared disk to a new path, could use that property. Existing bridge tools do not implement a checked OS-9 file receiver. | Plausible future route, not live-certified here. Needs exclusive-create semantics, bounded transfer/checksum, close/error handling and checkpoint invalidation after writes. Old-state restore still does not undo persistent guest writes. |
| D. Separate disposable image | Fresh ToolShed-formatted 40-track double-sided floppy, one verified program, mounted as `flop2` before DOS. Read-only host permissions applied. `load /d1/stgprobe`, then `stgprobe`, succeeded, including a same-media restore cycle. | **Chosen minimum loop.** Does not write canonical `/dd` at all. Cold boot for each new staging image; do not hot-swap a mounted filesystem and reuse old cache state. |
| E. Whole-session disposable media set | Copies of boot floppy and development VHD, separate test-state directory/name, artifact floppy and recorded hashes. No stock VHD attachment. | Used for all live experiments. More disk space, but isolates normal boot activity and controlled filesystem experiments from canonical media. |

No claim is made that B always fails, that C is impossible, or that filesystem buffers are rolled back by MAME. The supported procedure avoids the disputed boundary.

## Known-safe probe and provenance

[stgprobe.asm](../../MCP/examples/staging/stgprobe.asm) is an original minimal test fixture. Its call pattern follows upstream `level1/cmds/echo.asm` (`I$WritLn`, then `F$Exit`), using official `defs/os9.d`. It prints `MCP STAGING OK` plus CR to standard output and exits with zero unless writing fails. It does not access hardware, create files or change windows.

Build from the repository root:

```sh
/usr/local/bin/lwasm --6809 --format=os9 \
  --includedir=/Volumes/SEDONA/Projects/nitros9-reference/defs \
  --define=OS9.D=0 --define=Level=2 --define=H6309=0 \
  --output=/private/tmp/os9-stage-live/stgprobe \
  MCP/examples/staging/stgprobe.asm

/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 \
  ident /private/tmp/os9-stage-live/stgprobe
```

The explicit `OS9.D=0` satisfies the include guard under this assembler invocation. An initial invocation without it failed before generating a module; it was corrected rather than changing the reference definitions.

| Property | Verified value |
|---|---|
| Assembler | `/usr/local/bin/lwasm`, `lwasm from lwtools 4.22` |
| Compiler / linker | Neither used; direct assembler OS-9 module output |
| CPU selection | 6809-compatible instructions, executed on the frozen HD6309 target |
| ToolShed | Native `os9 from Toolshed 2.2` at the path above |
| Module / type / language | `stgprobe`, Prgrm, 6809 Obj; Ty/La `$11` |
| Size / CRC | 59 bytes / `$E0136B`, ToolShed `(Good)` |
| Attributes/revision / edition | `$81` (reentrant, revision 1) / edition 1 |
| Entry / requested data | `$0016` / `$0100` |
| Output SHA-256 | `003beb078a8dfe2bd8d0946307b533372300266a592ff205423766fcc18c77cb` |
| Source SHA-256 | `7216164053b39e04f9cad59092596a68435f22390453fb516cfbb820273f1685` |
| `defs/os9.d` SHA-256 | `7928ae45c697dc06619ea484f72f6d668ba5d3016d00c15928aace4a15dd70f6` |

The build record retains source/include hashes, upstream commit, exact executable/argument vector, cwd, version and output hash. It is separate from module inspection: a supplied provenance record is retained but its claims are not blindly certified by the stager. The live fixture record was generated from actual files/tool output.

## Host support contract

Example after `npm run build`:

```sh
npm --prefix MCP run stage -- \
  --artifact /private/tmp/os9-stage-live/stgprobe \
  --output-root /private/tmp/os9-stage-live/staged \
  --os9 /Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 \
  --provenance /private/tmp/os9-stage-live/build.json
```

For routine use, an ignored output root such as `MCP/work/staging` is appropriate. `node MCP/dist/os9-stage-cli.js ...` gives pure JSON stdout without npm's script banner.

The implementation:

1. Reads a bounded regular input file and accepts exactly one native Prgrm/Objct OS-9 module, with valid size, header parity, execution offset and a restricted module name. Concatenated packs, BASIC09 I-code and arbitrary files are out of scope.
2. Creates an exclusive unpredictable job directory with `mkdtemp`; all destinations inside it have fixed names. There is no existing-image, `/dd` or guest replace parameter.
3. Copies input bytes once into a private immutable-for-this-operation snapshot. Later changes to the original input cannot change the artifact being inspected/copied.
4. Executes ToolShed through argument arrays, without a shell, with a 30-second per-command timeout and bounded output. Paths with spaces work; an output-root comma is rejected because ToolShed uses comma as an image delimiter.
5. Inspects the snapshot CRC, formats a fresh full-size 40×2×18×256 image, copies without `-r`, sets read/execute guest attributes, inspects the installed module and exports it again for byte equality.
6. Records image SHA-256 and sets host mode 0444. This is an extra safeguard; exact MAME/host write-protection behavior must not be generalized across operating systems or privileged users.
7. Publishes `manifest.json` only after verification. Failure leaves `failure.json` and partial outputs in a private job directory, **without a successful manifest**. Do not mount a failed job's image.

The manifest contains `hostArtifact`, module identity, `guestDestination` (image + image-relative path, device unresolved), `replacedExisting: false`, `stagingMethod: "fresh-artifact-floppy"`, tool output and exact ToolShed argument vectors. No complete stdout-capture claim is added to `os9_run`.

No public MCP tool was necessary: the minimum new support is the host preparer plus the test fixture. Existing tools remain unchanged. The preparer never starts/stops MAME, mounts media or validates an arbitrary existing save state's disk relationship. **Those are explicit session steps below**, not implied capabilities of a successful staging result.

## Deterministic supported loop

1. Stop any emulator using the baseline media and confirm its exit. Record canonical hashes. Never mount `media/63SDC.VHD`.
2. Make new copies of `63EMU.DSK` and `63SDC-MCP-DEV.VHD` for this session. Do not copy an actively changing image. Configure `MAME_BOOT_FLOPPY` / `MAME_VHD` to those copies; leave canonical hardware settings intact. Use a separate test state name (here `NITROS9_READY_STATE=staging_ready`) and preferably a separate state directory.
3. Build and inspect; call the host preparer. Require `staged: true`, record the manifest and hashes. Never reuse/host-edit an image already attached to a running or saved guest.
4. `coco_start` → `coco_mount_flop({drive:"flop2",path:<new-image>})` while still at BASIC → `coco_type({text:"DOS"})`. Observe boot through a snapshot until Shell+ is actually idle. Keyboard queue completion alone does not certify boot completion.
5. Create a **new session-specific** state at that idle shell. Allow the save file to settle before loading; record its hash plus every mounted image's path/hash and launch configuration. This checkpoint is made *after* the final media set is attached. Never substitute old `nos9_ready_v2` for this checkpoint after changing media.
6. `os9_restore_ready` on this new paired state establishes the event-based post-load/fresh-shell handshake required by `os9_run`. The tested program performs no disk writes; retain the same image bytes throughout the restore experiment.
7. `os9_run({command:"load /d1/stgprobe"})`, then `os9_run({command:"stgprobe"})`. Both must return completed/status `000` and shellReady. `load` is used because the current run validator accepts a bare program name, not an absolute executable path. Inspect a snapshot for `MCP STAGING OK`; status alone does not prove displayed text.
8. `os9_run({command:"unlink stgprobe"})` releases the preload. Stop MAME. Rehash all session images and canonical images. A mismatch invalidates a checkpoint pairing unless the exact older media set is restored too; the conservative recovery is a cold boot and a new checkpoint.
9. Keep the verified module/manifest as build evidence. Dispose of the whole session media set when finished rather than deleting files in canonical `/dd`. This milestone did not leave a test executable installed in the canonical VHD.

For arbitrary future applications that write files, do not restore an earlier checkpoint against the changed images. Use a cold boot or a complete offline clone/checkpoint pairing. Same bytes are necessary, but are not a general license to mix emulator versions, device topology or concurrent image users.

## Live investigation results

All launches used `/private/tmp/os9-stage-live/63EMU.DSK` and `/private/tmp/os9-stage-live/63SDC-MCP-DEV.VHD`. The initial manual-format probe established geometry/mapping before exercising the implemented preparer. The final run used the preparer's new `stage-j5XgJ9/artifact.dsk` (host mode 0444).

- Initial fresh boot: save `staging_ready`, post-load readiness succeeded; `load /d1/stgprobe`, `stgprobe`, `unlink stgprobe` all returned `000`.
- Controlled B experiment: while stopped, ToolShed copied a unique disposable `stage-note` to the VHD clone root. Restored the pre-copy state; `list /dd/stage-note` returned `000` and displayed `DISPOSABLE HOST STAGING NOTE`. No guest write command was issued. This is limited observation, not an approval of B.
- Final A/D cycle: cold boot with the changed disposable clone and the preparer's new artifact floppy; create a replacement session checkpoint, then test the note, load/run/unlink and a same-media restore/run cycle. Detailed responses and integrity results are below.

ToolShed additionally reported the final artifact floppy's structure intact with one directory, one file and zero allocation-map/file-descriptor inconsistencies. A deliberately corrupted probe was rejected with `staged:false` and `ToolShed did not verify the module CRC`; no successful manifest was published for it.

## Validation and limits

The tests cover real fixture header identity, malformed headers/module packs, distinct destinations on repeated staging, no replacement flags, copied provenance, stable artifact snapshots, failed commands, bad CRC, installed-byte mismatch and ToolShed-delimiter rejection. A new mock setup bug (undefined failure selector matching the no-argument version call) was corrected with explicit user approval; assertions were unchanged.

The implementation uses Node filesystem APIs and `execFile` rather than POSIX shell commands. macOS/native ToolShed/MAME is live-tested here; Windows/Linux live behavior is not claimed. Multi-module bundles, dynamic hot staging, a guest receiver, automatic process installation and Window Manager implementation are deferred.

## Recorded final evidence

- [Build provenance](assets/artifact-staging/build.json): actual source/include/output hashes and assembler command.
- [Stage result](assets/artifact-staging/stage.json): full structured result, host and guest ToolShed ident reports, exact commands and image hash.
- [Launch command](assets/artifact-staging/launch.json): exact executable, argument array and cwd. `flop2` was mounted through MCP afterward, before DOS (verification sequence 22).
- [Checkpoint/media pairing](assets/artifact-staging/checkpoint-pair.json): final state and all mounted image hashes. This is an evidence record, not an added guard inside existing restore tools.
- [Verification results](assets/artifact-staging/verification.json): exact structuredContent for OS-9 tools and decoded JSON text results for other tools, keyed by live sequence number. No image/base64 payloads are embedded.
- [Restored probe screen](assets/artifact-staging/probe-restored.png): fresh marker/status and `MCP STAGING OK` after restoring the final paired checkpoint.

The job-local `staging_ready` filename was reused after the final cold boot; the actual post-load console at sequences 27 and 33 contains the new `01:38:50` Shell+ startup stamp, not the earlier `01:33:50` stamp. Thus the measured final restores used the replacement checkpoint. For routine jobs, use a unique state name as well as a unique media directory; do not rely on `fileFound` alone to distinguish an overwritten asynchronous save from an old file. Canonical `nos9_ready_v2` was not loaded or overwritten.

Final execution response (sequence 35, after the second final-state restore):

```json
{
  "command": "stgprobe",
  "completed": true,
  "commandCompleted": true,
  "status": 0,
  "statusText": "000",
  "timedOut": false,
  "shellReady": true,
  "outcome": "completed",
  "phase": "complete",
  "elapsedMs": 6856,
  "outputComplete": false,
  "marker": "MCPDONE40b771560a83226edfd8382c57e7a45a",
  "commandPromptMs": 1269
}
```

Sequence 33 reports `loadCompleted:true`, `shellVerified:true`, `postLoadEpoch:2` and a fresh handshake. Sequences 29 and 34 (`load /d1/stgprobe`), 30 and 35 (`stgprobe`), and 32 and 37 (`unlink stgprobe`) all return status `000`. Sequence 28 reads the disposable host-added note successfully after cold boot. Sequence 38 stops MAME with `{"ok":true}`. No test executable was copied to the canonical VHD; cleanup consisted of unlinking the probe and stopping the disposable session. Session artifacts remain isolated for inspection and can be discarded as a set.

### Media integrity

SHA-256 values below were measured before and after the investigation. All three canonical files are unchanged:

| Canonical file | Before = after SHA-256 |
|---|---|
| `media/63SDC.VHD` | `db2f0f444073d3b88a3610a63ec9f7d2e2dd4299347181faa6b2b2aa9637de2c` |
| `media/63SDC-MCP-DEV.VHD` | `4c6bdc0cd2f68aa57a9c75f75f15fe429f30926aa522917dd8d14aa1b9ae622e` |
| `media/63EMU.DSK` | `9a51adb8656b9f003c7f822352c291487362f1b4c43aac1a16672d5d2bcb4c40` |

Stock VHD remained mode `0444` and was never attached to MAME. No write operation targeted any canonical media file.

The disposable VHD clone initially matched the development VHD. Only the intentional stopped-host addition of `stage-note` changed it, to `5e9362718476ae36c68210871e35b801a6a4eaf1c69daaa176b02357a86e6147`. That hash remained unchanged through the old-state read experiment, final cold boot and final paired-state run/restore cycle. The disposable boot DSK remained identical to its source. Final artifact image remained `ba306138574d54e25b1568b02b4100af71c88cc0975e2b127d667c3a637f8370` across guest use. State SHA-256 is `d6746d6c3dfc1a59605af65547013567356ebbf6b5c4fb7ec426971ceb4b8917`.

Validation completed: **101 tests passed**, `npm run build` passed, `git diff --check` passed. The successful real ToolShed round trip and corrupted-CRC rejection supplement the unit tests. No commit was made.
