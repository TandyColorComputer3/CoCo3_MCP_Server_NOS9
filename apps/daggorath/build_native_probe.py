#!/usr/bin/env python3
"""Build disposable integration-gate probes; does not change dodaudio."""
from pathlib import Path
import hashlib,json,subprocess,sys
root=Path(__file__).resolve().parents[2]
out=Path(sys.argv[1]).resolve();out.mkdir(parents=True,exist_ok=True)
src=root/'apps/daggorath/src';records={}
os9='/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9'
for name,file,extra in [('natbeat','native_beat.c',['audio/ssc_io.c']),('natco','native_coordinator_cold.c' if '--cold' in sys.argv else 'native_coordinator.c',[])]:
    module=out/(name+'-module.asm')
    module.write_text(' section __os9\n fcc /'+name+'/\n fcb 0\nedition equ 1\nrev equ 1\n endsection\n')
    sources=[root/'apps/daggorath/probes'/file]+[src/p for p in ['audio/ipc.c']+extra+['os9.c']]+[module]
    command=['cmoc','--os9','-O0','--add-os9-stack-space=2048','-I'+str(src),'-I'+str(src/'audio'),'-o',str(out/name)]+list(map(str,sources))
    r=subprocess.run(command,cwd=root,capture_output=True,text=True)
    (out/(name+'.log')).write_text(r.stdout+r.stderr);r.check_returncode()
    ident=subprocess.check_output([os9,'ident',str(out/name)],text=True);assert '(Good)' in ident
    dependencies=sources+list((src/'audio').glob('*.h'))+[src/'audio/ssc.c',src/'platform.h']
    records[name]={'command':command,'sources':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in dependencies},'sha256':hashlib.sha256((out/name).read_bytes()).hexdigest(),'ident':ident}
    print(ident)
(out/'build.json').write_text(json.dumps(records,indent=2)+'\n')
