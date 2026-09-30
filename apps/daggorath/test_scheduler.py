#!/usr/bin/env python3
from pathlib import Path
import subprocess,tempfile
from import_gameplay import generate
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dodsched-test-') as tmp:
 out=Path(tmp)
 generate(out)
 cmd=['cc','-std=c99','-Wall','-Wextra','-Werror','-O2','-I'+str(app/'src'),
      '-I'+str(out),
      str(app/'test/scheduler.c'),str(app/'src/gameplay/scheduler.c'),'-o',str(out/'scheduler')]
 subprocess.run(cmd,check=True);subprocess.run([str(out/'scheduler')],check=True)
print('dodsched host ABI checks passed')
