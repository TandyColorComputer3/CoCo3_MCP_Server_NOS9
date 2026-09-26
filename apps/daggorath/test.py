#!/usr/bin/env python3
"""Host comparisons against VECTOR's documented fixed-point arithmetic, not screenshots."""
import ctypes,hashlib,json,re,subprocess,tempfile
from pathlib import Path
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dod-tests-') as tmp:
 lib=Path(tmp)/'logical.dylib'
 subprocess.run(['cc','-std=c99','-Wall','-Wextra','-Werror','-shared','-fPIC','-I'+str(app/'src'),str(app/'src/original/logical.c'),'-o',str(lib)],check=True)
 c=ctypes.CDLL(str(lib)); frame=(ctypes.c_ubyte*6144)()
 c.wizard_line(frame,0,0,4,0,0);assert frame[0]==0xf0,'exclusive final endpoint'
 frame=(ctypes.c_ubyte*6144)();c.wizard_line(frame,0,0,4,0,1);assert frame[0]==0x50,'fade counter begins at interval'
 frame=(ctypes.c_ubyte*6144)();c.wizard_line(frame,0,0,0,0,0);assert not any(frame)
 data=(app/'src/original/data.h').read_text()
 segments=[tuple(map(int,m.split(','))) for m in re.findall(r'\{([\d,]+)\},',data)]
 assert len(segments)==84 and segments[0]==(98,46,100,50)
 hashes={}
 for fade in range(0,33,2):
  ref=bytearray(4864)
  for x0,y0,x1,y1 in segments:
   dx,dy=x1-x0,y1-y0;n=max(abs(dx),abs(dy))
   if not n:continue
   sx=(1 if dx>=0 else -1)*(abs(dx)*256//n);sy=(1 if dy>=0 else -1)*(abs(dy)*256//n)
   for k in range(fade,n,fade+1):
    x=(x0*256+128+k*sx)//256;y=(y0*256+128+k*sy)//256
    if 0<=x<256 and 0<=y<152:ref[y*32+x//8]|=128>>(x%8)
  c.wizard_frame(frame,fade,0)
  assert bytes(frame[:4864])==ref,fade
  hashes[str(fade)]=hashlib.sha256(bytes(frame)).hexdigest()
  assert all(frame[y*32+x]==0 for y in range(160,192) for x in range(32))
 c.wizard_frame(frame,0,1)
 assert any(frame[168*32:175*32]) and any(frame[176*32:183*32])
 assert not any(frame[160*32:168*32])
 assert not any(frame[184*32:])
 print('22 rendering checks passed: 3 line rules, source geometry, 17 fade frames, message layout.')
 print(json.dumps(hashes,indent=2))
