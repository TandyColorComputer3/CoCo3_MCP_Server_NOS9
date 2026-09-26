# Installed Shell+ command completion investigation

Live-tested September 25, 2026 Pacific (September 26 UTC). Research only: no `os9_exec` implementation, MCP source changes, guest installation, media edits, or commit.

## Recommendation

Use **two separate shell input lines**, with an observed fresh prompt between them:

```text
<one supported foreground command><ENTER>
```

Wait for that command's newly returned session prompt, then send:

```text
echo MCPDONE<host-generated-128-bit-hex-request-id> %*<ENTER>
```

Accept completion/status only after observing a standalone `MCPDONE<id> <three decimal digits>` output row followed by the exact session prompt. Do not match marker substrings in echoed input. Do not send an intervening shell command before the status query.

This worked for success, failure, and output that scrolled through many screens. **Do not use `command; echo marker %*` as the general protocol:** failure skips the remaining commands on that line, and `%*` is expanded before the line runs, so it can contain a previous line's status.

These findings establish completion/status for the tested foreground console workflow. They do not establish complete output capture or unrestricted shell automation.

## 1. Fixture, method and identity

Every one of the 19 live experiments began with a successful `os9_restore_ready` call. The tool restored `nos9_ready_v2`, observed post-load completion, and installed/verified its fresh `MCP<session nonce>:` prompt. Start/stop operations were session setup/cleanup, not additional experiments. No experiment inherited another experiment's guest changes.

The canonical coco3h / HD6309, 2 MB, RGB, MPI/SCII configuration was preserved. Byte-identical temporary copies of the development VHD and boot floppy were attached under `/private/tmp/nos9-completion/`. The stock VHD was never mounted. The existing checkpoint was loaded, never saved.

A temporary Lua autoboot wrapper loaded the repository's **unchanged** bridge and added a read-only frame observer. It read `:gime` saved registers and the physical `:ram` character/attribute buffer at the verified display base. A host harness sampled its atomically replaced file every 10 ms and timestamped observations with a monotonic clock. This avoided adding MCP tools or modifying the bridge. The existing bridge's `read_text_console` is owned by the restore operation and is not a general post-restore observation endpoint; the next implementation will need an execution-owned observer lifecycle.

The test harness called the existing MCP tools through a fresh stdio client/server: `coco_start`, `os9_restore_ready`, `coco_type`, `coco_snapshot`, and `coco_stop`. Snapshots corroborated the physical-memory observations; OCR was not used.

### Exact running shell identity

Live `ident -m shell` returned:

```text
Header for:  Shell
Module size: $1B57    #6999
Module CRC:  $F7A3C4 (Good)
Hdr parity:  $65
Exec. off:   $0074    #116
Data Size:   $1F00    #7936
Edition:     $17      #23
Ty/La At/Rv: $11 $80
Prog mod, 6809 obj, re-en, R/O
```

Read-only ToolShed `ident` on `/CMDS/shell` returned the same Shell module size, CRC and edition. A binary dump contains `Shell+ v2.2a ` at module offset `$002F`. The bundled `/SOURCECODE/ASM/SHELL/shellplus2.2a.asm` specifies edition 23 and the same banner. Thus the installed running module is **Shell+ v2.2a, edition 23, CRC F7A3C4**, a 6809 object running in the HD6309 environment. This is more specific than relying on the filename or an upstream version number.

The disk file is a module pack, also containing Echo and other utilities. Its Echo module identifies as edition 5, size `$22`, CRC `$F5FF9A`; live command behavior below verifies echo availability. The bundled source was inspected for semantics, not reassembled or claimed to be a byte-for-byte source reconstruction.

## 2. Documentation/source findings

Primary local evidence, inspected read-only on the development VHD:

- `/DOCS/shellplus2.1.doc`: PROMPTS, shell variables, ONERR GOTO, miscellaneous features.
- `/DOCS/shellplus2.2a.docs`: version history and changes for 2.2a.
- `/SOURCECODE/ASM/SHELL/shellplus2.2a.asm`: command tables and execution flow.
- `/CMDS/shell`: module headers and binary banner.
- Boot `OS9Boot`: Nil, PipeMan, Piper and Pipe modules/descriptors are present.

The repository's referenced hardware-manual directory/index were unavailable. This investigation uses the installed guest's documents/source/binary, version-matched [MAME GIME source](https://github.com/mamedev/mame/blob/mame0289/src/mame/trs/gime.cpp), and live results. The [preceding execution design](NITROS9_EXEC_DESIGN.md) distinguishes visible text from full transcripts.

Relevant bundled source labels:

| Label/area | Finding |
|---|---|
| `L03CF` modifier table / `CmdSEMIC` | `;` invokes the foreground command and waits; error carry bypasses continuation (`L12AE` → `L12BA`). |
| `L12D2` / `L12D4` / `L12EC` | Foreground wait uses `F$Wait` and examines the child/signal status. |
| `L041B`, `L049D`, `L168A` | `%*` is inserted into the pre-parsed input from the saved error byte, formatted as three decimal digits. |
| `L01CE` / `L0191` | Line processing clears/updates the saved error state; error handling returns to the interactive prompt in this fixture. |
| `CmdX` / `CmdNX` | `X` and `-X` control shell termination on error; they do not turn a failing semicolon chain into an unconditional continuation. |
| `L0408`, pipe modifier entries | `!` and `\|` are pipe operators. |
| `L03DE` redirection table | `<`, `>`, `>>`, `>>>`, `<>`, `<>>`, `<>>>` are distinct path-routing forms. Do not interpret OS-9 `>>` as POSIX append. |
| `L083A` grouping path | Parentheses create a command group executed through Shell. |

The 2.1 documentation explicitly describes `%*` as the error from the last processed line and documents `|` alongside `!`. ONERR/GOTO is documented, but no procedure file was installed or edited to test scripted error traps. `X` shell termination and background `&` were not live-tested; neither is needed by the proposed protocol.

## 3. Live experiment results

In this table `M` stands for that experiment's unique host-generated marker, not a constant sent to the guest. The exact marker for each run is recorded below. Separate numbered input lines are shown with ` / next line: `; only the final three experiments explicitly waited for a newly returned prompt before posting the next line. Earlier next-line tests used keyboard-queue drainage and were exploratory, not the recommended synchronization protocol.

| Experiment | Exact command template(s), replacing `M` with unique marker | Observed result |
|---|---|---|
| identity | `ident -m shell; echo M` | Module header, marker, prompt. |
| success | `pwd; echo M` | `/DD`, marker, prompt. |
| failure | `dir /dd/MCPnosuchfile; echo M` | Error 216 and prompt; **no standalone marker**. |
| failure_nextline | `dir /dd/MCPnosuchfile` / next line: `echo M %*` | Marker carries `216`; prompt returns. |
| failure_nox | `-x` / next line: `dir /dd/MCPnosuchfile; echo M %*` | Error 216 and prompt; tail still skipped. |
| success_status | `pwd` / next line: `echo M %*` | Marker carries `000`. |
| scroll | `list /dd/docs/shellplus2.1.doc; echo M` | Many screens scroll, then marker and prompt remain visible. |
| stale_same_line | `dir /dd/MCPnosuchfile` / next line: `pwd; echo M %*` | `/DD` succeeds, but marker reports **216**, the previous line's error. |
| redirect_out | `echo HIDDEN >/nil; echo M` | No standalone HIDDEN output; marker appears on console after redirection ends. |
| redirect_in | `cat < /dd/startup; echo M` | Error 215; no marker. |
| pipe_bar | `echo PIPEOK \| cat; echo M` | Error 215; no marker. |
| pipe_bang | `echo PIPEOK ! cat; echo M` | Error 215; no marker. |
| subshell | `(echo CHILDOK); echo M` | CHILDOK, marker, outer session prompt. |
| redirect_in_tee | `tee /nil </dd/startup; echo M` | Startup contents including `montype r`, then marker/prompt. |
| pipe_tee | `echo PIPEOK \| tee /nil; echo M` | PIPEOK, marker, prompt. |
| bang_tee | `echo PIPEOK ! tee /nil; echo M` | PIPEOK, marker, prompt. |
| gated_success | `pwd` / **fresh prompt** / `echo M %*` | Marker `000`, fresh prompt. |
| gated_failure | `dir /dd/MCPnosuchfile` / **fresh prompt** / `echo M %*` | Error 216, marker `216`, fresh prompt. |
| gated_scroll | `list /dd/docs/shellplus2.1.doc` / **fresh prompt** / `echo M %*` | Many screens, marker `000`, fresh prompt. |

The `cat` failures are retained as actual results, not evidence that input redirection or pipes are unsupported: the `tee /nil` tests positively demonstrate both. The precise cause of those `cat` invocations' error 215 was not isolated. `/nil` is the installed null device, not an output disk file; no filesystem output redirection was performed.

For negative marker tests, the guest printed its error and returned to the prompt; observation continued four seconds after keyboard drainage without a standalone marker. Source control flow supports the interpretation that the same-line tail was skipped, rather than merely delayed.

### Unique markers and exact emitted rows

Initial exploratory runs used six random bytes, hex encoded. Final protocol runs used sixteen random bytes (128 bits). All were generated on the host after restoration, separately for every experiment.

| Experiment | Exact marker | Exact standalone marker output |
|---|---|---|
| identity | `MCPDONE4b70d1373b1c` | `MCPDONE4b70d1373b1c` |
| success | `MCPDONE2d2ea7701743` | `MCPDONE2d2ea7701743` |
| failure | `MCPDONEdf07af51e0a7` | Not emitted |
| failure_nextline | `MCPDONEfc71ab46b302` | `MCPDONEfc71ab46b302 216` |
| failure_nox | `MCPDONE32ddc2738870` | Not emitted |
| success_status | `MCPDONEc448d378b6a6` | `MCPDONEc448d378b6a6 000` |
| scroll | `MCPDONEda2764096ff6` | `MCPDONEda2764096ff6` |
| stale_same_line | `MCPDONE514255ea7787` | `MCPDONE514255ea7787 216` |
| redirect_out | `MCPDONE032e8f7a3932` | `MCPDONE032e8f7a3932` |
| redirect_in | `MCPDONE041e36928574` | Not emitted |
| pipe_bar | `MCPDONE2be89c15c784` | Not emitted |
| pipe_bang | `MCPDONEb1462bff2dac` | Not emitted |
| subshell | `MCPDONE455cbe133d5f` | `MCPDONE455cbe133d5f` |
| redirect_in_tee | `MCPDONEc013a9c9ed01` | `MCPDONEc013a9c9ed01` |
| pipe_tee | `MCPDONE2533466606d7` | `MCPDONE2533466606d7` |
| bang_tee | `MCPDONEa4d04af39805` | `MCPDONEa4d04af39805` |
| gated_success | `MCPDONE7caddd44611299d77f5abdffb9a5c9b5` | `MCPDONE7caddd44611299d77f5abdffb9a5c9b5 000` |
| gated_failure | `MCPDONE103c34a5597f6394a46adff0b9737a2b` | `MCPDONE103c34a5597f6394a46adff0b9737a2b 216` |
| gated_scroll | `MCPDONE9536b960f869eff6a843059e0c82982a` | `MCPDONE9536b960f869eff6a843059e0c82982a 000` |

## 4. Screen scrolling and detection

The hardware text buffer is deterministic for the verified native 80×25 mode: the observer reads 4000 physical bytes (character/attribute pairs) independently of the current CPU MMU mapping. Character bits used for the marker are ordinary ASCII letters and hex digits. Attribute/cursor blink does not change their underlying character sequence. Unknown glyphs elsewhere are irrelevant to matching these restricted marker rows.

The listed Shell+ document has 778 host-normalized text lines, far more than one screen. In the same-line scroll test the observer recorded 625 changed screen observations; in the final gated scroll test, 649. These are changed-view counts, **not output-line counts or a complete transcript**. The final screen showed the document's ending, the completion marker, and the prompt; early output and the initial ready marker had scrolled away.

Scrolling before the marker does not defeat this protocol: the final marker is emitted after the foreground work, remains near the bottom, and is followed by the prompt. This relies on an idle foreground console with no subsequent asynchronous writer, window switch, clear-screen operation or new command that erases the evidence. Background output or hostile prompt/marker spoofing is outside the demonstrated guarantee. A static marker or a substring match would be inadequate.

## 5. Measured latency

Timing starts immediately before posting the target command, after `os9_restore_ready` finishes. Measurements include the existing natural-keyboard input rate, guest work, and observation. They are not isolated CPU execution times.

| Final protocol run | First command's fresh prompt (ms) | Marker query sent (ms) | Marker first observed (ms) | Marker + final prompt (ms) | Query-to-confirmation (ms) |
|---|---:|---:|---:|---:|---:|
| gated_success | 1719.8 | 1720.1 | 7176.8 | 7210.1 | 5490.0 |
| gated_failure | 2971.5 | 2971.7 | 8434.5 | 8465.2 | 5493.5 |
| gated_scroll | 12966.6 | 12966.7 | 18444.5 | 18477.5 | 5510.8 |

The final prompt appeared 30.7–33.2 ms after the marker's first observed appearance. The status-query round trip added approximately **5.49–5.51 seconds** with the current natural-keyboard settings. That is the practical status-confirmation cost; it must not be advertised as a 30 ms command-execution API.

The observer sampled at frame boundaries and the host polled at 10 ms. The maximum measured host poll gaps in the three final runs were 13.0, 13.8 and 13.4 ms. The actual event-to-observation delay was not independently instrumented at the guest exit instruction; it includes up to a frame, polling/scheduling and file handoff. These are observed timings, not hard real-time bounds.

Keyboard drainage is demonstrably insufficient: in `gated_success`, `coco_type` returned at 733.2 ms but the command's fresh prompt was not observed until 1719.8 ms. In `gated_scroll`, typing drained at 3418.5 ms while the command's fresh prompt appeared at 12966.6 ms.

## 6. Status meaning and restrictions

`%*` can encode the previous completed input line's status on a **new line**. This fixture returned `000` for the tested successful commands and `216` for the failing directory operation. The error is not inferred by scraping an English message: it is emitted by Shell+'s variable expansion.

Treat this as a Shell+ line status, including parsing, launch, child error or signal conditions; do not call it an unrestricted POSIX exit status. Pipeline-wide status, background-job status, general groups and arbitrary interactive applications are not established by these tests. The pipe tests only verify successful streaming and continuation.

The status query itself is another successful line and can replace the previous status. If its response is lost, blindly querying `%*` again can report the query's status. Preserve the first result when observed; otherwise report status unknown and recover explicitly. Never insert a probe such as `pwd`, `date`, prompt-setting or echo between the target line and the status query.

## 7. Exact protocol proposed for the next milestone

1. Restore/verify the fixture with `os9_restore_ready`. Record its unique session prompt and observation epoch. Acquire exclusive input/machine-operation ownership.
2. Generate `requestId = randomBytes(16).toString("hex")` on the host. Set `marker = "MCPDONE" + requestId`. Use a fresh ID for each request, including after restoration.
3. Arm physical-text observation before posting one validated foreground command plus Enter. Initially allow known noninteractive commands such as `pwd`, `date`, `mdir`, `dir` and `procs`; reject background operations, terminal reconfiguration, shell replacement, arbitrary multi-line input and unvalidated shell metacharacters.
4. Require evidence of new input/activity after injection and then a newly returned **exact** session prompt as the last nonblank row. In the live tests, the prompt row changed while input was entered and then returned. An unchanged preexisting prompt is insufficient. If this transition cannot be established, return uncertainty/timeout rather than posting more input.
5. Without another guest command, send exactly `echo ` + marker + ` %*` + Enter as a separate input line. Do not append it with a semicolon; do not prequeue it while the foreground command could consume standard input.
6. Require a complete standalone row matching the exact request marker, one space and three decimal digits, followed by a newly returned exact session prompt. Reject echoed input: its row begins with the session prompt and `echo`, and contains literal `%*`, not the accepted standalone result format.
7. Return completion and status separately: for example `completed: true`, `promptReturned: true`, `statusAvailable: true`, `shellStatus: 216`, `success: false`. Preserve `outputComplete: false` or omit command output entirely; this milestone establishes no transcript guarantee.
8. Use one host monotonic deadline covering command and status-query phases. Missing prompt: do not inject the query. Missing marker or final prompt: return incomplete/unknown status, retain observations and require explicit recovery. Do not automatically replay the target command.

The 39-character marker plus space and three digits fits one 80-column output row. With the current 20-character session prompt, the status-query input echo also fits one row. Use the alphanumeric framing tested here; underscore-heavy decorations are unnecessary and would add another font/encoding case to validate.

Foreground completion is evidence under these restrictions, not proof against arbitrary guest programs intentionally emulating shell prompts. Extending to interactive programs, asynchronous writers, pipelines with component statuses, or hostile output requires stronger process/transport framing. No new guest helper or transport is required for the tested protocol.

## 8. Preservation and reproducibility

MAME stopped cleanly after each session. Before and after SHA-256 hashes matched for all repository media and the checkpoint; both temporary media copies also remained byte-identical:

| File | Before = after SHA-256 |
|---|---|
| `media/63SDC.VHD` | `db2f0f444073d3b88a3610a63ec9f7d2e2dd4299347181faa6b2b2aa9637de2c` |
| `media/63SDC-MCP-DEV.VHD` | `4c6bdc0cd2f68aa57a9c75f75f15fe429f30926aa522917dd8d14aa1b9ae622e` |
| `media/63EMU.DSK` | `9a51adb8656b9f003c7f822352c291487362f1b4c43aac1a16672d5d2bcb4c40` |
| `MCP/states/coco3h/nos9_ready_v2.sta` | `a0f4ee094db471c596b9ecb9826922f80d982d2571fe7e72672a255ebca2effa` |

Read-only inspection commands used the executable `/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9`, from the repository root:

```text
os9 dir media/63SDC-MCP-DEV.VHD,SOURCECODE/ASM/SHELL
os9 list media/63SDC-MCP-DEV.VHD,SOURCECODE/ASM/SHELL/shellplus2.2a.asm
os9 dir media/63SDC-MCP-DEV.VHD,CMDS
os9 ident media/63SDC-MCP-DEV.VHD,CMDS/shell
os9 dump media/63SDC-MCP-DEV.VHD,CMDS/shell
os9 ident media/63EMU.DSK,OS9Boot
```

The first attempted CMDS directory command used `media/63SDC-MCP-DEV.VHD/CMDS` instead of the comma separator and returned ToolShed error 214; it was corrected to the command above. Previously extracted bundled Shell+ documentation was reused from the preceding design investigation.

The temporary harness, read-only observer, launch JSON, exact MCP envelopes, PNGs, frame-transition traces and experiment summaries remain under `/private/tmp/nos9-completion/` for this session. They are not added as permanent architecture assets. The tables above retain the tested commands, exact markers/status results and timing evidence in Git-sized documentation. Existing repository changes were preserved; only this report was created for this task.
