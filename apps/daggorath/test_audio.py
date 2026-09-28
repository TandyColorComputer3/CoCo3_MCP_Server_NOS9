#!/usr/bin/env python3
from pathlib import Path
import subprocess,tempfile
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dod-audio-') as out:
 cmd=['cc','-std=c99','-Wall','-Wextra','-Werror','-I'+str(app.parent/'window-manager/test/compat'),'-I'+str(app/'src'),'-I'+str(app/'src/audio')]
 subprocess.run(cmd+[str(app/'src/audio/event.c'),str(app/'src/audio/ssc.c'),str(app/'src/audio/ssc_catalog.c'),str(app/'test/audio.c'),'-o',out+'/audio'],check=True)
 subprocess.run([out+'/audio'],check=True)
with tempfile.TemporaryDirectory(prefix='dod-client-') as out:
 subprocess.run(cmd+[str(app/'src/audio/event.c'),str(app/'src/audio/client.c'),str(app/'test/audio_client.c'),'-o',out+'/client'],check=True)
 subprocess.run([out+'/client'],check=True)
with tempfile.TemporaryDirectory(prefix='dod-service-') as out:
 subprocess.run(cmd+['-Dmain=audio_service_main','-c',str(app/'src/audio/service.c'),'-o',out+'/service.o'],check=True)
 subprocess.run(cmd+[str(app/'src/audio/event.c'),str(app/'test/audio_service.c'),out+'/service.o','-o',out+'/service'],check=True)
 subprocess.run([out+'/service'],check=True)
