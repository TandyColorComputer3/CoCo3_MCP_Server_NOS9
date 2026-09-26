#!/usr/bin/env python3
from pathlib import Path
import tempfile,subprocess
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dod-life-') as tmp:
 flags=['-std=c99','-Wall','-Wextra','-Werror','-I'+str(app.parent/'window-manager/test/compat'),'-I'+str(app/'src')]
 subprocess.run(['cc',*flags,'-Dmain=wizard_main','-c',str(app/'src/main.c'),'-o',tmp+'/main.o'],check=True)
 subprocess.run(['cc',*flags,str(app/'src/original/logical.c'),str(app/'test/lifecycle.c'),tmp+'/main.o','-o',tmp+'/test'],check=True)
 subprocess.run([tmp+'/test'],check=True)
