import ctypes,json,sys
from pathlib import Path
p=Path(sys.argv[1] if len(sys.argv)>1 else '/private/tmp/daggorath-timing/live')
c=ctypes.CDLL('/private/tmp/daggorath-timing/logical.dylib');a=(ctypes.c_ubyte*6144)();known={}
tab=[sum(((b>>(7-i))&1)*3<<(14-i*2) for i in range(8)).to_bytes(2,'big') for b in range(256)]
for fade,msg in [(n,0) for n in range(0,33,2)]+[(0,1),(255,0)]:
 c.wizard_frame(a,fade,msg);known[b''.join(tab[b] for b in a)]=(fade,msg)
b=(p/'graphics.bin').read_bytes();rows=(p/'graphics.csv').read_text().splitlines();groups=[];unknown=0
for i,row in enumerate(rows):
 raw=b[i*16000:(i+1)*16000];crop=b''.join(raw[(y+4)*80+8:(y+4)*80+72] for y in range(192));label=known.get(crop)
 if label is None:unknown+=1;continue
 fields=row.split(',');n=int(fields[0]);t=float(fields[1])
 if groups and groups[-1]['label']==label:groups[-1]['last']=n
 else:groups.append(dict(label=label,first=n,last=n,seconds=t))
r=dict(frames=len(rows),unknown=unknown,groups=groups);print(json.dumps(r,indent=2));(p/'classified.json').write_text(json.dumps(r,indent=2))
