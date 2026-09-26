#!/usr/bin/env python3
"""Reproduce the disposable SSC probe, without accessing guest media."""
from pathlib import Path
import hashlib, json, subprocess, sys
root = Path(__file__).resolve().parents[2]
out = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else root / 'MCP/work/audio-ssc/build'
out.mkdir(parents=True, exist_ok=True)
sources = ['probes/speech-sound/ssprobe.c', 'apps/daggorath/src/os9.c',
           'probes/speech-sound/module.asm']
command = ['cmoc', '--os9', '-O0', '--intermediate', '--intdir=' + str(out),
           '--add-os9-stack-space=1536', '-Iapps/daggorath/src',
           '-o', str(out / 'ssprobe'), *sources]
subprocess.run(command, cwd=root, check=True)
def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()
record = {'command': command, 'cwd': str(root), 'sources': {
    p: digest(root / p) for p in sources + ['apps/daggorath/src/platform.h']},
    'artifact': str(out / 'ssprobe'), 'sha256': digest(out / 'ssprobe'),
    'versions': {tool: subprocess.check_output([tool, '--version'], text=True).strip()
                 for tool in ['cmoc', 'lwasm', 'lwlink']}}
(out / 'build.json').write_text(json.dumps(record, indent=2) + '\n')
print(json.dumps(record, indent=2))
