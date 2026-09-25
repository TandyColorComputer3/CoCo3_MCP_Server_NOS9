# NitrOS-9 gap analysis

This is an evidence ledger and risk list, not a design. The repository has no NitrOS-9 implementation, boot fixture, process inspector, OS-9 command runner, or serial/DriveWire bridge. MCP/Documents and DOCS_INDEX.md are absent, so OS-9-specific assertions below are framed as questions to verify against primary manuals and live behavior before implementation. [Repository scan](../../MCP/src), [AGENTS.md](../../AGENTS.md).

## Reusable pieces

| Existing boundary | Why it can be reused | Present limit |
| --- | --- | --- |
| MCP server and handler injection | Tool registration and dependencies are guest-neutral. [index.ts](../../MCP/src/index.ts), [tools.ts:461-529](../../MCP/src/tools.ts) | Tool names/descriptions currently say CoCo or BASIC. |
| Node ↔ Lua request/reply bridge | Commands can reach MAME device APIs without embedding OS-specific logic in the transport. [protocol.ts](../../MCP/src/protocol.ts), [bridge-server.ts](../../MCP/src/bridge-server.ts) | No event stream or guest transaction ID. |
| MAME lifecycle | Starts an owned coco3 machine and detects bridge reachability. [mame-process.ts](../../MCP/src/mame-process.ts) | Does not classify guest boot stage. |
| Natural keyboard and screen | Can interact with whichever guest runs on the machine. [bridge.lua:393-404, 445-485](../../MCP/scripts/bridge.lua) | No command completion or text channel. |
| Image-device enumeration and mounting | Works at MAME device level, independent of filesystem contents. [bridge.lua:343-391](../../MCP/scripts/bridge.lua) | Only flop1/flop2 mounting is exposed. |
| MAME state wrapper | Restores emulated machine state. [tools.ts:428-458](../../MCP/src/tools.ts) | Host disk files and load completion are not coordinated. |

## Disk BASIC coupling

- coco_build_disk always invokes decb dskini -3 and decb copy. Its kinds are bas/bin/data; bas uses copy -t, which the code describes as BASIC tokenization. There is no OS-9 RBF image-building branch, despite ToolShed itself containing separate os9 tooling. [toolchain.ts:28-40](../../MCP/src/toolchain.ts), [tools.ts:62-65](../../MCP/src/tools.ts), [ToolShed repository](https://github.com/nitros9project/toolshed).
- README storage conventions and examples center BASIC source and .BIN files. They are guidance, not a generic guest filesystem model. [README.md](../../README.md), [storage/README.md](../../MCP/storage/README.md).
- The tool description's $0400 text-screen note is conditional on a CoCo 1/2-compatible map. No tool actually extracts BASIC screen text from memory. [tools.ts:66-71](../../MCP/src/tools.ts).
- The emulator itself is hard-coded to coco3 with fdc, but those choices do not prove the guest is Disk BASIC. There is no BASIC-prompt detection in code. [mame-process.ts:25-50](../../MCP/src/mame-process.ts), [tools.ts](../../MCP/src/tools.ts).

## Integration risks and unanswered questions

| Risk | Current evidence | Question to settle before implementation |
| --- | --- | --- |
| Boot/runtime state | coco_start proves a Lua ping, not that a disk booted or a shell is ready. Disks are mounted only by a later tool call; no boot image is supplied on MAME's command line. | Which boot sequence and observable guest milestones are reliable for the intended NitrOS-9 image? |
| OS-9 process isolation | No process ID, task state, module, or per-process memory context is exposed. | Which OS-9 process should a read or command refer to, and how can that context be observed without guessing? |
| Logical versus physical memory | The only memory tool reads/writes :maincpu program space at a 16-bit address. It exposes no physical bank or MMU-register snapshot. | How does that view relate to the selected OS-9 process at the moment of access? Verify with CoCo 3 and OS-9 manuals plus live experiments. |
| Memory access side effects | The Lua bridge calls ordinary program-space read_u8/write_u8, with no side-effect suppression or guest pause. | Can a diagnostic read touch mapped I/O or observe a changing process/MMU context? |
| Disk persistence across save restore | MAME save/load is asynchronous; the wrapper does not copy, hash, freeze, or verify mounted disk image bytes, nor compare mounts before/after load. | Which parts of external media state does the chosen MAME build restore, and what happens after guest writes between save and load? |
| Command/output capture | coco_type waits only for the natural-keyboard queue; coco_snapshot yields pixels. No structured guest stdout, prompt, exit status, or command boundary is available. | What observable channel can delimit a command and distinguish its output from other guest activity? |
| Floppy image format | The only image builder is decb-oriented; mounting itself does not inspect filesystem format. | Which verified OS-9 image layout and boot artifacts will be used? |
| Concurrent access | One TCP socket is retained, calls are request/reply, and output capture is screen based. | How will simultaneous guest activity, multiple clients, or long operations be attributed? |
| State synchronization | load returns scheduled; a subsequent screenshot may race a restore. save fileFound checks only a nonempty file and does not distinguish a newly written state from an older state of the same name. | What observation proves save or restore and guest readiness are complete? |
| DriveWire/serial transport | No serial device, port, socket stream, DriveWire protocol, or host-side service appears in the bridge or launch args. | Which MAME device and transport path would the intended setup use? Confirm against MAME and DriveWire primary sources. |
| Mutability/safety | coco_write_memory, mount, build_disk, type, and state load change guest or host state. No guest-mode guard exists. | Which operations are safe in each boot/runtime state? |

## Scope of evidence

The risk statements describe missing guarantees in this implementation; they do not assert that MAME or NitrOS-9 cannot supply them. The next phase needs local manuals, a known NitrOS-9 boot image, and targeted live tests before hardware-specific or OS-specific behavior is encoded.
