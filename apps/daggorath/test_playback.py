#!/usr/bin/env python3
"""All prepared cache frames must equal the unchanged original-derived renderer."""
import ctypes,subprocess,tempfile
from pathlib import Path
from prepare_frames import generate
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dod-cache-') as tmp:
 out=Path(tmp);generate(app,out)
 subprocess.run(['cc','-shared','-fPIC','-I'+str(app.parent/'window-manager/test/compat'),'-I'+str(app/'src'),'-I'+tmp,str(app/'src/playback.c'),str(app/'src/original/logical.c'),'-o',tmp+'/test.dylib'],check=True)
 c=ctypes.CDLL(tmp+'/test.dylib');a=(ctypes.c_ubyte*6144)();b=(ctypes.c_ubyte*6144)()
 for f,m in [(n,0) for n in range(0,33,2)]+[(0,1),(255,0)]:
  c.wizard_frame(a,f,m);c.playback_frame(b,f,m);assert bytes(a)==bytes(b),(f,m)
 print('19 cache frames match original-derived renderer byte for byte')
