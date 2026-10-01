#!/usr/bin/env python3
"""Default-stack-check resident callback boundary build and contract gates."""
from pathlib import Path
import json,re,subprocess,tempfile
from import_gameplay import generate

app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dodsched-callback-abi-') as tmp:
 out=Path(tmp);generate(out)
 subprocess.run(['python3',str(app/'build_gameplay.py'),'--out',str(out/'build')],check=True)
 record=json.loads((out/'build'/'build.json').read_text())
 resident=' '.join(record['command'])
 scheduler=' '.join(record['scheduler']['command'])
 assert '--function-stack=0' not in resident
 assert '--function-stack=0' in scheduler
 assert any(str(p).endswith('scheduler-callback-gateway.asm') for p in record['sources'])
 mainasm=(out/'build'/'main.s').read_text()
 assert '_scheduler_task_resident' in mainasm and '_stkcheck' in mainasm
 gateway=(out/'build'/'scheduler-callback-gateway.lst').read_text()
 # Each mapped-scheduler -> resident callback must derive its target
 # position-independently. A direct JMP to an imported resident symbol
 # encodes the link-relative address and is invalid when the Level II
 # process maps its program above $0000.
 for name in ('_scheduler_task_resident','_scheduler_present_resident',
              '_scheduler_progress_resident'):
  assert f'leax    {name},pcr' in gateway
 assert 'jmp     _scheduler_task_resident' not in gateway
 assert 'jmp     _scheduler_present_resident' not in gateway
 assert 'jmp     _scheduler_progress_resident' not in gateway
 # The resident program may be mapped at a nonzero Level II address. Verify
 # the final linked doddemo code, not merely the relocatable listing: every
 # gateway's PC-relative displacement must resolve to its resident callback
 # when the module is deliberately placed at $A000.
 subprocess.run(['python3',str(app/'build_demo.py'),'--out',str(out/'demo')],check=True)
 linked=(out/'demo'/'doddemo').read_bytes()
 symbols={}
 for line in (out/'demo'/'doddemo.map').read_text().splitlines():
  m=re.match(r'Symbol: (\S+) .* = ([0-9A-F]+)$',line)
  if m:symbols[m.group(1)]=int(m.group(2),16)
 for gateway,target,delta in (
   ('_scheduler_task_gateway','_scheduler_task_resident',5),
   ('_scheduler_present_gateway','_scheduler_present_resident',9),
   ('_scheduler_progress_gateway','_scheduler_progress_resident',9)):
  instruction=symbols[gateway]+delta
  assert linked[instruction:instruction+2]==bytes((0x30,0x8d))
  displacement=int.from_bytes(linked[instruction+2:instruction+4],'big',signed=True)
  assert 0xA000+instruction+4+displacement==0xA000+symbols[target]
 flags=['cc','-std=c99','-Wall','-Wextra','-Werror','-O2','-I'+str(app/'src'),'-I'+str(out)]
 sources=[app/'test/scheduler_callback_contract.c',app/'src/gameplay/scheduler.c',
          app/'src/gameplay/game.c',app/'src/gameplay/creature.c',app/'src/original/logical.c']
 subprocess.run(flags+list(map(str,sources))+['-o',str(out/'callback-contract')],check=True)
 subprocess.run([str(out/'callback-contract')],check=True)
print('dodsched default-stack resident callback ABI build and contract checks passed')
