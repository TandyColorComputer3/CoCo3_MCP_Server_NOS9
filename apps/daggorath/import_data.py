#!/usr/bin/env python3
"""Read only the pinned recovered source. Emit the selected wizard data, not the game."""
from pathlib import Path
import re,json,hashlib,subprocess
root=Path('/Volumes/SEDONA/Projects/daggorath-reference')
app=Path(__file__).resolve().parent
commit=subprocess.check_output(['git','-C',str(root),'rev-parse','HEAD'],text=True).strip()
assert commit=='a94326f00ebb16a106b540c58bc2ccf5f7b66dac'
d4=(root/'D4.ASM').read_text(); segments=[]; locations=[]
for label,end in [('WIZ1','WIZ2'),('WIZ0',None)]:
 start=re.search(r'^'+label+r'\s',d4,re.M).start()
 chunk=d4[start:]
 chunk=chunk[:re.search(r'^'+end+r'\s',chunk,re.M).start()] if end else chunk[:chunk.index('SVEND')+5]
 point=None
 for offset,line in enumerate(chunk.splitlines()):
  clean=line.split(';')[0]; m=re.search(r'\b(SVORG|SVECT|FCB|SVNEW|SVEND)\s*(.*)',clean)
  if not m:continue
  op,arg=m.groups();arg=arg.strip()
  if op in ('SVNEW','SVEND') or arg=='V$NEW':point=None;continue
  if op=='FCB' and not re.fullmatch(r'\d+,\s*\d+',arg):continue
  y,x=map(int,arg.split(','))
  if op=='SVORG':point=(x,y);continue
  if point:
   if op=='SVECT':
    # missing-macros.asm emits signed nibble deltas in two-pixel units.
    assert all((b-a)%2==0 and -16<=b-a<=14 for a,b in zip(point,(x,y)))
   segments.append((*point,x,y));locations.append({'label':label,'line':d4[:start].count('\n')+offset+1})
  point=(x,y)

def packed(section):return bytes(int(b,2) for b in re.findall(r'^\s*FCB\s+%([01]{8})',section,re.M))
def expand(data):
 bits=''.join(f'{b:08b}' for b in data);n=int(bits[:5],2)+1
 return [int(bits[i:i+5],2) for i in range(5,5+5*n,5)]
sw=(root/'SWCHAR.ASM').read_text();fonts=packed(sw[sw.index('SWCTAB  EQU'):sw.index('SPCTAB')])
assert len(fonts)==155
once=(root/'ONCE.ASM').read_text()
copyright=expand(packed(once[once.index(';  Display COPYRIGHT'):once.index('DEMO10  DEC')]))
intro=once[once.index('DEMO10  DEC'):once.index(';  Create Autoplay')]
p1=expand(packed(intro[:intro.index(';welcome message (part II)')]))
p2=expand(packed(intro[intro.index(';welcome message (part II)'):]))
out=['/* Generated from recovered Daggorath source '+commit+'.',
 ' * Original copyright: Dyna Micro, MCMLXXXII. See PROVENANCE.md.',
 ' * WIZ1 -> WIZ0 at unity scale; each row is x0,y0,x1,y1 in original draw order. */',
 'static const unsigned char wizard_segments[][4]={']
out += ['    {'+','.join(map(str,s))+'},' for s in segments];out+=['};']
for name,data in [('packed_font',fonts),('status_text',copyright),('intro1',p1),('intro2',p2)]:
 out+=['static const unsigned char '+name+'[]={'+','.join(map(str,data))+'};']
(app/'src/original/data.h').write_text('\n'.join(out)+'\n')
files=['D4.ASM','missing-macros.asm','VECTOR.ASM','VCTLST.ASM','MISC.ASM','ONCE.ASM','COMDAT.ASM','COMTXT.ASM','TXTSER.ASM','EXPAND.ASM','SWCHAR.ASM','CD.ASM']
(app/'src/original/provenance.json').write_text(json.dumps({'commit':commit,'repository':str(root),'files':{n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in files},'segments':locations,'messages':{'status':copyright,'intro1':p1,'intro2':p2}},indent=2)+'\n')
print(len(segments),'segments; message display codes:',copyright,p1,p2)
