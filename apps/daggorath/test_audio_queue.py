#!/usr/bin/env python3
from pathlib import Path
import subprocess,tempfile
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dod-audio-queue-') as tmp:
 exe=Path(tmp)/'queue'
 subprocess.run(['cc','-std=c99','-Wall','-Wextra','-Werror',
  '-I'+str(app.parent/'window-manager/test/compat'),'-I'+str(app/'src'),
  str(app/'src/audio/queue.c'),str(app/'test/audio_queue.c'),'-o',str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
