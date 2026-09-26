dofile('/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/scripts/bridge.lua')
local dir='/private/tmp/daggorath-timing/final-live/'
local frames=io.open(dir..'graphics.bin','wb')
local log=io.open(dir..'graphics.csv','w')
local g=emu.item(manager.machine.devices[':gime'].items['0/m_gime_registers'])
local ram=emu.item(manager.machine.devices[':ram'].items['0/m_pointer'])
local size=emu.item(manager.machine.devices[':ram'].items['0/m_size']):read(0)
local n=0
observer_subscription=emu.add_machine_frame_notifier(function()
 if (g:read(8)&0x80)==0 then return end
 local base=(((g:read(13)<<11)|(g:read(14)<<3))|((g:read(11)&15)*0x80000))%size
 if base+16000>size then return end
 n=n+1;frames:write(ram:read_block(base,16000));frames:flush()
 log:write(string.format('%d,%.9f,%d,%d,%d\n',n,manager.machine.time:as_double(),base,g:read(8),g:read(9)));log:flush()
end)
