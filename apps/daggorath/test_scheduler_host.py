#!/usr/bin/env python3
from pathlib import Path
import subprocess,tempfile
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dodsched-host-') as tmp:
 out=Path(tmp)/'host'
 subprocess.run(['cc','-std=c99','-Wall','-Wextra','-Werror','-O2','-I'+str(app/'src'),
  str(app/'test/scheduler_host.c'),str(app/'src/gameplay/scheduler-host.c'),'-o',str(out)],check=True)
 subprocess.run([str(out)],check=True)
print('dodsched retained-link host checks passed')
