#!/usr/bin/env python3
"""Source-semantic tests; live/backend M3 verification is not yet included."""
from pathlib import Path
import subprocess,tempfile
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dod-m3-') as out:
    target=str(Path(out)/'heartbeat')
    subprocess.run(['cc','-std=c99','-Wall','-Wextra','-Werror',
        '-I'+str(app.parent/'window-manager/test/compat'),
        '-I'+str(app/'src'),'-I'+str(app/'src/audio'),
        str(app/'test/heartbeat.c'),'-o',target],check=True)
    subprocess.run([target],check=True)
