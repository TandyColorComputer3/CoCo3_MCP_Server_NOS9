#!/usr/bin/env python3
"""Deterministic source-derived movement, timing and no-combat checks."""
from pathlib import Path
import subprocess,tempfile
from import_gameplay import generate
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dod-creature-') as tmp:
 out=Path(tmp);generate(out)
 subprocess.run(['cc','-std=c99','-Wall','-Wextra','-Werror','-O2','-I'+str(app/'src'),'-I'+str(app/'src/gameplay'),'-I'+str(out),
  str(app/'test/gameplay_creature.c'),str(app/'src/gameplay/game.c'),str(app/'src/gameplay/creature.c'),str(app/'src/original/logical.c'),'-o',str(out/'check')],check=True)
 subprocess.run([str(out/'check')],check=True)
