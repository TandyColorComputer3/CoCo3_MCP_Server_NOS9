# Milestone 1A: os9_restore_ready

Implemented and live-verified September 25, 2026 (Pacific). `os9_exec` is not implemented.

## Usage and contract

Set `NITROS9_READY_STATE=nos9_ready_v2` in the MCP environment (this is the default). The file is discovered under `stateDir/<configured machine>/<configured state>.sta`. Start the compatible canonical MAME session, then call:

```json
{"name":"os9_restore_ready","arguments":{"timeout_ms":30000}}
```

The optional timeout is a host monotonic deadline, 1000–120000 ms, default 30000. The result is available both as MCP `structuredContent` and JSON text. `isError` is true unless `ready` is true.

- `loadScheduled`: the bridge acknowledged the tracked request.
- `loadCompleted`: MAME's post-load callback was observed for that operation.
- `shellVerified`: a fresh transient prompt and standalone echo nonce were observed, followed by the prompt with the keyboard queue drained.
- `timedOut`: the host deadline expired; `phase` identifies the stage.
- `status`: `ready`, `state_missing`, `state_unreadable`, `post_load_timeout`, `shell_handshake_failed`, `bridge_failure`, `restore_invalidated`, `invalid_arguments`, or `busy`.
- `console`: diagnostic visible character cells; not a complete command transcript. Unknown font glyphs are replaced by U+FFFD.

No disk file or checkpoint is written by this operation. The shell prompt changes in guest RAM to `MCP<fresh nonce>:`; it remains there until changed or restored. A second restore generates a new nonce. Failure does not retry, reset, or cold-boot the guest.

## Implementation

[Node readiness coordinator](../../MCP/src/os9-ready.ts), [MCP registration](../../MCP/src/tools.ts), [Lua bridge](../../MCP/scripts/bridge.lua), and [tests](../../MCP/test/os9-ready.test.ts).

The Lua bridge retains `emu.add_machine_post_load_notifier` and reset subscriptions. A host token identifies one tracked load; a post-load generation identifies the observed restoration. Node polls operation status, rather than counting sleep intervals. Lua refuses competing mutations during the handshake; the MCP handler also excludes overlapping mutating tools. Read-only status and snapshot calls remain available. If a load times out before its callback, it stays abandoned/pending until its eventual callback or a MAME restart, preventing a late event from completing a different request.

The read-only console path enumerates known saved items on `:gime` and `:ram`, reads the GIME registers and 4000 bytes of physical RAM, and decodes the 80×25 character/attribute cells. No guest MMU register or memory write is used. MAME 0.289 live enumeration verified `0/m_gime_registers`, `0/m_size` and `0/m_pointer`. The checkpoint's registers were:

```text
6c 00 09 00 09 00 00 00 04 75 01 03 00 fc 00 00
```

This gives native attributed text, 225 scanlines / 9 scanlines per row, 80 columns, no virtual/vertical scrolling, and physical base `0x1fe000`. The reader intentionally rejects other layouts and wrapped/cross-bank buffers. It does not infer text from pixels or the current CPU logical address map.

Sources: [MAME notifier API](https://docs.mamedev.org/luascript/ref-common.html), [MAME 0.289 GIME renderer](https://github.com/mamedev/mame/blob/mame0289/src/mame/trs/gime.cpp), and [MAME physical RAM Lua example](https://github.com/mamedev/mame/blob/mame0289/plugins/cheatfind/init.lua). Local manuals referenced by AGENTS.md were absent; these primary sources and installed-build observations were used instead.

The bundled Shell+ documentation's `p=` syntax was tested on the installed EOU Shell+ 2.2a. Readiness first requires the saved idle Term prompt, then executes `p=MCP<nonce>:` and waits for that exact final prompt. It next executes `echo READY<nonce>` and requires a standalone marker row before the final prompt. An echoed input line containing the marker cannot satisfy that condition. These short commands neither install software nor edit startup.

## Limits and recovery

Use the frozen compatible machine/media/checkpoint described in [the final live-boot report](NITROS9_LIVE_BOOT.md#final-canonical-layout-scii-slot-4-and-option-3). This tool does not enforce a media-hash manifest or roll back disks; the caller remains responsible for fixture compatibility. It also does not synchronize guest time to host time.

Exclusive access includes MAME's UI: do not manually type, load, reset, or switch windows while restoration runs. Post-load notifications do not identify filenames, so an external UI load racing the scheduled load cannot be independently correlated. The bridge detects later extra loads/resets but cannot make simultaneous external UI operations safe.

This is a checkpoint-specific Term-shell handshake, not a general OS-9 process or shell detector. Unsupported screens fail without typing. Once typing has begun, a timeout or lost bridge can leave input still draining; inspect/recover the session rather than assuming input was cancelled. A pending load without a callback may require explicitly stopping/restarting MAME. No sleep duration is treated as proof of readiness.

## Live verification

Canonical topology, RAM, RGB and bridge were preserved. Byte-identical temporary copies of the development VHD and boot floppy were attached under `/private/tmp/nos9-m1a/`, protecting the repository media. The original checkpoint was loaded directly and never saved. No stock VHD was mounted.

The fresh rebuilt MCP server started MAME, loaded the fixture for setup, then entered `echo BEFORE1ARESTORE`. A snapshot showed that output. The new tool then restored the checkpoint and completed the handshake. A snapshot showed the nonce and prompt with the prior marker gone. Finally, `echo AFTER1ARESTORE` printed its result and returned to the new prompt; status reported the same PID and `coco3h`. MAME stopped cleanly.

Exact `os9_restore_ready` structured result (identical JSON data also returned in its text content; MCP `isError: false`):

```json
{
  "ready": true,
  "state": "nos9_ready_v2",
  "loadScheduled": true,
  "loadCompleted": true,
  "shellVerified": true,
  "timedOut": false,
  "status": "ready",
  "phase": "ready",
  "elapsedMs": 6216,
  "file": "/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/states/coco3h/nos9_ready_v2.sta",
  "postLoadEpoch": 2,
  "console": {
    "columns": 80,
    "rows": 25,
    "source": "physical_ram",
    "text": "link shell\necho Starting Windows...\nshell i=/w1&\nshell i=/w2&\necho Loading 49 fonts, plus graphics pointers, graphics patterns, etc...\ncd sys\nmerge stdfonts stdpats�2 stdpats�16 stdpats�4 stdptrs stdwnd isolatin1font ibmed\ncfont smallfont.27 macfonts ansifonts�65.fnt ia.fnt roguefonts\nmerge ibmpoker.fnt ibmcga.fnt\necho Loading extended patterns for MVCanvas\nmerge stdpats�2.plus stdpats�4.plus stdpats�16.plus\ncd ..\n\n* Need to fix VIEW first\n* boot�win<>>>/1\n\necho To run the GUI, simply type GSHELL\nmontype r\n\n{Term|02}/DD:p=MCP2c42b412a6d0f1fe:\n\nMCP2c42b412a6d0f1fe:echo READY2c42b412a6d0f1fe\nREADY2c42b412a6d0f1fe\n\nMCP2c42b412a6d0f1fe:"
  },
  "handshake": {
    "prompt": "MCP2c42b412a6d0f1fe:",
    "marker": "READY2c42b412a6d0f1fe",
    "markerObserved": true,
    "promptReturned": true
  }
}
```

Other exact JSON tool results, in live call order (snapshots return PNG image content plus the snapshot path):

- Call 1: `{"ok":true,"alreadyRunning":false,"pid":58197,"bridge":true}`
- Call 2: `{"scheduled":true,"name":"nos9_ready_v2"}`
- Call 3: `{"queued":true,"text":"echo BEFORE1ARESTORE{ENTER}"}`
- Call 4, `coco_snapshot`: PNG returned successfully; exact text content `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png`.
- Call 6, `coco_snapshot`: PNG returned successfully; exact text content `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png`.
- Call 7: `{"queued":true,"text":"echo AFTER1ARESTORE{ENTER}"}`
- Call 8, `coco_snapshot`: PNG returned successfully; exact text content `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png`.
- Call 9: `{"running":true,"pid":58197,"bridge":true,"driver":"coco3h","flop1":"/private/tmp/nos9-m1a/63EMU.DSK","posting":false,"empty":true}`
- Call 10: `{"ok":true}`

The raw MCP envelopes, PNGs, launch JSON and test log are retained in `/private/tmp/nos9-m1a/` for this working session; they are not new permanent architecture assets.

## Preservation and checks

Before and after SHA-256 values were identical:

| File | Before = after SHA-256 |
|---|---|
| `media/63SDC.VHD` | `db2f0f444073d3b88a3610a63ec9f7d2e2dd4299347181faa6b2b2aa9637de2c` |
| `media/63SDC-MCP-DEV.VHD` | `4c6bdc0cd2f68aa57a9c75f75f15fe429f30926aa522917dd8d14aa1b9ae622e` |
| `media/63EMU.DSK` | `9a51adb8656b9f003c7f822352c291487362f1b4c43aac1a16672d5d2bcb4c40` |
| `MCP/states/coco3h/nos9_ready_v2.sta` | `a0f4ee094db471c596b9ecb9826922f80d982d2571fe7e72672a255ebca2effa` |

Both temporary media copies also retained their original hashes. Stock VHD permissions remained 0444. The state was 142026 bytes before and after.

Validation: `npm test` (72 passing), `npm run build`, and `git diff --check`. Tests cover successful delayed post-load completion, configured state/machine paths, missing state, post-load timeout, wrong shell, missing marker, echoed-marker false positives, bridge failures/disconnect, invalidated operation, unsupported console layouts, concurrent mutations, real TCP request/reply transport, and tool discovery. Lua-specific API behavior and physical screen addressing were verified in the live session; the source guard alone is not a behavioral Lua test.

No commit or staging was performed. The preexisting README change and design document were preserved.
