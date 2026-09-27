local m=manager.machine
local mem=m.devices[':maincpu'].spaces.program
local root='/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/MCP/work/gameplay-m5/original/'
local steps={
{name="initial",cmd="E"},
{name="00-e",cmd="P L SWORD"},
{name="01-p-l-sword",cmd="D L"},
{name="02-d-l",cmd="GET R SWORD"},
{name="03-get-r-sword",cmd="S R"},
{name="04-s-r",cmd="P L SWORD"},
{name="05-p-l-sword",cmd="P R TORCH"},
{name="06-p-r-torch",cmd="D L"},
{name="07-d-l",cmd="D R"},
{name="08-d-r",cmd="GET L TORCH"},
{name="09-get-l-torch",cmd="USE LEFT"},
{name="10-use-left",cmd="EXAMINE BAG"},
{name="11-examine-bag",cmd="LOOK"},
{name="12-look",cmd="E"},
{name="13-e",cmd="LOOK"},
{name="14-look"}
}
local index=1;local done=false;local pressed=false
local function byte(a)return mem:read_u8(a)end
local function word(a)return byte(a)*256+byte(a+1) end
render_return=mem:install_read_tap(0xc65f,0xc65f,'render-return',function(a)
 if math.abs(m.devices[':maincpu'].state.PC.value-a)<2 and index==1 then done=true end
end)
-- A 6809 instruction read tap can see prefetch before JSR completes.
-- Qualify the actual HMAN99 STU LINPTR write instead (DF 11 at D2B4).
command_return=mem:install_write_tap(0x212,0x212,'command-return',function(a,d)
 local cpu=m.devices[':maincpu']
 if cpu.state.PC.value==0xd2b6 and cpu.state.U.value==0x2f1 and index>1 then done=true end
end)
frames=emu.add_machine_frame_notifier(function()
 local t=m.time:as_double()
 if t>20 and not pressed then pressed=true;m.natkeyboard:post(' ') end
 if pressed and byte(0x2ad)~=0 and byte(0x2b1)~=0 then
  local expected=steps[index]
  if done and byte(0x2b4)==0 and m.natkeyboard.empty and not m.natkeyboard.is_posting then
   local step=steps[index];local f=assert(io.open(root..step.name..'.bin','wb'))
   for a=0,0x3fff do f:write(string.char(byte(a))) end;f:close()
   local base=word(word(0x209));assert(base==0x1000 or base==0x2800)
   local q=assert(io.open(root..step.name..'-active.bin','wb'));for a=base,base+6143 do q:write(string.char(byte(a))) end;q:close()
   m.screens[':screen']:snapshot(root..step.name..'.png')
   local f=assert(io.open(root..'capture-times.txt','a'));f:write(step.name..' '..t..'\n');f:close()
   if not step.cmd then m:exit();return end
   m.natkeyboard:post(step.cmd..'\r');index=index+1;done=false
  end
 end
 if t>120 then m:exit() end
end)
