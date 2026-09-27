from pathlib import Path
import json
from PIL import Image,ImageDraw
w=Path('MCP/work/gameplay-m2');a=Path('docs/apps/assets/daggorath-gameplay-m2');records=json.loads((w/'normal-states.json').read_text())
out=Image.new('RGB',(640,288),'#202020');d=ImageDraw.Draw(out)
for row,(name,label) in enumerate([('empty','initial-dark'),('left','left-hand'),('right','right-hand')]):
 phase=next(r['phase'] for r in records if r.get('frame')==label)
 raw=(w/f'original/{name}-{255 if phase else 0}.bin').read_bytes()[152*32:160*32];bits=[]
 for b in raw:
  for n in range(7,-1,-1):bits.extend([255 if b&(1<<n) else 0]*2)
 reference=Image.frombytes('L',(512,8),bytes(bits));port=Image.open(w/f'normal-{label}.png').convert('L').crop((64,178,576,186))
 assert port.point(lambda p:255 if p>128 else 0).tobytes()==reference.tobytes(),name
 d.text((16,row*96+4),f'{name}, phase {phase}: cartridge RAM / actual NitrOS-9 snapshot',fill='white')
 out.paste(reference.resize((512,24),Image.Resampling.NEAREST),(64,row*96+24));out.paste(port.resize((512,24),Image.Resampling.NEAREST),(64,row*96+56))
out.save(a/'status-comparison.png');print('3 cartridge RAM / actual MCP screenshot pairs match all 4096 pixels')
