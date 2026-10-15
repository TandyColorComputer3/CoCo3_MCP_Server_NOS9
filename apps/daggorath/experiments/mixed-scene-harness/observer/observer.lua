-- Stage 3E.13 observer, derived from the preserved Stage 3E.10D compare
-- observer. Appended to a private copy of MCP/scripts/bridge.lua so it can
-- register read-only bridge commands in that file's local `commands` table.
-- No process descriptor traversal and no callback filesystem writes.
local m = manager.machine
local cpu = assert(m.devices[':maincpu'], 'main CPU unavailable')
local mem = assert(cpu.spaces.program, 'CPU program space unavailable')
local ramDevice = assert(m.devices[':ram'], 'RAM device unavailable')
local ramName = assert(ramDevice.items['0/m_pointer'], 'RAM pointer unavailable')
local sizeName = assert(ramDevice.items['0/m_size'], 'RAM size unavailable')
local ram = emu.item(ramName)
local ramSize = emu.item(sizeName):read(0)
assert(ramSize == 0x200000, 'expected 2 MB physical RAM')
assert(m.video and m.video.snapshot_pixels and m.video.snapshot_size,
       'video:snapshot_pixels API unavailable')
assert(emu.add_machine_frame_notifier, 'machine frame notifier unavailable')
assert(mem.install_write_tap, 'write tap API unavailable')

local MAX_EVENTS = 4096
-- Derived from status-client.c heartPatterns and present.c preload_heart():
-- two source bytes per row, with each 1 bpp pixel doubled horizontally.
local m0 = '0000000000000000000000000FF00FF0000000000000000000000000'
local m1 = '0000000000000000000000003FFC3FFC000000000000000000000000'
local lifecycle = 'WAITING_FOR_GRAPHICS'
local events, drained = {}, 0
local overflow, failed = false, nil
local edgeCount, age, static = 0, 0, nil
local currentEdge, lastGeneration = nil, nil
local newSeen, completeCount = {}, 0
local tapInstalled, frameRegistered, frameCallbacks = false, false, 0

local function pb(a)
    assert(type(a) == 'number' and a == math.floor(a) and a >= 0 and a + 1 <= ramSize,
           'physical RAM read out of range')
    local b = ram:read_block(a, 1)
    assert(b and #b == 1, 'physical RAM read failed')
    return string.byte(b, 1)
end
local function pw(a) return pb(a) * 256 + pb(a + 1) end
local function pdw(a) return pw(a) * 65536 + pw(a + 2) end
local function phys(a, task)
    local slot = math.floor(a / 0x2000)
    return mem:read_u8(0xffa0 + task * 8 + slot) * 0x2000 + (a % 0x2000)
end
local function record(kind, fields)
    if #events >= MAX_EVENTS then overflow = true; return end
    fields = fields or {}
    fields.kind = kind
    fields.time = m.time:as_double()
    events[#events + 1] = fields
end
local function classify(mask, phase)
    local current = phase == 0 and m0 or m1
    local old = phase == 0 and m1 or m0
    if mask == current then return 'new' end
    if mask == old then return 'obsolete' end
    if mask == string.rep('0', 56) then return 'blank' end
    local mixed, seenCurrent, seenOther = true, false, false
    -- The control phases differ within one row. Compare its 16-pixel halves
    -- so a partially published row remains distinguishable from either phase.
    for i = 1, 56, 4 do
        local row = mask:sub(i, i + 3)
        local currentHalf, oldHalf = current:sub(i, i + 3), old:sub(i, i + 3)
        if row == currentHalf then
            if currentHalf ~= oldHalf then seenCurrent = true end
        elseif row == oldHalf or row == '0000' then
            seenOther = true
        else mixed = false; break end
    end
    return mixed and seenCurrent and seenOther and 'partial-rows' or 'other'
end
local function classifier_controls()
    -- Only row 3 differs; mix its left and right 16-pixel halves.
    local partial = m0:sub(1, 28) .. m1:sub(29)
    local unknown = string.rep('A', 56)
    assert(classify(m0, 0) == 'new', 'phase-0 complete control failed')
    assert(classify(m1, 0) == 'obsolete', 'phase-0 old control failed')
    assert(classify(m1, 1) == 'new', 'phase-1 complete control failed')
    assert(classify(string.rep('0', 56), 0) == 'blank', 'blank control failed')
    assert(classify(partial, 0) == 'partial-rows', 'partial-row control failed')
    assert(classify(unknown, 0) == 'other', 'unknown-pattern control failed')
    return 6
end
local controlsPassed = classifier_controls()

local function snapshot_mask()
    local pixels = m.video:snapshot_pixels()
    local width, height = m.video:snapshot_size()
    if width ~= 640 or height ~= 239 then error('published raster dimensions changed') end
    local rows = {}
    for y = 178, 184 do
        local row = {}
        for byteIndex = 0, 3 do
            local bits = 0
            for bitIndex = 0, 7 do
                local x = 304 + byteIndex * 8 + bitIndex
                local pos = (y * width + x) * 4 + 1
                local blue, green, red = pixels:byte(pos, pos + 2)
                if blue ~= 0 or green ~= 0 or red ~= 0 then
                    bits = bits | (0x80 >> bitIndex)
                end
            end
            row[#row + 1] = string.format('%02X', bits)
        end
        rows[#rows + 1] = table.concat(row)
    end
    return table.concat(rows)
end

-- Host calls this only after MCP read_text_console reports the EOU text mode
-- unsupported during the named foreground run. The pixel classifier never
-- decides graphics entry or departure.
commands.msh_observer_lifecycle = function(params)
    local nextState = params and params.state
    if nextState == lifecycle then return { state = lifecycle, unchanged = true } end
    if lifecycle == 'WAITING_FOR_GRAPHICS' and nextState == 'GRAPHICS_ACTIVE' then
        lifecycle = nextState
        record('graphics_entry', { source = 'bridge_read_text_console_unsupported_display' })
    elseif lifecycle == 'GRAPHICS_ACTIVE' and nextState == 'GRAPHICS_DEPARTED' then
        lifecycle = nextState
        record('graphics_departure', { source = 'bridge_read_text_console_text_mode_return' })
    else
        return { state = lifecycle, error = 'invalid lifecycle transition' }
    end
    return { state = lifecycle }
end

commands.msh_observer_ready = function()
    return { ready = true, tapInstalled = tapInstalled, frameRegistered = frameRegistered,
             frameCallbacks = frameCallbacks, ramSize = ramSize,
             classifierControls = controlsPassed, lifecycle = lifecycle,
             source = 'stage-3e10d/compare-observer.lua' }
end

commands.msh_observer_drain = function(params)
    local limit = tonumber(params and params.limit) or 128
    if limit < 1 or limit > 256 then error('limit must be 1..256') end
    local batch = mark_array({})
    local last = math.min(#events, drained + limit)
    for i = drained + 1, last do batch[#batch + 1] = events[i] end
    drained = last
    return { events = batch, lifecycle = lifecycle, edgeCount = edgeCount,
             completeCount = completeCount, overflow = overflow, failure = failed }
end

commands.msh_observer_state = function()
    return { ready = tapInstalled and frameRegistered, tapInstalled = tapInstalled,
             frameRegistered = frameRegistered, frameCallbacks = frameCallbacks,
             ramSize = ramSize, lifecycle = lifecycle,
             edgeCount = edgeCount, completeCount = completeCount,
             overflow = overflow, failure = failed, bufferedEvents = #events - drained }
end

record('observer_ready', { source = 'stage-3e10d/compare-observer.lua',
                           classifierControls = controlsPassed,
                           raster = 'video:snapshot_pixels', tap = '$FF22' })
mixed_scene_pb1_tap = mem:install_write_tap(0xff22, 0xff22, 'msh13-pb1', function(a, data)
    if failed or a ~= 0xff22 then return end
    local old = mem:read_u8(0xff22) & 2
    local new = data & 2
    if old == new then return end
    local u = cpu.state.U.value
    local p = phys(u, 0)
    local rate, enabled = pb(p + 0x27), pb(p + 0x29)
    if (rate ~= 46 and rate ~= 45 and rate ~= 41) or enabled ~= 1 then return end
    if static and static ~= p then failed = 'driver static address changed'; record('failure', { reason = failed }); return end
    static = p
    local generation, phase = pdw(p + 0x2e), pb(p + 0x2b)
    if phase ~= (old >> 1) or ((phase ~ 1) & 1) ~= (new >> 1) then
        failed = 'pre-edge phase/PB1 disagreement'
        record('failure', { reason = failed, generation = generation, phase = phase, oldPB1 = old >> 1, newPB1 = new >> 1 })
        return
    end
    if lastGeneration and generation ~= lastGeneration then
        failed = 'generation gap or duplicate'
        record('failure', { reason = failed, priorGeneration = generation, expectedPrior = lastGeneration })
        return
    end
    edgeCount = edgeCount + 1
    lastGeneration = generation + 1
    currentEdge = { index = edgeCount, generation = generation + 1, phase = new >> 1,
                    rate = rate, edgeTime = m.time:as_double() }
    age = 0
    record('pb1_edge', { index = edgeCount, priorGeneration = generation, priorPhase = phase,
                         generation = currentEdge.generation, phase = currentEdge.phase,
                         pb1 = currentEdge.phase, rate = rate, driverStatic = p,
                         pc = cpu.state.PC.value })
end)
tapInstalled = true

mixed_scene_frame_notifier = emu.add_machine_frame_notifier(function()
    frameCallbacks = frameCallbacks + 1
    if failed or lifecycle ~= 'GRAPHICS_ACTIVE' or not currentEdge or age >= 4 then return end
    age = age + 1
    local generation = pdw(static + 0x2e)
    local phase = pb(static + 0x2b)
    if generation ~= currentEdge.generation or phase ~= currentEdge.phase then
        failed = 'post-edge generation/phase mismatch'
        record('failure', { reason = failed, edge = currentEdge.index,
                            generation = generation, phase = phase, age = age })
        return
    end
    local ok, mask = pcall(snapshot_mask)
    if not ok then failed = 'snapshot/classifier failure: ' .. tostring(mask); record('failure', { reason = failed }); return end
    local classification = classify(mask, phase)
    local reversion = newSeen[currentEdge.index] == true and classification == 'obsolete'
    local firstCorrect = false
    if classification == 'new' and not newSeen[currentEdge.index] then
        newSeen[currentEdge.index] = true
        completeCount = completeCount + 1
        firstCorrect = true
    end
    record('published_frame', { edge = currentEdge.index, generation = generation,
        phase = phase, age = age, classification = classification, mask = mask,
        firstCorrect = firstCorrect, obsoleteReversion = reversion,
        unexpectedBlank = classification == 'blank', framePeriod = m.screens[':screen'].frame_period })
    if reversion then failed = 'obsolete phase reversion after correct phase'; record('failure', { reason = failed, edge = currentEdge.index }) end
end)
frameRegistered = true
