#!/usr/bin/env python3
"""Artifact gates for the independent one-block source scheduler."""
from pathlib import Path
import tempfile,subprocess,json
app=Path(__file__).resolve().parent
with tempfile.TemporaryDirectory(prefix='dodsched-build-test-') as tmp:
 out=Path(tmp)/'build'
 subprocess.run(['python3',str(app/'build_gameplay.py'),'--out',str(out)],check=True)
 record=json.loads((out/'build.json').read_text())
 assert (out/'dodgame').stat().st_size<=30720
 assert (out/'dodsched').stat().st_size<=8192
 assert (out/'dodcmd').stat().st_size<=8192
 assert 'Subr mod' in record['scheduler']['ident'] and '$21' in record['scheduler']['ident']
 assert record['scheduler']['sha256']
print('dodsched one-block build and resident-size gates passed')
