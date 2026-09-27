local m=manager.machine
local mem=m.devices[':maincpu'].spaces.program
local w=assert(os.getenv('NATIVE_ORIGINAL'))
local f=assert(io.open(w..'/edges.csv','w'));f:write('time,data,pc,cc\n')
original_tap=mem:install_write_tap(0xff22,0xff22,'heartbeat',function(a,d,mask)f:write(tostring(m.time):gsub(',','')..','..d..','..m.devices[':maincpu'].state.PC.value..','..m.devices[':maincpu'].state.CC.value..'\n');f:flush()end)
local state=assert(io.open(w..'/state.csv','w'));state:write('time,enable,count,rate\n')
original_frame=emu.add_machine_frame_notifier(function()
 state:write(tostring(m.time):gsub(',','')..','..mem:read_u8(0x2b1)..','..mem:read_u8(0x2ae)..','..mem:read_u8(0x2af)..'\n');state:flush()
 if m.time:as_double()>40 then m:exit() end
end)
