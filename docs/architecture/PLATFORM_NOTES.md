# Platform notes

This file separates observed code branches from assumptions. The common MAME Lua API and the default statename machine subdirectory are cross-platform behaviors, not macOS special cases. [MAME statename documentation](https://docs.mamedev.org/commandline/commandline-all.html#core-state-playback-options).

| Area | Windows-specific | POSIX-specific | Shared/platform-neutral |
| --- | --- | --- | --- |
| Process stop | taskkill /PID … /T /F when platform=win32 and PID exists | SIGTERM to the owned child otherwise | Child-process tracking, idempotent start, log file, unexpected-exit callback. [mame-process.ts:110-181](../../MCP/src/mame-process.ts) |
| Executable/path config | Windows absolute tool paths are recognized by path.win32.isAbsolute even on a POSIX host; windowsHide is passed to child spawns | Native absolute and relative paths use node:path behavior | MAME_PATH, MAME_ROMPATH, DECB_PATH, BRIDGE_PORT, COCO_RAM; .env plus process environment. [config.ts](../../MCP/src/config.ts) |
| MAME launch | README and mame/README.md give mame.exe examples | The same Node spawn accepts a macOS/Linux executable path; no POSIX-only launch flag | Fixed coco3 arguments and directory options. [mame-process.ts:25-50](../../MCP/src/mame-process.ts) |
| Bridge | None in request/reply logic | None in request/reply logic | Loopback TCP, newline JSON, Lua emu.file socket and frame notifier. [bridge-server.ts](../../MCP/src/bridge-server.ts), [bridge.lua](../../MCP/scripts/bridge.lua) |
| State file | Native path separators used by Node/MAME | Native path separators used by Node/MAME | Default %g gives states/coco3/name.sta. [tools.ts:193-209](../../MCP/src/tools.ts) |
| ToolShed | Example install path toolshed/decb.exe | Executable name/path supplied by DECB_PATH | spawn without a shell; common command plan. [toolchain.ts](../../MCP/src/toolchain.ts) |

## Portability edges visible in code

- resolveToolPath recognizes Windows absolute paths independently of the host OS. resolveDiskPath does not perform the same Windows-absolute check on a POSIX host; a Windows-style disk path passed to a POSIX server would be treated as relative. This is a code observation, not a request to change it. [Source: config.ts:42-50](../../MCP/src/config.ts), [tools.ts:211-224](../../MCP/src/tools.ts).
- Windows process stop waits for taskkill's exit; POSIX stop sends SIGTERM and clears the child reference without awaiting the child's exit. The server reports ownership state, not proof of process termination. [Source: mame-process.ts:161-181](../../MCP/src/mame-process.ts).
- MAME's cwd is the executable directory, whereas decb's cwd is the MCP root. ToolShed request paths are passed through to decb rather than resolved by resolveDiskPath; callers relying on storage/ paths get the intended behavior because decb runs from the MCP root. [Source: mame-process.ts:142-147](../../MCP/src/mame-process.ts), [index.ts:69](../../MCP/src/index.ts), [toolchain.ts:44-87](../../MCP/src/toolchain.ts).
- Lua probes brief_instance_name, then briefname, and chooses the newer frame notifier API when available, falling back to register_frame. These are MAME-version compatibility branches rather than OS-specific ones. [Source: bridge.lua:270-300, 628-634](../../MCP/scripts/bridge.lua).
- The root README says Windows 10+ setup and Node 20+, while MCP/package.json accepts Node 18+. There is no repository-wide macOS or Linux integration test. Unit tests cover mocked Windows and POSIX termination, path config, exact MAME arguments, protocol, and mocked tool handlers. [README.md](../../README.md), [MCP/package.json](../../MCP/package.json), [process-manager.test.ts](../../MCP/test/process-manager.test.ts).

The current live macOS setup used Ample's MAME binary in the preceding validation, but the architecture does not depend on Ample-specific APIs: it launches the configured executable and speaks to MAME Lua over loopback TCP.
