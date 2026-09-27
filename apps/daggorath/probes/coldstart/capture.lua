-- Read-only instrumentation. No device register reads with side effects.
local root=assert(os.getenv('VIRQ_CAPTURE'))
local m=manager.machine
local cpu=m.devices[':maincpu'];local s=cpu.spaces['program']
local f=assert(io.open(root..'/trace.csv','w'))
f:write('time,kind,pc,cc,u,a,value,syssec,systick,irq,firq,suspend,sysblock,bytes\n')
local ram=emu.item(m.devices[':ram'].items['0/m_pointer'])
local mmu=emu.item(m.devices[':gime'].items['0/m_mmu'])
local irq=emu.item(cpu.items['0/m_irq_line'])
local firq=emu.item(cpu.items['0/m_firq_line'])
local suspend=emu.item(cpu.items['0/m_suspend'])
-- Task-0 DAT block from the saved MMU array. No side-effectful I/O reads.

local function log(kind,a,value)
 local base=mmu:read(0)*8192
 local pc=cpu.state['PC'].value;local bytes=''
 if pc+11<0xff00 then for i=0,11 do bytes=bytes..string.format('%02x',s:read_u8(pc+i)) end end
 f:write(tostring(m.time):gsub(',','')..','..kind..','..cpu.state['PC'].value..','..cpu.state['CC'].value..','..cpu.state['U'].value..','..a..','..value..','..ram:read(base+0x2d)..','..ram:read(base+0x2e)..','..irq:read(0)..','..firq:read(0)..','..suspend:read(0)..','..mmu:read(0)..','..bytes..'\n')
end
cold_taps={}
for page=0,0xef do
 local a=page*256+0x23
 cold_taps[#cold_taps+1]=s:install_write_tap(a,a,'count-'..page,function(a,d)
  if a~=cpu.state['U'].value+0x23 then return end
  local pc=cpu.state['PC'].value
  if s:read_u8(pc)==0x35 and s:read_u8(pc+1)==6 and s:read_u8(pc+2)==0x1c and s:read_u8(pc+3)==0xfe and s:read_u8(pc+4)==0x39 then log('callback',a,s:read_u8(a-1)*256+d) end
 end)
 a=page*256+0x24
 cold_taps[#cold_taps+1]=s:install_write_tap(a,a,'phase-'..page,function(a,d)
  local pc=cpu.state['PC'].value
  if s:read_u8(pc)==0x12 and s:read_u8(pc+1)==0x12 and s:read_u8(pc+2)==0x12 and s:read_u8(pc+3)==0x12 then log('phase',a,d);f:flush() end
 end)
end
cold_frame=emu.add_machine_frame_notifier(function() log('frame',0,0);f:flush() end)
local count=0
cold_taps[#cold_taps+1]=s:install_write_tap(0xff48,0xff48,'fdc-cmd',function(a,d) count=0;log('fdc-command',a,d) end)
cold_taps[#cold_taps+1]=s:install_read_tap(0xff4b,0xff4b,'fdc-data',function(a,d) count=count+1;if count==1 or count==256 then log('fdc-data',a,count) end end)
cold_taps[#cold_taps+1]=s:install_write_tap(0xff74,0xff74,'scii-data-write',function(a,d)log('scii-data-write',a,d)end)
cold_load=emu.add_machine_post_load_notifier(function()log('postload',0,0);f:flush()end)
local info=assert(io.open(root..'/saved-items.txt','w'))
for _,tag in ipairs({':gime',':maincpu',':ram'}) do for key,value in pairs(m.devices[tag].items) do info:write(tag..' '..key..' '..tostring(value)..'\n') end end
info:close()
