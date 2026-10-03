#!/usr/bin/env python3
"""C3 source import, choreography, oracle, build and architecture checks."""
from pathlib import Path
import hashlib,json,subprocess,tempfile
app=Path(__file__).resolve().parent
root=app.parent.parent
fixtures=app/'test/fixtures/compat-c3'
manifest=json.loads((fixtures/'manifest.json').read_text())
assert manifest['referenceCommit']=='a94326f00ebb16a106b540c58bc2ccf5f7b66dac'
assert manifest['segmentCount']==84
for rec in manifest['frames']:
 data=(fixtures/(rec['name']+'.frame')).read_bytes();assert len(data)==6144
 assert hashlib.sha256(data).hexdigest()==rec['sha256']
with tempfile.TemporaryDirectory() as td:
 out=Path(td);subprocess.run(['python3',str(app/'import_compat_wizard.py'),'--out',str(out)],check=True)
 for rel in ('src/compat/wizard-data.asm','src/compat/opening-data.h','test/fixtures/compat-c3/manifest.json'):
  assert (out/rel).read_bytes()==(app/rel).read_bytes(),rel
 for rec in manifest['frames']:assert (out/'test/fixtures/compat-c3'/(rec['name']+'.frame')).read_bytes()==(fixtures/(rec['name']+'.frame')).read_bytes()
exe=Path(tempfile.mktemp(prefix='compat-c3-seq-'))
try:
 subprocess.run(['cc','-std=c99','-Wall','-Wextra','-I'+str(app/'src'),'-I'+str(app/'src/compat'),
  str(app/'test/compat_c3_sequence.c'),str(app/'src/compat/opening.c'),'-o',str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
finally:
 exe.unlink(missing_ok=True)
wizard=(app/'src/compat/wizard-render.asm').read_text();opening=(app/'src/compat/opening.c').read_text()
assert 'include "wizard-data.asm"' in wizard and 'pcr' in wizard and 'WIZARD_SEGMENT_COUNT' in wizard
assert 'os_sleep(81)' in opening and 'for(fade=32;;fade-=2)' in opening and 'for(fade=0;fade<32;fade+=2)' in opening
assert 'game_init' not in opening and 'heartbeat' not in opening and 'scheduler' not in opening
with tempfile.TemporaryDirectory() as a,tempfile.TemporaryDirectory() as b:
 subprocess.run(['python3',str(app/'build_compat.py'),'--out',a],check=True,stdout=subprocess.DEVNULL)
 subprocess.run(['python3',str(app/'build_compat.py'),'--out',b],check=True,stdout=subprocess.DEVNULL)
 for name in ('dodc19','dodc20','dodc21','dodctxt','dodcopen'):
  x=Path(a)/name;y=Path(b)/name;assert x.read_bytes()==y.read_bytes(),name
 size=(Path(a)/'dodcopen').stat().st_size;assert size<=16384,size
 build=json.loads((Path(a)/'build.json').read_text());assert build['opening']['sha256']==hashlib.sha256((Path(a)/'dodcopen').read_bytes()).hexdigest()
 ident=build['opening']['ident']
 data_size=int(ident.split('Data size  : $',1)[1].split()[0],16)
 assert data_size<=16384,data_size
print('compat C3 source choreography, exact oracles, reproducibility and two-block program gate passed')
