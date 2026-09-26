#!/usr/bin/env python3
"""Verify retained live captures against the unchanged logical renderer.
Requires Pillow and a host C compiler. Does not modify the repository.
"""
import ctypes
import hashlib
import json
from pathlib import Path
import subprocess
import tempfile
from PIL import Image, ImageChops

assets = Path(__file__).resolve().parent
root = assets.parents[3]
with tempfile.TemporaryDirectory(prefix='dod-aspect-verify-') as tmp:
    library = Path(tmp) / 'logical.dylib'
    subprocess.run(['cc', '-shared', '-fPIC', '-I' + str(root/'apps/daggorath/src'),
                    str(root/'apps/daggorath/src/original/logical.c'), '-o', str(library)], check=True)
    frame = (ctypes.c_ubyte * 6144)()
    ctypes.CDLL(str(library)).wizard_frame(frame, 0, 1)
    result = {'logicalSha256': hashlib.sha256(bytes(frame)).hexdigest(), 'candidates': []}
    for width, name in [(256, 'one.png'), (512, 'two.png'), (640, 'two-and-half.png')]:
        origin = (640-width)//2
        expected = Image.new('L', (640,192))
        for y in range(192):
            for x in range(width):
                sx = x if width == 256 else x//2 if width == 512 else x*2//5
                if frame[y*32+sx//8] & (128 >> (sx%8)):
                    expected.putpixel((origin+x,y),255)
        actual = Image.open(assets/name).convert('L').crop((0,26,640,218))
        assert ImageChops.difference(actual,expected).getbbox() is None, name
        # Check side bands across all 200 active rows, not just the viewport.
        screen = Image.open(assets/name).convert('L')
        if origin:
            assert screen.crop((0,22,origin,222)).getbbox() is None
            assert screen.crop((640-origin,22,640,222)).getbbox() is None
        bbox = expected.crop((0,0,640,152)).getbbox()
        w,h = bbox[2]-bbox[0],bbox[3]-bbox[1]
        result['candidates'].append({'capture':name,'viewportWidth':width,
            'wizardBoundsExclusive':bbox,'wizardSize':[w,h], 'sideSpaceEach':origin,
            'pixelDifferences':0,'rawAspect':w/h,'apparentAspect4by3':w/h*239/480})
    print(json.dumps(result,indent=2))
