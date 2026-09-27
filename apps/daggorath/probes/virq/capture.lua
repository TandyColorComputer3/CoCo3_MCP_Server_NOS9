-- Read-only MAME taps for the disposable counter driver, not production bridge.
-- Match assembled callback tail AND U-relative counter address; no guest writes.
local root=assert(os.getenv('VIRQ_CAPTURE'))
local m=manager.machine
local cpu=m.devices[':maincpu']
local s=cpu.spaces['program']
local f=assert(io.open(root..'/callbacks.csv','w'))
f:write('time,frame,pc,static,count,cc\n')
local frame=0
local function tm() return tostring(m.time):gsub(',','') end
virq_frame=emu.add_machine_frame_notifier(function() frame=frame+1 end)
local function record(a,d,mask)
 if a%256~=0x23 then return end
 local u=cpu.state['U'].value
 if a~=(u+0x23) then return end
 local pc=cpu.state['PC'].value
 if s:read_u8(pc)==0x35 and s:read_u8(pc+1)==6 and s:read_u8(pc+2)==0x1c and s:read_u8(pc+3)==0xfe and s:read_u8(pc+4)==0x39 then
  f:write(tm()..','..frame..','..pc..','..u..','..(s:read_u8(a-1)*256+d)..','..cpu.state['CC'].value..'\n');f:flush()
 end
end
virq_counter_taps={}
for page=0,0xef do
 local a=page*256+0x23
 virq_counter_taps[#virq_counter_taps+1]=s:install_write_tap(a,a,'vc-'..page,record)
end
virq_load=emu.add_machine_post_load_notifier(function() f:write('#postload,'..tm()..','..frame..'\n');f:flush() end)
