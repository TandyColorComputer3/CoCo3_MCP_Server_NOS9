#!/usr/bin/env python3
"""C4 exact semantic state, map and initial-frame contracts."""
from pathlib import Path
import subprocess,tempfile,json,hashlib
from import_gameplay import generate
APP=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory() as td:
 out=Path(td);generate(out);exe=out/'c4'
 subprocess.run(['cc','-std=c99','-D_GNU_SOURCE','-DDAG_COMPAT_TEST','-Wall','-Wextra','-I'+str(APP/'src'),'-I'+str(APP/'src/compat'),'-I'+str(out),
  str(APP/'test/compat_c4.c'),str(APP/'src/compat/c4.c'),str(APP/'src/gameplay/game.c'),str(APP/'src/gameplay/opening-map.c'),str(APP/'src/original/logical.c'),'-o',str(exe)],check=True)
 generated=out/'oracle';generated.mkdir();subprocess.run([str(exe),str(generated)],check=True)
 fixture=APP/'test/fixtures/compat-c4';manifest=json.loads((fixture/'manifest.json').read_text())
 for name,record in manifest['files'].items():
  data=(generated/name).read_bytes();assert data==(fixture/name).read_bytes(),name
  assert hashlib.sha256(data).hexdigest()==record['sha256'] and len(data)==record['bytes']
 a=out/'a';b=out/'b';subprocess.run(['python3',str(APP/'build_compat.py'),'--out',str(a)],check=True,stdout=subprocess.DEVNULL);subprocess.run(['python3',str(APP/'build_compat.py'),'--out',str(b)],check=True,stdout=subprocess.DEVNULL)
 assert (a/'dodcinit').read_bytes()==(b/'dodcinit').read_bytes()
 build=json.loads((a/'build.json').read_text());ident=build['initial']['ident'];size=(a/'dodcinit').stat().st_size;data=int(ident.split('Data size  : $',1)[1].split()[0],16)
 assert size<=16384 and data<=16384
 print(f'compat C4 exact state/map/frame, reproducibility and Architecture-B gates passed ({size} program, {data} data)')
