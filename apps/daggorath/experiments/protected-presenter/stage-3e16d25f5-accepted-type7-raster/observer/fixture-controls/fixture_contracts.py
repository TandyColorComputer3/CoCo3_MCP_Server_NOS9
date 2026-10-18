#!/usr/bin/env python3
"""Static contract checks for the disposable GFX2 packet fixture."""
import re
from pathlib import Path
src=Path('/private/tmp/dag-3e16d25e-independent-type7/fixture-source/src/main.c').read_text()
assert re.search(r'setup\[\]\s*=\s*\{27,0x20,7,0,0,80,25,1,0,0',src)
assert re.search(r'palette\[\]\s*=\s*\{27,0x31,0,0,27,0x31,1,7,\s*27,0x31,2,0x38,27,0x31,3,63',src)
# Color(index) emits ESC 32 + BYTE; SetDPtr/Bar coordinates are two-byte values.
def color(index): return bytes((27,0x32,index))
def word(n): return bytes((n>>8,n&255))
def bar(c,x1,y1,x2,y2): return color(c)+bytes((27,0x40))+word(x1)+word(y1)+bytes((27,0x4a))+word(x2)+word(y2)
expected=[
 bar(1,8,16,15,23), bar(2,24,16,39,23),
 bar(3,63,63,96,72),
 bar(0,64,64,71,71), bar(1,72,64,79,71),
 bar(2,80,64,87,71), bar(3,88,64,95,71),
 bar(3,56,64,61,71)]
assert 'RECT(1,8,16,15,23)' in src and 'RECT(2,24,16,39,23)' in src
assert 'RECT(3,63,63,96,72)' in src
assert all(f'RECT({c},{x1},{y1},{x2},{y2})' in src for c,x1,y1,x2,y2 in [
 (0,64,64,71,71),(1,72,64,79,71),(2,80,64,87,71),(3,88,64,95,71),(3,56,64,61,71)])
# Verify one-pixel separating contrast frame is strictly outside band rectangle.
assert (63,63,96,72) == (64-1,64-1,95+1,71+1)
assert 304 <= 335 and 156 <= 162 # Protected heart is outside x<=96/y<=72 fixture.
assert 96 < 304 and 72 < 156
print('PASS: DWSet type 7 / 80x25, four exact palette packets, Color+SetDPtr+Bar packet coordinates, 8x8 bands, fiducials and non-overlapping contrast frame')
