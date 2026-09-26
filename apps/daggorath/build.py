#!/usr/bin/env python3
"""Build only on the host; preserve exact inputs, tools, commands and output identity."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess

app = Path(__file__).resolve().parent
parser = argparse.ArgumentParser()
parser.add_argument('--out', required=True)
for tool in ('cmoc', 'lwasm', 'lwlink', 'os9'):
    parser.add_argument('--' + tool, default=tool)
args = parser.parse_args()
out = Path(args.out).resolve()
out.mkdir(parents=True, exist_ok=True)

def fingerprint(p):
    p = Path(p).resolve()
    return {'path': str(p), 'sha256': hashlib.sha256(p.read_bytes()).hexdigest(), 'size': p.stat().st_size}

def execute(command):
    p = subprocess.run(command, cwd=out, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    if p.returncode:
        raise RuntimeError(f'{command!r}: exit {p.returncode}\n{p.stdout}')
    return p.stdout

tools = {}
for name in ('cmoc', 'lwasm', 'lwlink', 'os9'):
    resolved = shutil.which(getattr(args, name))
    if not resolved:
        raise RuntimeError(f'{name}: executable not found')
    tools[name] = dict(fingerprint(resolved), version=execute([resolved] + ([] if name == 'os9' else ['--version'])))
tools['host_cc'] = dict(fingerprint(shutil.which('cc')), version=execute(['cc', '--version']))
from prepare_frames import generate
cache = generate(app, out)
sources = [app/'src'/n for n in ('main.c', 'presentation.c', 'playback.c', 'original/logical.c', 'os9.c', 'window-path.c', 'module.asm')]
command = [tools['cmoc']['path'], '--os9', '-O0', '--intermediate', '--verbose',
           '--add-os9-stack-space=1536', '--intdir=' + str(out),
           '--lwasm=' + tools['lwasm']['path'], '--lwlink=' + tools['lwlink']['path'],
           '-I' + str(app/'src'), '-I' + str(out), '-o', 'dodwiz'] + [str(p) for p in sources]
log = execute(command)
(out/'build.log').write_text(log)
ident = execute([tools['os9']['path'], 'ident', str(out/'dodwiz')])
if '(Good)' not in ident:
    raise RuntimeError('ToolShed did not report a valid CRC')
# Record actual archives selected by the compiler's verbose link command.
# CMOC's installed share directory is discoverable from its emitted -L argument.
import re
library_dirs = re.findall(r'-L([^\s]+)', log)
libraries = []
for directory in library_dirs:
    for name in ('libcmoc-crt-os9.a', 'libcmoc-std-os9.a'):
        p = Path(directory)/name
        if p.exists(): libraries.append(fingerprint(p))
headers = [cache, app/'prepare_frames.py'] + list((app/'src').rglob('*.h')) + [app/'src/original/provenance.json']
cmoc_include = re.search(r"-I'([^']*/cmoc/include)'", log)
if cmoc_include:
    for name in ('cmoc.h', 'stdarg.h'):
        headers.append(Path(cmoc_include[1])/name)
record = {'sources': [fingerprint(p) for p in sources + headers + [Path(__file__)]],
          'tools': tools, 'libraries': libraries, 'command': command, 'cwd': str(out),
          'output': fingerprint(out/'dodwiz'), 'ident': ident,
          'upstreamApiReference': 'nitros9-reference f470fa52eb172b59b22c1b722074998cb42de9b1'}
(out/'build.json').write_text(json.dumps(record, indent=2) + '\n')
print(ident)
print(out/'build.json')
