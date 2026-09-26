#!/usr/bin/env python3
from pathlib import Path
import subprocess,sys,json,hashlib
root=Path(__file__).resolve().parents[2];src=root/'apps/daggorath/src';out=Path(sys.argv[1]).resolve();out.mkdir(parents=True,exist_ok=True)
records={}
for name,parts in [('dodaudio',['service.c','event.c','ipc.c','ssc.c','ssc_io.c','service-module.asm']),('dodsnd',['harness.c','event.c','ipc.c','client.c','harness-module.asm'])]:
 sources=[src/'audio'/p for p in parts]+[src/'os9.c']
 cmd=['cmoc','--os9','-O0','--intermediate','--intdir='+str(out),'--add-os9-stack-space=2048','-I'+str(src),'-I'+str(src/'audio'),'-o',str(out/name)]+list(map(str,sources))
 r=subprocess.run(cmd,cwd=root,capture_output=True,text=True);(out/(name+'.log')).write_text(r.stdout+r.stderr);r.check_returncode()
 ident=subprocess.check_output(['/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9','ident',str(out/name)],text=True)
 assert '(Good)' in ident
 records[name]={'command':cmd,'sources':{str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest() for p in sources+list((src/'audio').glob('*.h'))+[src/'platform.h']},'sha256':hashlib.sha256((out/name).read_bytes()).hexdigest(),'ident':ident}
 print(ident)
(out/'build.json').write_text(json.dumps(records,indent=2)+'\n')
