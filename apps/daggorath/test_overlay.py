#!/usr/bin/env python3
"""Dynamic command-overlay ABI/build checks; no emulator or guest media."""
from pathlib import Path
import subprocess,tempfile,json
from import_gameplay import generate
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dod-overlay-test-') as tmp:
 out=Path(tmp);generate(out)
 flags=['cc','-std=c99','-Wall','-Wextra','-Werror','-O2','-I'+str(app/'src'),'-I'+str(out)]
 subprocess.run(flags+[str(app/'test/overlay_execution.c'),str(app/'src/gameplay/command-overlay.c'),str(app/'src/gameplay/game.c'),str(app/'src/original/logical.c'),'-o',str(out/'execute')],check=True)
 subprocess.run([str(out/'execute')],check=True)
 subprocess.run(flags+[str(app/'test/overlay_host.c'),str(app/'src/gameplay/overlay-host.c'),'-o',str(out/'host')],check=True)
 subprocess.run([str(out/'host')],check=True)
 subprocess.run(['python3',str(app/'build_gameplay.py'),'--out',str(out/'build')],check=True)
 record=json.loads((out/'build/build.json').read_text())
 assert (out/'build/dodgame').stat().st_size<=32768
 assert (out/'build/dodcmd').stat().st_size<=8192
 assert 'Subr mod' in record['overlay']['ident'] and '$21' in record['overlay']['ident']
 host=(app/'src/gameplay/overlay-host.c').read_text();shim=(app/'src/gameplay/overlay-host-shim.asm').read_text()
 assert 'F$NMLoad' in host and 'os9     $22' in shim and 'os9     $00' in shim and 'os9     $02' in shim and 'os9     $1d' in shim
 assert 'leax    dodcmd_path,pcr' in shim and 'fcs     "/d1/dodcmd"' in shim
 print('overlay build/map/lifecycle checks passed')
