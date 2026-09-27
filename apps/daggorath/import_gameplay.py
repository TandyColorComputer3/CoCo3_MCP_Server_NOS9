#!/usr/bin/env python3
"""Extract selected DATA from the pinned recovered source; never ship its ROM code."""
from pathlib import Path
import subprocess,re,json,hashlib
REF=Path('/Volumes/SEDONA/Projects/daggorath-reference')
COMMIT='a94326f00ebb16a106b540c58bc2ccf5f7b66dac'
def generate(out):
 out=Path(out);out.mkdir(parents=True,exist_ok=True)
 assert subprocess.check_output(['git','-C',str(REF),'rev-parse','HEAD'],text=True).strip()==COMMIT
 cmd=['lwasm','--format=raw','-I',str(REF),'--symbol-dump='+str(out/'original.sym'),'-o',str(out/'original.rom'),str(REF/'DAGGORATH.ASM')]
 subprocess.run(cmd,check=True)
 symbols={k:int(v,16) for k,v in re.findall(r'^(\S+) EQU \$([0-9A-F]+)$',(out/'original.sym').read_text(),re.M)}
 rom=(out/'original.rom').read_bytes()
 def b(p):return rom[p-0xc000]
 def word(p):return b(p)*256+b(p+1)
 def segments(pc):
  stack=[];point=None;result=[];relative=False;steps=0
  while True:
   steps+=1;assert steps<10000
   c=b(pc);pc+=1
   if relative:
    if c==0:relative=False;point=None;continue
    y,x=point;dy=c>>4;dx=c&15;dy=dy-16 if dy&8 else dy;dx=dx-16 if dx&8 else dx
    q=((y+dy*2)&255,(x+dx*2)&255);result.append((*point,*q));point=q;continue
   if c==250:pc=stack.pop();point=None
   elif c==251:stack.append(pc+2);pc=word(pc);point=None
   elif c==252:relative=True
   elif c==253:pc=word(pc);point=None
   elif c==254:break
   elif c==255:point=None
   else:
    q=(c,b(pc));pc+=1
    if point is not None:result.append((*point,*q))
    point=q
  assert not stack
  return result
 # VIEWER FLATAB draw order is retained separately; these IDs have no game meaning.
 names=['LPASAG','LDOOR','LSDOOR','LWALL','FPASAG','FDOOR','FSDOOR','FWALL','RPASAG','RDOOR','RSDOOR','RWALL','CELINE','LPEEK','RPEEK']
 for table,count in [('FWDVER',4),('FWDCRE',4)]:
  for i in range(count):
   addr=word(symbols[table]+i*2);names.append(next(k for k,v in symbols.items() if v==addr))
 lines=['/* Generated original data only. Dyna Micro; see PROVENANCE.md. */']
 flat=[];ranges=[]
 for name in names:
  v=segments(symbols[name]);ranges.append((len(flat),len(v)));flat+=v
 lines+=['static const unsigned char game_vectors[][4]={']+['{'+','.join(map(str,v))+'},' for v in flat]+['};']
 lines+=['static const unsigned short game_lists[][2]={'+','.join('{%d,%d}'%v for v in ranges)+'};']
 def array(name,values):lines.append('static const unsigned char '+name+'[]={'+','.join(map(str,values))+'};')
 for name,label,size in [('odb','ODBTAB',100),('cdb','CDBTAB',96),('omx','OMXTAB',18),('cmt','CMTTAB',12),('font','SWCTAB',155)]:
  array(name,[b(symbols[label]+i) for i in range(size)])
 # STATUS:COPY$ skips the first expanded byte (token class; PARSER:PARS20).
 # Retain the original 5-bit alphabet, not a hand-transcribed name catalog.
 for name,label in [('status_adjectives','ADJTAB'),('status_generics','GENTAB'),('parser_commands','CMDTAB'),('parser_directions','DIRTAB')]:
  p=symbols[label];count=b(p);p+=1;rows=[];classes=[]
  def five_at(bit):return sum(((b(p+(bit+i)//8)>>(7-(bit+i)%8))&1)<<(4-i) for i in range(5))
  for _ in range(count):
   length=five_at(0)+1
   expanded=[five_at(5+i*5) for i in range(length)]
   classes.append(expanded[0]);values=expanded[1:]
   rows.append(values+[255]);p+=(5+length*5+7)//8
  lines.append('static const unsigned char '+name+'[][16]={'+','.join('{'+','.join(map(str,row))+'}' for row in rows)+'};')
  array(name+'_classes',classes)
 for name in ['T.PULL','T.STOW','T.LT','T.RT']:
  lines.append('#define PAR_'+name[2:]+' '+str(symbols[name]))
 array('status_hearts',[b(symbols['SPCTAB']+i) for i in range(28)])
 p=symbols['XXXTAB'];special=[]
 while b(p)<128:special.extend(b(p+i) for i in range(4));p+=4
 array('special',special)
 p=symbols['FLATAB'];order=[]
 while b(p)<128:order.append(b(p));p+=9
 array('draw_order',order)
 # Level zero has an empty up list, then its down list.
 p=symbols['VFTTAB'];vertical=[]
 for side in range(2):
  while b(p)<128:vertical.extend([b(p)+side*2,b(p+1),b(p+2)]);p+=3
  p+=1
 array('vertical',vertical)
 (out/'game_data.h').write_text('\n'.join(lines)+'\n')
 record={'commit':COMMIT,'command':cmd,'vectorLists':names,'files':{p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in REF.glob('*.ASM')},'headerSha256':hashlib.sha256((out/'game_data.h').read_bytes()).hexdigest()}
 (out/'game-data-provenance.json').write_text(json.dumps(record,indent=2)+'\n')
 return record
if __name__=='__main__':
 import sys
 generate(sys.argv[1])
