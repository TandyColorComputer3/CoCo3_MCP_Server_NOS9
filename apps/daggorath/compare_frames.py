#!/usr/bin/env python3
"""Read-only pixel comparison of MCP snapshots with the logical renderer.
Requires Pillow and a host C compiler; writes JSON only to stdout.
"""
import argparse,ctypes,json,subprocess,tempfile
from pathlib import Path
from PIL import Image
parser=argparse.ArgumentParser();parser.add_argument('snapshots',type=Path);args=parser.parse_args()
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dod-frames-') as tmp:
 lib=Path(tmp)/'logical.dylib'
 subprocess.run(['cc','-shared','-fPIC','-I'+str(app/'src'),str(app/'src/original/logical.c'),'-o',str(lib)],check=True)
 c=ctypes.CDLL(str(lib));frame=(ctypes.c_ubyte*6144)();expected={}
 for fade in list(range(0,33,2))+[255]:
  for message in ([0,1] if fade==0 else [0]):
   c.wizard_frame(frame,fade,message);expected[(fade,message)]=bytes(frame)
 results=[]
 for p in sorted(args.snapshots.glob('snapshot-*.png'),key=lambda p:int(p.stem.split('-')[1])):
  im=Image.open(p).convert('RGB')
  if im.width!=640 or im.height<222:continue
  bits=bytearray(6144)
  for y in range(192):
   for x in range(256):
    if min(im.getpixel((x+192,y+26)))>200:bits[y*32+x//8]|=128>>(x%8)
  matches=[{'fade':f,'messages':m} for (f,m),v in expected.items() if v==bytes(bits)]
  if matches:
   sides=all(max(im.getpixel((x,y)))<16 for y in range(22,222) for x in list(range(192))+list(range(448,640)))
   results.append({'snapshot':p.name,'matches':matches,'sideBandsBlack':sides,'pixelDifferences':0})
 print(json.dumps(results,indent=2))
