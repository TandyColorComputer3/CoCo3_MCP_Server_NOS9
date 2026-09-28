local m=manager.machine
local cpu=m.devices[':maincpu']
local mem=cpu.spaces.program
local root=assert(os.getenv('DOD_AUDIO_ROOT'))
local log=assert(io.open(root..'/events.csv','w'))
log:write('time,marker,pc\n');log:flush()
local code={}
local function add(...) for _,v in ipairs({...}) do code[#code+1]=v end end
for id=0,22 do
 add(0x86,id,0xb7,0x60,0x00) -- LDA #id; STA $6000
 add(0xc6,0xff,0xbd,0xc7,0xd0) -- LDB #255; JSR original SOUNDX
 add(0x86,0x80+id,0xb7,0x60,0x00) -- end marker
 add(0x8e,0xff,0xff,0x30,0x1f,0x26,0xfc) -- bounded inter-effect pause
end
add(0x20,0xfe) -- idle loop when all effects finish
local injected=false
local finished=nil
catalog_tap=mem:install_write_tap(0x6000,0x6000,'catalog',function(a,d,mask)
 local t=m.time:as_double()
 log:write(string.format('%.9f,%d,%04x\n',t,d,cpu.state.PC.value));log:flush()
 if d==0x80+22 then finished=t end
end)
catalog_frame=emu.add_machine_frame_notifier(function()
 local t=m.time:as_double()
 if not injected and t>1.50 then
  injected=true
  mem:write_u8(0x029c,0) -- original NOISEF, prevent wizard buzz overlap
  mem:write_u8(0x02b1,0) -- original HBEATF, prevent PB1 overlap
  mem:write_u8(0x6000,255)
  for i,v in ipairs(code) do mem:write_u8(0x4fff+i,v) end
  cpu.state.PC.value=0x5000
  log:write(string.format('%.9f,inject,%04x\n',t,cpu.state.PC.value));log:flush()
 end
 if finished and t>finished+0.60 then log:close();m:exit() end
 if t>120 then log:write('timeout\n');log:close();m:exit() end
end)
