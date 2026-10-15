-- Separate, private breakpoint diagnostic. The committed raster observer runs first.
local D='/private/tmp/c6-stage3e13d2/'
local m=manager.machine
local cpu=m.devices[':maincpu']
local mem=cpu.spaces.program
local dbg=m.debugger
local ram=emu.item(m.devices[':ram'].items['0/m_pointer'])
local f=assert(io.open(D..'linked-map.json','r'))
local linked=json_decode(f:read('*a'));f:close()
local moduleFile=assert(io.open('/private/tmp/c6-stage3e13b/modules/dodhbp','rb'))
local moduleBytes=moduleFile:read('*a');moduleFile:close()
assert(#moduleBytes==linked.size,'linked module size mismatch')
local function linked_binary_bytes(offset,n)
  assert(offset>=0 and offset+n<=#moduleBytes,'linked module read outside bounds')
  return {moduleBytes:byte(offset+1,offset+n)}
end
local calibrated=false
local calibrationError=nil
local brokerBase=nil
local brokerPid=nil
local brokerPdLogical=nil
local brokerTask=nil
local brokerPdPhysical=nil
local brokerCodePhysical=nil
local activeDat=nil
local processDat=nil
local installed=false
local armedAt=nil
local breakpointIds={}
local lastConsole=0
local trace={}
local function pb(a)
  assert(a>=0 and a<2097152,'RAM read outside 2 MB')
  local v=ram:read_block(a,1)
  assert(v and #v==1,'RAM read failed')
  return string.byte(v,1)
end
local function pw(a)return pb(a)*256+pb(a+1)end
local function phys(a,task)
  assert(a>=0 and a<65536 and (task==0 or task==1),'bad logical address')
  return mem:read_u8(0xffa0+task*8+math.floor(a/0x2000))*0x2000+(a%0x2000)
end
local function bytes(a,n)
  local t={};for i=0,n-1 do t[#t+1]=mem:read_u8((a+i)&0xffff) end;return t
end
local function same(a,b)
  if #a~=#b then return false end
  for i=1,#a do if a[i]~=b[i] then return false end end
  return true
end
local function module_name(base)
  local off=mem:read_u8(base+4)*256+mem:read_u8(base+5)
  if off>=linked.size then return nil end
  local name=''
  for i=0,31 do
    local v=mem:read_u8(base+off+i)
    name=name..string.char(v&0x7f)
    if v&0x80~=0 then return name end
  end
  return nil
end
local driverBase=nil
local driverBytes=assert(io.open('/private/tmp/c6-stage3e13b/build-b/dheartbeat','rb')):read('*a')
local stateEvents={}
local function append(kind, extra)
  if #stateEvents>=400 then return end
  local t={kind=kind,time=m.time:as_double(),pc=cpu.state.PC.value}
  if brokerPdPhysical then
    t.brokerState=pb(brokerPdPhysical+0x0c)
    t.brokerQueue=pw(brokerPdPhysical+0x0d)
    t.brokerSignal=pb(brokerPdPhysical+0x19)
  end
  t.current=pw(phys(0x50,0))
  t.activeHead=pw(phys(0x52,0))
  t.sleepHead=pw(phys(0x56,0))
  for k,v in pairs(extra or {}) do t[k]=v end
  stateEvents[#stateEvents+1]=t
end
local marks={
  {'service',0x09a1}, {'clock',0x1c28}, {'snapshot',0x0975},
  {'claim',0x1754}, {'wait',0x17d7}, {'wait_syscall',0x18aa},
  {'wait_return',0x18ad}, {'pipe_read',0x1920},
  {'pipe_write',0x1972}, {'reply',0x0d9d},
}
local driverMarks={{'wake_check',0x0539},{'send_wake',0x0549},{'send_return',0x054c}}
local function install()
  assert(calibrated and not calibrationError and brokerBase and brokerPid,'broker calibration incomplete at generation 6')
  driverBase=cpu.state.PC.value-0x051d
  assert(driverBase>=0 and driverBase+0x0566<65536,'driver base invalid')
  assert(same(bytes(driverBase,16),{driverBytes:byte(1,16)}),'driver header mismatch')
  for _,x in ipairs(marks) do
    local expected=linked_binary_bytes(x[2],x[1]=='wait_syscall' and 3 or 2)
    assert(same(bytes(brokerBase+x[2],#expected),expected),'broker instruction mismatch '..x[1])
    for i=0,#expected-1 do assert(pb(phys(brokerBase+x[2]+i,brokerTask))==expected[i+1],'broker physical mismatch '..x[1]) end
    local id=cpu.debug:bpset(brokerBase+x[2],'', 'printf "E13D2 '..x[1]..' PC=%04X Y=%04X U=%04X S=%04X\\n",pc,y,u,s ; time ; g')
    assert(id,'broker breakpoint failed '..x[1]);breakpointIds[#breakpointIds+1]=id
  end
  for _,x in ipairs(driverMarks) do
    local expected={driverBytes:byte(x[2]+1,x[2]+2)}
    assert(same(bytes(driverBase+x[2],2),expected),'driver instruction mismatch '..x[1])
    local id=cpu.debug:bpset(driverBase+x[2],'','printf "E13D2 '..x[1]..' PC=%04X Y=%04X U=%04X S=%04X A=%02X B=%02X\\n",pc,y,u,s,a,b ; time ; g')
    assert(id,'driver breakpoint failed '..x[1]);breakpointIds[#breakpointIds+1]=id
  end
  local brokerLogical=brokerPdLogical
  -- The calibration hook runs only while the broker is current; its descriptor is
  -- the exact D.Proc pointer under the installed Level II layout.
  assert(brokerLogical>0 and pb(brokerPdPhysical)==brokerPid,'broker descriptor invalid')
  local pdLogical=brokerLogical
  private_e13d2_proc=mem:install_write_tap(0x50,0x51,'e13d2-current',function(a,d)
    if not installed then return end
    append('D.Proc write',{address=a,data=d})
  end)
  private_e13d2_broker=mem:install_write_tap(pdLogical+0x0c,pdLogical+0x0e,'e13d2-broker',function(a,d)
    if not installed then return end
    append('broker descriptor write',{address=a,data=d})
  end)
  armedAt=m.time:as_double();installed=true
  append('armed',{driverBase=driverBase,brokerLogical=pdLogical,brokerPid=brokerPid})
  return #breakpointIds
end
if not dbg then error('MAME debugger unavailable') end
if dbg.execution_state=='stop' then dbg:command('g') end
private_e13d2_map=mem:install_write_tap(linked.symbols.mapStage,linked.symbols.mapStage,'e13d2-map',function(a,d)
  if calibrated or calibrationError or d~=1 or cpu.state.Y.value~=0 then return end
  local ok,err=pcall(function()
    local ptr=pw(phys(0x50,0));assert(ptr>0 and ptr<65536,'bad current process pointer')
    local pd=phys(ptr,0);local pid=pb(pd);local base=pw(pd+0x11)
    assert(base%0x2000==0 and base+linked.size<=65536,'invalid module base')
    assert(same(bytes(base,16),linked.module_header),'module header mismatch')
    assert(module_name(base)=='dodhbp','module name mismatch')
    assert(cpu.state.PC.value==base+linked.instruction_offsets.map_first+4,'map-stage PC mismatch')
    local task=mem:read_u8(0xff91)&1
    local slot=math.floor((base+linked.instruction_offsets.map_first)/0x2000)
    local bank=mem:read_u8(0xffa0+task*8+slot)
    local imageBank=pb(pd+0x40+slot*2+1)
    assert(bank==imageBank,'DAT bank mismatch')
    local pa=bank*0x2000+((base+linked.instruction_offsets.map_first)%0x2000)
    assert(same(bytes(base+linked.instruction_offsets.map_first,6),{pb(pa),pb(pa+1),pb(pa+2),pb(pa+3),pb(pa+4),pb(pa+5)}),'physical instruction mismatch')
    brokerTask=task;brokerPdPhysical=pd;brokerCodePhysical=pa
    brokerBase=base;brokerPid=pid;brokerPdLogical=ptr;calibrated=true
    activeDat={};for i=0,15 do activeDat[#activeDat+1]=mem:read_u8(0xffa0+i) end
    processDat={};for i=0,7 do processDat[#processDat+1]=pb(pd+0x40+i*2+1) end
  end)
  if not ok then calibrationError=tostring(err) end
end)
function private_e13d2_arm(generation,phase)
  if generation~=6 then return {error='wrong generation for arming'} end
  if installed then return {error='breakpoints already installed'} end
  if calibrationError then return {error=calibrationError} end
  local ok,result=pcall(install)
  if not ok then calibrationError='tracer installation: '..tostring(result);return {error=calibrationError} end
  return {armedAt=armedAt,generation=generation,phase=phase,brokerPid=brokerPid,brokerBase=brokerBase,driverBase=driverBase,count=result}
end
function private_e13d2_edge(generation,phase,driverStatic)
  if not installed or generation<7 or generation>8 then return end
  append('PB1 edge',{generation=generation,phase=phase,driverStatic=driverStatic,waitArmed=pb(driverStatic+0x32),wakePid=pb(driverStatic+0x33)})
end
local oldState=commands.msh_observer_state
commands.msh_observer_state=function(params)
  local r=oldState(params)
  local c=dbg.consolelog
  while lastConsole<#c do
    lastConsole=lastConsole+1
    if installed and #trace<900 then trace[#trace+1]=tostring(c[lastConsole]) end
  end
  r.e13d2={calibrated=calibrated,calibrationError=calibrationError,brokerBase=brokerBase,brokerPid=brokerPid,
    brokerTask=brokerTask,brokerPdPhysical=brokerPdPhysical,brokerCodePhysical=brokerCodePhysical,driverBase=driverBase,
    activeDat=activeDat and mark_array(activeDat) or nil,processDat=processDat and mark_array(processDat) or nil,
    installed=installed,armedAt=armedAt,breakpointCount=#breakpointIds,
    stateEvents=mark_array(stateEvents),consoleLines=#trace,trace=mark_array(trace)}
  return r
end
