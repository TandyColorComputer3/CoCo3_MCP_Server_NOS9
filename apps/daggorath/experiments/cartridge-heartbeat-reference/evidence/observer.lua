-- Stage 3E.0 original-cartridge read-only heartbeat AV observer.
-- No guest writes. Direct taps observe cartridge RAM/PIA and completed output frames.
local root=os.getenv('STAGE3E_ROOT') or '/private/tmp/c6-stage3e0/calibration/'
local duration=tonumber(os.getenv('STAGE3E_DURATION') or '45')
local keyAt=tonumber(os.getenv('STAGE3E_KEY_AT') or '-1')
local keyPosted=false
local m=manager.machine
local cpu=m.devices[':maincpu']
local space=cpu.spaces['program']
local screen=m.screens[':screen']
local eventId=0
local currentEvent=0
local heartWrites=0
local firstHeartWrite=false
local nonheartWrites=0
local frameNo=0
local done=false
local heartbeatSnapshot=false
local eventStartFrame=-999
local lastSnapshotKey=''
local frameFile=assert(io.open(root..'frames.csv','w'))
local eventFile=assert(io.open(root..'events.csv','w'))
local infoFile=assert(io.open(root..'observer-info.txt','w'))
frameFile:write('callback_frame,screen_frame,emu_s,event_id,heartc,heartr,hearts,hbeatf,heartf,flip_desc,display_base,active_hex,raster_hex,raster_pair_mismatches,nonheart_writes,autptr,update,pc\n')
eventFile:write('event_id,kind,emu_s,screen_frame,pc,addr,old,new,heartc,heartr,hearts,display_base,detail\n');eventFile:flush()

local function r8(a) return space:read_u8(a & 0xffff) end
local function r16(a) return r8(a)*256+r8(a+1) end
local function hx(bytes) local t={} for i=1,#bytes do t[#t+1]=string.format('%02X',bytes[i]) end return table.concat(t) end
local function display_base() local d=r16(0x0209) if d==0 then return 0,d end return r16(d),d end
local function heart_bytes(base)
 local t={}
 if base~=0x1000 and base~=0x2800 then return t end
 local p=base+0x130f
 for row=0,6 do t[#t+1]=r8(p+row*32);t[#t+1]=r8(p+row*32+1) end
 return t
end
local function bright(p) local r=(p>>16)&0xff;local g=(p>>8)&0xff;local b=p&0xff;return (r+g+b)>=500 and 1 or 0 end
local function raster_bytes()
 local t={} local mismatch=0
 if not screen then return t,0 end
 -- Verified legacy VDG placement from the retained 640x236 storyboard:
 -- 256 logical pixels occupy x=64..575 at 2x horizontal; y=0 is output y=24.
 local x0,y0=304,177
 for row=0,6 do
  for cell=0,1 do
   local v=0
   for bit=0,7 do
    local x=x0+(cell*8+bit)*2
    local a=bright(screen:pixel(x,y0+row))
    local b=bright(screen:pixel(x+1,y0+row))
    if a~=b then mismatch=mismatch+1 end
    v=(v<<1)|a
   end
   t[#t+1]=v
  end
 end
 return t,mismatch
end
local function log_event(kind,addr,old,new,detail)
 local base=display_base()
 eventFile:write(string.format('%u,%s,%.9f,%u,%04X,%04X,%02X,%02X,%u,%u,%u,%04X,%s\n',currentEvent,kind,m.time:as_double(),screen and screen:frame_number() or 0,cpu.state.PC.value,addr or 0,old or 0,new or 0,r8(0x02ae),r8(0x02af),r8(0x02b0),base,detail or ''))
 eventFile:flush()
end
local heartAddr={}
for _,base in ipairs({0x1000,0x2800}) do local p=base+0x130f for row=0,6 do heartAddr[p+row*32]=true;heartAddr[p+row*32+1]=true end end
local taps={}
stage3e_taps=taps
local function safe(tag,fn) return function(...) local ok,err=pcall(fn,...) if not ok then infoFile:write(string.format('ERROR tag=%s emu=%.9f error=%s\n',tag,m.time:as_double(),tostring(err)));infoFile:flush();done=true;pcall(function() frameFile:close() end);pcall(function() eventFile:close() end);m:exit() end end end
taps[#taps+1]=space:install_write_tap(0x02ae,0x02ae,'s3e-heartc',safe('heartc',function(a,d,mask)
 local old=r8(a)
 if d==0 and old~=0 then eventId=eventId+1;currentEvent=eventId;eventStartFrame=screen and screen:frame_number() or 0;heartWrites=0;firstHeartWrite=false;log_event('T0_LOGICAL',a,old,d,'HEARTC reached zero') end
end))
taps[#taps+1]=space:install_write_tap(0xff22,0xff22,'s3e-pb1',safe('pb1',function(a,d,mask)
 local old=r8(a)
 if ((old~d)&0x02)~=0 then log_event('T2_PB1',a,old,d,'PB1 bit1 transition') end
end))
for a,_ in pairs(heartAddr) do
 local aa=a
 taps[#taps+1]=space:install_write_tap(aa,aa,'s3e-heart-'..string.format('%04x',aa),safe('heart-'..string.format('%04x',aa),function(addr,d,mask)
  local old=r8(addr);heartWrites=heartWrites+1
  if not firstHeartWrite then firstHeartWrite=true;log_event('V1_FIRST_WRITE',addr,old,d,'store=1') end
  if heartWrites==28 then log_event('V2_FINAL_WRITE',addr,old,d,'store=28') end
 end))
end
taps[#taps+1]=space:install_write_tap(0x1000,0x3fff,'s3e-framebuffer',safe('framebuffer',function(a,d,mask)
 if not heartAddr[a] then nonheartWrites=nonheartWrites+1 end
end))
infoFile:write(string.format('mame=%s screen=%s size=%ux%u frame_period=%.12f scan_period=%.12f duration=%.3f\n',emu.app_version(),tostring(screen),screen and screen.width or 0,screen and screen.height or 0,screen and screen.frame_period or 0,screen and screen.scan_period or 0,duration))
infoFile:write('rom_sha256=35e6a77354dcf1a3048f276824b7a0f9f759115fdd40603664cebfb3a7da6571\n')
infoFile:write('screen_mapping=x64+source_x*2,y24+source_y;heart=x304,y177,32x7 output pixels\n')
infoFile:flush()
stage3e_frameNotifier=emu.add_machine_frame_notifier(safe('frame',function()
 if done then return end
 frameNo=frameNo+1
 if keyAt>=0 and not keyPosted and m.time:as_double()>=keyAt then keyPosted=true;m.natkeyboard:post(' ');infoFile:write(string.format('posted_space=%.9f\n',m.time:as_double()));infoFile:flush() end
 local base,desc=display_base()
 local hb=heart_bytes(base)
 local rb,mis=raster_bytes()
 frameFile:write(string.format('%u,%u,%.9f,%u,%u,%u,%u,%u,%u,%04X,%04X,%s,%s,%u,%u,%04X,%u,%04X\n',frameNo,screen and screen:frame_number() or 0,m.time:as_double(),currentEvent,r8(0x02ae),r8(0x02af),r8(0x02b0),r8(0x02b1),r8(0x02ad),desc,base,hx(hb),hx(rb),mis,nonheartWrites,r16(0x020d),r8(0x0206),cpu.state.PC.value))
 frameFile:flush();nonheartWrites=0
 if currentEvent>=2 and not heartbeatSnapshot and screen then heartbeatSnapshot=true;pcall(function() screen:snapshot(root..'heartbeat-calibration.png') end) end
 local sf=screen and screen:frame_number() or 0;local age=sf-eventStartFrame
 if currentEvent>=2 and age>=1 and age<=2 and screen then local k=string.format('%04u-%02d',currentEvent,age);if k~=lastSnapshotKey then lastSnapshotKey=k;pcall(function() screen:snapshot(root..'raster-'..k..'.png') end) end end
 if frameNo==1800 and screen then pcall(function() screen:snapshot(root..'calibration-frame.png') end) end
 if m.time:as_double()>=duration then done=true;frameFile:close();eventFile:close();infoFile:close();m:exit() end
end))
