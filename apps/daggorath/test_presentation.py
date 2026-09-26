#!/usr/bin/env python3
from pathlib import Path
import tempfile,subprocess
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dod-present-') as tmp:
 subprocess.run(['cc','-std=c99','-Wall','-Wextra','-Werror','-I'+str(app.parent/'window-manager/test/compat'),'-I'+str(app/'src'),str(app/'src/presentation.c'),str(app/'test/presentation.c'),'-o',tmp+'/test'],check=True)
 subprocess.run([tmp+'/test'],check=True)
