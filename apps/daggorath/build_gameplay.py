#!/usr/bin/env python3
"""Reproducible Daggorath core plus one Level II command overlay build."""
from pathlib import Path
import argparse,subprocess,json,hashlib,shutil
from import_gameplay import generate
app=Path(__file__).resolve().parent
p=argparse.ArgumentParser();p.add_argument('--out',required=True);p.add_argument('--os9',default='/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9');a=p.parse_args();out=Path(a.out).resolve();out.mkdir(parents=True,exist_ok=True)
provenance=generate(out)
host_source=app/'src'/'gameplay'/'overlay-host.c'
host_object=out/'overlay-host-opt.o'
scheduler_host_source=app/'src'/'gameplay'/'scheduler-host.c'
scheduler_host_object=out/'scheduler-host-opt.o'
host_cmd=['cmoc','--os9','-O2','--compile','-DDOD_COMMAND_OVERLAY','--intdir='+str(out),'-I'+str(app/'src'),'-I'+str(out),'-o',str(host_object),str(host_source)]
r=subprocess.run(host_cmd,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(out/'overlay-host-build.log').write_text(r.stdout)
if r.returncode:print(r.stdout);raise SystemExit(r.returncode)
scheduler_host_cmd=['cmoc','--os9','-O2','--compile','--intdir='+str(out),'-I'+str(app/'src'),'-I'+str(out),'-o',str(scheduler_host_object),str(scheduler_host_source)]
r=subprocess.run(scheduler_host_cmd,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(out/'scheduler-host-build.log').write_text(r.stdout)
if r.returncode:print(r.stdout);raise SystemExit(r.returncode)
sources=[app/'src'/s for s in ['gameplay/main.c','gameplay/game.c','gameplay/maze-random.asm','gameplay/input.c','gameplay/overlay-host-shim.asm','gameplay/overlay-callback-gateway.asm','gameplay/scheduler-host-shim.asm','gameplay/scheduler-callback-gateway.asm','gameplay/module.asm','presentation.c','window-path.c','os9.c','original/logical.c','audio/native_heartbeat.c','audio/ipc.c','audio/client.c','audio/queue.c','audio/event.c']]
cmd=['cmoc','--os9','-O2','--intermediate','--verbose','--add-os9-stack-space=1536','-DDOD_COMMAND_OVERLAY','--intdir='+str(out),'-I'+str(app/'src'),'-I'+str(app/'src/audio'),'-I'+str(out),'-o','dodgame']+list(map(str,sources+[host_object,scheduler_host_object]))
r=subprocess.run(cmd,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(out/'build.log').write_text(r.stdout)
if r.returncode:print(r.stdout);raise SystemExit(r.returncode)
ident=subprocess.check_output([a.os9,'ident',str(out/'dodgame')],text=True);assert '(Good)' in ident

# The callable body is a separate CMOC leaf and is deliberately linked only
# with the few runtime helpers it actually references.  lwlink produces raw
# relocated code; lwasm then supplies the verified $21 module header/CRC.
overlay_source=app/'src/gameplay/command-overlay.c'
overlay_asm=out/'command-overlay.s'
overlay_cmd=['cmoc','--os9','-O2','--function-stack=0','-S','-I'+str(app/'src'),'-I'+str(out),str(overlay_source)]
r=subprocess.run(overlay_cmd,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(out/'overlay-build.log').write_text(r.stdout)
if r.returncode:print(r.stdout);raise SystemExit(r.returncode)
if not overlay_asm.exists():raise SystemExit('CMOC did not produce command-overlay.s')
def run(command):subprocess.run(command,cwd=out,check=True)
run(['lwasm','-fobj','--pragma=forwardrefmax','-DOS9','-o','command-overlay.o','command-overlay.s'])
run(['lwasm','-fobj','--pragma=forwardrefmax','-DOS9','-o','command-overlay-module.o',str(app/'src/gameplay/command-overlay-module.asm')])
crt=Path('/usr/local/share/cmoc/lib/libcmoc-crt-os9.a');std=Path('/usr/local/share/cmoc/lib/libcmoc-std-os9.a')
run(['lwar','--extract',str(crt),'MUL16.os9_o','DIV16.os9_o','shiftByteRightUnsigned.os9_o','tfrZtoB.os9_o'])
run(['lwar','--extract',str(std),'MUL168.os9_o'])
overlay_link=['lwlink','--format=raw','--output=command-overlay-code.bin','command-overlay-module.o','command-overlay.o','MUL16.os9_o','DIV16.os9_o','shiftByteRightUnsigned.os9_o','tfrZtoB.os9_o','MUL168.os9_o']
run(overlay_link)
overlay_module=out/'dodcmd'
run(['lwasm','--format=os9','--pragma=forwardrefmax','-o',str(overlay_module),'-I'+str(out),str(app/'src/gameplay/command-overlay-pack.asm')])
overlay_ident=subprocess.check_output([a.os9,'ident',str(overlay_module)],text=True);assert '(Good)' in overlay_ident
scheduler_source=app/'src/gameplay/scheduler.c'
scheduler_asm=out/'scheduler.s'
scheduler_cmd=['cmoc','--os9','-O2','--function-stack=0','-S','-I'+str(app/'src'),'-I'+str(out),str(scheduler_source)]
r=subprocess.run(scheduler_cmd,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(out/'scheduler-build.log').write_text(r.stdout)
if r.returncode:print(r.stdout);raise SystemExit(r.returncode)
run(['lwasm','-fobj','--pragma=forwardrefmax','-DOS9','-o','scheduler.o','scheduler.s'])
run(['lwasm','-fobj','--pragma=forwardrefmax','-DOS9','-o','scheduler-module.o',str(app/'src/gameplay/scheduler-module.asm')])
# scheduler.c's source-faithful CMOV20 attack arithmetic needs CMOC's
# unsigned 16-bit multiply/divide helpers in addition to the existing shift.
run(['lwar','--extract',str(crt),'MUL16.os9_o','DIV16.os9_o','shiftLeft.os9_o'])
scheduler_link=['lwlink','--format=raw','--output=scheduler-code.bin','scheduler-module.o','scheduler.o','MUL16.os9_o','DIV16.os9_o','shiftLeft.os9_o']
run(scheduler_link)
scheduler_module=out/'dodsched'
run(['lwasm','--format=os9','--pragma=forwardrefmax','-o',str(scheduler_module),'-I'+str(out),str(app/'src/gameplay/scheduler-pack.asm')])
scheduler_ident=subprocess.check_output([a.os9,'ident',str(scheduler_module)],text=True);assert '(Good)' in scheduler_ident
overlay_parts=[overlay_source,host_source,scheduler_host_source,app/'src'/'gameplay'/'overlay-callback-gateway.asm',app/'src'/'gameplay'/'command-overlay-module.asm',app/'src'/'gameplay'/'command-overlay-pack.asm']
scheduler_parts=[scheduler_source,app/'src'/'gameplay'/'scheduler-api.h',app/'src'/'gameplay'/'scheduler-module.asm',app/'src'/'gameplay'/'scheduler-pack.asm']
record={'command':cmd,'hostCommand':host_cmd,'schedulerHostCommand':scheduler_host_cmd,'cwd':str(out),'ident':ident,'sources':{str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in sources+overlay_parts+scheduler_parts},'data':provenance,'tools':{t:subprocess.check_output([t,'--version'],text=True) for t in ['cmoc','lwasm','lwlink']},'sha256':hashlib.sha256((out/'dodgame').read_bytes()).hexdigest(),'overlay':{'command':overlay_cmd+overlay_link,'ident':overlay_ident,'sha256':hashlib.sha256(overlay_module.read_bytes()).hexdigest()},'scheduler':{'command':scheduler_cmd+scheduler_link,'ident':scheduler_ident,'sha256':hashlib.sha256(scheduler_module.read_bytes()).hexdigest()}}
(out/'build.json').write_text(json.dumps(record,indent=2)+'\n');print(ident)
