#!/usr/bin/env python3
"""Build the native heartbeat driver/descriptor using identified external defs.
Never install or modify reference sources or guest media.
"""
from pathlib import Path
import argparse,subprocess,json,hashlib
app=Path(__file__).resolve().parent
p=argparse.ArgumentParser();p.add_argument('--out',required=True);p.add_argument('--reference',default='/Volumes/SEDONA/Projects/nitros9-reference');p.add_argument('--lwasm',default='lwasm');p.add_argument('--os9',default='/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9');a=p.parse_args()
out=Path(a.out).resolve();out.mkdir(parents=True,exist_ok=True);ref=Path(a.reference).resolve()
(out/'defsfile').write_text('Level equ 2\nH6309 equ 1\n'+''.join(' use '+str(ref/'defs'/f)+'\n' for f in ['os9.d','scf.d','coco.d']))
commands=[];ids={}
for name,file in [('dheartbeat','driver.asm'),('dhb','descriptor.asm')]:
 src=app/'src/audio/native'/file
 cmd=[a.lwasm,'--format=os9','--pragma=condundefzero','-I',str(out),'--list='+str(out/(name+'.lst')),'-o',str(out/name),str(src)];commands.append(cmd);subprocess.run(cmd,check=True)
 ids[name]=subprocess.check_output([a.os9,'ident',str(out/name)],text=True);assert '(Good)' in ids[name]
(out/'dhbpack').write_bytes((out/'dheartbeat').read_bytes()+(out/'dhb').read_bytes())
record={'commands':commands,'ident':ids,'outputs':{n:hashlib.sha256((out/n).read_bytes()).hexdigest() for n in ['dheartbeat','dhb','dhbpack']},'sources':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in list((app/'src/audio/native').glob('*.asm'))+[Path(__file__)]+[ref/'defs'/f for f in ['os9.d','scf.d','coco.d']]},'upstream':subprocess.check_output(['git','-C',str(ref),'rev-parse','HEAD'],text=True).strip(),'assembler':subprocess.check_output([a.lwasm,'--version'],text=True)}
(out/'build.json').write_text(json.dumps(record,indent=2)+'\n');print(json.dumps(ids,indent=2))
