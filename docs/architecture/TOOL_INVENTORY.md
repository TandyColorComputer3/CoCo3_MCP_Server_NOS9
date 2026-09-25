# MCP tool inventory

The following 14 names are the complete TOOL_INFO and input-schema set registered by [tools.ts:32-80, 470-529](../../MCP/src/tools.ts). All successful results except snapshots are MCP text content, usually a JSON string. Errors are text content with isError=true. The bridge commands named below are internal JSON commands, not additional MCP tools.

| MCP tool | Inputs | Node handler and internal path | Result and effects |
| --- | --- | --- | --- |
| coco_start | none | tools.ts:227 → bridge.start → mame.start → ping | JSON ok, alreadyRunning, pid, bridge. Opens listener and may spawn MAME. |
| coco_stop | none | tools.ts:252 → mame.stop → bridge.stop | JSON ok. Stops owned MAME child and listener. |
| coco_status | none | tools.ts:259 → local process status → ping → status | JSON running, pid, bridge; when reachable also driver, flop1, posting, empty. Read-only. |
| coco_mount_flop | path, optional drive flop1/flop2 | tools.ts:288 → resolveDiskPath → mount | JSON briefname and filename. Mounts a host disk image; may expose it to guest writes. |
| coco_unmount_flop | optional drive flop1/flop2 | tools.ts:304 → unmount | JSON briefname. Unloads image. |
| coco_type | text, optional pressEnter (default true) | tools.ts:315 → type → repeated wait_idle | JSON queued and actual text sent. Changes guest input/state. |
| coco_snapshot | none | tools.ts:336 → snapshot | PNG image content and absolute file path. Replaces snapshots/current.png on the primary path. |
| coco_soft_reset | none | tools.ts:371 → soft_reset | JSON reset=soft. Resets emulated system after Lua reply. |
| coco_list_images | none | tools.ts:381 → list_images | JSON array with image tags, names, filenames, exists. Read-only. |
| coco_build_disk | dskPath, array of hostPath/cocoName/kind bas/bin/data | tools.ts:391 → toolchain.buildDisk → decb subprocess | JSON dskPath and completed step strings. Creates/modifies a host disk image; no MAME or bridge call. |
| coco_read_memory | hex address, integer length | tools.ts:404 → read_mem | JSON data as space-separated uppercase hex bytes. Reads current CPU program map. |
| coco_write_memory | hex address, space-separated hex data | tools.ts:416 → write_mem | JSON bytes count. Writes current CPU program map. |
| coco_save_state | name | tools.ts:428 → save_state → host file poll | JSON scheduled, name, fileFound, file. Writes a MAME state file. |
| coco_load_state | name | tools.ts:441 → host file check → load_state | JSON scheduled, name. Schedules machine restore; no completion acknowledgement. |

## Common validation and semantics

- MCP schemas use Zod; the handlers additionally validate drive selection, memory addresses/length, and state names. State names reject empty strings and slash/backslash; disk mounting resolves storage/ paths from the MCP root and other relative paths from the server's process working directory. [Source: tools.ts:94-125, 211-224, 470-529](../../MCP/src/tools.ts).
- coco_type appends {ENTER} unless pressEnter=false or text already ends with that exact token; queue idle is checked for at most 15 seconds. It is not a guest-command completion check. [Source: tools.ts:315-334](../../MCP/src/tools.ts).
- coco_save_state has a two-second file poll. A found nonempty file establishes only that a file exists; it does not provide a MAME save-complete event. coco_load_state checks existence before scheduling, then returns immediately. [Source: tools.ts:193-209, 428-458](../../MCP/src/tools.ts); [MAME running machine](https://docs.mamedev.org/luascript/ref-core.html).
- The inventory is validated by [stdio-list.test.ts](../../MCP/test/stdio-list.test.ts) and [tools.test.ts](../../MCP/test/tools.test.ts). The tests mostly exercise injected fakes and source patterns, not a NitrOS-9 runtime.
