#!/usr/bin/env python3
"""Exact C1 fixture, PIC-source, presenter and reproducible-build checks."""
from pathlib import Path
import hashlib,json,re,subprocess,tempfile

app=Path(__file__).resolve().parent
fixtures=app/'test/fixtures/compat-c1'
expected={
 '019.frame':'8b61e472f68fcb438fb3ad8ca72438619160dcb3c4254cde34fbbb4875b62ea6',
 '019.oracle':'747af69974b46b059231b1086da174d1afc565d3a7067e9886a9550a35b6641b',
 '019.state':'41e3ff24bedd2a921e4a9154d638d775df35c2642f2e603fde4b66c37ca78d56',
 '020.frame':'b70d84efed762cdab978732c4bc312fe5055c57a8827c0fb93921f098cb258ab',
 '020.oracle':'7b62fd1f25f531e7b83de18a7f5872973c076896b7924b7ebf19f94886d233fa',
 '020.state':'0b6777628fbcf5162243b0f878df70a92b6a0daa5256d807baec745d295526d0',
 '021.frame':'a53ed0e353483f436c3305df6135a6c2c2b0eca2630bd9f6c33acaf12de61c1e',
 '021.oracle':'cec2c7feef26f57f05c6580c3a3adc555ef309ac7b413ee5f4ca0ab06e66a976',
 '021.state':'0a3ea23c66fbb41e5d795ff91165d4d06dcce2db18ab3463b1c11cb221b78de0',
}
for name,digest in expected.items():
 data=(fixtures/name).read_bytes()
 assert hashlib.sha256(data).hexdigest()==digest,name
 assert len(data)==(3333 if name.endswith('.state') else 6144),(name,len(data))

contributions={'019':(4,(187,93,188,105)),'020':(118,(85,69,162,114)),'021':(251,(74,67,173,126))}
for beat,(count,bounds) in contributions.items():
 before=(fixtures/f'{beat}.frame').read_bytes();after=(fixtures/f'{beat}.oracle').read_bytes()
 pixels=[]
 for offset,(a,b) in enumerate(zip(before,after)):
  changed=a^b
  for bit in range(8):
   if changed&(0x80>>bit):pixels.append(((offset%32)*8+bit,offset//32))
 assert len(pixels)==count,(beat,len(pixels))
 assert (min(x for x,y in pixels),min(y for x,y in pixels),max(x for x,y in pixels),max(y for x,y in pixels))==bounds

header=(app/'src/compat/compat.h').read_text();giant=(app/'src/compat/giant-render.asm').read_text();expand=(app/'src/compat/expand.asm').read_text()
for text in ['DAG_COMPAT_FRAME_BYTES 6144','DAG_COMPAT_STATE_BYTES 3333','DAG_COMPAT_CCB_BASE 0x01D4','DAG_COMPAT_CCB_END 0x03F4','DAG_COMPAT_NEIBOR 0x07F4']:
 assert text in header,text
assert '_dag_compat_render_giant' in giant and 'leax    rpeek,pcr' in giant and 'leax    fwdcre,pcr' in giant
assert re.search(r'drawit\s+pshs\s+u\s+pshs\s+x\s+lbsr\s+setfad\s+puls\s+x',giant,re.S)
assert re.search(r'^hlfscl\s+fcb\s+\$ff$',giant,re.M)
assert re.search(r'^bakscl\s+fcb\s+\$9c,\$64,\$41,\$28,\$1a,\$10,\$0a,\$06,\$03,\$01$',giant,re.M)
assert '$c000' not in giant.lower() and '$ff00' not in giant.lower()
assert '_dag_compat_expand_strip' in expand and 'leay    doubled,pcr' in expand
assert expand.count('        expand_one\n')==8 and 'tfm' not in expand.lower() and 'ldq' not in expand.lower()

with tempfile.TemporaryDirectory(prefix='dag-compat-c1-') as temporary:
 root=Path(temporary);binary=root/'host'
 subprocess.run(['cc','-std=c99','-Wall','-Wextra','-Werror','-I'+str(app.parent/'window-manager/test/compat'),'-I'+str(app/'src'),str(app/'src/compat/present.c'),str(app/'test/compat_c1.c'),'-o',str(binary)],check=True)
 subprocess.run([str(binary)],check=True)
 builds=[]
 for name in ('a','b'):
  out=root/name
  subprocess.run(['python3',str(app/'build_compat.py'),'--out',str(out)],check=True,stdout=subprocess.DEVNULL)
  builds.append({p.name:p.read_bytes() for p in out.glob('dodc??')})
 assert builds[0]==builds[1] and set(builds[0])=={'dodc19','dodc20','dodc21'}
 record=json.loads((root/'a/build.json').read_text())
 for beat,item in record['fixtures'].items():
  assert '(Good)' in item['ident'] and 'Prog mod' in item['ident'] and '$11 $80' in item['ident']
  assert len(builds[0][item['module']])<=16384

print('compat C1 exact oracles, PIC invariants, presenter and reproducible build passed')
