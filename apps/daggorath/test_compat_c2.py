#!/usr/bin/env python3
"""C2 source oracle, PIC text, bounded presenter and build checks."""
from pathlib import Path
import hashlib,json,re,subprocess,tempfile

app=Path(__file__).resolve().parent
fixture=app/'test/fixtures/compat-c2'
manifest=json.loads((fixture/'manifest.json').read_text())
assert manifest['referenceCommit']=='a94326f00ebb16a106b540c58bc2ccf5f7b66dac'
for item in manifest['frames']:
    data=(fixture/(item['name']+'.frame')).read_bytes()
    assert len(data)==6144 and hashlib.sha256(data).hexdigest()==item['sha256']
assert [item['cursor'] for item in manifest['frames']]==[0,33,33,34,40,64,72,95,127,96,0]

asm=(app/'src/compat/text.asm').read_text()
assert '_dag_compat_text_apply' in asm and 'compat_glyphs,pcr' in asm
assert re.search(r'_dag_compat_text_apply\s+pshs\s+cc,dp,x,y,u',asm)
assert re.search(r'deposit_loop\s+lda\s+,y\+.*?sta\s+,u.*?leau\s+32,u',asm,re.S)
assert re.search(r'scroll_copy\s+ldd\s+,x\+\+.*?std\s+,u\+\+',asm,re.S)
assert re.search(r'clear_status.*?clr\s+CT_CURSOR,x.*?clr\s+CT_CURSOR\+1,x',asm,re.S)
for forbidden in ('$1000','$2800','$ff00','$c000','pia$','sam'):
    assert forbidden not in asm.lower(),forbidden

with tempfile.TemporaryDirectory(prefix='dag-compat-c2-') as temporary:
    root=Path(temporary);generated=root/'generated'
    subprocess.run(['python3',str(app/'import_compat_text.py'),'--out',str(generated)],check=True)
    for path in [app/'src/compat/text-glyphs.asm',app/'test/compat-c2-data.h',*fixture.glob('*')]:
        other=generated/path.relative_to(app)
        assert path.read_bytes()==other.read_bytes(),path
    binary=root/'present'
    subprocess.run(['cc','-std=c99','-Wall','-Wextra','-Werror','-I'+str(app.parent/'window-manager/test/compat'),'-I'+str(app/'src'),str(app/'src/compat/present.c'),str(app/'test/compat_c2_present.c'),'-o',str(binary)],check=True)
    subprocess.run([str(binary)],check=True)
    builds=[]
    for name in ('a','b'):
        out=root/name
        subprocess.run(['python3',str(app/'build_compat.py'),'--out',str(out)],check=True,stdout=subprocess.DEVNULL)
        builds.append({name:(out/name).read_bytes() for name in ('dodc19','dodc20','dodc21','dodctxt')})
    assert builds[0]==builds[1]
    record=json.loads((root/'a/build.json').read_text());item=record['text']
    assert '(Good)' in item['ident'] and '$11 $80' in item['ident']
    assert len(builds[0]['dodctxt'])<=16384
    data=int(re.search(r'Data size\s+: \$[0-9A-F]+\s+#(\d+)',item['ident']).group(1))
    assert data<=16384

print('compat C2 source oracles, PIC text, primary presentation and reproducible build passed')
