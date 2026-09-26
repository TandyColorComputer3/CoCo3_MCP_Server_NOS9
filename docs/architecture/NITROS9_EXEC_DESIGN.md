# NitrOS-9 console execution design — Milestone 1

Status: design only, September 25, 2026. No MCP implementation, emulator configuration change, guest installation, media write, or live command experiment is part of this investigation.

## 1. Scope and recommendation

The fixture is the frozen canonical EOU environment and `nos9_ready_v2` described in the [final live-boot report](NITROS9_LIVE_BOOT.md#final-canonical-layout-scii-slot-4-and-option-3). Its hardware and clock configuration are inputs to this design, not work items. Older configurations in that report are historical.

**Recommend a staged hybrid:** use MAME's post-load notification to acknowledge restoration; decode the active hardware text screen for baseline verification and diagnostics; establish a fresh shell prompt/nonce handshake; then add a loss-detecting output stream before promising general command output. Initially validate a bounded foreground command subset against the unchanged guest. Evaluate a narrowly scoped SCF observer for that stream; if matching and observing the installed guest reliably proves impractical, use a small guest helper in a separately authorized milestone. Do not substitute screen polling for a complete transcript.

Three different facts need separate evidence:

1. MAME finished restoring machine state.
2. The intended shell accepts and completes a new request.
3. All output belonging to a particular foreground command was captured.

A load acknowledgement, an empty keyboard queue, and a familiar prompt respectively do not establish those three facts.

## 2. Evidence and limits

### Repository and frozen fixture

Reviewed [tool handlers](../../MCP/src/tools.ts), [bridge server](../../MCP/src/bridge-server.ts), [protocol](../../MCP/src/protocol.ts), and [Lua bridge](../../MCP/scripts/bridge.lua), together with the [architecture](CURRENT_ARCHITECTURE.md), [console alternatives](NITROS9_CONSOLE_DESIGN.md), [guest inventory](NITROS9_GUEST_ENVIRONMENT.md), [transport inventory](NITROS9_TRANSPORT_INVENTORY.md), and latest live-boot findings.

| Existing mechanism | What it establishes | Missing evidence |
|---|---|---|
| `coco_load_state` / Lua `load_state` | Named file found; `machine:load` scheduled | Actual post-load completion, usable shell |
| `coco_type` / `type`, followed by `wait_idle` | Natural-keyboard posting has drained | Guest consumed the whole line, child exited, shell returned |
| `coco_snapshot` | Screen image | Character stream, process identity, command completion |
| `coco_read_memory` | Bytes from current CPU program-space mapping within 64K | Physical screen RAM, another process's memory, full console history |
| TCP request/reply bridge | Correlated JSON command responses | Restore epochs, output events, command ownership |

The bridge dispatches commands from a frame callback. Node tracks pending request IDs; unsolicited messages are not a supported event channel. The present save handler's file-presence check also does not prove a newly requested save completed when a file already exists. That matters for creating future fixtures, although this task uses the already verified checkpoint.

The final live test demonstrated restoration without cold boot, continued bridge connectivity, and a subsequent working `date -t`. It did not implement automatic readiness detection. Earlier restoration evidence even showed a prompt that did not immediately repaint: pixel equality or a fixed repaint delay would be a poor requirement. The checkpoint resumes its saved clock timeline; restore must not promise host-time resynchronization.

### Guest evidence inspected read-only

ToolShed inspection of the development image found:

- `/SYS/env.file`: `CONDVTYP=2`, documented there as an 80-column text device window; `CONXSIZ=80`, `CONYSIZ=25`, `CONSHELL=Shell`, `CONSHPRM=i=/1`, and `CONSTRT=startup -p`.
- `/DOCS/shellplus2.1.doc`: configurable prompt syntax, sequential command syntax, and shell options.
- `/DOCS/shellplus2.2a.docs`: changes for the Shell+ version shown in the live boot.
- The live report identifies loaded SCF, VTIO, Term, and window modules. The prompt includes device/process/directory context; it is not a universal literal.

The repository's referenced `MCP/Documents/` and `DOCS_INDEX.md` were unavailable during this investigation. Consequently, guest details below are grounded in bundled guest documentation and primary project source, not an unexamined hardware manual.

Upstream NitrOS-9 source was inspected at commit `f470fa52eb172b59b22c1b722074998cb42de9b1`: [SCF](https://github.com/nitros9project/nitros9/blob/f470fa52eb172b59b22c1b722074998cb42de9b1/level1/modules/scf.asm), [VTIO](https://github.com/nitros9project/nitros9/blob/f470fa52eb172b59b22c1b722074998cb42de9b1/level2/coco3/modules/vtio.asm), [CoWin](https://github.com/nitros9project/nitros9/blob/f470fa52eb172b59b22c1b722074998cb42de9b1/level2/coco3/modules/cowin.asm), [GrfDrv](https://github.com/nitros9project/nitros9/blob/f470fa52eb172b59b22c1b722074998cb42de9b1/level2/cmds/grfdrv.asm), and [Shell+](https://github.com/nitros9project/nitros9/blob/f470fa52eb172b59b22c1b722074998cb42de9b1/level1/cmds/shellplus.asm). These explain architecture; they are **not established as binary-identical to the installed EOU modules**. No runtime address or module offset may be taken from them without matching the installed binary.

## 3. Can this console be read without OCR?

**The configured hardware text console makes deterministic visible-character extraction a practical candidate.** This is stronger than assuming every CoCo screen contains ASCII RAM, but weaker than having validated a runtime decoder against the checkpoint.

The version-matched [MAME 0.289 GIME renderer](https://github.com/mamedev/mame/blob/mame0289/src/mame/trs/gime.cpp) distinguishes native text and graphics modes. Its text paths fetch character bytes, optionally accompanied by attributes, from emulated RAM. `get_video_base`, `record_scanline_res`, and `get_data_with_attributes` show that address, bank, horizontal offset, stride and attributes matter. They also separate character contents from blink rendering. Reading those underlying cells can avoid cursor/blink differences in screenshots.

The proposed reader must:

1. Discover and validate the active GIME mode and physical RAM interface on the installed build.
2. Derive the display base, RAM bank, text width, row height, horizontal offset, stride and attribute format from that mode. Do not assume a fixed BASIC screen address or a contiguous 80×25 byte array.
3. Read a coherent frame's visible cells, decode character codes using the matched renderer/font behavior, and return rows plus mode metadata. Preserve attributes separately when needed to distinguish cursor or window state.
4. Identify the intended active terminal/window. A visible prompt in another window is insufficient.
5. Reject unsupported graphics modes or unknown layouts rather than returning plausible text.

The existing CPU-space memory read is insufficient: NitrOS-9 switches mappings between tasks, whereas GIME display fetches use physical RAM addressing. Do not switch the guest MMU just to observe it. `read_direct` is not a general physical-RAM selector.

MAME's own version-matched [cheatfind plugin](https://github.com/mamedev/mame/blob/mame0289/plugins/cheatfind/init.lua) provides a concrete Lua precedent: it finds devices whose `shortname` is `ram`, reads `0/m_size` with `emu.item`, and reads blocks from `0/m_pointer`. This is a candidate read-only physical access route. Exact RAM device tags and GIME saved-item names still need enumeration on installed Ample MAME; saved-item names are an internal coupling, not a stable terminal API.

### What a decoded screen cannot promise

A screen is a viewport, not a transcript. Several output lines can scroll away between frame callbacks. Repeated identical lines, clears, cursor addressing, paging, and redraws defeat simple row-difference reconstruction. Sampling faster does not establish losslessness. Graphics-mode text may consist only of glyph pixels; no character-cell recovery is promised for it.

Even write taps on video RAM observe rendering and scrolling copies, not necessarily the original text stream. A captured cell can represent a copied old character or a newly printed one. Therefore expose screen-derived output as `outputSource: "screen"` and `outputComplete: false` unless the bounded operation's complete capture has independently been established. Never silently truncate `mdir` to its last screen.

## 4. Practical alternatives

### Comparative assessment

Reliability below assumes the necessary validation has been performed, not merely that the approach is theoretically available.

| Approach | Reliability and text quality | Prompt/completion detection | Guest changes | MAME coupling / real CoCo | Performance / complexity | Save-state interaction |
|---|---|---|---|---|---|---|
| **A. Keyboard + screen images** | Useful human evidence; OCR can misread and misses scrolled output | Fresh prompt plus marker helps; image recognition alone is weak | None; optional transient prompt | Input/display capture adapters needed on real hardware | Image/OCR cost; low initial, high robust-detector effort | Images can be stale; discard all pre-load observations |
| **B. Physical text/video memory** | Exact visible cells in supported text mode; incomplete history | Deterministic prompt/marker matching with fresh observations; screen spoofing remains possible | None; transient prompt desirable | Strong GIME/MAME coupling; real hardware needs a separate memory observer | Small block reads; medium complexity | Re-resolve display/mapping state; clear history and command epoch |
| **C. Lua observation of guest output/rendering** | Potential complete byte stream at a verified SCF boundary; video taps alone retain B's limits | Stream sequence plus shell marker; process/path context can strengthen attribution | None if entirely observational | Strong emulator and installed-module coupling; no direct real-hardware equivalent | Batching efficient; hooks/ABI validation high complexity | Revalidate hooks after load; flush observer buffers and renew epoch |
| **D. Small guest helper/protocol** | Strongest explicit command/result framing, counts and status | Helper acknowledges start, child termination and end-of-output | New guest program and launch/bootstrap procedure | Guest protocol portable; transport implementation still required | Efficient; moderate/high guest development effort | Guest protocol state rewinds; host reconnect/nonce handshake required |
| **E. DriveWire/Becker console** | Ordered text bytes if configured as console; still needs framing | Prompt/sentinel or helper protocol | Load/configure suitable SCF modules, descriptors and shell paths | Becker is emulator-oriented; DriveWire has a real-CoCo path | Efficient; server/device/guest integration substantial | External server/socket state does not rewind with MAME |
| **F. Serial/SCF console** | Ordered bytes with correct serial settings and buffering | Prompt/sentinel or helper protocol | UART/SCF driver, descriptor and redirected shell | Strong real-hardware path; current frozen target is not this configuration | Baud-limited but adequate; medium/high integration effort | Reopen/re-synchronize external endpoint; discard old bytes |
| **G. Staged hybrid** | B for readiness/diagnostics plus C or D for complete output | Restore event + nonce handshake + framed execution | None initially; helper only if selected later | Separate protocol from MAME observation for later portability | Incremental; more components but explicit boundaries | One host session epoch coordinates all observers/transports |

### C: where instrumentation must actually observe

There is no generic MAME object exposing the NitrOS-9 shell's stdout. The console path involves SCF, VTIO/window support and GrfDrv. SCF also handles line discipline and echo. Its source includes a fast text path into GrfDrv, so a hook on only one VTIO character-output routine can miss traffic.

A candidate observer would recognize the installed SCF output entry/return path, resolve the caller's memory mapping and path, and copy accepted output bytes into a bounded Lua buffer. It must distinguish requested bytes from successfully written bytes, handle bulk writes and line writes, and account for input echo and other processes sharing the terminal. Physical video writes are a diagnostic fallback, not proof of stdout capture.

The [Lua memory API](https://docs.mamedev.org/luascript/ref-mem.html) supplies read/write taps and mapping-change notifications. Taps must not replace values when used as observers. An instruction/PC observer may require debugger facilities; do not assume a portable Lua execution-hook API exists or enable it without measuring overhead. The exact interception mechanism is an implementation feasibility gate.

Resolve installed module identity, size, CRC, relocation and task mapping before any ABI-dependent observation. Fail closed on a mismatch. A hard-coded PC address that happens to work once is not acceptable.

### D: helper semantics

A helper can receive a request ID and command, launch a foreground child, drain its output, wait for termination, and send a framed end record with a status. Design framing to tolerate arbitrary output bytes and include lengths, sequence numbers and a session nonce. Concurrent draining is necessary to avoid pipe-buffer deadlocks. Verify pipe and process-service details against the actual guest before coding.

Executing in a child shell differs from executing inside the persistent interactive shell: changes such as `chd` may not persist. Define that contract explicitly. Background descendants may retain output handles beyond child termination; they require rejection or a separate job model. Installing the helper would change guest media and the baseline, and is **not authorized by this design task**.

### E/F: availability is not readiness

The [transport inventory](NITROS9_TRANSPORT_INVENTORY.md) distinguishes modules in boot files from library files. DriveWire/Becker and serial-related modules existing somewhere on the VHD does not make the current Term console a byte-stream endpoint. Enabling those paths requires separately verified guest descriptors, shell routing and host endpoints. The frozen hardware must remain unchanged in Milestone 1. Keep a transport-neutral execution protocol so those later backends can reuse it.

## 5. Prompt and sentinel protocol

The bundled Shell+ documentation describes `p=prompt`, a maximum of 21 characters, quoted prompts, and expansions for `#`, `@`, `$`, `(` and `)`. A candidate transient prompt is `p=MCPa1b2c3:`: short, ordinary characters with no expansion tokens. This changes shell runtime state, not startup or the on-disk shell module. Restore will revert it, so establish a fresh value after each restore. Exact behavior must be tested on the installed Shell+ 2.2a; do not patch documented old module offsets.

Shell+ supports sequential commands and waits for foreground children in the inspected source. A candidate test sequence is:

```text
echo MCPBEGINa1b2c3; mdir; echo MCPENDa1b2c3
```

This syntax is a **validation candidate**, not a procedure executed or proven on the current fixture in this task. Check the installed echo utility, separator handling, error/exit options, line length, and paging first.

A robust interpretation requires:

- Capture armed before typing; a new host-generated command nonce within a new session epoch.
- Begin/end records recognized as emitted output, not the echoed input line containing those same strings. Wrapped input echo must not satisfy the protocol.
- A fresh matching prompt after the end marker. An unchanged prompt already in video memory is not a return event.
- A rule for errors that skip the final marker. Missing marker is not automatically a hung child.
- Foreground-only semantics. A marker after an asynchronous launch does not mean that job completed.
- Output framing resistant to accidental marker collisions. A simple marker is not protection against a command intentionally spoofing the protocol.

The docs describe `%*` as the error code from the last processed line. That does not establish same-line expansion timing or make it POSIX `$?`. Return `exitCode: null` until the actual shell semantics or helper protocol can establish status. `completed: true` means execution finished, not that the program succeeded.

For the first release, accept a documented, parsed subset of noninteractive foreground commands. Reject unsupported newlines, background syntax, redirection and shell metacharacter combinations instead of interpolating arbitrary strings into a wrapper. Natural-keyboard `post_coded` also interprets coded keys; literal text needs a verified encoding path. Do not automatically feed Enter to pagers or interrupt arbitrary guest programs.

## 6. Proposed `os9_restore_ready` flow

MAME documents `machine:load` as scheduled work, not synchronous restoration. The [machine API](https://docs.mamedev.org/luascript/ref-core.html) also permits a pending save/load to be displaced by another request. The [common Lua API](https://docs.mamedev.org/luascript/ref-common.html) supplies `emu.add_machine_post_load_notifier`; retain its subscription handle. Verify this API is exposed by the installed build during implementation.

Proposed state machine:

```text
IDLE -> CHECK_FIXTURE -> LOAD_REQUESTED -> POST_LOAD_OBSERVED
     -> OBSERVERS_REBOUND -> SHELL_CANDIDATE -> NONCE_PROBE -> READY
                                               any failure -> FAILED
```

1. Acquire exclusive machine-operation ownership. Serialize state operations, keyboard input, reset and media operations across old and new MCP tools.
2. Verify fixture identity: configured machine, MAME build, state path/hash and intended media identity. Use `stateDir/<configured-machine>/nos9_ready_v2.sta`, never the older `nos9_ready` state. Require compatible media; do not silently accept a changed disk or restore disk files.
3. Arm a host operation token and the post-load observer **before** scheduling the load. Return/poll an operation status rather than treating `scheduled` as success.
4. Wait for the post-load notification under a host monotonic deadline. Because the callback does not identify the requested filename, correlate through exclusive ownership and one pending load. A reset, competing load, disconnect or external UI operation invalidates the attempt.
5. Increment the observation epoch, clear old output and keyboard-request bookkeeping, and revalidate physical-memory handles, mapping-dependent taps and screen mode. Do not assume Lua-side buffers or TCP state rewind consistently with guest RAM.
6. Observe a compatible terminal and idle-shell candidate; check keyboard queue drainage as supporting evidence only. Where available, corroborate the shell's input path/process state. A sleeping process alone is not proof of readiness.
7. Establish a fresh transient prompt and harmless nonce probe only for this recognized checkpoint. Require probe output and a fresh prompt. If the fixture instead appears to be inside an application, fail rather than sending repeated blind commands.
8. Return ready only when all required evidence is present, including the restore event and successful probe. Report phase, elapsed host time and diagnostic screen text on failure.

No arbitrary sleep is a success criterion. Poll intervals can limit load, but the observed conditions determine progress. Emulated time can rewind or pause, so it is unsuitable for deadlines. A paused emulator, missing callback, corrupt/mismatched state, wrong console, stalled probe and lost bridge need distinct diagnostics.

Save states do not roll back external disk contents. A fixture manifest should record the compatible media hashes from a quiescent point; hash/check policy must not race guest writes. Neither a restored shell nor matching screen pixels establishes filesystem consistency. The current immutable stock image is never a runtime attachment.

## 7. Proposed `os9_exec` contract and flow

Example result shape (illustrative, not observed output):

```json
{
  "command": "mdir",
  "completed": true,
  "timedOut": false,
  "promptReturned": true,
  "output": "...",
  "outputComplete": true,
  "outputSource": "scf_stream",
  "truncated": false,
  "exitCode": null,
  "sessionId": "...",
  "commandId": "..."
}
```

Validate `timeout_ms` as finite, positive and bounded by service policy. Use separate bounds for capture bytes and input length. Timeout and capture limits must not depend on guest time.

Execution sequence:

1. Require a healthy ready session, acquire its exclusive command lock, and validate the supported command grammar.
2. Allocate a fresh command ID and arm capture before injecting input. Record output sequence positions so previous output cannot be attributed to the new command.
3. Observe start acknowledgement, ordered output and completion evidence. Keyboard drainage is only an input-delivery milestone.
4. Require the end condition and fresh prompt. Return partial output with explicit incompleteness on overflow, unsupported rendering or lost events.
5. On deadline expiry, return `timedOut: true`, `completed: false`, the observed prompt flag and partial output. The guest may still be executing: mark the session uncertain and reject further execution until explicitly recovered.

Transport failure is not a guest timeout. Command completion and output completeness are independent: a command can finish while capture overflows. Preserve this distinction even if the first API exposes fewer fields. Do not automatically replay commands or restore the checkpoint after a timeout; those actions can repeat side effects or discard useful guest state.

For stream capture, retain raw bytes and provide normalized text with a documented character/control-code policy. CR/LF, backspace, tabs, escape sequences and input echo need deliberate handling. A local console stream may combine stdout, stderr and unrelated process output; do not claim separate channels or exact process attribution without observing paths or using the helper.

Prefer batched, bounded reads of an output ring with sequence numbers and explicit overflow indication. This fits the existing bridge's request/reply structure without immediately adding unsolicited events. For visible text, a nominal 80×25 screen with character/attribute pairs is about 4 KB per sample; polling the entire 2 MB RAM or sending one TCP message per character is unnecessary.

## 8. Milestone 1 stages and acceptance gates

### Stage 1 — restoration lifecycle and deterministic screen reader

Implement the post-load operation record, host deadline and session epoch; then prove the physical text decoder against `nos9_ready_v2`. Test address/mode changes, attributes, cursor blink and multiple MMU mappings without writing guest memory. Compare decoded text with known screen evidence, including repeated lines and scrolling. Unknown modes must return an explicit unsupported error.

Deliver `os9_restore_ready` only after a fresh probe can demonstrate shell responsiveness. A post-load event alone is not sufficient.

### Stage 2 — unchanged-guest completion experiment

Validate transient prompts and the sentinel sequence using short read-only foreground commands. Include success, nonexistent command, errors, input echo, wrapped input, paging and timeout cases. This stage can expose a clearly limited screen-observation prototype, but must not present screen-derived text as a complete arbitrary-command transcript.

### Stage 3 — dependable `os9_exec` output

First evaluate the SCF observer against the exact installed binaries, including the fast text path. Acceptance requires zero lost/duplicated bytes on output longer than a screen, correct sequencing, bounded buffers and explicit overflow reporting. Keep ABI-sensitive observation behind a replaceable capture backend.

If those requirements cannot be met without fragile offsets or debugger dependence, choose the small guest helper rather than accumulate heuristics. That decision requires separate permission to install guest software and create a new compatible checkpoint. Byte-stream transport migration remains a later milestone; no hardware investigation is needed now.

The public Milestone 1 completion gate is **restore + fresh ready handshake + bounded foreground execution + honest output completeness + safe timeout recovery**. A useful decoder alone does not satisfy the requested execution contract.

### Required failure and restore tests

- Missing/corrupt state, missing post-load event, paused machine, bridge disconnect, competing reset/load.
- Stale prompt/marker after restore; restore during capture; fresh nonce and epoch required.
- Wrong fixture/media identity; no automatic disk rollback.
- Unsupported screen mode, wrong active window, changing CPU mappings and invalid observer handles.
- Output exceeding one screen, repeated identical lines, fast bulk writes, echo and buffer overflow.
- Failed command, shell error that skips sentinel, pager/interactive command and background syntax.
- Timeout followed by attempted execution: session remains uncertain until recovery.
- Restore followed immediately by a harmless command: no cold boot; checkpoint-time behavior documented.

## 9. Reproducibility notes

This design reused documented live evidence; it did not start MAME or exercise proposed prompt/hook behavior. Read-only guest inspection used the native ToolShed utility. Representative exact commands, with working directory at the repository root:

```sh
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 dir media/63SDC-MCP-DEV.VHD,DOCS
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 list media/63SDC-MCP-DEV.VHD,DOCS/shellplus2.1.doc
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 list media/63SDC-MCP-DEV.VHD,DOCS/shellplus2.2a.docs
/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9 list media/63SDC-MCP-DEV.VHD,SYS/env.file
```

Upstream source was fetched into `/private/tmp` only. MAME source references are pinned to `mame0289`; NitrOS-9 references are pinned above. Current online Lua documentation supplies API candidates; installed-build capability checks remain mandatory. No new architecture assets are necessary for this design.
