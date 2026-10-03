#!/usr/bin/env python3
"""Public opening reaches real commands using retained transcript and modules."""
from pathlib import Path
import subprocess,tempfile
from import_gameplay import generate
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dod-public-m5-') as tmp:
 out=Path(tmp);generate(out)
 (out/'cmoc.h').write_text('#include <stdio.h>\n#include <string.h>\n')
 command=['cc','-std=c99','-O2','-DDOD_COMMAND_OVERLAY',
  '-I'+str(out),'-I'+str(app/'src'),'-I'+str(app/'src/audio')]
 parts=['test/public_m5.c','src/gameplay/game.c','src/gameplay/command-overlay.c',
  'src/gameplay/scheduler.c','src/gameplay/opening-map.c','src/gameplay/primary-text.c',
  'src/original/logical.c']
 subprocess.run(command+[str(app/p) for p in parts]+['-o',str(out/'public')],check=True)
 subprocess.run([str(out/'public')],check=True)
