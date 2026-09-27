local m=manager.machine
local mem=m.devices[':maincpu'].spaces.program
local w="/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/work/gameplay-m3/original"
local statusDone=false
status_entry=mem:install_read_tap(0xc5d9,0xc5d9,'status-entry',function(a)if math.abs(m.devices[':maincpu'].state.PC.value-a)<2 then statusDone=false end end)
status_return=mem:install_read_tap(0xc608,0xc608,'status-return',function(a)if math.abs(m.devices[':maincpu'].state.PC.value-a)<2 then statusDone=true end end)
local seen={};local sent=0;local pressed=false;local large=false;local lastLabel=nil;local stable=0
local function word(a)return mem:read_u8(a)*256+mem:read_u8(a+1) end
capture_subscription=emu.add_machine_frame_notifier(function()
 local t=m.time:as_double()
 if not pressed and t>20 then pressed=true;m.natkeyboard:post(' ') end
 if pressed and mem:read_u8(0x2ad)~=0 and mem:read_u8(0x2b1)~=0 then
  local left=word(0x21d);local right=word(0x21f);local phase=mem:read_u8(0x2b0)
  local label=left==0 and right==0 and 'empty' or left~=0 and right==0 and 'left' or left==0 and right~=0 and 'right' or 'both'
  if label~=lastLabel then lastLabel=label;stable=0 end;stable=stable+1
  if phase==255 then large=true end
  local key=label..'-'..phase
  if not seen[key] and large and stable>2 and statusDone then
   seen[key]=true
   local f=assert(io.open(w..'/'..key..'.bin','wb'))
   for a=0x1000,0x3fff do f:write(string.char(mem:read_u8(a))) end;f:close()
   local q=assert(io.open(w..'/'..key..'-state.bin','wb'));for _,a in ipairs({0x229,0x21d,0x21f,0x224}) do q:write(string.char(mem:read_u8(a),mem:read_u8(a+1))) end;for a=0xe87,0xea2 do q:write(string.char(mem:read_u8(a))) end;q:close()
   m.screens[':screen']:snapshot(w..'/'..key..'.png')
   local f=assert(io.open(w..'/states.txt','a'));f:write(string.format('%f %s %04x %04x\n',t,key,left,right));f:close()
  end
  if m.natkeyboard.empty then
   if sent==0 and seen['empty-0'] and seen['empty-255'] then m.natkeyboard:post('P R W SW\r');sent=1
   elseif sent==1 and seen['right-0'] and seen['right-255'] then m.natkeyboard:post('P L P T\r');sent=2
   elseif sent==2 and seen['both-0'] and seen['both-255'] then m.natkeyboard:post('S R\r');sent=3
   elseif sent==3 and seen['left-0'] and seen['left-255'] then m:exit() end
  end
 end
 if t>90 then m:exit() end
end)
