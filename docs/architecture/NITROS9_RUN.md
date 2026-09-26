# os9_run — foreground completion and Shell+ status

Implemented and live-verified September 25, 2026 Pacific (September 26 UTC). `os9_exec` is not implemented. This tool returns completion/status and does not return stdout or claim complete command-output capture.

## Usage

Start the compatible canonical MAME session, call `os9_restore_ready`, then:

```json
{"command":"mdir","timeout_ms":30000}
```

`timeout_ms` is optional: default 30000, integer range 1000–120000 ms. It covers the command and the separate status query, using host monotonic time. Cleanup has a separately bounded one-second bridge request.

A verified session is cached in the MCP process by bridge identity, with the exact prompt and post-load epoch established by `os9_restore_ready`. Successful runs, including nonzero guest statuses, retain the session for the next run. Restarting the MCP requires restoring again. Raw keyboard input, reset, load, media changes and other mutating MCP operations conservatively invalidate the cached session; save-state creation does not. Read-only status/snapshot operations preserve it.

## Protocol and reuse

The implementation follows [the live command-completion investigation](NITROS9_COMMAND_COMPLETION.md) and [the execution design](NITROS9_EXEC_DESIGN.md):

1. Acquire exclusive console ownership and verify the cached epoch, supported hardware text mode, idle keyboard, and exact session prompt.
2. Send the command followed by Enter.
3. Observe activity away from the old prompt, then an exact newly returned prompt with keyboard posting drained. An unchanged old prompt cannot complete the command.
4. Only then send a new input line: `echo MCPDONE<32 random hex digits> %*` followed by Enter. The ID uses 16 cryptographically random host bytes per request.
5. Again require activity and a fresh prompt. Parse exactly one standalone row containing that request's marker, one space and three decimal digits. Require the prompt after that row and status in the byte range 0–255.
6. Release ownership, checking for an intervening reset/load. Preserve the session only when the operation remains healthy.

No semicolon framing is used. No `%*` appears on the target command line. No additional guest command occurs between target completion and the status query. Echoed input is not an accepted marker row. Old markers from previous runs do not match the new request.

[os9-run.ts](../../MCP/src/os9-run.ts) imports the existing `decodeConsole` from [os9-ready.ts](../../MCP/src/os9-ready.ts). The Lua `begin_run`/`finish_run` commands reuse the existing exclusive observer record and `read_text_console` physical RAM path. No second decoder, OCR, memory-write mechanism, guest helper, disk installation or state creation was added. Reset now also advances the observation epoch.

The verified fixture is Shell+ v2.2a edition 23, native 80×25 attributed text, using the previously verified canonical topology. Unknown display modes fail explicitly. See [the completion report](NITROS9_COMMAND_COMPLETION.md#2-documentationsource-findings) for bundled Shell+ documentation/source and the failure/expansion semantics supporting the protocol.

## Input scope

This first version accepts a program name beginning with an ASCII letter, followed by letters, digits, underscore or hyphen. Arguments contain letters, digits, underscore, dot, slash or hyphen, separated by spaces. The complete command is limited to 58 characters so its echo fits beside the 20-character prompt on an 80-column row.

It rejects control/non-ASCII characters, newlines (including trailing newlines), natural-keyboard coded keys, quotes, wildcards, expansion, separators, redirection, pipes, grouping, background syntax and shell/console-control commands. Examples of rejected input include `pwd; echo ...`, `echo %*`, `pwd | cat`, `shell`, `ex` and `montype r`.

The intended workload is one noninteractive foreground program, such as `mdir`, `pwd`, `date`, `dir`, `procs`, `ident` or `list`. Unknown program names are allowed so their normal OS-9 launch errors can be returned. The grammar is not a filesystem-write sandbox or a proof that an arbitrary executable is noninteractive: callers must choose appropriate programs. Interactive programs, pagers, console reconfiguration and programs deliberately spoofing the prompt remain unsupported. On loss of a provable prompt, the tool times out rather than feeding the status query into an unknown application.

Exclusive ownership includes MAME's UI: do not manually type, switch windows, reset or load while the tool is active. Asynchronous output can obscure terminal evidence. Failure does not retry the command, re-query `%*`, interrupt the guest, cold-boot, or automatically restore a checkpoint.

## Structured result

Results are returned in both MCP `structuredContent` and JSON text content.

| Field | Meaning |
|---|---|
| `command` | Requested command. |
| `commandCompleted` | The target's fresh return prompt was observed; can remain true if the later status phase fails. |
| `completed` | The command/status protocol reached a valid marker and returned prompt. Check `outcome` and `shellReady` as well: release failure can subsequently invalidate the session. |
| `status`, `statusText` | Parsed Shell+ line status, e.g. `216` and `"216"`; null when unavailable. |
| `timedOut` | Host deadline expired during the protocol. |
| `shellReady` | Final verified prompt and successful ownership release. |
| `outcome` | `completed`, `invalid_input`, `shell_not_ready`, `timeout`, `protocol_error`, `bridge_failure`, or `busy`. |
| `phase` | Preflight, command, status-marker phase, or complete. |
| `marker` | Unique request marker when execution reaches the command phase. |
| `commandPromptMs`, `elapsedMs` | Host monotonic measurements from operation start. |
| `outputComplete` | Always false; no command transcript is returned. |
| `error`, `cleanupError` | Diagnostic failure details when applicable. |

MCP `isError` is false for `outcome: "completed"`, **including nonzero OS-9 status**. A nonzero guest status describes the program/line failure, not a bridge failure. `%*` is Shell+'s preceding-line status; it is not an aggregate pipeline status or a complete Unix process-status model.

Timeout, lost shell, malformed marker or bridge failure invalidates the cached session. Use explicit `os9_restore_ready` recovery before another run. Input rejection and busy responses do not invalidate an otherwise healthy session. A status-query failure may leave the target already completed; do not blindly repeat it. The status query itself can overwrite `%*`.

## Tests and live verification

- `npm test`: **94 passed**, zero failed/skipped.
- `npm run build`: passed.
- `git diff --check`: passed.

Tests cover statuses 000/216, fresh markers, strict two-stage sequencing, stale prompt rejection, command and status-phase timeouts, lost/unsupported screens, echoed/duplicate/malformed/unexpected markers, invalid status range, changed epochs, disconnects and release failures, input injection, concurrent mutating tools, session invalidation, MCP nonzero-status semantics, real TCP transport and tool discovery. Existing restore tests remain in the full suite.

The live test started a freshly built MCP and canonical MAME using byte-identical temporary copies of the dev VHD and boot floppy. It began with `os9_restore_ready`. The following three `os9_run` calls ran **consecutively without another restore**, proving that the failed command preserved a usable shell:

| Command | Status | Elapsed | MCP isError |
|---|---:|---:|---|
| `mdir` | 000 | 7222 ms | false |
| `MCPNoSuchCommand` | 216 | 8743 ms | false |
| `pwd` | 000 | 7359 ms | false |

A final snapshot showed all three standalone markers, the nonexistent-command error, `/DD` from `pwd`, and the final session prompt. MAME then stopped cleanly. No temporary observation wrapper was used in this test: these results came from the actual new MCP tool and existing physical console reader.

### Exact MCP result data

The objects below are the exact `structuredContent` values, also serialized in the MCP text content. Each of these four calls returned `isError: false`. The complete envelopes and the PNG are retained for this session in `/private/tmp/os9-run-live/`.

#### os9_restore_ready

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
  "elapsedMs": 6224,
  "file": "/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/states/coco3h/nos9_ready_v2.sta",
  "postLoadEpoch": 1,
  "console": {
    "columns": 80,
    "rows": 25,
    "source": "physical_ram",
    "text": "link shell\necho Starting Windows...\nshell i=/w1&\nshell i=/w2&\necho Loading 49 fonts, plus graphics pointers, graphics patterns, etc...\ncd sys\nmerge stdfonts stdpats�2 stdpats�16 stdpats�4 stdptrs stdwnd isolatin1font ibmed\ncfont smallfont.27 macfonts ansifonts�65.fnt ia.fnt roguefonts\nmerge ibmpoker.fnt ibmcga.fnt\necho Loading extended patterns for MVCanvas\nmerge stdpats�2.plus stdpats�4.plus stdpats�16.plus\ncd ..\n\n* Need to fix VIEW first\n* boot�win<>>>/1\n\necho To run the GUI, simply type GSHELL\nmontype r\n\n{Term|02}/DD:p=MCP126c873bfa51d609:\n\nMCP126c873bfa51d609:echo READY126c873bfa51d609\nREADY126c873bfa51d609\n\nMCP126c873bfa51d609:"
  },
  "handshake": {
    "prompt": "MCP126c873bfa51d609:",
    "marker": "READY126c873bfa51d609",
    "markerObserved": true,
    "promptReturned": true
  }
}
```

#### os9_run: mdir

```json
{
  "command": "mdir",
  "completed": true,
  "commandCompleted": true,
  "status": 0,
  "statusText": "000",
  "timedOut": false,
  "shellReady": true,
  "outcome": "completed",
  "phase": "complete",
  "elapsedMs": 7222,
  "outputComplete": false,
  "marker": "MCPDONE894e6197bb7d6e20af8d12ff7f74b9b6",
  "commandPromptMs": 1333
}
```

#### os9_run: MCPNoSuchCommand

```json
{
  "command": "MCPNoSuchCommand",
  "completed": true,
  "commandCompleted": true,
  "status": 216,
  "statusText": "216",
  "timedOut": false,
  "shellReady": true,
  "outcome": "completed",
  "phase": "complete",
  "elapsedMs": 8743,
  "outputComplete": false,
  "marker": "MCPDONEf211b2ec89dab051d9dedada007333a6",
  "commandPromptMs": 3137
}
```

#### os9_run: pwd

```json
{
  "command": "pwd",
  "completed": true,
  "commandCompleted": true,
  "status": 0,
  "statusText": "000",
  "timedOut": false,
  "shellReady": true,
  "outcome": "completed",
  "phase": "complete",
  "elapsedMs": 7359,
  "outputComplete": false,
  "marker": "MCPDONEada6f630a496883daec63c2b28313cc2",
  "commandPromptMs": 1754
}
```

Other live MCP results:

- `coco_start`: `{"ok":true,"alreadyRunning":false,"pid":77839,"bridge":true}`
- `coco_stop`: `{"ok":true}`
- `coco_snapshot`: PNG returned successfully; exact text content `/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/snapshots/current.png`.

## Media/state preservation

Original stock media was never mounted. The original dev media and floppy were not attached; temporary byte-identical copies were used. SHA-256 before and after matched for every original and both temporary copies:

| File | Before = after SHA-256 |
|---|---|
| `media/63SDC.VHD` | `db2f0f444073d3b88a3610a63ec9f7d2e2dd4299347181faa6b2b2aa9637de2c` |
| `media/63SDC-MCP-DEV.VHD` | `4c6bdc0cd2f68aa57a9c75f75f15fe429f30926aa522917dd8d14aa1b9ae622e` |
| `media/63EMU.DSK` | `9a51adb8656b9f003c7f822352c291487362f1b4c43aac1a16672d5d2bcb4c40` |
| `MCP/states/coco3h/nos9_ready_v2.sta` | `a0f4ee094db471c596b9ecb9826922f80d982d2571fe7e72672a255ebca2effa` |

No media, checkpoint, hardware configuration or guest software change was made. No commit or staging was performed.
