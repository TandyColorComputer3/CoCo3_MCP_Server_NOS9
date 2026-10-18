-- Disposable Stage 3E.16D.25E observer, appended to a private copy of
-- MCP/scripts/bridge.lua. The global notifier handle deliberately follows the
-- confirmed D.25D lifetime correction; no process or guest memory is inspected.
local m = manager.machine
assert(m.video and m.video.snapshot_pixels and m.video.snapshot_size,
       'MAME video snapshot API unavailable')
assert(emu.add_machine_frame_notifier, 'MAME frame notifier unavailable')
assert(commands and mark_array, 'private bridge command table unavailable')
local ram_size_name = m.devices[':ram'] and m.devices[':ram'].items['0/m_size']
assert(ram_size_name, 'physical RAM size item unavailable')
local ram_bytes = emu.item(ram_size_name):read(0)

local MAX_EVENTS = 256
local MAX_SNAPSHOT_ATTEMPTS = 512
local COARSE_CADENCE = 8 -- 512 attempts cover 4096 active callbacks, versus 3760 in D.25F.3.
local LATER_SECONDS = 3.0 -- Provisional evidence label; not proof that F$Sleep returned.
local REQUIRED_STABLE = 3
local PALETTE = { 0x000000, 0x555555, 0xAAAAAA, 0xFFFFFF }
local state = {
    initialized = false, lifecycle = 'WAITING_FOR_GRAPHICS', frames = 0,
    activeCallbacks = 0, snapshotAttempts = 0, stable = 0,
    complete = false, failure = nil, overflow = false,
    transform = nil, lastSignature = nil, controls = 0, ramBytes = ram_bytes,
    matches = 0, rejects = 0, omittedDiagnosticRecords = 0,
    reasonCounts = {}, firstCandidate = nil, firstMatch = nil,
    firstActive = nil, firstLater = nil, firstRejected = nil, finalMismatch = nil,
    stableSequence = {}, attemptTimes = {}, activationSeen = false,
    activeStart = nil, activeEnd = nil, burst = 0, acquisitionCount = 0,
}
local events, drained = {}, 0

local function now() return m.time:as_double() end
local function record(kind, fields)
    if #events >= MAX_EVENTS then
        state.overflow = true
        state.omittedDiagnosticRecords = state.omittedDiagnosticRecords + 1
        return
    end
    fields = fields or {}
    fields.kind = kind
    fields.emulatedTime = now()
    events[#events + 1] = fields
end
local function u32(pixels, x, y, width)
    local pos = (y * width + x) * 4 + 1
    local value = string.unpack('=I4', pixels, pos)
    return value, value & 0x00FFFFFF, pixels:sub(pos, pos + 3)
end
local function rgb_at(pixels, x, y, width, height)
    if x < 0 or y < 0 or x >= width or y >= height then return nil end
    local _, rgb = u32(pixels, x, y, width)
    return rgb
end
local function rect_is(pixels, width, height, x, y, rw, rh, expected)
    if rw < 1 or rh < 1 or x < 0 or y < 0 or x + rw > width or y + rh > height then return false end
    for yy = y, y + rh - 1 do
        for xx = x, x + rw - 1 do
            if rgb_at(pixels, xx, yy, width, height) ~= expected then return false end
        end
    end
    return true
end

-- Locate markers drawn by the independent fixture, not by searching for the
-- expected four-band result. Formula: snapshotX = sx*logicalX + ox; likewise Y.
local function find_transform(pixels, width, height)
    local found = nil
    for sy = 1, 4 do
        for sx = 1, 4 do
            local ah, aw = 8 * sy, 8 * sx
            for y = 0, height - ah do
                for x = 0, width - aw do
                    if rgb_at(pixels, x, y, width, height) == PALETTE[2]
                       and rect_is(pixels, width, height, x, y, aw, ah, PALETTE[2])
                       and rect_is(pixels, width, height, x + 16 * sx, y,
                                   16 * sx, 8 * sy, PALETTE[3])
                       and rect_is(pixels, width, height, x + 48 * sx, y + 48 * sy,
                                   6 * sx, 8 * sy, PALETTE[4]) then
                        local candidate = { sx = sx, sy = sy,
                            ox = x - 8 * sx, oy = y - 16 * sy,
                            markerX = x, markerY = y }
                        if found then return nil, 'ambiguous_calibration_markers' end
                        found = candidate
                    end
                end
            end
        end
    end
    return found, found and nil or 'calibration_markers_not_found'
end

local function collect_raw_band(pixels, width, height, t)
    local x0 = t.ox + 64 * t.sx
    local y0 = t.oy + 64 * t.sy
    local raw, rgb = {}, {}
    for band = 0, 3 do
        local expected = PALETTE[band + 1]
        local color = nil
        for y = y0, y0 + 8 * t.sy - 1 do
            for x = x0 + band * 8 * t.sx, x0 + (band + 1) * 8 * t.sx - 1 do
                local word, decoded, bytes = u32(pixels, x, y, width)
                if color == nil then color = decoded end
                if decoded ~= expected or decoded ~= color then
                    return { complete = false, reason = 'band_color_or_uniformity_mismatch',
                             band = band, x = x, y = y,
                             rawWord = string.format('%08X', word),
                             expectedRGB = string.format('%06X', expected),
                             actualRGB = string.format('%06X', decoded) }
                end
                raw[#raw + 1] = bytes
            end
        end
        rgb[#rgb + 1] = string.format('%06X', color or 0)
    end
    local bandBytes = table.concat(raw)
    local function raw_word(x, y)
        local word, decoded, bytes = u32(pixels, x, y, width)
        return { word = string.format('%08X', word), rgb = string.format('%06X', decoded),
                 hostEndianBytes = (bytes:gsub('.', function(c) return string.format('%02X', string.byte(c)) end)) }
    end
    local sample = { complete = true, x = x0, y = y0, width = 32 * t.sx, height = 8 * t.sy,
        bandRGB = mark_array(rgb),
        rawBandHostEndianHex = (bandBytes:gsub('.', function(c) return string.format('%02X', string.byte(c)) end)),
    }
    if t.markerX then sample.rawMarkerWords = {
            color1 = raw_word(t.markerX, t.markerY),
            color2 = raw_word(t.markerX + 16 * t.sx, t.markerY),
            color3 = raw_word(t.markerX + 48 * t.sx, t.markerY + 48 * t.sy),
        } end
    return sample
end

local function classify_pixels(pixels, width, height, profile)
    profile = profile or 'fixture'
    if type(width) ~= 'number' or type(height) ~= 'number'
       or width < 1 or height < 1 or width ~= math.floor(width) or height ~= math.floor(height) then
        return { complete = false, reason = 'invalid_dimensions' }
    end
    if type(pixels) ~= 'string' or #pixels ~= width * height * 4 then
        return { complete = false, reason = 'pixel_length_mismatch',
                 expectedBytes = width * height * 4, actualBytes = type(pixels) == 'string' and #pixels or -1 }
    end
    local t
    if profile == 'presenter' then
        if width ~= 640 or height ~= 239 then
            return { complete = false, reason = 'unexpected_presenter_geometry',
                snapshotWidth = width, snapshotHeight = height, snapshotBytes = #pixels }
        end
        -- Independent D.25E fixture established X'=X, Y'=Y+22 for this mode.
        t = { sx = 1, sy = 1, ox = 0, oy = 22 }
    elseif profile == 'fixture' then
        local why
        t, why = find_transform(pixels, width, height)
        if not t then return { complete = false, reason = why } end
        -- Fixture-only contrast frame makes the black band's extent visible.
        local px0, py0 = t.ox + 63 * t.sx, t.oy + 63 * t.sy
        if not rect_is(pixels, width, height, px0, py0, 34 * t.sx, 1 * t.sy, PALETTE[4])
           or not rect_is(pixels, width, height, px0, py0 + 9 * t.sy, 34 * t.sx, 1 * t.sy, PALETTE[4])
           or not rect_is(pixels, width, height, px0, py0, 1 * t.sx, 10 * t.sy, PALETTE[4])
           or not rect_is(pixels, width, height, px0 + 33 * t.sx, py0, 1 * t.sx, 10 * t.sy, PALETTE[4]) then
            return { complete = false, reason = 'calibration_frame_mismatch' }
        end
    else
        return { complete = false, reason = 'invalid_profile' }
    end
    local sample = collect_raw_band(pixels, width, height, t)
    if not sample.complete then return sample end
    sample.snapshotWidth = width
    sample.snapshotHeight = height
    sample.snapshotBytes = #pixels
    sample.pixelFormat = '32-bit RGB integer, host-endian packed binary; RGB=word[23:0]'
    sample.snapshotTarget = tostring(m.video.snapshot_target)
    sample.snapNative = m.video.snap_native
    sample.coordinateTransform = { sx = t.sx, sy = t.sy, ox = t.ox, oy = t.oy,
        formulaX = 'snapshotX = sx * logicalX + ox',
        formulaY = 'snapshotY = sy * logicalY + oy' }
    sample.logicalOrder = '0,1,2,3'
    sample.paletteValues = mark_array({ 0x00, 0x07, 0x38, 0x3F })
    sample.profile = profile
    return sample
end

local function signature(sample)
    local t = sample.coordinateTransform
    return table.concat({ sample.snapshotWidth, sample.snapshotHeight,
        t.sx, t.sy, t.ox, t.oy, sample.rawBandHostEndianHex }, ':')
end
local function hex(bytes)
    return (bytes:gsub('.', function(c) return string.format('%02X', string.byte(c)) end))
end
local function capture_crop(pixels, width, height)
    if type(pixels) ~= 'string' or type(width) ~= 'number' or type(height) ~= 'number'
       or width < 96 or height < 94 or #pixels ~= width * height * 4 then return nil end
    local rows = {}
    for y = 86, 93 do
        local offset = (y * width + 64) * 4 + 1
        rows[#rows + 1] = pixels:sub(offset, offset + 127)
    end
    return table.concat(rows)
end
local function crop_evidence(pixels, width, height, sequence, timestamp, activeSequence)
    local crop = capture_crop(pixels, width, height)
    local sum = 1
    if crop then
        local a, b = 1, 0
        for i = 1, #crop do
            a = (a + crop:byte(i)) % 65521
            b = (b + a) % 65521
        end
        sum = b * 65536 + a
    end
    return { sequence = sequence, activeSequence = activeSequence,
        emulatedTime = timestamp, snapshotWidth = width, snapshotHeight = height,
        snapshotBytes = type(pixels) == 'string' and #pixels or -1,
        cropHex = crop and hex(crop) or nil, cropBytes = crop and #crop or 0,
        cropAdler32 = string.format('%08X', sum) }
end
local function should_sample(activeSequence, burst, attempts)
    return attempts < MAX_SNAPSHOT_ATTEMPTS
       and (activeSequence == 1 or burst > 0 or activeSequence % COARSE_CADENCE == 0)
end
local function retain_first_candidate(pixels, width, height, sequence, timestamp)
    if not state.firstCandidate and type(pixels) == 'string' and #pixels > 0 then
        state.firstCandidate = { pixels = pixels, crop = capture_crop(pixels, width, height),
            width = width, height = height, sequence = sequence, emulatedTime = timestamp }
    end
end
local function stability_step(sample, prior)
    if not sample.complete then prior.last = nil; prior.count = 0; return false end
    local current = signature(sample)
    if current == prior.last then prior.count = prior.count + 1
    else prior.last = current; prior.count = 1 end
    return prior.count >= REQUIRED_STABLE
end

-- Actual Lua positive/negative fixtures run at observer startup in MAME before
-- any guest command. Python's separate transliteration is only a static screen.
local function synthetic_frame(width, height, sx, sy, ox, oy, bands, marker, widthBand, bandWidth, heightBand, bandHeight, shiftX, frame)
    local pixels = {}
    for i = 1, width * height do pixels[i] = string.pack('=I4', PALETTE[1]) end
    local function put_rect(lx, ly, w, h, color)
        local x0, y0 = ox + lx * sx, oy + ly * sy
        for y = y0, y0 + h * sy - 1 do
            for x = x0, x0 + w * sx - 1 do
                pixels[y * width + x + 1] = string.pack('=I4', color)
            end
        end
    end
    if marker ~= false then
        put_rect(8,16,8,8,PALETTE[2]); put_rect(24,16,16,8,PALETTE[3]);
        put_rect(56,64,6,8,PALETTE[4])
    end
    if frame ~= false then put_rect(63,63,34,10,PALETTE[4]) end
    for band = 0, 3 do
        local bw = band == (widthBand or -1) and (bandWidth or 8) or 8
        local bh = band == (heightBand or -1) and (bandHeight or 8) or 8
        put_rect(64 + band * 8 + (shiftX or 0),64,bw,bh,
                 PALETTE[(bands or {0,1,2,3})[band + 1] + 1])
    end
    return table.concat(pixels)
end
local function self_tests()
    local good = synthetic_frame(192, 144, 1, 1, 7, 9, nil, true)
    local ok = classify_pixels(good, 192, 144)
    assert(ok.complete and ok.logicalOrder == '0,1,2,3', 'correct pattern fixture failed')
    assert(ok.coordinateTransform.ox == 7 and ok.coordinateTransform.oy == 9, '1x transform fixture failed')
    local function rejected(name, pixels, w, h)
        local v = classify_pixels(pixels, w, h)
        assert(not v.complete, name .. ' was incorrectly accepted')
    end
    rejected('reversed bands', synthetic_frame(192,144,1,1,7,9,{3,2,1,0},true),192,144)
    rejected('missing color', synthetic_frame(192,144,1,1,7,9,{0,1,2,2},true),192,144)
    rejected('duplicate color', synthetic_frame(192,144,1,1,7,9,{0,1,1,3},true),192,144)
    rejected('missing calibration markers', synthetic_frame(192,144,1,1,7,9,nil,false),192,144)
    rejected('uniform image', string.rep(string.pack('=I4', PALETTE[1]), 192 * 144),192,144)
    rejected('shifted pattern', synthetic_frame(192,144,1,1,7,9,nil,true,nil,nil,nil,nil,1),192,144)
    rejected('incorrect width', synthetic_frame(192,144,1,1,7,9,nil,true,1,7),192,144)
    rejected('incorrect height', synthetic_frame(192,144,1,1,7,9,nil,true,nil,nil,1,7),192,144)
    rejected('truncated bytes', good:sub(1,#good-4),192,144)
    local alt = synthetic_frame(220,180,2,2,11,13,nil,true)
    local altResult = classify_pixels(alt,220,180)
    assert(altResult.complete and altResult.coordinateTransform.sx == 2
           and altResult.coordinateTransform.ox == 11, 'alternative geometry/scale fixture failed')
    local history = { count = 0, last = nil }
    local alt2 = synthetic_frame(220,180,2,2,12,13,nil,true)
    local unstable = classify_pixels(alt2,220,180)
    assert(not stability_step(altResult,history), 'first sample cannot be stable')
    assert(not stability_step(unstable,history), 'changed coordinate transform cannot be stable')
    assert(not stability_step(altResult,history), 'stability must restart after inconsistency')
    assert(not stability_step(altResult,history), 'only two repeated samples seen')
    assert(stability_step(altResult,history), 'three consecutive equal frames required')
    local presenter = synthetic_frame(640,239,1,1,0,22,nil,false,nil,nil,nil,nil,nil,false)
    local pg = classify_pixels(presenter,640,239,'presenter')
    assert(pg.complete and pg.x == 64 and pg.y == 86 and pg.snapshotBytes == 611840,
        'presenter profile rejected complete field without fixture markers')
    assert(not classify_pixels(presenter,640,239,'fixture').complete,
        'fixture profile accepted image without its markers')
    assert(not classify_pixels(synthetic_frame(640,239,1,1,0,22,{3,2,1,0},false,nil,nil,nil,nil,nil,false),640,239,'presenter').complete,
        'presenter reversed bands accepted')
    assert(not classify_pixels(synthetic_frame(640,239,1,1,0,22,{0,1,2,2},false,nil,nil,nil,nil,nil,false),640,239,'presenter').complete,
        'presenter incorrect color accepted')
    assert(not classify_pixels(synthetic_frame(640,239,1,1,0,22,nil,false,nil,nil,nil,nil,1,false),640,239,'presenter').complete,
        'presenter shifted field accepted')
    assert(not classify_pixels(presenter,639,239,'presenter').complete,
        'presenter wrong geometry accepted')
    assert(not classify_pixels(presenter:sub(1,#presenter-4),640,239,'presenter').complete,
        'presenter truncated pixels accepted')
    local pHistory = { count = 0, last = nil }
    assert(not stability_step(pg,pHistory) and not stability_step(pg,pHistory),
        'presenter stable too early')
    assert(stability_step(pg,pHistory), 'presenter three-frame sequence rejected')
    assert(not stability_step({complete=false},pHistory)
        and not stability_step(pg,pHistory), 'presenter interruption did not reset stability')
    local wrong = synthetic_frame(640,239,1,1,0,22,{3,2,1,0},false,nil,nil,nil,nil,nil,false)
    retain_first_candidate(wrong,640,239,17,1.25)
    assert(state.firstCandidate.pixels == wrong and #state.firstCandidate.crop == 1024
           and state.firstCandidate.sequence == 17, 'first rejected raw candidate not retained')
    state.firstCandidate = nil
    local attempts, sampled, stable, burst = 0, {}, 0, 0
    for frame = 1, 360 do
        if should_sample(frame, burst, attempts) then
            attempts = attempts + 1
            sampled[#sampled + 1] = frame
            local complete = frame >= 304
            if complete then stable = stable + 1; burst = math.max(0, REQUIRED_STABLE - stable)
            else stable = 0; burst = 0 end
            if stable == REQUIRED_STABLE then break end
        end
    end
    assert(sampled[1] == 1 and sampled[#sampled-2] == 304
           and sampled[#sampled-1] == 305 and sampled[#sampled] == 306,
           'delayed presentation or consecutive burst failed')
    assert(attempts < MAX_SNAPSHOT_ATTEMPTS and attempts > 32,
           'activation exhausted the snapshot budget')
    assert(not should_sample(2,0,1) and should_sample(8,0,1),
           'coarse cadence failed')
    assert(not should_sample(1,0,MAX_SNAPSHOT_ATTEMPTS), 'budget bound failed')
    local interrupted = { count = 0, last = nil }
    assert(not stability_step(pg,interrupted) and not stability_step(pg,interrupted),
        'interrupted sequence did not start')
    assert(not stability_step({complete=false},interrupted)
        and not stability_step(pg,interrupted) and not stability_step(pg,interrupted)
        and stability_step(pg,interrupted), 'interrupted sequence failed to restart')
    local activation = synthetic_frame(640,239,1,1,0,-10,nil,false,nil,nil,nil,nil,nil,false)
    -- The early activation pattern is at logical Y=32, hence snapshot Y=54.
    assert(collect_raw_band(activation,640,239,{sx=1,sy=1,ox=0,oy=-10}).complete,
        'activation source-pattern control failed')
    local crop = crop_evidence(presenter,640,239,304,3.2,304)
    assert(crop.cropBytes == 1024 and #crop.cropHex == 2048,
        'raw crop retention failed')
    assert(MAX_EVENTS == 256 and MAX_EVENTS < MAX_SNAPSHOT_ATTEMPTS,
        'event ledger must be capped separately from sampling')
    return 39
end
state.controls = self_tests()
state.initialized = true

commands.msh_observer_lifecycle = function(params)
    local nextState = params and params.state
    if nextState == state.lifecycle then return { state = state.lifecycle, unchanged = true } end
    if state.lifecycle == 'WAITING_FOR_GRAPHICS' and nextState == 'GRAPHICS_ACTIVE' then
        state.lifecycle = nextState
        state.activeStart = now()
        record('graphics_entry', { source = 'session-owned read_text_console unsupported-display' })
    elseif state.lifecycle == 'GRAPHICS_ACTIVE' and nextState == 'GRAPHICS_DEPARTED' then
        state.lifecycle = nextState
        state.activeEnd = now()
        record('graphics_departure', { source = 'session-owned read_text_console text-mode return' })
    else return { state = state.lifecycle, error = 'invalid lifecycle transition' } end
    return { state = state.lifecycle }
end
commands.msh_observer_ready = function()
    return { ready = state.initialized and state.controls == 39,
        classifierSelfTests = state.controls, notifierInstalled = state.notifierInstalled == true,
        ramBytes = state.ramBytes,
        frameCallbacks = state.frames, activeCallbacks = state.activeCallbacks,
        snapshotAttempts = state.snapshotAttempts, complete = state.complete,
        stableSamples = state.stable, lifecycle = state.lifecycle,
        matches = state.matches, rejects = state.rejects,
        reasonCounts = state.reasonCounts,
        omittedDiagnosticRecords = state.omittedDiagnosticRecords,
        snapshotBudget = MAX_SNAPSHOT_ATTEMPTS, coarseCadence = COARSE_CADENCE,
        acquisitionCount = state.acquisitionCount,
        firstActive = state.firstActive, firstLater = state.firstLater,
        firstRejected = state.firstRejected, finalMismatch = state.finalMismatch,
        firstMatch = state.firstMatch, stableSequence = mark_array(state.stableSequence),
        attemptTimes = mark_array(state.attemptTimes),
        activeStart = state.activeStart, activeEnd = state.activeEnd,
        activationSeen = state.activationSeen,
        firstCandidatePresent = state.firstCandidate ~= nil,
        firstMatchPresent = state.firstMatch ~= nil,
        failure = state.failure, overflow = state.overflow }
end
commands.msh_observer_state = function()
    return commands.msh_observer_ready()
end
commands.msh_observer_drain = function(params)
    local limit = tonumber(params and params.limit) or 128
    if limit < 1 or limit > 256 then error('limit must be 1..256') end
    local batch = mark_array({})
    local last = math.min(#events, drained + limit)
    for i = drained + 1, last do batch[#batch + 1] = events[i] end
    drained = last
    return { events = batch, lifecycle = state.lifecycle,
        frames = state.frames, activeCallbacks = state.activeCallbacks,
        snapshotAttempts = state.snapshotAttempts, stableSamples = state.stable,
        complete = state.complete, failure = state.failure, overflow = state.overflow,
        matches = state.matches, rejects = state.rejects,
        reasonCounts = state.reasonCounts,
        omittedDiagnosticRecords = state.omittedDiagnosticRecords,
        bufferedEvents = #events - drained }
end

commands.msh_observer_raw_info = function()
    local c = state.firstCandidate
    if not c then return { present = false } end
    return { present = true, sequence = c.sequence, emulatedTime = c.emulatedTime,
        width = c.width, height = c.height, byteLength = #c.pixels,
        pixelFormat = '32-bit host-endian words; RGB=word[23:0]',
        cropX = 64, cropY = 86, cropWidth = 32, cropHeight = 8,
        cropHex = c.crop and hex(c.crop) or nil,
        reason = c.reason, classification = c.classification }
end
commands.msh_observer_raw_chunk = function(params)
    local c = state.firstCandidate
    if not c then error('no first candidate') end
    local offset = tonumber(params and params.offset)
    local length = tonumber(params and params.length)
    if not offset or not length or offset ~= math.floor(offset) or length ~= math.floor(length)
       or offset < 0 or length < 1 or length > 8192 or offset + length > #c.pixels then
        error('invalid raw snapshot range')
    end
    return { offset = offset, length = length,
        hex = hex(c.pixels:sub(offset + 1, offset + length)) }
end

record('observer_ready', { source = 'D.25D-retained-notifier-pattern',
    classifierSelfTests = state.controls, ramBytes = state.ramBytes,
    pixelFormat = 'MAME snapshot_pixels 32-bit host-endian RGB integer' })
type7_frame_notifier = emu.add_machine_frame_notifier(function()
    state.frames = state.frames + 1
    if state.lifecycle ~= 'GRAPHICS_ACTIVE' then return end
    state.activeCallbacks = state.activeCallbacks + 1
    local activeSeq = state.activeCallbacks
    if state.complete or state.failure then return end
    if state.snapshotAttempts >= MAX_SNAPSHOT_ATTEMPTS then
        state.failure = 'snapshot_budget_exhausted'
        record('observer_failure', { reason = state.failure, activeSequence = activeSeq })
        return
    end
    if not should_sample(activeSeq, state.burst, state.snapshotAttempts) then return end
    state.snapshotAttempts = state.snapshotAttempts + 1
    state.attemptTimes[#state.attemptTimes + 1] = { sequence = state.frames,
        activeSequence = activeSeq, emulatedTime = now() }
    local ok, result = pcall(function()
        local pixels = m.video:snapshot_pixels()
        local width, height = m.video:snapshot_size()
        local evidence = crop_evidence(pixels, width, height, state.frames, now(), activeSeq)
        retain_first_candidate(pixels, width, height, state.frames, evidence.emulatedTime)
        local classified = classify_pixels(pixels, width, height, 'presenter')
        if type(pixels) == 'string' and width == 640 and height == 239
           and #pixels == width * height * 4 then
            local activation = collect_raw_band(pixels,width,height,{sx=1,sy=1,ox=0,oy=-10})
            if activation.complete then state.activationSeen = true end
        end
        return { evidence = evidence, classified = classified }
    end)
    if not ok then
        state.failure = 'snapshot_or_classifier_exception: ' .. tostring(result)
        record('observer_failure', { reason = state.failure })
        return
    end
    state.acquisitionCount = state.acquisitionCount + 1
    local evidence, classified = result.evidence, result.classified
    evidence.complete = classified.complete
    evidence.reason = classified.reason
    evidence.mismatchX = classified.x
    evidence.mismatchY = classified.y
    evidence.actualRGB = classified.actualRGB
    evidence.expectedRGB = classified.expectedRGB
    if not state.firstActive then
        state.firstActive = evidence
        record('first_graphics_active', { sequence = evidence.sequence,
            activeSequence = activeSeq, emulatedTime = evidence.emulatedTime,
            activationSeen = state.activationSeen })
    end
    if not state.firstLater and state.activeStart
       and evidence.emulatedTime - state.activeStart >= LATER_SECONDS then
        evidence.provisionalWindow = 'elapsed 3.0 emulated seconds; guest F$Sleep return unobserved'
        state.firstLater = evidence
        record('first_provisional_later', { sequence = evidence.sequence,
            activeSequence = activeSeq, emulatedTime = evidence.emulatedTime })
    end
    if classified.complete then
        state.matches = state.matches + 1
        local sig = signature(classified)
        if sig == state.lastSignature then state.stable = state.stable + 1
        else state.lastSignature = sig; state.stable = 1; state.stableSequence = {} end
        evidence.stableOrdinal = state.stable
        state.stableSequence[#state.stableSequence + 1] = evidence
        if not state.firstMatch then
            state.firstMatch = evidence
            record('first_exact_match', { sequence = evidence.sequence,
                activeSequence = activeSeq, emulatedTime = evidence.emulatedTime })
        end
        state.burst = math.max(0, REQUIRED_STABLE - state.stable)
        if state.stable >= REQUIRED_STABLE then
            state.complete = true
            state.burst = 0
            record('stable_three_frames', { first = state.stableSequence[1].sequence,
                last = evidence.sequence, emulatedTime = evidence.emulatedTime })
        end
    else
        state.rejects = state.rejects + 1
        local reason = classified.reason or 'unknown'
        state.reasonCounts[reason] = (state.reasonCounts[reason] or 0) + 1
        if not state.firstRejected then state.firstRejected = evidence end
        state.finalMismatch = evidence
        state.stable = 0
        state.lastSignature = nil
        state.stableSequence = {}
        state.burst = 0
        if reason == 'unexpected_presenter_geometry' or reason == 'pixel_length_mismatch'
           or reason == 'invalid_dimensions' then
            state.failure = reason
            record('observer_failure', { reason = reason,
                sequence = evidence.sequence, emulatedTime = evidence.emulatedTime })
        end
    end
end)
state.notifierInstalled = true
record('observer_notifier_installed', { globalHandle = type(type7_frame_notifier),
    lifetime = 'whole observer session; D.25D confirmed correction' })
