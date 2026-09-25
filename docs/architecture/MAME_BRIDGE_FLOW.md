# MAME bridge flow

## Transport and dispatch

MCP tool call → registered handler in tools.ts → BridgeClient.request(command, params) → one JSON object plus newline on a 127.0.0.1 TCP socket → bridge.lua command table → MAME Lua API → response JSON with matching id, ok, and result/error → handler-formatted MCP content. Node owns the listener; Lua connects as a client through emu.file("rw") using socket.127.0.0.1:port. The Lua frame notifier reads 1024-byte chunks, buffers to newline, dispatches with pcall, and writes one reply. Node correlates requests by incremental IDs and times out pending calls; a new connection replaces the old socket. [Source: protocol.ts](../../MCP/src/protocol.ts), [bridge-server.ts](../../MCP/src/bridge-server.ts), [bridge.lua:502-634](../../MCP/scripts/bridge.lua).

The protocol has request/reply only: no unsolicited guest-output event, boot milestone event, disk-change notification, or state-load-complete event. Lua JSON parsing/serialization is implemented in bridge.lua rather than an external library. Malformed Lua-side input is dropped without a reply; malformed replies are logged by Node. [Source: bridge.lua:1-188, 540-634](../../MCP/scripts/bridge.lua), [bridge-server.ts:30-119](../../MCP/src/bridge-server.ts).

## Every tool, end to end

| MCP handler → Node work | Bridge command | bridge.lua → MAME operation |
| --- | --- | --- |
| coco_start → start listener, spawn MAME, poll ping | ping | cmd_ping → emu.time(), proving Lua/bridge reachability |
| coco_stop → stop owned child and listener | none | No Lua command; Node process control |
| coco_status → child status, ping, then status | ping, status | cmd_status → manager.machine.system.name, flop1 image filename, natural-keyboard flags; Lua also queries RAM option, but Node omits it from MCP response |
| coco_mount_flop → validate drive and resolve host path | mount | cmd_mount → find image by brief/instance name → image:load(path), propagating error |
| coco_unmount_flop → validate drive | unmount | cmd_unmount → image:unload() |
| coco_type → append Enter as requested; poll idle | type, wait_idle | cmd_type → natkeyboard.in_use=true and post_coded(text); cmd_wait_idle → empty and not is_posting |
| coco_snapshot → remove current.png, request capture, await PNG, return bytes | snapshot | cmd_snapshot → first machine screen:snapshot(path); fallback machine.video:snapshot() |
| coco_soft_reset → forward request | soft_reset | cmd_soft_reset replies, then dispatch calls machine:soft_reset() |
| coco_list_images → forward request | list_images | cmd_list_images → enumerate manager.machine.images and report tags, names, filename, exists |
| coco_build_disk → Toolchain.buildDisk → host decb subprocess | none | No Lua/MAME operation |
| coco_read_memory → normalize address and length | read_mem | cmd_read_mem → maincpu program space:read_u8 for each address |
| coco_write_memory → normalize address | write_mem | cmd_write_mem → validate all hex byte tokens, then program space:write_u8 |
| coco_save_state → schedule then poll states/coco3/name.sta | save_state | cmd_save_state → manager.machine:save(name) |
| coco_load_state → check states/coco3/name.sta then schedule | load_state | cmd_load_state → manager.machine:load(name) |

Handler locations: [tools.ts:226-459](../../MCP/src/tools.ts). Lua locations: [bridge.lua:308-519](../../MCP/scripts/bridge.lua).

## MAME launch and configuration

The executable comes from MAME_PATH; ROM search path from MAME_ROMPATH. Node invokes the coco3 driver with -window, -skip_gameinfo, -natural, -nomouse, -mouse_device none, -ext fdc, -ramsize COCO_RAM, -rompath, -autoboot_script bridge.lua, -autoboot_delay 0, -snapshot_directory, and -state_directory. It sets MAME's cwd to the executable's directory and passes BRIDGE_PORT in the environment; scripts/bridge.port is also written because Lua environment access may be unavailable. [Source: mame-process.ts:25-50, 125-159](../../MCP/src/mame-process.ts), [config.ts](../../MCP/src/config.ts).

## Keyboard, screen, state, and memory boundaries

- Keyboard uses MAME's natural keyboard coded-text API. MAME documents brace codes such as {ENTER}; the repository's tool description also mentions {BREAK}, but {BREAK} is absent from the currently published MAME recognized-code list, so its behavior needs live verification. Polling empty/is_posting proves queue drainage only. [Source: bridge.lua:393-404](../../MCP/scripts/bridge.lua); [MAME natural keyboard](https://docs.mamedev.org/luascript/ref-input.html).
- Snapshot uses a named PNG through screen:snapshot(path). If that fails, video:snapshot() uses MAME's configured name, so Node scans for a recent PNG. Both paths return image bytes, not machine-readable text. [Source: tools.ts:336-369](../../MCP/src/tools.ts), [bridge.lua:445-485](../../MCP/scripts/bridge.lua); [MAME screen device](https://docs.mamedev.org/luascript/ref-devices.html), [video manager](https://docs.mamedev.org/luascript/ref-core.html).
- Save/load are scheduled MAME operations. MAME's default statename %g makes the state file path states/coco3/name.sta for this driver on all host platforms. The Node lookup checks that exact nonempty file. Load returns scheduled without a guest-ready or completion signal. [Source: tools.ts:193-209, 428-458](../../MCP/src/tools.ts), [bridge.lua:492-500](../../MCP/scripts/bridge.lua); [MAME statename](https://docs.mamedev.org/commandline/commandline-all.html#core-state-playback-options), [running machine](https://docs.mamedev.org/luascript/ref-core.html).
- Memory access uses manager.machine.devices[":maincpu"].spaces["program"] and read_u8/write_u8. The API accepts 0000–FFFF only, without selecting a process or physical RAM bank. The repository labels this the current 64K MMU window; exact CoCo 3 mapping and OS-9 context behavior require manuals or live validation before stronger claims. [Source: tools.ts:94-113, 404-426](../../MCP/src/tools.ts), [bridge.lua:304-306, 406-443](../../MCP/scripts/bridge.lua); [MAME address-space API](https://docs.mamedev.org/luascript/ref-mem.html).

## Floppy and ToolShed boundaries

Mounting takes flop1/flop2, resolves a host path, finds a MAME image device, then calls image:load(path) without unloading first. Unmount calls image:unload(). List-images enumerates all devices, while status reports only flop1. A mounted image is a host file; the bridge does not copy or snapshot its contents. [Source: tools.ts:288-314, 381-389](../../MCP/src/tools.ts), [bridge.lua:270-302, 343-391](../../MCP/scripts/bridge.lua); [MAME image interface](https://docs.mamedev.org/luascript/ref-devices.html).

ToolShed integration is a separate host-side path: planDecbSteps creates decb dskini -3 output, then decb copy -t for kind=bas or decb copy for bin/data. Each step is a spawned process; nonzero exit aborts and surfaces stderr, then stdout, then the exit code. The repository calls the result a Disk BASIC image; this wrapper has no OS-9 image-format branch. [Source: toolchain.ts:28-87](../../MCP/src/toolchain.ts); [ToolShed project](https://github.com/nitros9project/toolshed).
