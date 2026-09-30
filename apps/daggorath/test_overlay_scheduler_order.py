#!/usr/bin/env python3
from pathlib import Path
import subprocess,tempfile
from import_gameplay import generate
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dod-one-slot-') as tmp:
 out=Path(tmp);generate(out)
 subprocess.run(['cc','-std=c99','-Wall','-Wextra','-Werror','-O2','-I'+str(app/'src'),'-I'+str(out),
  str(app/'test/overlay_scheduler_order.c'),str(app/'src/gameplay/overlay-host.c'),
  str(app/'src/gameplay/scheduler-host.c'),'-o',str(out/'one-slot')],check=True)
 subprocess.run([str(out/'one-slot')],check=True)
print('overlay/scheduler one-slot ordering checks passed')
