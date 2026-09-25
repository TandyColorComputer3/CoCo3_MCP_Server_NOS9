# Current architecture

This is an archaeology record of the repository as inspected on 2026-09-25. It describes the current implementation, not a NitrOS-9 design. This audit changes documentation only.

## Evidence and limits

- Repository evidence: [entry point](../../MCP/src/index.ts), [tool handlers](../../MCP/src/tools.ts), [MAME controller](../../MCP/src/mame-process.ts), [TCP bridge](../../MCP/src/bridge-server.ts), [protocol](../../MCP/src/protocol.ts), [Lua script](../../MCP/scripts/bridge.lua), and [ToolShed wrapper](../../MCP/src/toolchain.ts).
- The manuals and index required by [AGENTS.md](../../AGENTS.md) are absent from this checkout: MCP/Documents/ and DOCS_INDEX.md do not exist. No CoCo register, OS-9 syscall, or process-memory mapping is asserted here without a source. Emulator API facts below use the linked primary MAME documentation.
- The [package](../../MCP/package.json) declares Node 18+, TypeScript ES2022/Node16, MCP SDK 1.30.0, and a fixed Node test list via tsx. The root README says Node 20+; that is a documentation discrepancy, not a runtime check.

## Runtime topology

MCP client ⇄ stdio ⇄ McpServer in Node ⇄ local TCP newline-delimited JSON ⇄ Lua autoboot script in MAME ⇄ emulated devices.

The Node server also launches MAME as a child process and invokes ToolShed decb as a separate host subprocess for disk building. Disk building never enters the Lua bridge. There are 14 registered MCP tools and no repository registration of MCP resources or prompts. [Source: index.ts:17-20, 35-74](../../MCP/src/index.ts), [tools.ts:32-80, 517-529](../../MCP/src/tools.ts).

At startup, Node resolves the MCP directory, merges MCP/.env with process environment (process environment wins), creates storage, state, snapshot, work, and log directories, writes scripts/bridge.port, then connects a StdioServerTransport. The MAME controller and TCP listener are constructed but MAME is launched by coco_start. An unexpected MAME exit closes the bridge listener. [Source: index.ts:35-74](../../MCP/src/index.ts).

## MAME session

coco_start starts the TCP listener before spawning MAME, then polls Lua ping for up to 15 seconds. Failure stops both sides. The controller tracks only its own child process; status is not an OS-wide MAME discovery mechanism. The launch is fixed to the coco3 driver, windowed mode, natural keyboard, no mouse, fdc extension, configurable RAM and ROM path, Lua autoboot script, snapshot directory, and state directory. It writes MAME stdout and stderr to logs/mame.log. [Source: tools.ts:226-257](../../MCP/src/tools.ts), [mame-process.ts:25-50, 125-176](../../MCP/src/mame-process.ts).

Config paths and defaults are in [config.ts](../../MCP/src/config.ts): MAME_PATH and MAME_ROMPATH are required at launch; DECB_PATH defaults to decb; BRIDGE_PORT defaults to 18765; COCO_RAM defaults to 512K. The Lua script reads scripts/bridge.port first, then BRIDGE_PORT, then 18765. [Source: bridge.lua:219-267](../../MCP/scripts/bridge.lua).

## Main subsystems

| Subsystem | Current behavior | Boundary |
| --- | --- | --- |
| Keyboard | Posts coded keystrokes through MAME natural keyboard and polls until its queue is idle. | Idle means delivery ended, not that a guest command finished. |
| Screen | Requests a MAME screen snapshot, falls back to video snapshot, reads a PNG and returns image plus path. | No text, cursor, terminal, or command output model. |
| Memory | Reads/writes bytes through the main CPU program address space at 16-bit addresses. | Current mapped CPU view; no physical RAM or OS-9 process selector. |
| Floppy | Finds flop1/flop2 among MAME image devices and loads/unloads host image paths. | No guest filesystem or write-protection policy. |
| State | Schedules MAME machine save/load and checks states/coco3/name.sta. | Load completion and host disk image consistency are not checked. |
| Disk build | Runs decb dskini then copy operations against host paths. | Disk BASIC oriented; not a generic NitrOS-9 filesystem service. |

Details and per-tool paths are in [TOOL_INVENTORY.md](TOOL_INVENTORY.md) and [MAME_BRIDGE_FLOW.md](MAME_BRIDGE_FLOW.md).

## Reusable boundaries and current coupling

The stdio MCP registration pattern, dependency-injected handlers, request-ID bridge, MAME process controller, screen capture, natural keyboard input, image-device enumeration, and scheduled state wrappers are potentially reusable without assuming a BASIC prompt. They expose hardware-level interaction rather than a particular guest command interpreter. [Source: tools.ts:461-529](../../MCP/src/tools.ts), [bridge-server.ts](../../MCP/src/bridge-server.ts).

The launch hard-codes coco3 and fdc. Storage guidance and coco_build_disk assume Disk BASIC-oriented filenames and decb operations. The memory description mentions a CoCo 1/2-compatible text screen at $0400, but the implementation does not decode screen text at that address. No code detects whether BASIC, NitrOS-9, or another guest is running. [Source: mame-process.ts:25-50](../../MCP/src/mame-process.ts), [toolchain.ts:28-40](../../MCP/src/toolchain.ts), [tools.ts:66-71](../../MCP/src/tools.ts).

## Primary emulator references

- [MAME Lua running machine: asynchronous save/load and soft reset](https://docs.mamedev.org/luascript/ref-core.html)
- [MAME Lua natural keyboard](https://docs.mamedev.org/luascript/ref-input.html)
- [MAME Lua screen and image devices](https://docs.mamedev.org/luascript/ref-devices.html)
- [MAME Lua address spaces](https://docs.mamedev.org/luascript/ref-mem.html)
- [MAME command line statename convention](https://docs.mamedev.org/commandline/commandline-all.html#core-state-playback-options)
