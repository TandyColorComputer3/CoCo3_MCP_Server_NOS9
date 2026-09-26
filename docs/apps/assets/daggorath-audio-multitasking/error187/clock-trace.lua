-- Research-only read observer; no guest or hardware writes.
local trace = assert(io.open('/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/work/audio-187/clock-trace.csv','w'))
trace:write('frame,emulated_time,year,month,day,hour,minute,second,tick\n')
local frame, last = 0, ''
audio_research_observer = emu.add_machine_frame_notifier(function()
 frame=frame+1
 local dev=manager.machine.devices[':ram']
 if not dev or not dev.items['0/m_pointer'] then return end
 local raw=emu.item(dev.items['0/m_pointer']):read_block(0x28,7)
 local a={string.byte(raw,1,7)}
 local key=table.concat(a,',')
 if key~=last then trace:write(frame..','..tostring(manager.machine.time):gsub(',','')..','..key..'\n');trace:flush();last=key end
end)
