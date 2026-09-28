#!/usr/bin/env python3
"""Reproducible, resident Gameplay M1 build. Does not install to guest media."""
from pathlib import Path
import argparse,subprocess,json,hashlib,shutil
from import_gameplay import generate
app=Path(__file__).resolve().parent
p=argparse.ArgumentParser();p.add_argument('--out',required=True);p.add_argument('--os9',default='/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9');a=p.parse_args();out=Path(a.out).resolve();out.mkdir(parents=True,exist_ok=True)
provenance=generate(out)
sources=[app/'src'/s for s in ['gameplay/main.c','gameplay/game.c','gameplay/creature.c','gameplay/input.c','gameplay/module.asm','presentation.c','window-path.c','os9.c','original/logical.c','audio/native_heartbeat.c','audio/ipc.c']]
cmd=['cmoc','--os9','-O0','--intermediate','--verbose','--add-os9-stack-space=1536','--intdir='+str(out),'-I'+str(app/'src'),'-I'+str(app/'src/audio'),'-I'+str(out),'-o','dodgame']+list(map(str,sources))
r=subprocess.run(cmd,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(out/'build.log').write_text(r.stdout)
if r.returncode:print(r.stdout);raise SystemExit(r.returncode)
ident=subprocess.check_output([a.os9,'ident',str(out/'dodgame')],text=True);assert '(Good)' in ident
record={'command':cmd,'cwd':str(out),'ident':ident,'sources':{str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in sources},'data':provenance,'tools':{t:subprocess.check_output([t,'--version'],text=True) for t in ['cmoc','lwasm','lwlink']},'sha256':hashlib.sha256((out/'dodgame').read_bytes()).hexdigest()}
(out/'build.json').write_text(json.dumps(record,indent=2)+'\n');print(ident)
