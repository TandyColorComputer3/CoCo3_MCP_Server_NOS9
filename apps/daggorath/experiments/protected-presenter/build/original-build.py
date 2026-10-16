#!/usr/bin/env python3
from pathlib import Path
import hashlib,json,re,subprocess,sys
root=Path('/private/tmp/c6-stage3e16b-protected')
app=Path('/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9/apps/daggorath')
os9=Path('/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9')
out=Path(sys.argv[1]);out.mkdir(parents=True,exist_ok=False)
def run(args):
 p=subprocess.run([str(x) for x in args],cwd=out,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
 if p.returncode:raise RuntimeError(' '.join(map(str,args))+'\n'+p.stdout)
 return p.stdout
def hash(p):return hashlib.sha256(p.read_bytes()).hexdigest()
common=['cmoc','--os9','-O2','--intermediate','--verbose','--intdir='+str(out),'-I'+str(app/'src'),'-I'+str(app/'src/compat'),'-I'+str(app/'src/audio')]
client=[root/'src/client.c',app/'src/compat/heartbeat-broker-client.asm',app/'src/gameplay/opening-heartbeat.c',app/'src/audio/ipc.c',app/'src/os9.c']
presenter=[root/'src/presenter.c',app/'src/audio/native_heartbeat.c',app/'src/audio/native-heartbeat-wait.asm',app/'src/audio/ipc.c',app/'src/window-path.c',app/'src/os9.c']
configs=[('protcl',client,['--add-os9-stack-space=1024']),('protpr',presenter,['--add-os9-stack-space=1024','-DDAG_HEARTBEAT_CONTROL_PLANE'])]
results={}
for name,sources,flags in configs:
 cmd=common+flags+['-o',name]+list(map(str,sources))
 (out/(name+'.log')).write_text(run(cmd))
 module=out/name
 ident=run([os9,'ident',module])
 assert '(Good)' in ident and re.search(r'Header for\s*:\s*'+name+r'\s*$',ident,re.M|re.I),ident
 assert re.search(r'Ty/La At/Rv:\s*\$11\s+\$80',ident),ident
 data=int(re.search(r'Data size\s*:\s*\$[0-9A-Fa-f]+\s+#(\d+)',ident).group(1))
 maptext=(out/(name+'.map')).read_text()
 end=int(re.search(r'Section:\s+program_end.*?load at\s+([0-9A-Fa-f]+)',maptext).group(1),16)
 results[name]={'sha256':hash(module),'size':module.stat().st_size,'data':data,'program_end':end,'program_dat':(end+8191)//8192,'data_dat':(data+8191)//8192,'stack_reserve':1024,'ident':ident,'source_hashes':{str(p):hash(p) for p in sources},'command':cmd}
(root/'evidence'/('build-'+out.name+'.json')).write_text(json.dumps(results,indent=2)+'\n')
print(json.dumps({n:{k:v for k,v in x.items() if k in ('sha256','size','data','program_end','program_dat','data_dat','stack_reserve')} for n,x in results.items()},indent=2))
