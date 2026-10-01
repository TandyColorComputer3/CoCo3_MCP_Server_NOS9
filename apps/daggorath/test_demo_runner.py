#!/usr/bin/env python3
"""Deterministic runner-level gate for doddemo's bounded AUTTAB reel."""
from pathlib import Path
import subprocess,tempfile
from import_gameplay import generate

app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='doddemo-runner-') as tmp:
 out=Path(tmp);generate(out)
 (out/'cmoc.h').write_text('#include <stdio.h>\n#include <string.h>\n')
 flags=['cc','-std=c99','-Wall','-Wextra','-Werror','-I'+str(out),'-I'+str(app/'src'),'-I'+str(app/'src/audio')]
 subprocess.run(flags+['-Dmain=demo_main','-c',str(app/'src/gameplay/demo.c'),'-o',str(out/'demo.o')],check=True)
 subprocess.run(flags+[str(app/'test/demo_runner.c'),str(out/'demo.o'),'-o',str(out/'demo-runner')],check=True)
 subprocess.run([str(out/'demo-runner')],check=True)
print('doddemo runner choreography checks passed')
