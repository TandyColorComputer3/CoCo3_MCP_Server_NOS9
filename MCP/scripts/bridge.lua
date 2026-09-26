-- CoCo 3 MCP bridge. Node listens. This script connects with emu.file.

local JSON_NULL = {}
local ARRAY = {}

local function mark_array(items)
  return setmetatable(items, ARRAY)
end

local function is_array(value)
  return getmetatable(value) == ARRAY
end

local function json_escape(text)
  text = string.gsub(text, "\\", "\\\\")
  text = string.gsub(text, "\"", "\\\"")
  text = string.gsub(text, "\r", "\\r")
  text = string.gsub(text, "\n", "\\n")
  text = string.gsub(text, "\t", "\\t")
  return text
end

local function json_encode(value)
  local kind = type(value)
  if value == JSON_NULL or value == nil then
    return "null"
  elseif kind == "boolean" then
    if value then
      return "true"
    end
    return "false"
  elseif kind == "number" then
    return string.format("%.14g", value)
  elseif kind == "string" then
    return "\"" .. json_escape(value) .. "\""
  elseif kind == "table" then
    if is_array(value) then
      local parts = {}
      for index = 1, #value do
        parts[index] = json_encode(value[index])
      end
      return "[" .. table.concat(parts, ",") .. "]"
    end
    local parts = {}
    for key, item in pairs(value) do
      if type(key) == "string" and item ~= nil then
        parts[#parts + 1] = json_encode(key) .. ":" .. json_encode(item)
      end
    end
    return "{" .. table.concat(parts, ",") .. "}"
  end
  return "null"
end

local function json_decode(text)
  local index = 1
  local length = #text
  local parse_value

  local function peek()
    return string.sub(text, index, index)
  end

  local function skip()
    while true do
      local char = peek()
      if char == " " or char == "\t" or char == "\r" or char == "\n" then
        index = index + 1
      else
        return
      end
    end
  end

  local function parse_string()
    index = index + 1
    local parts = {}
    while index <= length do
      local char = peek()
      if char == "\"" then
        index = index + 1
        return table.concat(parts)
      elseif char == "\\" then
        local escaped = string.sub(text, index + 1, index + 1)
        if escaped == "\"" or escaped == "\\" or escaped == "/" then
          parts[#parts + 1] = escaped
        elseif escaped == "n" then
          parts[#parts + 1] = "\n"
        elseif escaped == "r" then
          parts[#parts + 1] = "\r"
        elseif escaped == "t" then
          parts[#parts + 1] = "\t"
        elseif escaped == "u" then
          local hex = string.sub(text, index + 2, index + 5)
          local code = tonumber(hex, 16) or 63
          if utf8 and utf8.char then
            parts[#parts + 1] = utf8.char(code)
          else
            parts[#parts + 1] = "?"
          end
          index = index + 4
        else
          parts[#parts + 1] = escaped
        end
        index = index + 2
      else
        parts[#parts + 1] = char
        index = index + 1
      end
    end
    error("unterminated string")
  end

  local function parse_number()
    local start = index
    if peek() == "-" then
      index = index + 1
    end
    while string.match(peek(), "%d") do
      index = index + 1
    end
    if peek() == "." then
      index = index + 1
      while string.match(peek(), "%d") do
        index = index + 1
      end
    end
    local exponent = peek()
    if exponent == "e" or exponent == "E" then
      index = index + 1
      if peek() == "+" or peek() == "-" then
        index = index + 1
      end
      while string.match(peek(), "%d") do
        index = index + 1
      end
    end
    return tonumber(string.sub(text, start, index - 1))
  end

  local function parse_object()
    index = index + 1
    local object = {}
    skip()
    if peek() == "}" then
      index = index + 1
      return object
    end
    while true do
      skip()
      local key = parse_string()
      skip()
      if peek() ~= ":" then
        error("expected colon")
      end
      index = index + 1
      object[key] = parse_value()
      skip()
      local char = peek()
      if char == "}" then
        index = index + 1
        return object
      elseif char == "," then
        index = index + 1
      else
        error("expected comma")
      end
    end
  end

  local function parse_array()
    index = index + 1
    local items = mark_array({})
    skip()
    if peek() == "]" then
      index = index + 1
      return items
    end
    while true do
      items[#items + 1] = parse_value()
      skip()
      local char = peek()
      if char == "]" then
        index = index + 1
        return items
      elseif char == "," then
        index = index + 1
      else
        error("expected comma")
      end
    end
  end

  parse_value = function()
    skip()
    local char = peek()
    if char == "\"" then
      return parse_string()
    elseif char == "{" then
      return parse_object()
    elseif char == "[" then
      return parse_array()
    elseif string.sub(text, index, index + 3) == "true" then
      index = index + 4
      return true
    elseif string.sub(text, index, index + 4) == "false" then
      index = index + 5
      return false
    elseif string.sub(text, index, index + 3) == "null" then
      index = index + 4
      return JSON_NULL
    end
    return parse_number()
  end

  return parse_value()
end

local function script_directory()
  if not debug or not debug.getinfo then
    return nil
  end
  local info = debug.getinfo(1, "S")
  if not info or type(info.source) ~= "string" then
    return nil
  end
  local source = info.source
  if string.sub(source, 1, 1) == "@" then
    source = string.sub(source, 2)
  end
  return string.match(source, "^(.*[/\\])")
end

local function read_port_file()
  local dir = script_directory()
  if not dir then
    return nil
  end
  local path = dir .. "bridge.port"
  local ok, line = pcall(function()
    if not io or not io.open then
      return nil
    end
    local handle = io.open(path, "r")
    if not handle then
      return nil
    end
    local text = handle:read("*l")
    handle:close()
    return text
  end)
  if not ok or type(line) ~= "string" then
    return nil
  end
  return string.match(line, "^%s*(%d+)%s*$")
end

local function bridge_port()
  local from_file = read_port_file()
  if from_file then
    return from_file
  end
  local ok, value = pcall(function()
    return os.getenv("BRIDGE_PORT")
  end)
  if ok and type(value) == "string" and value ~= "" then
    return value
  end
  return "18765"
end

local floppy_drives = { flop1 = true, flop2 = true }

-- MAME 0.289+ exposes brief_instance_name (not briefname).
local function image_briefname(image)
  local brief = image.brief_instance_name or image.briefname
  if brief ~= nil and tostring(brief) ~= "" then
    return tostring(brief)
  end
  return ""
end

local function image_instancename(image)
  local name = image.instance_name
  if name ~= nil and tostring(name) ~= "" then
    return tostring(name)
  end
  return ""
end

local function find_image(brief)
  local machine = manager.machine
  if not machine or not machine.images then
    return nil
  end
  for tag, image in pairs(machine.images) do
    if image_briefname(image) == brief or image_instancename(image) == brief then
      return image, tag
    end
  end
  return nil
end

local function program_space()
  return manager.machine.devices[":maincpu"].spaces["program"]
end

local function cmd_ping(_params)
  local ticks = 0
  local ok, value = pcall(function()
    return emu.time()
  end)
  if ok and type(value) == "number" then
    ticks = value
  end
  return { t = ticks }
end

local function cmd_status(_params)
  local machine = manager.machine
  local image = find_image("flop1")
  local filename = JSON_NULL
  if image and image.filename and image.filename ~= "" then
    filename = image.filename
  end
  local keyboard = machine.natkeyboard
  local ram = ""
  pcall(function()
    ram = tostring(machine.options.entries.ramsize:value())
  end)
  if ram == "" then
    ram = "current MMU window / configured -ramsize"
  end
  return {
    driver = machine.system.name,
    ram = ram,
    flop1 = filename,
    posting = keyboard.is_posting and true or false,
    empty = keyboard.empty and true or false,
  }
end

local function cmd_list_images(_params)
  local list = mark_array({})
  for tag, image in pairs(manager.machine.images) do
    local filename = JSON_NULL
    if image.filename and image.filename ~= "" then
      filename = image.filename
    end
    list[#list + 1] = {
      tag = tostring(image.tag or tag),
      briefname = image_briefname(image),
      instance_name = image_instancename(image),
      filename = filename,
      exists = image.exists and true or false,
    }
  end
  return list
end

local function cmd_mount(params)
  local brief = params.briefname
  if not floppy_drives[brief] then
    error("no " .. tostring(brief) .. " image device")
  end
  local image = find_image(brief)
  if not image then
    error("no " .. tostring(brief) .. " image device")
  end
  -- Do not unload the current image before load() succeeds: MAME Lua Scripting
  -- Interface, "Image device interface" documents image:load(filename) as
  -- "Returns nil if no error or a string describing an error if an error
  -- occurred" (https://docs.mamedev.org/luascript/ref-devices.html) with no
  -- requirement to unload first. A pre-emptive unload would otherwise discard
  -- the currently mounted disk even when the requested path turns out to be bad.
  local err = image:load(params.path)
  if err then
    error(err)
  end
  return { briefname = brief, filename = image.filename }
end

local function cmd_unmount(params)
  local brief = params.briefname
  local image = find_image(brief)
  if not image then
    error("no " .. tostring(brief) .. " image device")
  end
  image:unload()
  return { briefname = brief }
end

local function cmd_type(params)
  local keyboard = manager.machine.natkeyboard
  keyboard.in_use = true
  keyboard:post_coded(params.text or "")
  return { queued = true }
end

local function cmd_wait_idle(_params)
  local keyboard = manager.machine.natkeyboard
  local idle = keyboard.empty and not keyboard.is_posting
  return { idle = idle and true or false }
end

local function cmd_read_mem(params)
  local memory = program_space()
  local address = tonumber(params.address, 16) or 0
  local count = tonumber(params.length) or 0
  if address < 0 or address > 0xffff or count < 1 or address + count - 1 > 0xffff then
    error("address/length outside current 64K MMU window")
  end
  local parts = {}
  for offset = 0, count - 1 do
    parts[#parts + 1] = string.format("%02X", memory:read_u8(address + offset))
  end
  return { data = table.concat(parts, " ") }
end

local function cmd_write_mem(params)
  local memory = program_space()
  local address = tonumber(params.address, 16)
  if address == nil or address < 0 or address > 0xffff then
    error("address outside current 64K MMU window")
  end
  -- Validate every whitespace-separated token before writing anything: a garbage
  -- string that merely contains a hex-looking substring (e.g. "ZZ 41 QQ") must be
  -- rejected outright, not silently pruned down to whichever tokens happen to match.
  local tokens = {}
  for token in string.gmatch(params.data or "", "%S+") do
    if not string.match(token, "^%x%x$") then
      error("invalid data byte: " .. tostring(token))
    end
    tokens[#tokens + 1] = token
  end
  if address + #tokens - 1 > 0xffff then
    error("write past end of current 64K MMU window")
  end
  for offset, token in ipairs(tokens) do
    memory:write_u8(address + offset - 1, tonumber(token, 16))
  end
  return { bytes = #tokens }
end

local function cmd_snapshot(params)
  local machine = manager.machine
  local screen = nil
  for _, candidate in pairs(machine.screens) do
    screen = candidate
    break
  end
  if not screen then
    error("no screen")
  end
  -- screen:snapshot([filename]) takes an explicit path (MAME Lua Scripting
  -- Interface, "Screen device": https://docs.mamedev.org/luascript/ref-devices.html),
  -- so this primary path is deterministic — no race, no filename guessing.
  local snap_ok, snap_err = pcall(function()
    local err = screen:snapshot(params.path)
    if err ~= nil then
      error(tostring(err))
    end
  end)
  if snap_ok then
    return { path = params.path, method = "screen" }
  end
  -- video:snapshot() has no filename parameter at all (MAME Lua Scripting
  -- Interface, "Video manager": "Saves snapshot files according to the current
  -- configuration" — https://docs.mamedev.org/luascript/ref-core.html). This is
  -- a real, documented MAME API limitation, not an unverified assumption: the
  -- caller (tools.ts) has no choice but to look for the newest PNG MAME wrote to
  -- the configured snapshot directory.
  local video_ok, video_err = pcall(function()
    machine.video:snapshot()
  end)
  if video_ok then
    return {
      method = "video",
      path = params.path,
      snapshotDirectory = params.snapshotDirectory,
      fallbackError = tostring(snap_err),
    }
  end
  error(tostring(snap_err) .. "; fallback: " .. tostring(video_err))
end

local function cmd_soft_reset(_params)
  -- Soft reset runs after the reply is written so the bridge can answer.
  return { reset = "soft" }
end

local function cmd_save_state(params)
  manager.machine:save(params.name)
  return { scheduled = true, name = params.name }
end

-- Host-side restore operation metadata is deliberately not registered as save items.
-- https://docs.mamedev.org/luascript/ref-common.html: post-load notification.
local restore_operation = nil
local load_epoch = 0
local untracked_load_pending = false
local post_load_available = emu.add_machine_post_load_notifier ~= nil
if post_load_available then
  post_load_notifier = emu.add_machine_post_load_notifier(function()
    load_epoch = load_epoch + 1
    untracked_load_pending = false
    if restore_operation then
      if restore_operation.completed then
        restore_operation.invalidated = true
      elseif restore_operation.abandoned then
        restore_operation = nil
      else
        restore_operation.completed = true
        restore_operation.epoch = load_epoch
      end
    end
  end)
end
if emu.add_machine_reset_notifier then
  restore_reset_notifier = emu.add_machine_reset_notifier(function()
    load_epoch = load_epoch + 1
    if restore_operation then restore_operation.invalidated = true end
    untracked_load_pending = false
  end)
end

local function owned_restore(params)
  if not restore_operation or params.token ~= restore_operation.token then
    error("restore operation not owned")
  end
  if restore_operation.invalidated or (params.epoch and params.epoch ~= restore_operation.epoch) then
    error("restore operation invalidated")
  end
  return restore_operation
end

local function cmd_load_state(params)
  manager.machine:load(params.name)
  untracked_load_pending = post_load_available
  return { scheduled = true, name = params.name }
end

local function cmd_load_state_tracked(params)
  if not post_load_available then error("post-load notifier unavailable") end
  if restore_operation or untracked_load_pending then error("state load already pending; wait or restart MAME") end
  if type(params.token) ~= "string" or not string.match(params.token, "^%x+$") then error("invalid restore token") end
  if type(params.name) ~= "string" or not string.match(params.name, "^[%w_-]+$") then error("invalid state name") end
  if not cmd_wait_idle({}).idle then error("keyboard queue is not empty") end
  restore_operation = { token = params.token, completed = false, epoch = load_epoch, invalidated = false }
  local ok, err = pcall(function() manager.machine:load(params.name) end)
  if not ok then restore_operation = nil; error(err) end
  return { scheduled = true, name = params.name, token = params.token }
end

local function cmd_load_state_status(params)
  local op = owned_restore(params)
  return { token = op.token, completed = op.completed, epoch = op.epoch, invalidated = op.invalidated }
end

local function cmd_finish_restore(params)
  if not restore_operation or params.token ~= restore_operation.token then error("restore operation not owned") end
  if restore_operation.completed then
    restore_operation = nil
  else
    -- A timed-out scheduled load may still complete. Do not attribute its later
    -- notification to a second request. Until then, explicit restart is recovery.
    restore_operation.abandoned = true
  end
  return { released = restore_operation == nil }
end

-- Reuse the exclusive console lease and physical reader after restore ends.
-- No state is loaded here: completed means this observer may read the console.
local function cmd_begin_run(params)
  if restore_operation or untracked_load_pending then error("console operation busy") end
  if params.epoch ~= load_epoch then error("console epoch mismatch") end
  if type(params.token) ~= "string" or #params.token ~= 32 or not string.match(params.token, "^%x+$") then
    error("invalid run token")
  end
  restore_operation = { token = params.token, completed = true, epoch = load_epoch, invalidated = false }
  return { token = params.token, epoch = load_epoch }
end

local function cmd_finish_run(params)
  if not restore_operation or params.token ~= restore_operation.token then error("console operation not owned") end
  local invalidated = restore_operation.invalidated
  local result = cmd_finish_restore(params)
  result.invalidated = invalidated
  return result
end

local function cmd_read_text_console(params)
  local op = owned_restore(params)
  if not op.completed then error("state load not complete") end
  local machine = manager.machine
  local gime = machine.devices[":gime"]
  local ram = machine.devices[":ram"]
  if not gime or not ram or not gime.items["0/m_gime_registers"] or not ram.items["0/m_pointer"] then
    return { supported = false, reason = "GIME/RAM saved items unavailable" }
  end
  -- Exact saved-item interface confirmed on Ample 0.289; MAME's mame0289
  -- plugins/cheatfind/init.lua also uses emu.item(RAM 0/m_pointer):read_block.
  local regs = emu.item(gime.items["0/m_gime_registers"])
  local r = {}
  for i = 0, 15 do r[i + 1] = regs:read(i) end
  -- Intentionally reject other modes. Source: mame0289 src/mame/trs/gime.cpp,
  -- get_lines_per_row / record_full_body_scanline / update_geometry.
  -- Native 80-column attributed text, 225 scanlines / 9 = 25 rows, no scrolling.
  if (r[1] & 0x80) ~= 0 or (r[9] & 0x87) ~= 4 or (r[10] & 0x75) ~= 0x75
      or r[13] ~= 0 or r[16] ~= 0 then
    return { supported = false, reason = "requires EOU 80x25 hardware text, no virtual/vertical scrolling" }
  end
  local size = emu.item(ram.items["0/m_size"]):read(0)
  -- GIME get_video_base + record_scanline_res bank selection. Read physical
  -- RAM, independent of the active OS-9 task's CPU MMU window; never write it.
  local video_base = (r[14] << 11) | (r[15] << 3)
  if video_base + 4000 > 0x80000 then return { supported = false, reason = "cross-bank display unsupported" } end
  local base = (video_base | ((r[12] & 15) * 0x80000)) % size
  if base + 4000 > size then return { supported = false, reason = "wrapped display unsupported" } end
  local block = emu.item(ram.items["0/m_pointer"]):read_block(base, 4000)
  if not block or #block ~= 4000 then error("physical console read failed") end
  local hex = {}
  for i = 1, #block do hex[i] = string.format("%02x", string.byte(block, i)) end
  return { supported = true, registers = mark_array(r), cells = table.concat(hex),
    physicalBase = base, idle = cmd_wait_idle({}).idle, epoch = op.epoch }
end

local commands = {
  ping = cmd_ping,
  status = cmd_status,
  list_images = cmd_list_images,
  mount = cmd_mount,
  unmount = cmd_unmount,
  type = cmd_type,
  wait_idle = cmd_wait_idle,
  read_mem = cmd_read_mem,
  write_mem = cmd_write_mem,
  snapshot = cmd_snapshot,
  soft_reset = cmd_soft_reset,
  save_state = cmd_save_state,
  load_state = cmd_load_state,
  load_state_tracked = cmd_load_state_tracked,
  load_state_status = cmd_load_state_status,
  read_text_console = cmd_read_text_console,
  finish_restore = cmd_finish_restore,
  begin_run = cmd_begin_run,
  finish_run = cmd_finish_run,
}

local sock = nil
local buffer = ""

local function reply(id, ok, result, err)
  if not sock then
    return
  end
  local payload
  if ok then
    payload = { id = id, ok = true, result = result }
    if result == nil then
      payload.result = {}
    end
  else
    payload = { id = id, ok = false, error = tostring(err) }
  end
  local wrote = pcall(function()
    sock:write(json_encode(payload) .. "\n")
  end)
  if not wrote then
    sock = nil
  end
end

local function handle_line(line)
  if line == "" then
    return
  end
  local parsed_ok, message = pcall(json_decode, line)
  if not parsed_ok or type(message) ~= "table" then
    return
  end
  local id = message.id or ""
  if manager.machine == nil then
    reply(id, false, nil, "machine not ready")
    return
  end
  local handler = commands[message.cmd]
  if not handler then
    reply(id, false, nil, "unknown command")
    return
  end
  local params = message.params
  if type(params) ~= "table" then
    params = {}
  end
  -- Keep low-level callers from racing the multi-request handshake. Node also
  -- excludes concurrent mutating MCP calls, including requests already running.
  if restore_operation then
    local mutation = message.cmd == "type" or message.cmd == "write_mem" or message.cmd == "mount"
      or message.cmd == "unmount" or message.cmd == "soft_reset" or message.cmd == "save_state"
      or message.cmd == "load_state" or message.cmd == "load_state_tracked" or message.cmd == "begin_run"
    if mutation then
      local allowed = message.cmd == "type" and restore_operation.completed and not restore_operation.invalidated
        and params.token == restore_operation.token and params.epoch == restore_operation.epoch
      if not allowed then reply(id, false, nil, "restore operation busy"); return end
    end
  end
  local ran, result = pcall(handler, params)
  if ran then
    reply(id, true, result, nil)
    if message.cmd == "soft_reset" then
      pcall(function()
        manager.machine:soft_reset()
      end)
    end
  else
    reply(id, false, nil, result)
  end
end

local function try_connect()
  if sock ~= nil then
    return
  end
  local handle = emu.file("rw")
  local err = handle:open("socket.127.0.0.1:" .. bridge_port())
  if err then
    return
  end
  sock = handle
  buffer = ""
end

local function on_frame()
  pcall(function()
    try_connect()
    if not sock then
      return
    end
    local read_ok, chunk = pcall(function()
      return sock:read(1024)
    end)
    if not read_ok then
      sock = nil
      buffer = ""
      return
    end
    if chunk == nil then
      sock = nil
      buffer = ""
      return
    end
    if type(chunk) ~= "string" or chunk == "" then
      return
    end
    buffer = buffer .. chunk
    while true do
      local newline = string.find(buffer, "\n", 1, true)
      if not newline then
        break
      end
      local line = string.sub(buffer, 1, newline - 1)
      buffer = string.sub(buffer, newline + 1)
      if string.sub(line, -1) == "\r" then
        line = string.sub(line, 1, -2)
      end
      handle_line(line)
    end
  end)
end

-- MAME 0.289+ uses add_machine_frame_notifier; older builds use register_frame.
-- Keep a global reference so the notifier is not garbage-collected.
if emu.add_machine_frame_notifier then
  frame_notifier = emu.add_machine_frame_notifier(on_frame)
else
  emu.register_frame(on_frame)
end
