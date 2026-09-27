#!/usr/bin/env python3
from pathlib import Path
import tempfile,subprocess
from import_gameplay import generate
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory() as temp:
 out=Path(temp);generate(out);(out/'cmoc.h').write_text('#include <stdio.h>\n#include <string.h>\n')
 flags=['cc','-I'+str(out),'-I'+str(app/'src'),'-I'+str(app/'src/audio')]
 subprocess.run(flags+['-Dmain=gameplay_main','-c',str(app/'src/gameplay/main.c'),'-o',str(out/'main.o')],check=True)
 subprocess.run(flags+[str(app/'test/gameplay_lifecycle.c'),str(app/'src/gameplay/game.c'),str(app/'src/original/logical.c'),str(out/'main.o'),'-o',str(out/'test')],check=True)
 subprocess.run([str(out/'test')],check=True)
