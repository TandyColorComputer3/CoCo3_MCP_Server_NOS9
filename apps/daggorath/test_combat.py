#!/usr/bin/env python3
"""Combat M1 source-state tests; no emulator or media writes."""
from pathlib import Path
import subprocess,tempfile
from import_gameplay import generate
app=Path(__file__).resolve().parent
main=(app/'src/gameplay/main.c').read_text()
combat=main.index('result=game_command_combat')
heartbeat=main.index('native_heartbeat_rate',combat)
optional=main.index('audio_queue_admit',heartbeat)
assert combat < heartbeat < optional
with tempfile.TemporaryDirectory(prefix='dod-combat-') as tmp:
 out=Path(tmp);generate(out)
 subprocess.run(['cc','-std=c99','-Wall','-Wextra','-Werror','-O2',
  '-I'+str(app/'src'),'-I'+str(out),str(app/'test/gameplay_combat.c'),
  str(app/'src/gameplay/game.c'),str(app/'src/original/logical.c'),
  '-o',str(out/'check')],check=True)
 subprocess.run([str(out/'check')],check=True)
