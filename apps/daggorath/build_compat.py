#!/usr/bin/env python3
"""Build the bounded C1 Giant fixtures and C2 text demonstration."""
from pathlib import Path
import argparse,subprocess,json,hashlib

app=Path(__file__).resolve().parent
p=argparse.ArgumentParser();p.add_argument('--out',required=True);p.add_argument('--os9',default='/Users/magneto-optimus/Documents/Coding/toolshed-2.2/build/unix/os9/os9');a=p.parse_args()
out=Path(a.out).resolve();out.mkdir(parents=True,exist_ok=True)
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
(out/'build.json').write_text(json.dumps(record,indent=2)+'\n')
for beat in records:print(records[beat]['ident'])
