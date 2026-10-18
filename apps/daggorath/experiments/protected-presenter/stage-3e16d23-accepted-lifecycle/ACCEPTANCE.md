# Stage 3E.16D.23 acceptance record

## Exact modules

| Module | Size | Data request | Entry | ToolShed CRC | SHA-256 |
|---|---:|---:|---:|---|---|
| `ppown20` | 7,282 | 53 | `$000D` | `FC2F47 Good` | `896a47b794a8b5c4e50dc9b21af50ac1b57924f8cd2870094ddd2ab8caee97de` |
| `ppclient3` | 7,637 | 217 | `$000D` | `E9C525 Good` | `eba7a09379b15ef4a3c09bd07e48107d90720ddd3e97b736acb57363685701e7` |

The exact ToolShed report is `evidence/module-ident.txt`; the recorded source
and build identities are in `evidence/artifact-source-provenance.json` and
`provenance.json`. The D.23 run verified that host binaries and extracted
staged guest modules were byte-identical.

## Accepted lifecycle topology

1. Parent owns the CoWin graphics path and all visible PutBlk operations.
2. Parent forks one client. Child memory request is **B=4**.
3. Child sends PipeMan-framed presentation request; parent presents it and
   sends the 16-byte ACK. Child validates that ACK.
4. Child sends shutdown request; parent sends the 16-byte shutdown ACK.
5. Child persists `CLIENT_EXIT` and exits zero. Parent collects it with
   `F$Wait`; returned PID matches the forked PID and child status is zero.
6. Parent closes graphics/IPC resources, restores the text display using the
   tested Select/DWEnd/Close sequence, and returns to Shell+ with status 000.

This was demonstrated in the tested **type-5 graphics environment**. The
parent log contains PRE_WAIT, POST_WAIT, WAIT_VALIDATED, CLEANUP_ENTER,
GRAPHICS_CLOSE_RESULT, both pipe-close results, PARENT_LOG_CLOSE,
CLEANUP_COMPLETE, and PARENT_RETURN_INTENT checkpoints. The aggregate cleanup
checkpoint was zero. These records prove the reported guest checkpoints; they
are not instruction-level observation of each internal close operation.

## D.23 run result

- Phase A established a writable disposable D1: `mdir -e` completed with
  status 000, the disk changed, and ToolShed `dcheck` accepted it.
- Phase B used a fresh disposable D1. `pwd`, `date`, both module loads, and
  media checks completed with status 000 and fresh prompts. Resident listing
  showed one `ppown20` and one `ppclient3` entry before launch.
- `ppown20` was run exactly once. Host completion reported `completed=true`,
  status `000`, no timeout, and Shell+ ready. Parent PID was 3; child PID was
  4 in this run. Do not treat those PIDs as fixed requirements.
- The parent log is 574 bytes, SHA-256
  `852c8e4a425cf577a735e77cc64749ffe73f266b1172ddb459f5424ae6b8e132`.
  It is well-formed ASCII/CRLF and ends with `PARENT_EXIT 00 00 00`.
- The child log SHA-256 is
  `e2c35113e2e367667ed9ca8fa958ac799ff1e8c0c7b8efb92a98b72762148f2d`.
  It records both 16-byte ACKs and successful validation, then client exit
  code zero.
- `ppc3exit.log` exists on this writable D1; SHA-256
  `8c13de29003549e69e20a24198d710f318a3b9411b50e71059452481822db5ba`.
  It records requested status 00 and main return `NEXT`. Earlier absence of
  this file on a read-only disk did not establish child failure.
- The graphics observer recorded graphics entry at
  `334.32899891308 s`, pattern loss at `396.52574639081 s`, and display
  departure at `396.55912269748 s`; the supported text-return sample was
  record 4034. The host command completed and a post-run `pwd` succeeded.
- D.23 D1 hashes: pristine reference
  `7a6cd706305af35fc641b2626aedc6839229ea6750c1e7f26e4a9bcc1cc62382`,
  Phase A post-run `810e82ebbaa1964fe0bcc3d73392d8e837bfe46331bc6fb834148bc70f3699b8`,
  Phase B post-run
  `e70491517505538f438ebced2c3de5a87b64216a37236eda95c5d25fbf0ba148`.
  Disk images are intentionally kept out of Git.

The prior D.9 2,048-byte parent-log corruption at offset `$0100` did not recur
in D.23: the current parent log is correctly formatted at that offset. This
does not identify the cause of the earlier malformed log.

## Runtime configuration and procedure

The preserved Phase B launch evidence records MAME 0.289 `ample-289-u2`,
machine `coco3h`, HD6309E, 2 MB RAM, private RGB configuration
`screen_config=1`, disposable boot/VHD/state copies, and D1 on `flop2`. The
private config, launch record, host command trace, MAME output, observer events,
checkpoints, and extracted logs are in `evidence/phaseB/`. Phase A contains
the separate writable-media qualification.

For a future reproduction, first verify this directory's checksum manifest,
then stage these exact modules to a **fresh disposable** D1 with the established
binary-safe ToolShed procedure and readable/executable file attributes. Use
private copies of all writable media and the recorded private RGB configuration.
Run `pwd`, `date`, module loads, and media checks to status 000 before a single
parent launch. Keep MAME/bridge ports isolated. Capture the same observer,
parent checkpoints, and post-run files. The actual disposable disk, VHD, ROM,
state, and MAME executable are not archived here.

The D.23 report supports successful guest completion and Shell+ restoration;
it does not prove system-wide absence of unrelated/orphan processes, exact
instruction-level `F$Exit`, or heartbeat deadlines.
