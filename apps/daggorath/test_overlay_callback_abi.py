#!/usr/bin/env python3
"""Mapped dodcmd -> stack-checked resident CMOC callback ABI gates."""
from pathlib import Path
import json,re,subprocess,tempfile
from import_gameplay import generate

app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dod-overlay-callback-abi-') as tmp:
 out=Path(tmp);generate(out)
 subprocess.run(['python3',str(app/'build_gameplay.py'),'--out',str(out/'build')],check=True)
 record=json.loads((out/'build'/'build.json').read_text())
 assert '--function-stack=0' not in ' '.join(record['command'])
 assert any(str(p).endswith('overlay-callback-gateway.asm') for p in record['sources'])
 gateway=(out/'build'/'overlay-callback-gateway.lst').read_text()
 for name in ('_overlay_health_resident','_overlay_object_name_resident',
             '_overlay_render_status_resident'):
  assert f'leax    {name},pcr' in gateway
  assert f'jmp     {name}' not in gateway
 subprocess.run(['python3',str(app/'build_demo.py'),'--out',str(out/'demo')],check=True)
 linked=(out/'demo'/'doddemo').read_bytes();symbols={}
 for line in (out/'demo'/'doddemo.map').read_text().splitlines():
  m=re.match(r'Symbol: (\S+) .* = ([0-9A-F]+)$',line)
  if m:symbols[m.group(1)]=int(m.group(2),16)
 # Each gateway begins LDX 2,S; LDY ,X; LEAX target,PCR.  Resolve the final
 # linked displacement at a deliberately nonzero Level II module base.
 for gateway_name,target in (
   ('_overlay_health_gateway','_overlay_health_resident'),
   ('_overlay_object_name_gateway','_overlay_object_name_resident'),
   ('_overlay_render_status_gateway','_overlay_render_status_resident')):
  instruction=symbols[gateway_name]+5
  assert linked[instruction:instruction+2]==bytes((0x30,0x8d))
  displacement=int.from_bytes(linked[instruction+2:instruction+4],'big',signed=True)
  assert 0xA000+instruction+4+displacement==0xA000+symbols[target]
 # The resident command contains no --function-stack=0 exemption.  Each
 # gateway target must consequently retain the ordinary CMOC prologue
 # (PSHS U; LBSR _stkcheck) in the final linked image.
 for name in ('_overlay_health_resident','_overlay_object_name_resident',
              '_overlay_render_status_resident'):
  at=symbols[name]
  assert linked[at:at+3]==bytes((0x34,0x40,0x17))
 flags=['cc','-std=c99','-Wall','-Wextra','-Werror','-O2',
        '-I'+str(app/'src'),'-I'+str(out)]
 sources=[app/'test/overlay_callback_contract.c',app/'src/gameplay/command-overlay.c',
          app/'src/gameplay/game.c',app/'src/original/logical.c']
 subprocess.run(flags+list(map(str,sources))+['-o',str(out/'callback-contract')],check=True)
 subprocess.run([str(out/'callback-contract')],check=True)
print('dodcmd resident callback gateway ABI checks passed')
