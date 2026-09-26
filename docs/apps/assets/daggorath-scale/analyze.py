from pathlib import Path
import ctypes,subprocess,json,re
from PIL import Image,ImageChops
p=Path('/private/tmp/daggorath-scale');root=Path('/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9');a=root/'docs/apps/assets/daggorath-scale';a.mkdir(exist_ok=True)
subprocess.run(['cc','-shared','-fPIC','-I'+str(root/'apps/daggorath/src'),str(root/'apps/daggorath/src/original/logical.c'),'-o',str(p/'logical.dylib')],check=True)
c=ctypes.CDLL(str(p/'logical.dylib'));data=(root/'apps/daggorath/src/original/data.h').read_text();segments=[tuple(map(int,m.split(','))) for m in re.findall(r'\{([\d,]+)\},',data)]
frames={}
for name,fade,msg in [('wizard',0,0),('message',0,1),('thin',14,0),('fine',32,0)]:
 f=(ctypes.c_ubyte*6144)();c.wizard_frame(f,fade,msg);frames[name]=(bytes(f),fade)
records=[json.loads(l) for l in (p/'live/results.jsonl').read_text().splitlines()]
snaps=[r for r in records if r['request']['name']=='coco_snapshot']
results={}
for mode,w,h in [('a',512,192),('b',426,160),('c',384,144)]:
 ox,oy=(640-w)//2,(200-h)//2;ys={y*192//h for y in range(h)};out={}
 for name,(f,fade) in frames.items():
  im=Image.new('L',(640,200));pix=im.load()
  for x in range(640):
   for y in (0,199,oy-2,oy+h+1):pix[x,y]=255
  for y in range(200):
   for x in (0,639,ox-2,ox+w+1):pix[x,y]=255
  for y in range(h):
   sy=y*192//h
   for x in range(w):
    sx=x*256//w
    if f[sy*32+sx//8]&(128>>(sx%8)):pix[ox+x,oy+y]=255
  matches=[]
  for r in snaps:
   file=p/'live'/('snapshot-'+str(r['id'])+'.png')
   if file.exists() and ImageChops.difference(Image.open(file).convert('L').crop((0,22,640,222)),im).getbbox() is None:matches.append({'id':r['id'],'time':r['completedAt']})
  if matches:
   first=p/'live'/('snapshot-'+str(matches[0]['id'])+'.png');Image.open(first).save(a/(mode+'-'+name+'.png'))
  pts=[(x,y) for y in range(152) for x in range(256) if f[y*32+x//8]&(128>>(x%8))]
  lost=sum(y not in ys for x,y in pts)
  # Count original vector segments with raster points, and those with none surviving vertical sampling.
  eligible=gone=0
  for seg in segments:
   b=(ctypes.c_ubyte*6144)();c.wizard_line(b,*seg,fade)
   rows=[y for y in range(152) if any(b[y*32:(y+1)*32])]
   if rows:eligible+=1;gone+=not any(y in ys for y in rows)
  out[name]={'matches':matches,'logicalSetPixels':len(pts),'unsampledLogicalSetPixels':lost,'lossPercent':100*lost/len(pts),'segmentsWithPixels':eligible,'segmentsEntirelyUnsampled':gone}
  if name=='message':
   textpts=[(x,y) for y in range(168,183) for x in range(256) if f[y*32+x//8]&(128>>(x%8))]
   out[name]['messageInkPixels']=len(textpts);out[name]['messageInkUnsampled']=sum(y not in ys for x,y in textpts)
   # Inverse status glyph ink: zero bits within the seven glyph rows.
  if name=='wizard':
   crop=im.crop((ox,oy,ox+w,oy+(152*h+191)//192));bbox=crop.getbbox();out[name]['scaledWizardBboxExclusive']=bbox
 results[mode]={'width':w,'height':h,'origin':[ox,oy],'mapsBytes':2*w+2*h,'viewportBytes':((w+7)//8)*h,'sourceRowsSkipped':192-len(ys),'frames':out}
# Font-shape ambiguity after vertical sampling only, before horizontal replication.
font=list(map(int,re.search(r'packed_font\[\]=\{([^}]+)',data).group(1).split(',')))
def glyph(code):
 b=font[code*5:code*5+5]
 return [sum(((b[k//8]>>(7-k%8))&1)<<(4-i) for i,k in enumerate(range(5+y*5,10+y*5))) for y in range(7)]
for v in results.values():
 keep={y*192//v['height'] for y in range(v['height'])};v['fontVerticalSampling']={}
 for base in (152,168,176):
  rows=[i for i in range(7) if base+i in keep];groups={}
  for code in range(1,27):groups.setdefault(tuple(glyph(code)[i] for i in rows),[]).append(chr(64+code))
  v['fontVerticalSampling'][str(base)]={'rowsRetained':rows,'letterCollisions':[x for x in groups.values() if len(x)>1]}
(p/'measurements.json').write_text(json.dumps(results,indent=2)+'\n')
for k,v in results.items():print(k,[(n,len(s['matches']),s['lossPercent'],s['segmentsEntirelyUnsampled']) for n,s in v['frames'].items()])
