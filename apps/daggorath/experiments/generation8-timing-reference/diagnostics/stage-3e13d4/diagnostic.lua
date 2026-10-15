-- Separate, private breakpoint diagnostic. The committed raster observer runs first.
local D='/private/tmp/c6-stage3e13d4b/'
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
local physicalSize=emu.item(m.devices[':ram'].items['0/m_size']):read(0)
assert(physicalSize==0x200000,'physical RAM save-item is not 2 MB')
local initialDat=nil
local osTask=nil
local stateEvents={}
local markerOffsets={
  {'service_in',0x09a1,2},
  {'clock_call',0x09b5,3},
  {'clock_in',0x1c28,2},
  {'copy_call',0x1c4a,3},
  {'copy_return',0x1c4d,2},
  {'clock_return',0x09b8,2},
  {'snapshot_call',0x09d5,3},
  {'snapshot_wrapper',0x0975,2},
  {'native_snapshot_in',0x156f,2},
  {'native_snapshot_return',0x098f,2},
}
local function broker_physical(logical)
  assert(type(logical)=='number' and logical>=0 and logical<65536,'bad broker logical address')
  local slot=math.floor(logical/0x2000)
  local bank=processDat and processDat[slot+1]
  assert(type(bank)=='number' and bank>=0 and bank<256,'missing broker DAT bank')
  local pa=bank*0x2000+(logical%0x2000)
  assert(pa>=0 and pa<physicalSize,'broker translation exceeds RAM save-item')
  return pa
end
local function broker_bytes(logical,n)
  assert(n>0 and logical+n<=65536,'broker read invalid')
  local out={}
  for i=0,n-1 do out[#out+1]=pb(broker_physical(logical+i)) end
  return out
end
local function append(kind,fields)
  if #stateEvents>=250 then return end
  local e={kind=kind,time=m.time:as_double(),pc=cpu.state.PC.value}
  local sysPtr=pw(phys(0x50,0))
  e.currentBefore=sysPtr
  if brokerPdPhysical then
    e.brokerState=pb(brokerPdPhysical+0x0c)
    e.brokerSignal=pb(brokerPdPhysical+0x19)
  end
  for k,v in pairs(fields or {}) do e[k]=v end
  stateEvents[#stateEvents+1]=e
end
local function install()
  assert(calibrated and not calibrationError and brokerBase and brokerPid and brokerPdLogical,'broker calibration incomplete')
  assert(physicalSize==2097152,'RAM save-item size changed')
  assert(pb(brokerPdPhysical)==brokerPid,'broker descriptor PID changed')
  assert(pw(brokerPdPhysical+0x11)==brokerBase,'broker module base changed')
  -- The broker mapped GP buffers after the initialization probe. Capture
  -- its current saved image at this generation-6 VIRQ boundary.
  local fresh={}
  for slot=0,7 do
    fresh[#fresh+1]=pb(brokerPdPhysical+0x40+slot*2+1)
  end
  processDat=fresh
  local codeSlot=math.floor(brokerBase/0x2000)
  assert(codeSlot==6,'unexpected broker code slot')
  local codeBank=fresh[codeSlot+1]
  assert(type(codeBank)=='number' and codeBank<physicalSize/0x2000,
         'broker code bank outside physical RAM')
  osTask=pb(brokerPdPhysical+6) -- OS-9 task number; distinct from the hardware task-select bit
  for _,mark in ipairs(markerOffsets) do
    local logical=brokerBase+mark[2]
    local expected=linked_binary_bytes(mark[2],mark[3])
    assert(same(broker_bytes(logical,mark[3]),expected),
           'broker physical instruction mismatch '..mark[1])
  end
  -- Exact linked control flow for the first service_heart clock call.
  assert(same(linked_binary_bytes(0x09b5,3),{0x17,0x12,0x70}),'clock call changed')
  assert(same(linked_binary_bytes(0x09d5,3),{0x17,0xff,0x9d}),'snapshot call changed')
  assert(same(linked_binary_bytes(0x1c4a,3),{0x10,0x3f,0x1b}),'F$CpyMem call changed')
  for _,mark in ipairs(markerOffsets) do
    local label=mark[1]
    local action='printf "E13D4B '..label..' PC=%04X U=%04X S=%04X Y=%04X\\n",pc,u,s,y ; time ; g'
    local id=cpu.debug:bpset(brokerBase+mark[2],'',action)
    assert(id,'execution breakpoint failed '..label)
    breakpointIds[#breakpointIds+1]=id
  end
  private_e13d4b_proc=mem:install_write_tap(0x50,0x51,'e13d4b-current',function(a,d)
    if installed then append('D.Proc write',{address=a,data=d}) end
  end)
  installed=true;armedAt=m.time:as_double()
  append('armed',{brokerBase=brokerBase,brokerPid=brokerPid,
                  servicePhysical=broker_physical(brokerBase+0x09a1),
                  clockPhysical=broker_physical(brokerBase+0x1c28)})
  return #breakpointIds
end
if not dbg then error('MAME debugger unavailable') end
if dbg.execution_state=='stop' then dbg:command('g') end
private_e13d4b_map=mem:install_write_tap(linked.symbols.mapStage,linked.symbols.mapStage,'e13d4b-map',function(a,d)
  if calibrated or calibrationError or d~=1 or cpu.state.Y.value~=0 then return end
  local ok,err=pcall(function()
    local ptr=pw(phys(0x50,0));assert(ptr>0 and ptr<65536,'bad current process pointer')
    local pd=phys(ptr,0)
    local pid=pb(pd);local base=pw(pd+0x11)
    assert(base%0x2000==0 and base+linked.size<=65536,'invalid broker module base')
    assert(same(bytes(base,16),linked.module_header),'broker module header mismatch')
    assert(module_name(base)=='dodhbp','broker module name mismatch')
    assert(cpu.state.PC.value==base+linked.instruction_offsets.map_first+4,'map-stage instruction mismatch')
    local task=mem:read_u8(0xff91)&1
    local slot=math.floor((base+linked.instruction_offsets.map_first)/0x2000)
    local bank=mem:read_u8(0xffa0+task*8+slot)
    assert(bank==pb(pd+0x40+slot*2+1),'broker live DAT differs from saved image')
    local pa=bank*0x2000+((base+linked.instruction_offsets.map_first)%0x2000)
    assert(pa+6<=physicalSize,'calibration instruction exceeds RAM save-item')
    assert(same(bytes(base+linked.instruction_offsets.map_first,6),
                {pb(pa),pb(pa+1),pb(pa+2),pb(pa+3),pb(pa+4),pb(pa+5)}),
           'calibration instruction physical mismatch')
    brokerTask=task;brokerPdPhysical=pd;brokerPdLogical=ptr
    brokerCodePhysical=pa;brokerBase=base;brokerPid=pid
    activeDat={};for i=0,15 do activeDat[#activeDat+1]=mem:read_u8(0xffa0+i) end
    processDat={};for i=0,7 do processDat[#processDat+1]=pb(pd+0x40+i*2+1) end
    initialDat={table.unpack(processDat)}
    calibrated=true
  end)
  if not ok then calibrationError=tostring(err) end
end)
function private_e13d4b_arm(generation,phase)
  if generation~=6 then return {error='wrong generation for arming'} end
  if installed then return {error='already installed'} end
  if calibrationError then return {error=calibrationError} end
  local ok,result=pcall(install)
  if not ok then calibrationError='calibration/arming: '..tostring(result);return {error=calibrationError} end
  return {armedAt=armedAt,generation=generation,phase=phase,
          brokerPid=brokerPid,brokerBase=brokerBase,breakpointCount=result}
end
function private_e13d4b_edge(generation,phase,driverStatic)
  if installed and (generation==7 or generation==8) then
    append('PB1 edge',{generation=generation,phase=phase,driverStatic=driverStatic})
  end
end
local oldState=commands.msh_observer_state
commands.msh_observer_state=function(params)
  local r=oldState(params)
  local c=dbg.consolelog
  while lastConsole<#c do
    lastConsole=lastConsole+1
    if installed and #trace<900 then trace[#trace+1]=tostring(c[lastConsole]) end
  end
  r.e13d4b={calibrated=calibrated,calibrationError=calibrationError,
    brokerBase=brokerBase,brokerPid=brokerPid,brokerTask=brokerTask,
    brokerPdLogical=brokerPdLogical,brokerPdPhysical=brokerPdPhysical,
    brokerCodePhysical=brokerCodePhysical,physicalSize=physicalSize,osTask=osTask,
    activeDat=activeDat and mark_array(activeDat) or nil,
    processDat=processDat and mark_array(processDat) or nil,
    initialDat=initialDat and mark_array(initialDat) or nil,
    codeSlot=brokerBase and math.floor(brokerBase/0x2000) or nil,
    codeBank=processDat and brokerBase and processDat[math.floor(brokerBase/0x2000)+1] or nil,
    installed=installed,armedAt=armedAt,breakpointCount=#breakpointIds,
    stateEvents=mark_array(stateEvents),consoleLines=#trace,trace=mark_array(trace)}
  return r
end
