#!/usr/bin/env python3
"""Reproducible disposable VIRQ probe; references external defs read-only."""
import pathlib,subprocess,json,hashlib,sys
root=pathlib.Path(__file__).resolve().parents[4];src=root/'apps/daggorath/src';out=pathlib.Path(sys.argv[1]).resolve();out.mkdir(parents=True,exist_ok=True)
ref=pathlib.Path('/Volumes/SEDONA/Projects/nitros9-reference');here=pathlib.Path(__file__).parent
(out/'defsfile').write_text('Level equ 2\nH6309 equ 1\n'+''.join(' use '+str(ref/'defs'/f)+'\n' for f in ['os9.d','scf.d','coco.d']))
commands=[]
for name,file in [('vcounter','counter.asm'),('vc','descriptor.asm')]:
 cmd=['lwasm','--format=os9','--pragma=condundefzero','-I',str(out),'--list='+str(out/(name+'.lst')),'-o',str(out/name),str(here/file)];commands.append(cmd);subprocess.run(cmd,check=True)
(out/'vctest-module.asm').write_text(' section __os9\n fcc /vctest/\n fcb 0\nedition equ 1\nrev equ 1\n endsection\n')
cmd=['cmoc','--os9','-O0','--add-os9-stack-space=2048','-I'+str(src),'-I'+str(src/'audio'),'-o',str(out/'vctest'),str(here/'controller.c'),str(src/'audio/ipc.c'),str(src/'os9.c'),str(out/'vctest-module.asm')];commands.append(cmd);subprocess.run(cmd,check=True)
(out/'vcpack').write_bytes((out/'vcounter').read_bytes()+(out/'vc').read_bytes())
tool='/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9'
ids={n:subprocess.check_output([tool,'ident',str(out/n)],text=True) for n in ['vcounter','vc','vctest','vcpack']}
assert all('(Good)' in s for s in ids.values())
(out/'build.json').write_text(json.dumps({'commands':commands,'upstream':subprocess.check_output(['git','-C',str(ref),'rev-parse','HEAD'],text=True).strip(),'versions':{n:subprocess.check_output([n,'--version'],text=True,stderr=subprocess.STDOUT) for n in ['lwasm','cmoc']},'ident':ids,'source_sha256':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [here/'counter.asm',here/'descriptor.asm',here/'controller.c',src/'audio/ipc.c',src/'os9.c',src/'platform.h',src/'audio/ipc.h']},'sha256':{n:hashlib.sha256((out/n).read_bytes()).hexdigest() for n in ids}},indent=2))
print(json.dumps(ids,indent=2))
