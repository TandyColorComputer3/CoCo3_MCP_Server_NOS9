#!/usr/bin/env python3
"""Source-backed initial map and retained four-row prompt contract."""
import ctypes
import subprocess
import tempfile
import unittest
from pathlib import Path
from import_gameplay import generate

APP=Path(__file__).resolve().parent
SOURCE=Path('/Volumes/SEDONA/Projects/daggorath-reference')

class OpeningM4(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  cls.temp=tempfile.TemporaryDirectory();out=Path(cls.temp.name);generate(out)
  subprocess.run(['cc','-O2','-shared','-fPIC','-I'+str(APP/'src'),'-I'+str(out),
    str(APP/'src/gameplay/game.c'),str(APP/'src/gameplay/opening-map.c'),
    str(APP/'src/gameplay/primary-text.c'),str(APP/'src/original/logical.c'),
    '-o',str(out/'opening.so')],check=True)
  cls.lib=ctypes.CDLL(str(out/'opening.so'))
 @classmethod
 def tearDownClass(cls):cls.temp.cleanup()
 def test_map_source_geometry_and_demo_state(self):
  source=(SOURCE/'MAPPER.ASM').read_text()
  for instruction in ('LDA     #6','LDD     #$0008','LDD     #$1054','LDD     #$2418'):
   self.assertIn(instruction,source)
  game=ctypes.create_string_buffer(2608);frame=ctypes.create_string_buffer(6144)
  self.lib.game_init_demo(game,21);self.lib.opening_render_map(game,frame)
  raw=frame.raw;maze=game.raw[:1024]
  # MAPP10 paints each source maze cell as six bytes at one horizontal byte.
  for row,col in ((0,0),(12,22),(31,31),(11,22)):
   if (row,col)==(12,22):continue  # player marker replaces center bytes
   self.assertEqual(raw[row*192+col],255 if maze[row*32+col]==255 else 0)
  at=12*192+22
  self.assertEqual([raw[at+32*i] for i in range(6)],
                   [0,0x24,0x18,0x18,0x24,0])
 def test_primary_cr_dot_cursor_and_scroll(self):
  source=(SOURCE/'MISC.ASM').read_text();self.assertIn('M$PROM1 FCB     I.CR,I.DOT',source)
  text=ctypes.create_string_buffer(130);frame=ctypes.create_string_buffer(6144)
  self.lib.primary_clear(text);self.lib.primary_prompt(text)
  cells=text.raw[:128]
  self.assertEqual(cells[:32],bytes(32))
  self.assertEqual(cells[32:34],bytes([30,28]))
  self.assertEqual(cells[34:],bytes(94))
  self.lib.primary_render(text,frame)
  self.assertEqual(frame.raw[:160*32],bytes(160*32))
  self.assertEqual(frame.raw[176*32:],bytes(16*32))
  self.lib.primary_clear(text)
  for code in (1,31,2,31,3,31,4,31,5):self.lib.primary_character(text,code)
  self.assertEqual([text.raw[i*32] for i in range(4)],[2,3,4,5])
 def test_prepare_keeps_original_copyright_status(self):
  source=(SOURCE/'MISC.ASM').read_text()
  prepare=source[source.index('PREPAX'):source.index(';;;;;        END',source.index('PREPAX'))]
  self.assertNotIn('CLRSTS',prepare)
  frame=ctypes.create_string_buffer(6144);wizard=ctypes.create_string_buffer(6144)
  self.lib.game_render_prepare(frame)
  prepared=frame.raw
  self.lib.wizard_copyright(frame)
  self.lib.wizard_frame(wizard,255,0)
  self.assertEqual(frame.raw[:152*32],prepared[:152*32])
  self.assertEqual(frame.raw[152*32:160*32],wizard.raw[152*32:160*32])
  self.assertEqual(frame.raw[160*32:],prepared[160*32:])
 def test_copyright_survives_wizard_clear_until_map(self):
  faded=ctypes.create_string_buffer(6144)
  cleared=ctypes.create_string_buffer(6144)
  self.lib.wizard_frame(faded,255,0)
  self.lib.wizard_frame(cleared,0,0)
  status=slice(152*32,160*32)
  self.assertEqual(faded.raw[status],cleared.raw[status])
  self.assertNotEqual(faded.raw[status],bytes(8*32))
  game=ctypes.create_string_buffer(2608)
  mapped=ctypes.create_string_buffer(6144)
  self.lib.game_init_demo(game,21)
  self.lib.opening_render_map(game,mapped)
  self.assertNotEqual(mapped.raw[status],faded.raw[status])

if __name__=='__main__':unittest.main()
