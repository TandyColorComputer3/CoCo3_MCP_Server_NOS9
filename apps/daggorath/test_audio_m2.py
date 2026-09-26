#!/usr/bin/env python3
from pathlib import Path
import subprocess,tempfile
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dod-m2-') as out:
    common=['cc','-std=c99','-Wall','-Wextra','-Werror','-I'+str(app.parent/'window-manager/test/compat'),'-I'+str(app/'src'),'-I'+str(app/'src/audio')]
    for name,sources in [('policy',['src/audio/event.c','test/audio_policy.c']),('backend',['src/audio/ssc.c','test/audio_m2.c']),('recipes',['src/audio/ssc.c','test/audio_recipes.c'])]:
        target=str(Path(out)/name)
        subprocess.run(common+[str(app/s) for s in sources]+['-o',target],check=True)
        subprocess.run([target],check=True)
