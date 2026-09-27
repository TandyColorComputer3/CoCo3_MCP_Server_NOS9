-- Read-only diagnostic hooks; never change emulated memory, I/O or registers.
local root=assert(os.getenv('DOD_GATE_CAPTURE'), 'set DOD_GATE_CAPTURE to an existing directory with trailing slash')
local machine=manager.machine
local function now() return (tostring(machine.time):gsub(',','')) end
local desc=assert(io.open(root..'items.txt','w'))
for tag,dev in pairs(machine.devices) do
 if tag:find('pia') or tag:find('ssc') or tag==':gime' then
  desc:write(tag..'\n');for name,id in pairs(dev.items) do desc:write('  '..name..' '..id..'\n') end
 end
end
desc:close()
local regs=emu.item(machine.devices[':ext:multi:slot2:ssc:cocossc_ay'].items['0/m_regs'])
local ram=emu.item(machine.devices[':ram'].items['0/m_pointer'])
local gime=emu.item(machine.devices[':gime'].items['0/m_gime_registers'])
local frames=assert(io.open(root..'frames.csv','w'))
frames:write('time,year,month,day,hour,minute,second,tick,ay0,ay1,ay2,ay3,ay4,ay5,ay6,ay7,ay8,ay9,ay10,ay11,ay12,ay13,ay14,ay15,gime8,gime9,mute\n')
gap_frame=emu.add_machine_frame_notifier(function()
 local a={now()};local raw=ram:read_block(0x28,7)
 for i=1,7 do a[#a+1]=string.byte(raw,i) end
 for i=0,15 do a[#a+1]=regs:read(i) end
 a[#a+1]=gime:read(8);a[#a+1]=gime:read(9);a[#a+1]=tostring(machine.sound.muted)
 frames:write(table.concat(a,',')..'\n');frames:flush()
end)
local io_log=assert(io.open(root..'writes.csv','w'));io_log:write('time,address,data,pc\n')
local space=machine.devices[':maincpu'].spaces['program']
gap_taps={}
for _,addr in ipairs({0xff01,0xff03,0xff23,0xff7d,0xff7e,0xff7f,0xff98,0xff99}) do
 gap_taps[#gap_taps+1]=space:install_write_tap(addr,addr,'gap-'..addr,function(a,d,m)
  io_log:write(now()..','..a..','..d..','..machine.devices[':maincpu'].state['PC'].value..'\n')
 end)
end
local streams={};local blocks=assert(io.open(root..'audio-blocks.csv','w'));blocks:write('time,tag,first_sample,count\n')
for tag,sound in pairs(machine.sounds) do
 if tag:find('coco_sac_tag') or tag:find('ssc_audio') then
  sound.hook=true;streams[tag]={file=assert(io.open(root..tag:gsub(':','_')..'.f32','wb')),count=0}
 end
end
emu.register_sound_update(function(data)
 for tag,channels in pairs(data) do
  local stream=streams[tag]
  if stream then
   local samples=channels[1];local b={}
   for i,x in ipairs(samples) do b[i]=string.pack('<f',x) end
   stream.file:write(table.concat(b));blocks:write(now()..','..tag..','..stream.count..','..#samples..'\n')
   stream.count=stream.count+#samples
  end
 end
 blocks:flush();io_log:flush()
end)
