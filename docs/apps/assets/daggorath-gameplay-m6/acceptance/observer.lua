-- Private read-only lifecycle instrumentation plus established digital-key input.
local R='/private/tmp/m6-boundary/'
local f=assert(io.open(R..'meta.json'));local meta=json_decode(f:read('*a'));f:close()
local m=manager.machine;local cpu=m.devices[':maincpu'];local mem=cpu.spaces.program
local log=assert(io.open(R..'trace.jsonl','w'))
local function event(kind,t) t=t or {};t.kind=kind;t.time=m.time:as_double();log:write(json_encode(t)..'\n');log:flush() end
local function byte(a)return mem:read_u8(a&0xffff)end
local function word(a)return byte(a)*256+byte(a+1)end
local nativeU=nil;local tickCount=0;local edgeCount=0;local lastbit=nil;local activeInterval=false
local initialTicks=0;local base,Y,U;local ready=false;local busy=false;local last={};local taps={}
local function valid(b) if b<0 then return false end;for i,v in ipairs(meta.header)do if byte(b+i-1)~=v then return false end end;return true end
local function state()
 if not U then return {} end
 local text='';for i=0,31 do local v=byte(U-34+i);if v==0 then break end;text=text..string.char(v)end
 return {base=base,y=Y,u=U,input=text,n=byte(U-53),dirty=byte(U-52),error=byte(U-56),key=byte(U-54),inputEmpty=byte(U-41),signal=byte(Y+meta.symbols._signalFlag),row=byte(Y+meta.symbols._game+2579),col=byte(Y+meta.symbols._game+2580),phase=byte(Y+meta.symbols._creatureScheduler+64)}
end
local function once(kind,t)
 local k=kind..':'..tostring(t.pc or '')..':'..tostring(m.time)
 if not last[k]then last={};last[k]=true;event(kind,t)end
end
local function arm()
taps[#taps+1]=mem:install_write_tap(0,0xefff,'m6-lifecycle-write',function(a,d)
 local pc=cpu.state.PC.value;local y=cpu.state.Y.value
 if a==cpu.state.U.value+0x21 and byte(pc)==0xec and byte(pc+1)==0xc8 and byte(pc+2)==0x2c then
 nativeU=cpu.state.U.value;tickCount=tickCount+1;event('heartbeat_tick',{tick=tickCount,u=nativeU,active=byte(nativeU+0x22),rate=byte(nativeU+0x27),remaining=byte(nativeU+0x28),enabled=byte(nativeU+0x29),fault=byte(nativeU+0x2a),phase=byte(nativeU+0x2b)})
 end
 if nativeU and a==nativeU+0x22 and d==0 and byte(pc)==0x6f and byte(pc+1)==0xc8 and byte(pc+2)==0x23 then activeInterval=false;event('virq_removed',{tick=tickCount,edges=edgeCount})end
 if not base then local b=pc-meta.symbols._main-2;if valid(b)then base=b;Y=y;event('main_enter',{base=b,y=y});taps[#taps+1]=mem:install_read_tap(math.max(0,cpu.state.S.value-1600),math.min(0xefff,cpu.state.S.value+100),'m6-lifecycle-read',function(a,d)
 if not base or cpu.state.Y.value~=Y then return end
 local pc=cpu.state.PC.value;local name=meta.returns[tostring(pc-base)]
 if name=='_game_init'and valid(base)then local f=assert(io.open(R..'port-initial.bin','wb'));for i=0,2605 do f:write(string.char(byte(Y+meta.symbols._game+i)))end;f:close();event('initial_state',{bytes=2606,clockTicks=initialTicks,second=math.floor(initialTicks/60)})end
 if name and valid(base) and name~='_game_input'and name~='_os_sleep'and name~='_native_heartbeat_take_ticks'then once('leave',{name=name,pc=pc,status=cpu.state.B.value})end
end)end end
 if base and y==Y and valid(base)then
  local off=pc-base;local name=meta.entries[tostring(off)]
  if name then
   if name=='_main'then return end
   if name=='_game_init'then initialTicks=word(cpu.state.U.value-40)end
   if name=='_game_input'or name=='_os_sleep'then
    U=cpu.state.U.value
    -- Called from main only; its saved frame points to main's 56-byte locals.
    if name=='_os_sleep' and word(cpu.state.S.value+2)==base+0x167e then ready=true end
   end
   if name=='_game_input'then ready=true end
   if name~='_game_input' and name~='_os_sleep' and name~='_native_heartbeat_take_ticks'then once('enter',{name=name,pc=pc})end
   if name=='_native_heartbeat_enable'then activeInterval=true;lastbit=nil end
   if name=='_game_creature_advance'then once('simulation_batch',{pc=pc,ticks=word(U-38),phase=byte(Y+meta.symbols._creatureScheduler+64),callback=tickCount})end
   if name=='_native_heartbeat_close'then local t=state();t.pc=pc;event('loop_terminated',t);busy=false;ready=false end
  end
  if a==Y+meta.symbols._creatureScheduler+64 and d==0 and off>=meta.symbols._game_creature_advance and off<meta.symbols._game_input then event('qten_boundary',{callback=tickCount,phase=0})end
  if a==Y+meta.symbols._presentedGeneration+3 then local p=Y+meta.symbols._presentedGeneration;event('presented',{generation=byte(p)*16777216+byte(p+1)*65536+byte(p+2)*256+d,observedEdges=edgeCount,callback=tickCount})end
  if a==cpu.state.U.value-56 and off>=meta.symbols._main and off<0x1760 then U=cpu.state.U.value end
 end
end)
end
taps[#taps+1]=mem:install_read_tap(0xff40,0xff5a,'m6-fdc-read',function(a,d) if activeInterval then event('active_fdc',{address=a,data=d,read=true,pc=cpu.state.PC.value})end end)
taps[#taps+1]=mem:install_write_tap(0xff40,0xff5a,'m6-fdc-write',function(a,d) if activeInterval then event('active_fdc',{address=a,data=d,read=false,pc=cpu.state.PC.value})end end)
taps[#taps+1]=mem:install_read_tap(0xff74,0xff76,'m6-scii-read',function(a,d)if activeInterval then event('active_fdc',{address=a,data=d,read=true,pc=cpu.state.PC.value})end end)
taps[#taps+1]=mem:install_write_tap(0xff74,0xff76,'m6-scii-write',function(a,d)if activeInterval then event('active_fdc',{address=a,data=d,read=false,pc=cpu.state.PC.value})end end)
taps[#taps+1]=mem:install_write_tap(0xff22,0xff22,'m6-pb1',function(a,d)
 if activeInterval and nativeU and cpu.state.U.value==nativeU then local bit=d&2;if lastbit==nil or bit~=lastbit then edgeCount=edgeCount+1;lastbit=bit;event('heartbeat_edge',{edge=edgeCount,tick=tickCount,bit=bit,pc=cpu.state.PC.value})end end
end)
for _,name in ipairs({'type','read_text_console','finish_run'})do
 local old=commands[name];commands[name]=function(p)
  if name=='type'then event('bridge_type',{text=p.text})end
  local result=old(p)
  if name=='finish_run'then event('bridge_finish',result)end
  if name=='read_text_console'and result.supported and not busy then
   local k=result.cells;if k~=lastconsole then lastconsole=k;event('console',{supported=result.supported,idle=result.idle,cells=result.cells,epoch=result.epoch})end
  end
  return result
 end
end
local shift,brk;for _,p in pairs(m.ioport.ports)do for name,field in pairs(p.fields)do if name=='SHIFT'then shift=field elseif name=='BREAK'then brk=field end end end
assert(shift and brk)
local chord=0;local armed=false
m6_frame=emu.add_machine_frame_notifier(function()
 if not armed then local a=io.open(R..'arm');if a then a:close();armed=true;arm();event('observer_armed')end end
 local block=emu.item(m.devices[':ram'].items['0/m_pointer']):read_block(0x28,7);if block and #block==7 then local c=assert(io.open(R..'clock.tmp','w'));c:write(json_encode({time=m.time:as_double(),year=block:byte(1),month=block:byte(2),day=block:byte(3),hour=block:byte(4),minute=block:byte(5),second=block:byte(6),remaining=block:byte(7)}));c:close();os.rename(R..'clock.tmp',R..'clock.json')end
 if chord>0 then if chord==6 then brk:set_value(1);event('break_down')end;chord=chord-1;if chord==0 then brk:clear_value();shift:clear_value();event('keys_released')end end
 if ready and base and valid(base)then
  local s=state();s.empty=m.natkeyboard.empty;s.posting=m.natkeyboard.is_posting;s.time=m.time:as_double()
  if s.error==0 and s.dirty==0 and s.inputEmpty==1 and s.empty and not s.posting then
   local out=assert(io.open(R..'ready.tmp','w'));out:write(json_encode(s));out:close();os.rename(R..'ready.tmp',R..'ready.json')
   local inp=io.open(R..'keyboard.txt');if inp then local text=inp:read('*a');inp:close();os.remove(R..'keyboard.txt');event('harness_input',{text=text,state=s});ready=false;busy=true;if text=='@SHIFT_BREAK@'then shift:set_value(1);chord=8 else m.natkeyboard.in_use=true;m.natkeyboard:post_coded(text)end end
  end
 end
end)
