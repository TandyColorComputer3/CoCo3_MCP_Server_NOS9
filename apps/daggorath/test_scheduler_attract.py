#!/usr/bin/env python3
"""AUTTAB 1-9 through the callable scheduler body and production overlay."""
from pathlib import Path
import subprocess,tempfile
from import_gameplay import generate
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dodsched-attract-') as tmp:
 out=Path(tmp);generate(out)
 flags=['cc','-std=c99','-Wall','-Wextra','-Werror','-O2','-I'+str(app/'src'),'-I'+str(out)]
 sources=[app/'test/scheduler_attract.c',app/'src/gameplay/scheduler.c',
          app/'src/gameplay/command-overlay.c',app/'src/gameplay/game.c',
          app/'src/gameplay/creature.c',app/'src/original/logical.c']
 subprocess.run(flags+list(map(str,sources))+['-o',str(out/'attract')],check=True)
 subprocess.run([str(out/'attract')],check=True)
print('dodsched AUTTAB integration checks passed')
