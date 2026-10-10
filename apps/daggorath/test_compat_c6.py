#!/usr/bin/env python3
"""C6 repeated HUMAN/parser movement and inventory execution gate."""
from pathlib import Path
import subprocess,tempfile
from import_gameplay import generate

APP=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dag-c6-') as td:
 out=Path(td);generate(out);exe=out/'c6'
 sources=[APP/'test/compat_c6.c',APP/'src/compat/c4.c',APP/'src/compat/c5.c',
          APP/'src/gameplay/game.c',APP/'src/gameplay/command-overlay.c',
          APP/'src/original/logical.c']
 subprocess.run(['cc','-std=c99','-D_GNU_SOURCE','-DDAG_COMPAT_TEST','-DDAG_COMPAT_C6','-Wall','-Wextra',
  '-I'+str(APP/'src'),'-I'+str(APP/'src/compat'),'-I'+str(out),*map(str,sources),
  '-o',str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
print('compat C6 connected interactive checks passed')
