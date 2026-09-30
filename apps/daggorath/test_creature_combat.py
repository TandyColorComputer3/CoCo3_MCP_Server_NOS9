#!/usr/bin/env python3
"""Source-state CRETUR:CMOV20 tests through the production dodsched body."""
from pathlib import Path
import subprocess,tempfile
from import_gameplay import generate
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dod-creature-combat-') as tmp:
 out=Path(tmp);generate(out)
 flags=['cc','-std=c99','-Wall','-Wextra','-Werror','-O2','-I'+str(app/'src'),'-I'+str(out)]
 sources=[app/'test/creature_combat.c',app/'src/gameplay/scheduler.c',app/'src/gameplay/game.c',
          app/'src/gameplay/creature.c',app/'src/original/logical.c']
 subprocess.run(flags+list(map(str,sources))+['-o',str(out/'check')],check=True)
 subprocess.run([str(out/'check')],check=True)
print('source-state CRETUR combat checks passed')
