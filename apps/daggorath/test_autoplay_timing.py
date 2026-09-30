#!/usr/bin/env python3
"""COMPLR:HSLOW logical-jiffy and source AUTTAB 1–9 timing checks."""
from pathlib import Path
import subprocess,tempfile
from import_gameplay import generate
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dod-autoplay-timing-') as tmp:
 out=Path(tmp);generate(out)
 flags=['cc','-std=c99','-Wall','-Wextra','-Werror','-O2','-I'+str(app/'src'),'-I'+str(out)]
 subprocess.run(flags+[str(app/'test/autoplay_timing.c'),
  str(app/'src/gameplay/command-overlay.c'),str(app/'src/gameplay/game.c'),
  str(app/'src/gameplay/creature.c'),str(app/'src/original/logical.c'),'-o',str(out/'timing')],check=True)
 subprocess.run([str(out/'timing')],check=True)
 print('autoplay timing checks passed')
