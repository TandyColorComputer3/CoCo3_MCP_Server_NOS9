local n=0
local mem=manager.machine.devices[":maincpu"].spaces["program"]
local f=io.open("/private/tmp/daggorath-timing/original/frames.bin","wb")
local log=io.open("/private/tmp/daggorath-timing/original/timing.csv","w")
local function word(a) return mem:read_u8(a)*256+mem:read_u8(a+1) end
capture_subscription=emu.add_machine_frame_notifier(function()
 n=n+1
 local descriptor=word(0x209);local base=word(descriptor)
 log:write(string.format("%d,%.9f,%d,%d,%d,%d,%d\n",n,manager.machine.time:as_double(),base,mem:read_u8(0x22d),mem:read_u8(0x29c),mem:read_u8(0x2b4),manager.machine.devices[":maincpu"].state["PC"].value))
 if base==0x1000 or base==0x2800 then
 local data={};for i=0,6143 do data[#data+1]=string.char(mem:read_u8(base+i)) end;f:write(table.concat(data))
 else f:write(string.rep(string.char(0),6144)) end
 if n==1200 then f:close();log:close();manager.machine:exit() end
end)
