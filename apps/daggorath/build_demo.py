#!/usr/bin/env python3
"""Build the bounded AUTTAB 1..10 production demonstration."""
from pathlib import Path
import argparse,subprocess,hashlib,json
from import_gameplay import generate
app=Path(__file__).resolve().parent
p=argparse.ArgumentParser();p.add_argument('--out',required=True);p.add_argument('--os9',default='/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9');a=p.parse_args();out=Path(a.out).resolve();out.mkdir(parents=True,exist_ok=True)
generate(out)
for name,source in [('overlay-host-opt.o','overlay-host.c'),('scheduler-host-opt.o','scheduler-host.c')]:
 cmd=['cmoc','--os9','-O2','--compile','-DDOD_COMMAND_OVERLAY','--intdir='+str(out),'-I'+str(app/'src'),'-I'+str(out),'-o',str(out/name),str(app/'src'/'gameplay'/source)]
 subprocess.run(cmd,cwd=out,check=True)
sources=['gameplay/demo.c','gameplay/game.c','gameplay/overlay-host-shim.asm','gameplay/overlay-callback-gateway.asm','gameplay/scheduler-host-shim.asm','gameplay/scheduler-callback-gateway.asm','gameplay/demo-module.asm','presentation.c','window-path.c','os9.c','original/logical.c','audio/native_heartbeat.c','audio/ipc.c','audio/client.c','audio/event.c']
cmd=['cmoc','--os9','-O2','--intermediate','--verbose','--add-os9-stack-space=1536','-DDOD_COMMAND_OVERLAY','--intdir='+str(out),'-I'+str(app/'src'),'-I'+str(app/'src'/'audio'),'-I'+str(out),'-o','doddemo']+[str(app/'src'/x) for x in sources]+[str(out/'overlay-host-opt.o'),str(out/'scheduler-host-opt.o')]
r=subprocess.run(cmd,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(out/'build.log').write_text(r.stdout)
if r.returncode: print(r.stdout);raise SystemExit(r.returncode)
ident=subprocess.check_output([a.os9,'ident',str(out/'doddemo')],text=True);assert '(Good)' in ident
(out/'build.json').write_text(json.dumps({'command':cmd,'ident':ident,'sha256':hashlib.sha256((out/'doddemo').read_bytes()).hexdigest()},indent=2)+'\n');print(ident)
