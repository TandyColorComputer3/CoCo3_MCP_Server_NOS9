-- Separate, private breakpoint diagnostic. The committed raster observer runs first.
local D='/private/tmp/c6-stage3e13d/'
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
local markNames={service_in=true,heart_put_in=true,heart_put_out=true,
 admit_in=true,query_in=true,query_return=true,second_snapshot_return=true,
 admit_decide=true,admit_accept=true,admit_defer=true,admit_wait=true,
 left_copy_in=true,left_put_in=true,left_put_out=true,
 right_copy_in=true,right_put_in=true,right_put_out=true,
 bottom_copy_in=true,bottom_put_in=true,bottom_put_out=true}
local function install()
  assert(calibrated and not calibrationError and brokerBase and brokerPid,
         'broker calibration incomplete at generation 6')
  local n=0
  for _,x in ipairs(linked.marks) do
    if markNames[x[1]] then
      local action='printf "E13D '..x[1]..' PC=%04X Y=%04X U=%04X S=%04X\\n",pc,y,u,s ; time ; g'
      local id=cpu.debug:bpset(brokerBase+x[2],'',action)
      assert(id,'breakpoint installation failed: '..x[1])
      breakpointIds[#breakpointIds+1]=id
      n=n+1
    end
  end
  assert(n==20,'incomplete linked mark set')
  -- These function-entry marks are verified against the exact linked module
  -- bytes before registration.  The snapshot mark covers all callers, so the
  -- chronological trace must identify the service_heart instance.
  for _,x in ipairs({{'snapshot_in',linked.symbols.native_heartbeat_snapshot},
                      {'reply_in',0x0d9d}}) do
    local action='printf "E13D '..x[1]..' PC=%04X Y=%04X U=%04X S=%04X\\n",pc,y,u,s ; time ; g'
    local id=cpu.debug:bpset(brokerBase+x[2],'',action)
    assert(id,'breakpoint installation failed: '..x[1])
    breakpointIds[#breakpointIds+1]=id
    n=n+1
  end
  armedAt=m.time:as_double()
  installed=true
  return n
end
if not dbg then error('MAME debugger unavailable') end
if dbg.execution_state=='stop' then dbg:command('g') end
private_e13d_map=mem:install_write_tap(linked.symbols.mapStage,linked.symbols.mapStage,'e13d-map',function(a,d)
  if calibrated or calibrationError or d~=1 or cpu.state.Y.value~=0 then return end
  local ok,err=pcall(function()
    local ptr=pw(phys(0x50,0));assert(ptr>0 and ptr<65536,'bad current process pointer')
    local pd=phys(ptr,0)
    local pid=pb(pd)
    local base=pw(pd+0x11)
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
    local raw={};for i=0,5 do raw[#raw+1]=pb(pa+i)end
    assert(same(raw,bytes(base+linked.instruction_offsets.map_first,6)),'physical instruction mismatch')
    for _,x in ipairs(linked.marks) do
      local logical=base+x[2]
      assert(same(bytes(logical,#x[3]),x[3]),'mark mismatch '..x[1])
      for i=0,#x[3]-1 do
        assert(pb(phys(logical+i,task))==x[3][i+1],'physical mark mismatch '..x[1])
      end
    end
    for _,offset in ipairs({linked.symbols.native_heartbeat_snapshot,0x0d9d}) do
      local image=bytes(base+offset,2)
      local code=linked_binary_bytes(offset,2)
      assert(same(image,code),'function-entry mismatch '..offset)
      for i=0,1 do assert(pb(phys(base+offset+i,task))==code[i+1],
                         'physical function-entry mismatch '..offset) end
    end
    activeDat={};for i=0,15 do activeDat[#activeDat+1]=mem:read_u8(0xffa0+i) end
    processDat={};for i=0,7 do processDat[#processDat+1]=pb(pd+0x40+i*2+1) end
    brokerTask=task;brokerPdPhysical=pd;brokerCodePhysical=pa
    brokerBase=base;brokerPid=pid;calibrated=true
  end)
  if not ok then calibrationError=tostring(err) end
end)
function private_e13d_arm(generation,phase)
  if generation~=6 then return {error='wrong generation for arming'} end
  if installed then return {error='breakpoints already installed'} end
  if calibrationError then return {error=calibrationError} end
  local ok,result=pcall(install)
  if not ok then
    calibrationError='breakpoint installation: '..tostring(result)
    return {error=calibrationError}
  end
  return {armedAt=armedAt,generation=generation,phase=phase,
          brokerPid=brokerPid,brokerBase=brokerBase,count=result}
end
local oldState=commands.msh_observer_state
commands.msh_observer_state=function(params)
  local r=oldState(params)
  local c=dbg.consolelog
  while lastConsole<#c do
    lastConsole=lastConsole+1
    local s=tostring(c[lastConsole])
    if installed and (#trace<1200) then trace[#trace+1]=s end
  end
  r.e13d={calibrated=calibrated,calibrationError=calibrationError,brokerBase=brokerBase,
    brokerPid=brokerPid,brokerTask=brokerTask,brokerPdPhysical=brokerPdPhysical,
    brokerCodePhysical=brokerCodePhysical,
    activeDat=activeDat and mark_array(activeDat) or nil,
    processDat=processDat and mark_array(processDat) or nil,
    installed=installed,armedAt=armedAt,
    breakpointCount=#breakpointIds,consoleLines=#trace,trace=mark_array(trace)}
  return r
end
