local n=0
capture_subscription=emu.add_machine_frame_notifier(function()
 n=n+1
 if n==498 then
 local f=io.open("/private/tmp/daggorath-aspect/original-memory.bin","wb")
 local mem=manager.machine.devices[":maincpu"].spaces["program"]
 for a=0x1000,0x3fff do f:write(string.char(mem:read_u8(a))) end
 f:close()
 local f=io.open("/private/tmp/daggorath-aspect/original-config.txt","w")
 f:write("screen_config="..manager.machine.ioport.ports[":screen_config"]:read().."\n")
 local t=manager.machine.render.ui_target
 f:write("view="..t.current_view.name.." aspect="..t.current_view.effective_aspect.."\n")
 local c=manager.machine.screens[":screen"].container
 f:write("scale="..c.xscale..","..c.yscale.." offset="..c.xoffset..","..c.yoffset.."\n")
 f:close()
 end
 if n%6==0 then manager.machine.screens[":screen"]:snapshot(string.format("/private/tmp/daggorath-aspect/original/frame-%04d.png",n)) end
 if n==900 then manager.machine:exit() end
end)
