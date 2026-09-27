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
local root='/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/work/gameplay-m2/'
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


local space=machine.devices[':maincpu'].spaces.program
local trace=assert(io.open(root..'input-trace.jsonl','w'))
local function byte(a)return space:read_u8(a & 65535) end
local function word(a)return byte(a)*256+byte(a+1) end
local function str(a,n)local s={} for i=0,n-1 do local c=byte(a+i);if c==0 then break end;s[#s+1]=string.char(c) end;return table.concat(s) end
local function match(a,s)for i=1,#s,2 do if byte(a+(i-1)//2)~=tonumber(s:sub(i,i+1),16) then return false end end;return true end
local function event(k,s)s=s or {};s.kind=k;s.time=machine.time:as_double();trace:write(json_encode(s)..'\n');trace:flush() end
local base=nil;local minimumStack=65535
m2_input=space:install_write_tap(0,0xefff,'m2-ready',function(a,d)
 local cpu=machine.devices[':maincpu'];local pc=cpu.state.PC.value;local u=cpu.state.U.value;local y=cpu.state.Y.value
 if a==u-50 and match(pc-8,'1723953262e7c8cee6c8ce5d10260003') and match(pc-4486,'87cd4c294c1e118112') then
  base=pc-4486;local g=y+44
  local s={phase='ready',time=machine.time:as_double(),base=base,u=u,y=y,n=byte(u-47),dirty=byte(u-46),error=d,key=byte(u-48),input=str(u-34,32),row=byte(g+2579),col=byte(g+2580),dir=byte(g+2581),rate=byte(g+2584),lit=byte(g+2586),hand=word(g+2596),torch=word(g+2598),rightHand=word(g+2604),shownPhase=byte(u-41),empty=machine.natkeyboard.empty,posting=machine.natkeyboard.is_posting}
  s.stack=cpu.state.S.value;minimumStack=math.min(minimumStack,s.stack);s.minimumObservedStack=minimumStack;s.pixels=word(y+0x2341);s.mappedLength=word(y+0x2343);s.mmu=mark_array({});local mmu=emu.item(machine.devices[':gime'].items['0/m_mmu']);for i=0,15 do s.mmu[#s.mmu+1]=mmu:read(i) end
  local f=assert(io.open(root..'ready.tmp','w'));f:write(json_encode(s));f:close();os.rename(root..'ready.tmp',root..'ready.json');event('ready',s)
 end
end)
local chord=0;local shiftKey;local breakKey
for _,port in pairs(machine.ioport.ports) do for name,field in pairs(port.fields) do if name=='SHIFT' then shiftKey=field elseif name=='BREAK' then breakKey=field end end end
assert(shiftKey and breakKey)
m2_keyboard=emu.add_machine_frame_notifier(function()
 if chord>0 then if chord==6 then breakKey:set_value(1) end;chord=chord-1;if chord==0 then breakKey:clear_value();shiftKey:clear_value() end end
 local f=io.open(root..'keyboard.txt','r')
 if f and machine.natkeyboard.empty and not machine.natkeyboard.is_posting then
  local s=f:read('*a');f:close();os.remove(root..'keyboard.txt');event('enqueue',{text=s})
  if s=='@SHIFT_BREAK@' then shiftKey:set_value(1);chord=8 else machine.natkeyboard.in_use=true;machine.natkeyboard:post_coded(s) end
 elseif f then f:close() end
end)
