#!/usr/bin/env python3
"""Long mapped EXAMINE rendering services resident progress safely."""
from pathlib import Path
import re,subprocess,tempfile
from import_gameplay import generate

app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dod-overlay-progress-') as tmp:
 out=Path(tmp);generate(out)
 flags=['cc','-std=c99','-Wall','-Wextra','-Werror','-O2',
        '-I'+str(app/'src'),'-I'+str(out)]
 sources=[app/'test/overlay_examine_progress.c',app/'src/gameplay/command-overlay.c',
          app/'src/gameplay/game.c',app/'src/original/logical.c']
 subprocess.run(flags+list(map(str,sources))+['-o',str(out/'progress')],check=True)
 subprocess.run([str(out/'progress')],check=True)
 subprocess.run(['python3',str(app/'build_gameplay.py'),'--out',str(out/'build')],check=True)
 linked=(out/'build'/'dodgame').read_bytes();symbols={}
 for line in (out/'build'/'dodgame.map').read_text().splitlines():
  match=re.match(r'Symbol: (\S+) .* = ([0-9A-F]+)$',line)
  if match:symbols[match.group(1)]=int(match.group(2),16)
 gateway=symbols['_overlay_progress_gateway'];target=symbols['_overlay_progress_resident']
 instruction=gateway+5
 assert linked[instruction:instruction+2]==bytes((0x30,0x8d))
 displacement=int.from_bytes(linked[instruction+2:instruction+4],'big',signed=True)
 assert 0xA000+instruction+4+displacement==0xA000+target
 assert linked[target:target+3]==bytes((0x34,0x40,0x17))
print('overlay EXAMINE resident progress gateway checks passed')
