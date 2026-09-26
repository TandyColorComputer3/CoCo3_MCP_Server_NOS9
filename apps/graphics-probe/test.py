#!/usr/bin/env python3
import subprocess,tempfile
from pathlib import Path
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='gfx-test-') as out:
 flags=['-std=c99','-Wall','-Wextra','-Werror','-I'+str(app.parent/'window-manager/test/compat'),'-I'+str(app/'src')]
 subprocess.run(['cc',*flags,'-Dmain=probe_main','-c',str(app/'src/main.c'),'-o',out+'/main.o'],check=True)
 subprocess.run(['cc',*flags,str(app/'test/lifecycle.c'),out+'/main.o','-o',out+'/test'],check=True)
 subprocess.run([out+'/test'],check=True)
