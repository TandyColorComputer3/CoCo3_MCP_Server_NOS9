#!/usr/bin/env python3
"""Build the public launcher and its two chained opening phases."""
from pathlib import Path
import argparse,hashlib,json,subprocess,shutil
from import_gameplay import generate

app=Path(__file__).resolve().parent
parser=argparse.ArgumentParser();parser.add_argument('--out',required=True)
parser.add_argument('--os9',default='/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9')
args=parser.parse_args();out=Path(args.out).resolve();out.mkdir(parents=True,exist_ok=True)
generate(out)
subprocess.run(['python3',str(app/'build.py'),'--out',str(out),'--os9',args.os9],check=True)
heartbeat=out/'heartbeat';heartbeat.mkdir(exist_ok=True)
subprocess.run(['python3',str(app/'build_heartbeat.py'),'--out',str(heartbeat),'--os9',args.os9],check=True,
               stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
shutil.copyfile(heartbeat/'dhbpack',out/'dhbpack')
include=['-I'+str(app/'src'),'-I'+str(app/'src/audio'),'-I'+str(out)]
common=['cmoc','--os9','-O2','--intermediate','--verbose',
        '--add-os9-stack-space=1536','--intdir='+str(out)]+include
targets={
 'daggorath':['opening-launch.c','phase-chain.c','opening-module.asm'],
 'dodintro':['gameplay/demo.c','gameplay/game.c','gameplay/maze-random.asm','gameplay/primary-text.c',
             'gameplay/opening-map.c','gameplay/opening-heartbeat.c',
             'gameplay/opening-phase-module.asm',
             'presentation.c','window-path.c','phase-chain.c','os9.c','original/logical.c',
             'audio/native_heartbeat.c','audio/ipc.c'],
}
records={}
for name,sources in targets.items():
 command=common+(['-DDOD_COMMAND_OVERLAY','-DDOD_OPENING_PHASE'] if name=='dodintro' else [])
 command+=['-o',name]+[str(app/'src'/s) for s in sources]
 result=subprocess.run(command,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
 (out/(name+'.build.log')).write_text(result.stdout)
 if result.returncode:raise RuntimeError(f'{name}: {result.stdout}')
 ident=subprocess.check_output([args.os9,'ident',str(out/name)],text=True)
 if '(Good)' not in ident:raise RuntimeError(f'{name}: invalid module CRC: {ident}')
 data=(out/name).read_bytes()
 records[name]={'size':len(data),'sha256':hashlib.sha256(data).hexdigest(),
                'ident':ident,'command':command}
pack=(out/'dhbpack').read_bytes()
records['dhbpack']={'size':len(pack),'sha256':hashlib.sha256(pack).hexdigest(),
                    'members':['DHeartbeat','dhb'],'build':str(heartbeat/'build.json')}
(out/'opening-build.json').write_text(json.dumps(records,indent=2)+'\n')
for name,record in records.items():print(name,record['size'],record['sha256'])
