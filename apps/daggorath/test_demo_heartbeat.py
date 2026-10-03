#!/usr/bin/env python3
"""Regression: doddemo reconciles a native edge that occurs mid full flip."""
from pathlib import Path
import subprocess,tempfile
from import_gameplay import generate

app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='doddemo-heartbeat-') as tmp:
 out=Path(tmp);generate(out)
 (out/'cmoc.h').write_text('#include <stdio.h>\n#include <string.h>\n')
 flags=['cc','-std=c99','-Wall','-Wextra','-Werror','-I'+str(out),'-I'+str(app/'src'),'-I'+str(app/'src/audio')]
 subprocess.run(flags+['-Dmain=demo_main','-c',str(app/'src/gameplay/demo.c'),'-o',str(out/'demo.o')],check=True)
 subprocess.run(flags+[str(app/'test/demo_heartbeat.c'),str(out/'demo.o'),'-o',str(out/'demo-heartbeat')],check=True)
 subprocess.run([str(out/'demo-heartbeat')],check=True)
print('doddemo heartbeat flip reconciliation checks passed')
