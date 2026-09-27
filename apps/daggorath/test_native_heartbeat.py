#!/usr/bin/env python3
"""New semantic client/ownership tests; existing M1/M2/M3 tests stay unchanged."""
from pathlib import Path
import subprocess,tempfile
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dod-native-') as tmp:
 target=str(Path(tmp)/'native')
 subprocess.run(['cc','-std=c99','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-I'+str(app.parent/'window-manager/test/compat'),'-I'+str(app/'src'),'-I'+str(app/'src/audio'),str(app/'test/native_heartbeat.c'),str(app/'src/audio/native_heartbeat.c'),'-o',target],check=True)
 subprocess.run([target],check=True)
