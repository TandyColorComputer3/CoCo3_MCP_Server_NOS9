#!/usr/bin/env python3
"""Read-only verification of twelve retained captures. Requires Pillow and cc."""
import ctypes
import json
from pathlib import Path
import subprocess
import tempfile
from PIL import Image, ImageChops
assets=Path(__file__).resolve().parent
root=assets.parents[3]
results=[]
with tempfile.TemporaryDirectory(prefix='dod-scale-verify-') as tmp:
    library=Path(tmp)/'logical.dylib'
    subprocess.run(['cc','-shared','-fPIC','-I'+str(root/'apps/daggorath/src'),
                    str(root/'apps/daggorath/src/original/logical.c'),'-o',str(library)],check=True)
    renderer=ctypes.CDLL(str(library))
    for mode,w,h in [('a',512,192),('b',426,160),('c',384,144)]:
        ox,oy=(640-w)//2,(200-h)//2
        for name,fade,msg in [('wizard',0,0),('message',0,1),('thin',14,0),('fine',32,0)]:
            frame=(ctypes.c_ubyte*6144)()
            renderer.wizard_frame(frame,fade,msg)
            expected=Image.new('L',(640,200));pixels=expected.load()
            for x in range(640):
                for y in (0,199,oy-2,oy+h+1):pixels[x,y]=255
            for y in range(200):
                for x in (0,639,ox-2,ox+w+1):pixels[x,y]=255
            for y in range(h):
                sy=y*192//h
                for x in range(w):
                    sx=x*256//w
                    if frame[sy*32+sx//8]&(128>>(sx%8)):pixels[ox+x,oy+y]=255
            file=mode+'-'+name+'.png'
            actual=Image.open(assets/file).convert('L').crop((0,22,640,222))
            assert ImageChops.difference(actual,expected).getbbox() is None,file
            results.append({'capture':file,'pixelDifferences':0})
print(json.dumps(results,indent=2))
