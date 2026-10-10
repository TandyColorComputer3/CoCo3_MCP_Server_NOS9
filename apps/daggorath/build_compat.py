#!/usr/bin/env python3
"""Build the bounded C1 Giant fixtures and C2 text demonstration."""
from pathlib import Path
import argparse,subprocess,json,hashlib,shutil
from import_gameplay import generate

app=Path(__file__).resolve().parent
p=argparse.ArgumentParser();p.add_argument('--out',required=True);p.add_argument('--os9',default='/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9');a=p.parse_args()
out=Path(a.out).resolve();out.mkdir(parents=True,exist_ok=True)
game_data=generate(out)
common=[app/'test/compat_c1_target.c',app/'src/compat/giant-render.asm',app/'src/compat/expand.asm',app/'src/compat/present.c',app/'src/window-path.c',app/'src/os9.c']
fixtures=app/'test/fixtures/compat-c1'

def emit_fixture(beat):
    state=(fixtures/f'{beat}.state').read_bytes();frame=(fixtures/f'{beat}.frame').read_bytes();oracle=(fixtures/f'{beat}.oracle').read_bytes()
    changes=[(i,a^b) for i,(a,b) in enumerate(zip(frame,oracle)) if a!=b]
    target=out/f'fixture-{beat}.asm';dollar='$'
    def rows(data,width=16):
        return '\n'.join('        fcb     '+','.join(f'{dollar}{v:02x}' for v in data[i:i+width]) for i in range(0,len(data),width))
    packed=bytes(value for offset,xor in changes for value in (offset>>8,offset&255,xor))
    target.write_text(f'''        section rodata
        export  _fixtureState,_fixtureFrame,_fixtureChanges
        export  _fixtureChangeCount,_fixtureBeat,_fixtureMode
_fixtureState
{rows(state)}
_fixtureFrame
{rows(frame)}
_fixtureChanges
{rows(packed)}
_fixtureChangeCount fdb {dollar}{len(changes):04x}
_fixtureBeat fcb {dollar}{int(beat):02x}
_fixtureMode fcb {dollar}{1 if beat=='019' else 0:02x}
        endsection
''')
    return target,len(changes)

records={}
for beat in ('019','020','021'):
    fixture,changed=emit_fixture(beat);name='dodc'+beat[1:]
    sources=common+[fixture]
    cmd=['cmoc','--os9','-O2','--intermediate','--verbose','--add-os9-stack-space=1536','--intdir='+str(out),'-I'+str(app/'src'),'-o',name]+list(map(str,sources))
    r=subprocess.run(cmd,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(out/f'{name}.log').write_text(r.stdout)
    if r.returncode:print(r.stdout);raise SystemExit(r.returncode)
    ident=subprocess.check_output([a.os9,'ident',str(out/name)],text=True);assert '(Good)' in ident
    records[beat]={'module':name,'changedBytes':changed,'ident':ident,'sha256':hashlib.sha256((out/name).read_bytes()).hexdigest()}
record={'commandTemplate':['cmoc','--os9','-O2','--add-os9-stack-space=1536'],'fixtures':records,'sources':{str(f.relative_to(app)):hashlib.sha256(f.read_bytes()).hexdigest() for f in common},'tools':{t:subprocess.check_output([t,'--version'],text=True) for t in ['cmoc','lwasm','lwlink']}}
text_sources=[app/'test/compat_c2_target.c',app/'src/compat/text.asm',app/'src/compat/expand.asm',app/'src/compat/present.c',app/'src/window-path.c',app/'src/os9.c']
cmd=['cmoc','--os9','-O2','--intermediate','--verbose','--add-os9-stack-space=1536','--intdir='+str(out),'-I'+str(app/'src'),'-I'+str(app/'test'),'-o','dodctxt']+list(map(str,text_sources))
r=subprocess.run(cmd,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(out/'dodctxt.log').write_text(r.stdout)
if r.returncode:print(r.stdout);raise SystemExit(r.returncode)
ident=subprocess.check_output([a.os9,'ident',str(out/'dodctxt')],text=True);assert '(Good)' in ident
record['text']={'module':'dodctxt','ident':ident,'sha256':hashlib.sha256((out/'dodctxt').read_bytes()).hexdigest(),'sources':{str(f.relative_to(app)):hashlib.sha256(f.read_bytes()).hexdigest() for f in text_sources}}
c3_sources=[app/'test/compat_c3_target.c',app/'src/compat/opening.c',app/'src/compat/wizard-render.asm',
    app/'src/compat/text.asm',app/'src/compat/expand.asm',app/'src/compat/present.c',app/'src/window-path.c',app/'src/os9.c']
cmd=['cmoc','--os9','-O2','--intermediate','--verbose','--add-os9-stack-space=1536','--intdir='+str(out),
     '-I'+str(app/'src'),'-I'+str(app/'src/compat'),'-o','dodcopen']+list(map(str,c3_sources))
r=subprocess.run(cmd,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(out/'dodcopen.log').write_text(r.stdout)
if r.returncode:print(r.stdout);raise SystemExit(r.returncode)
ident=subprocess.check_output([a.os9,'ident',str(out/'dodcopen')],text=True);assert '(Good)' in ident
record['opening']={'module':'dodcopen','ident':ident,'sha256':hashlib.sha256((out/'dodcopen').read_bytes()).hexdigest(),
  'sources':{str(f.relative_to(app)):hashlib.sha256(f.read_bytes()).hexdigest() for f in c3_sources}}
c4_sources=[app/'test/compat_c4_target.c',app/'src/compat/opening.c',app/'src/compat/c4.c',app/'src/compat/wizard-render.asm',
    app/'src/compat/text.asm',app/'src/compat/expand.asm',app/'src/compat/present.c',app/'src/gameplay/maze-random.asm',app/'src/window-path.c',app/'src/os9.c']
cmd=['cmoc','--os9','-O2','--intermediate','--verbose','--add-os9-stack-space=1536','--intdir='+str(out),
     '-I'+str(app/'src'),'-I'+str(app/'src/compat'),'-I'+str(out),'-o','dodcinit']+list(map(str,c4_sources))
r=subprocess.run(cmd,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(out/'dodcinit.log').write_text(r.stdout)
if r.returncode:print(r.stdout);raise SystemExit(r.returncode)
ident=subprocess.check_output([a.os9,'ident',str(out/'dodcinit')],text=True);assert '(Good)' in ident
record['initial']={'module':'dodcinit','ident':ident,'sha256':hashlib.sha256((out/'dodcinit').read_bytes()).hexdigest(),
  'data':game_data,'sources':{str(f.relative_to(app)):hashlib.sha256(f.read_bytes()).hexdigest() for f in c4_sources}}
c5_sources=[app/'test/compat_c5_target.c',app/'src/compat/c4.c',app/'src/compat/c5.c',
    app/'src/compat/giant-render.asm',app/'src/compat/vector-line.asm',app/'src/compat/text.asm',app/'src/compat/expand.asm',app/'src/compat/present.c',
    app/'src/gameplay/game.c',app/'src/gameplay/maze-random.asm',app/'src/gameplay/overlay-host.c',
    app/'src/gameplay/overlay-host-shim.asm',app/'src/gameplay/overlay-callback-gateway.asm',
    app/'src/gameplay/scheduler-host.c',app/'src/gameplay/scheduler-host-shim.asm',
    app/'src/gameplay/scheduler-callback-gateway.asm',app/'src/gameplay/opening-heartbeat.c',
    app/'src/audio/native_heartbeat.c',app/'src/original/logical.c',app/'src/window-path.c',app/'src/os9.c']
c5_sources.insert(-3,app/'src/gameplay/input.c');c5_sources.insert(-3,app/'src/audio/ipc.c')
cmd=['cmoc','--os9','-O2','--intermediate','--verbose','--add-os9-stack-space=1536','-DDOD_COMMAND_OVERLAY','-DDAG_COMPAT_VIEWER_16','-DDAG_COMPAT_HEARTBEAT',
     '--intdir='+str(out),'-I'+str(app/'src'),'-I'+str(app/'src/compat'),'-I'+str(app/'src/audio'),'-I'+str(out),'-o','dodcplay']+list(map(str,c5_sources))
r=subprocess.run(cmd,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(out/'dodcplay.log').write_text(r.stdout)
if r.returncode:print(r.stdout);raise SystemExit(r.returncode)
ident=subprocess.check_output([a.os9,'ident',str(out/'dodcplay')],text=True);assert '(Good)' in ident
record['play']={'module':'dodcplay','ident':ident,'sha256':hashlib.sha256((out/'dodcplay').read_bytes()).hexdigest(),
  'sources':{str(f.relative_to(app)):hashlib.sha256(f.read_bytes()).hexdigest() for f in c5_sources}}
c6_sources=[app/'test/compat_c6_target.c']+[
    app/'src/compat/giant-render-c6.asm' if source==app/'src/compat/giant-render.asm' else source
    for source in c5_sources[1:] if source!=app/'src/audio/native_heartbeat.c']
c6_sources.insert(1,app/'src/compat/heartbeat-broker-client.asm')
cmd=['cmoc','--os9','-O2','--intermediate','--verbose','--add-os9-stack-space=1536','-DDOD_COMMAND_OVERLAY','-DDAG_COMPAT_VIEWER_16','-DDAG_COMPAT_HEARTBEAT','-DDAG_COMPAT_C6',
     '--intdir='+str(out),'-I'+str(app/'src'),'-I'+str(app/'src/compat'),'-I'+str(app/'src/audio'),'-I'+str(app/'test'),'-I'+str(out),'-o','dodcgame']+list(map(str,c6_sources))
r=subprocess.run(cmd,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(out/'dodcgame.log').write_text(r.stdout)
if r.returncode:print(r.stdout);raise SystemExit(r.returncode)
ident=subprocess.check_output([a.os9,'ident',str(out/'dodcgame')],text=True);assert '(Good)' in ident
record['interactive']={'module':'dodcgame','ident':ident,'sha256':hashlib.sha256((out/'dodcgame').read_bytes()).hexdigest(),
  'sources':{str(f.relative_to(app)):hashlib.sha256(f.read_bytes()).hexdigest() for f in c6_sources}}
broker_sources=[app/'src/compat/heartbeat-broker-service.c',app/'src/audio/native_heartbeat.c',
                app/'src/audio/ipc.c',app/'src/os9.c']
cmd=['cmoc','--os9','-O2','--intermediate','--verbose','--add-os9-stack-space=768','-DDAG_HEARTBEAT_CONTROL_PLANE','--intdir='+str(out),
     '-I'+str(app/'src'),'-I'+str(app/'src/compat'),'-I'+str(app/'src/audio'),'-o','dodhbp']+list(map(str,broker_sources))
r=subprocess.run(cmd,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(out/'dodhbp.log').write_text(r.stdout)
if r.returncode:print(r.stdout);raise SystemExit(r.returncode)
ident=subprocess.check_output([a.os9,'ident',str(out/'dodhbp')],text=True);assert '(Good)' in ident
record['interactive']['broker']={'module':'dodhbp','ident':ident,
  'sha256':hashlib.sha256((out/'dodhbp').read_bytes()).hexdigest(),
  'sources':{str(f.relative_to(app)):hashlib.sha256(f.read_bytes()).hexdigest() for f in broker_sources}}
stress_sources=[app/'test/compat_c6_broker_target.c',app/'test/compat_c6_trace_io.asm',app/'src/compat/heartbeat-broker-client.asm',
                app/'src/compat/expand.asm',app/'src/compat/present.c',app/'src/gameplay/opening-heartbeat.c',
                app/'src/audio/ipc.c',app/'src/window-path.c',app/'src/os9.c']
cmd=['cmoc','--os9','-O2','--intermediate','--verbose','--add-os9-stack-space=1024',
     '-DDAG_COMPAT_HEARTBEAT','-DDAG_COMPAT_C6','--intdir='+str(out),
     '-I'+str(app/'src'),'-I'+str(app/'src/compat'),'-I'+str(app/'src/audio'),'-o','dodhbst']+list(map(str,stress_sources))
r=subprocess.run(cmd,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(out/'dodhbst.log').write_text(r.stdout)
if r.returncode:print(r.stdout);raise SystemExit(r.returncode)
ident=subprocess.check_output([a.os9,'ident',str(out/'dodhbst')],text=True);assert '(Good)' in ident
record['interactive']['stress']={'module':'dodhbst','ident':ident,
  'sha256':hashlib.sha256((out/'dodhbst').read_bytes()).hexdigest()}
show_sources=[app/'test/compat_c6_trace_show.c',app/'src/audio/ipc.c',app/'src/os9.c']
cmd=['cmoc','--os9','-O2','--add-os9-stack-space=512','--intdir='+str(out),
     '-I'+str(app/'src'),'-I'+str(app/'src/audio'),'-o','dodhbs']+list(map(str,show_sources))
r=subprocess.run(cmd,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(out/'dodhbs.log').write_text(r.stdout)
if r.returncode:print(r.stdout);raise SystemExit(r.returncode)
# C6 extends PTURN's connected command graph without replacing the accepted
# C5 dodcmd artifact.  The output filename is distinct; its OS-9 module name
# remains dodcmd because the resident ABI links that established name.
c6_overlay=app/'src/gameplay/command-overlay.c'
c6_overlay_cmd=['cmoc','--os9','-O2','--function-stack=0','-DDAG_COMPAT_C6','-S',
                '-I'+str(app/'src'),'-I'+str(out),str(c6_overlay)]
r=subprocess.run(c6_overlay_cmd,cwd=out,text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);(out/'dodcmd6-build.log').write_text(r.stdout)
if r.returncode:print(r.stdout);raise SystemExit(r.returncode)
def run_c6(command):subprocess.run(command,cwd=out,check=True)
run_c6(['lwasm','-fobj','--pragma=forwardrefmax','-DOS9','-o','command-overlay-c6.o','command-overlay.s'])
run_c6(['lwasm','-fobj','--pragma=forwardrefmax','-DOS9','-o','command-overlay-module-c6.o',str(app/'src/gameplay/command-overlay-module.asm')])
crt=Path('/usr/local/share/cmoc/lib/libcmoc-crt-os9.a');std=Path('/usr/local/share/cmoc/lib/libcmoc-std-os9.a')
run_c6(['lwar','--extract',str(crt),'MUL16.os9_o','DIV16.os9_o','shiftByteRightUnsigned.os9_o','tfrZtoB.os9_o'])
run_c6(['lwar','--extract',str(std),'MUL168.os9_o'])
c6_overlay_link=['lwlink','--format=raw','--output=command-overlay-code.bin','command-overlay-module-c6.o','command-overlay-c6.o',
                 'MUL16.os9_o','DIV16.os9_o','shiftByteRightUnsigned.os9_o','tfrZtoB.os9_o','MUL168.os9_o']
run_c6(c6_overlay_link)
run_c6(['lwasm','--format=os9','--pragma=forwardrefmax','-o','dodcmd6','-I'+str(out),str(app/'src/gameplay/command-overlay-pack.asm')])
c6_overlay_ident=subprocess.check_output([a.os9,'ident',str(out/'dodcmd6')],text=True);assert '(Good)' in c6_overlay_ident
record['interactive']['overlay']={'module':'dodcmd6','ident':c6_overlay_ident,
  'sha256':hashlib.sha256((out/'dodcmd6').read_bytes()).hexdigest(),'command':c6_overlay_cmd+c6_overlay_link}
heartbeat_out=out/'heartbeat';heartbeat_out.mkdir(exist_ok=True)
subprocess.run(['python3',str(app/'build_heartbeat.py'),'--out',str(heartbeat_out),'--os9',a.os9],check=True,
               stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
shutil.copyfile(heartbeat_out/'dhbpack',out/'dhbpack')
record['play']['heartbeatPack']={'size':(out/'dhbpack').stat().st_size,
  'sha256':hashlib.sha256((out/'dhbpack').read_bytes()).hexdigest()}
(out/'build.json').write_text(json.dumps(record,indent=2)+'\n')
for beat in records:print(records[beat]['ident'])
