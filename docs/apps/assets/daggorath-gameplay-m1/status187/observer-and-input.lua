-- CoCo 3 MCP bridge. Node listens. This script connects with emu.file.

local JSON_NULL = {}
local ARRAY = {}

local function mark_array(items)
  return setmetatable(items, ARRAY)
end

local function is_array(value)
  return getmetatable(value) == ARRAY
end

local function json_escape(text)
  text = string.gsub(text, "\\", "\\\\")
  text = string.gsub(text, "\"", "\\\"")
  text = string.gsub(text, "\r", "\\r")
  text = string.gsub(text, "\n", "\\n")
  text = string.gsub(text, "\t", "\\t")
  return text
end

local function json_encode(value)
  local kind = type(value)
  if value == JSON_NULL or value == nil then
    return "null"
  elseif kind == "boolean" then
    if value then
      return "true"
    end
    return "false"
  elseif kind == "number" then
    return string.format("%.14g", value)
  elseif kind == "string" then
    return "\"" .. json_escape(value) .. "\""
  elseif kind == "table" then
    if is_array(value) then
      local parts = {}
      for index = 1, #value do
        parts[index] = json_encode(value[index])
      end
      return "[" .. table.concat(parts, ",") .. "]"
    end
    local parts = {}
    for key, item in pairs(value) do
      if type(key) == "string" and item ~= nil then
        parts[#parts + 1] = json_encode(key) .. ":" .. json_encode(item)
      end
    end
    return "{" .. table.concat(parts, ",") .. "}"
  end
  return "null"
end

-- Read-only diagnostic hooks; never change emulated memory, I/O or registers.
local function encode_record(e)
 local fields={}
 for k,v in pairs(e) do
  assert(type(v)=='number' or type(v)=='boolean')
  fields[#fields+1]=string.format('%q',k)..':'..tostring(v)
 end
 return '{'..table.concat(fields,',')..'}'
end
local root='/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/work/gameplay-187-resolution/cancel-chord/'
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
for _,addr in ipairs({0xff01,0xff03,0xff22,0xff23,0xff7d,0xff7e,0xff7f,0xff98,0xff99}) do
 gap_taps[#gap_taps+1]=space:install_write_tap(addr,addr,'gap-'..addr,function(a,d,m)
  io_log:write(now()..','..a..','..d..','..machine.devices[':maincpu'].state['PC'].value..'\n')
 end)
end
local pia=machine.devices[':pia1']
local ports=assert(io.open(root..'pia.csv','w'));ports:write('time,ddrb,outb,ctlb,cc\n')
native_pia_frame=emu.add_machine_frame_notifier(function()
 ports:write(now()..','..emu.item(pia.items['0/m_ddr_b']):read(0)..','..emu.item(pia.items['0/m_out_b']):read(0)..','..emu.item(pia.items['0/m_ctl_b']):read(0)..','..machine.devices[':maincpu'].state['CC'].value..'\n');ports:flush()
end)
local reads=assert(io.open(root..'reads.csv','w'));reads:write('time,address,pc,cc\n')
for _,addr in ipairs({0xff22,0xff23}) do
 gap_taps[#gap_taps+1]=space:install_read_tap(addr,addr,'native-read-'..addr,function(a,d,m)
 reads:write(now()..','..a..','..machine.devices[':maincpu'].state['PC'].value..','..machine.devices[':maincpu'].state['CC'].value..'\n')
 end)
end

local ticks=assert(io.open(root..'ticks.csv','w'))
ticks:write('time,pc,u,cc,active,rate,remaining,enabled,fault\n')
hb_taps={}
for page=0,0xef do
 local address=page*256+0x21
 hb_taps[#hb_taps+1]=space:install_write_tap(address,address,'hb-tick-'..page,function(a,d)
  local cpu=machine.devices[':maincpu'];local u=cpu.state['U'].value;local pc=cpu.state['PC'].value
  if a==u+0x21 and space:read_u8(pc)==0x6d and space:read_u8(pc+1)==0xc8 and space:read_u8(pc+2)==0x29 then
   ticks:write(now()..','..pc..','..u..','..cpu.state['CC'].value..','..space:read_u8(u+0x22)..','..space:read_u8(u+0x27)..','..space:read_u8(u+0x28)..','..space:read_u8(u+0x29)..','..space:read_u8(u+0x2a)..'\n');ticks:flush()
  end
 end)
end

local fdc=assert(io.open(root..'fdc.csv','w'));fdc:write('time,kind,address,data,pc\n')
fdc_read=space:install_read_tap(0xff48,0xff4b,'game-fdc-read',function(a,d)fdc:write(now()..',read,'..a..','..d..','..machine.devices[':maincpu'].state.PC.value..'\n');fdc:flush()end)
fdc_write=space:install_write_tap(0xff48,0xff4b,'game-fdc-write',function(a,d)fdc:write(now()..',write,'..a..','..d..','..machine.devices[':maincpu'].state.PC.value..'\n');fdc:flush()end)

local trace=assert(io.open(root..'input-trace.jsonl','w'))
local lastReady=nil;local mainbase=nil;local frameU=nil;local dataY=nil;local devs={}
local function event(kind,t)
 t=t or {};t.kind=kind;t.time=machine.time:as_double()
 trace:write(json_encode(t)..'\n');trace:flush()
end
local function byte(a)return space:read_u8(a & 0xffff)end
local function str(a,n)local t={} for i=0,n-1 do local c=byte(a+i);if c==0 then break end;t[#t+1]=string.char(c)end;return table.concat(t)end
local function match(pc,hex)
 for i=1,#hex,2 do if byte(pc+(i-1)//2)~=tonumber(hex:sub(i,i+1),16) then return false end end
 return true
end
local function appstate()
 local u=frameU;local y=dataY;local g=y+44
 return {u=u,y=y,base=mainbase,n=byte(u-46),dirty=byte(u-45),error=byte(u-49),result=byte(u-44),key=byte(u-47),input=str(u-34,32),previous=byte(u-40)*256+byte(u-39),clockNow=byte(u-38)*256+byte(u-37),delta=byte(u-36)*256+byte(u-35),row=byte(g+2579),col=byte(g+2580),dir=byte(g+2581),rate=byte(g+2584),lit=byte(g+2586),hand=byte(g+2596)*256+byte(g+2597),torch=byte(g+2598)*256+byte(g+2599),empty=machine.natkeyboard.empty and true or false,posting=machine.natkeyboard.is_posting and true or false}
end
local inp='e6c8cf5d1026000316000316';local ready='1720ae3262e7c8cfe6c8cf5d';local ender='1729cd3262e7c8d0e6c8cf5d'
local sites={
[3055]={label="os_intercept()",offset=-49,sig="1725643262e7c8cfe6c8cf5d10260003"},
[3091]={label="os_clock()",offset=-49,sig="17264f3264e7c8cfe6c8cf5d10260003"},
[3168]={label="screen_open()",offset=-49,sig="e784172064e7c8cfe6c8cf5d10260003"},
[3200]={label="os_clock()",offset=-49,sig="1725e23264e7c8cfe6c8cf5d10260003"},
[3228]={label="native_heartbeat_open()",offset=-49,sig="172c793262e7c8cfe6c8cf5d10260003"},
[3263]={label="native_heartbeat_rate()",offset=-49,sig="172cf13264e7c8cfe6c8cf5d10260003"},
[3291]={label="native_heartbeat_enable()",offset=-49,sig="172d093262e7c8cfe6c8cf5d10260003"},
[3319]={label="os_signal_value()",offset=-49,sig="1724f03262e7c8cfe6c8cf5d10260003"},
[3350]={label="os_clock()",offset=-49,sig="17254c3264e7c8cfe6c8cf5d10260003"},
[3435]={label="delta > 300 guard",offset=-49,sig="160008c6bbe7c8cf160373e6a90a44e7"},
[3579]={label="native_heartbeat_rate()",offset=-49,sig="172bb53264e7c8cfe6c8cf5d10260003"},
[3743]={label="screen_path()",offset=-49,sig="171d013264e7c8cfe6c8cf5d10260003"},
[3843]={label="game_command()",offset=-44,sig="170fed3264e7c8d4e6c8d4c101102700"},
[3935]={label="native_heartbeat_rate()",offset=-49,sig="172a513264e7c8cfe6c8cf5d10260003"},
[4259]={label="screen_present()",offset=-49,sig="171e783262e7c8cfe6c8cf5d10260003"},
[4304]={label="os_sleep()",offset=-49,sig="1720ae3262e7c8cfe6c8cf5d10260003"},
[4335]={label="native_heartbeat_close()",offset=-48,sig="1729cd3262e7c8d0e6c8cf5d10260009"},
[4352]={label="native_heartbeat_close()",offset=-49,sig="0000e6c8d0e7c8cf171e42e7c8d0e6c8"},
[4358]={label="screen_close()",offset=-48,sig="c8cf171e42e7c8d0e6c8cf5d10260009"},
[4375]={label="screen_close()",offset=-49,sig="0000e6c8d0e7c8cfe6c8cf4f3406e6a9"},
}
input_trace_tap=space:install_write_tap(0,0xefff,'input-boundaries',function(a,d)
 local cpu=machine.devices[':maincpu'];local pc=cpu.state.PC.value;local u=cpu.state.U.value
 -- Match by linked module bytes, never by an assumed load address.
 if a==u-49 or a==u-48 or a==u-44 then
  for off,site in pairs(sites) do
   local base=pc-off
   if base>=0 and a==u+site.offset and match(pc-8,site.sig) and match(base,"87cd465d465211812a") then
    mainbase=base;frameU=u;dataY=cpu.state.Y.value
    local t=appstate();t.site=site.label;t.returnCode=d;t.storePC=pc
    if d~=0 or site.label=="game_command()" or site.label=="screen_present()" or site.offset==-48 then event('operation_result',t) end
   end
  end
 end
 -- Level II FExit: STB P$Signal,X; LEAY P$PID,X; BRA ... (source fexit.asm).
 local x=cpu.state.X.value
 if a==x+25 and match(pc-3,'e78819') and match(pc,'310120') then
  event('process_exit',{pid=byte(x),parent=byte(x+1),status=d,pc=pc,descriptor=x,module=byte(x+17)*256+byte(x+18)})
 end
 if mainbase and pc==mainbase+0x3289 and a==u-4 then
  local packet={};for i=0,6 do packet[#packet+1]=byte(u-17+i) end
  event('clock_packet',{packet=mark_array(packet),status=d,validate=byte(u+7),pc=pc})
 end
 if mainbase and a==frameU-49 and d~=0 then local t=appstate();t.storePC=pc;t.returnCode=d;event('main_error_store',t) end
 if a==u-49 and match(pc,inp) and match(pc-8,"171d013264e7c8cf") then
  mainbase=pc-0xe9f;frameU=u;dataY=cpu.state.Y.value
  local s=appstate();if s.key~=0 or d~=0 then event('app_read',s) end
 end
 if mainbase and pc==mainbase+0x10c8 then
  local s=appstate();s.pc=pc
  if s.dirty==0 and s.error==0 then
   s.phase='ready';s.time=machine.time:as_double();lastReady=s
   local f=assert(io.open(root..'ready.tmp','w'));f:write(json_encode(s));f:close();os.rename(root..'ready.tmp',root..'ready.json')
   if s.input~=(lastInput or '') then event('buffer_ready',s);lastInput=s.input end
  end
 end
 if mainbase and pc==mainbase+0x10e7 then event('teardown',appstate());mainbase=nil end
 -- VTIO EndPtr update followed by LDB V.ULCase,U, source ClickChk.
 if a==u+0x33 and byte(pc)==0xe6 and byte(pc+1)==0xc8 and byte(pc+2)==0x21 then
  devs[u]=true;event('vtio_enqueue',{dev=u,character=cpu.state.A.value,endPtr=d,inpPtr=byte(u+0x34),pc=pc})
 end
 for dev in pairs(devs) do
  if a==dev+0x34 and pc==42595 then event('vtio_consume',{dev=dev,character=cpu.state.A.value,inpPtr=d,endPtr=byte(dev+0x33),pc=pc}) end
  if a>=dev+0x80 and a<dev+0x100 and pc==43341 then event('vtio_buffer_write',{dev=dev,character=d,index=a-dev-0x80,pc=pc}) end
 end
end)
local prevRows='';local lastFlags='';local chord=0;local shiftKey;local breakKey
for _,port in pairs(machine.ioport.ports) do for name,field in pairs(port.fields) do if name=='SHIFT' then shiftKey=field elseif name=='BREAK' then breakKey=field end end end
assert(shiftKey and breakKey,'SHIFT/BREAK fields not found')
input_frame=emu.add_machine_frame_notifier(function()
 if chord>0 then
  if chord==6 then breakKey:set_value(1);event('chord_break_down') end
  chord=chord-1
  if chord==0 then breakKey:clear_value();shiftKey:clear_value();event('chord_released') end
 end
 local t={}
 for i=0,6 do local p=machine.ioport.ports[':row'..i];if p then t[#t+1]=p:read() end end
 local rows=table.concat(t,',')
 if rows~=prevRows then event('matrix',{rows=rows});prevRows=rows end
 local flags=tostring(machine.natkeyboard.empty)..','..tostring(machine.natkeyboard.is_posting)
 if flags~=lastFlags then event('queue_flags',{flags=flags});lastFlags=flags end
 local f=io.open(root..'keyboard.txt','r')
 if f and machine.natkeyboard.empty and not machine.natkeyboard.is_posting then
  local text=f:read('*a');f:close();os.remove(root..'keyboard.txt')
  event('enqueue',{text=text});if text=='@SHIFT_BREAK@' then shiftKey:set_value(1);chord=8;event('chord_shift_down') else machine.natkeyboard.in_use=true;machine.natkeyboard:post_coded(text);event('posted',{text=text}) end
 elseif f then f:close() end
end)
local ports=assert(io.open(root..'keyboard-description.txt','w'));ports:write(machine.natkeyboard:dump());ports:close()
