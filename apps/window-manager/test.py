#!/usr/bin/env python3
"""Exercise application lifecycle in host C with a fault-injecting OS boundary."""
import os
from pathlib import Path
import subprocess
import tempfile
app=Path(__file__).resolve().parent
cc=os.environ.get('CC','cc')
with tempfile.TemporaryDirectory(prefix='wm-test-') as out:
    flags=['-std=c99','-Wall','-Wextra','-Werror','-I'+str(app/'test/compat'),'-I'+str(app/'src')]
    subprocess.run([cc,*flags,'-Dmain=wm_main','-c',str(app/'src/main.c'),'-o',out+'/main.o'],check=True)
    extra=[str(app/'src'/n) for n in ('profile.c','persistence.c','m2.c','font-catalog.c')]
    subprocess.run([cc,*flags,*extra,str(app/'test/file-stubs.c'),out+'/main.o',str(app/'src/window.c'),str(app/'src/ui.c'),str(app/'test/lifecycle.c'),'-o',out+'/lifecycle'],check=True)
    subprocess.run([out+'/lifecycle'],check=True)

    subprocess.run([cc,*flags,*extra,str(app/'src/window.c'),str(app/'src/ui.c'),str(app/'test/profiles.c'),'-o',out+'/profiles'],check=True)
    subprocess.run([out+'/profiles'],check=True)
