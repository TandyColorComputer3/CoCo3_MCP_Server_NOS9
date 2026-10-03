#!/usr/bin/env python3
"""Connected C5 HUMAN/AUTTAB/parser/scheduler trajectory gate."""
from pathlib import Path
import subprocess,tempfile
from import_gameplay import generate
APP=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dag-c5-') as td:
 out=Path(td);generate(out);exe=out/'c5'
 sources=[APP/'test/compat_c5.c',APP/'src/compat/c4.c',APP/'src/compat/c5.c',
          APP/'src/gameplay/game.c',APP/'src/gameplay/command-overlay.c',
          APP/'src/gameplay/scheduler.c',APP/'src/original/logical.c']
 subprocess.run(['cc','-std=c99','-D_GNU_SOURCE','-DDAG_COMPAT_TEST','-Wall','-Wextra',
  '-I'+str(APP/'src'),'-I'+str(APP/'src/compat'),'-I'+str(out),*map(str,sources),'-o',str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
print('compat C5 connected execution checks passed')
