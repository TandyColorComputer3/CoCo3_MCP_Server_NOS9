#!/usr/bin/env python3
"""Exact full-render equivalence for the approved input-row reuse path."""
import ctypes,subprocess,tempfile,unittest
from pathlib import Path
from import_gameplay import generate
APP=Path(__file__).resolve().parent
class InputRender(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  cls.temp=tempfile.TemporaryDirectory();out=Path(cls.temp.name);generate(out)
  subprocess.run(['cc','-O2','-shared','-fPIC','-I'+str(APP/'src'),'-I'+str(out),str(APP/'src/gameplay/game.c'),str(APP/'src/original/logical.c'),'-o',str(out/'game.so')],check=True)
  cls.lib=ctypes.CDLL(str(out/'game.so'))
 @classmethod
 def tearDownClass(cls):cls.temp.cleanup()
 def compare(self,commands):
  g=ctypes.create_string_buffer(8192);self.lib.game_init(g,0)
  for command in commands:self.assertEqual(self.lib.game_command(g,command.encode()),0)
  cached=ctypes.create_string_buffer(6144);fresh=ctypes.create_string_buffer(6144)
  self.lib.game_render(g,cached,b'',b'OK')
  underlay=ctypes.create_string_buffer(cached.raw[184*32:191*32],224)
  for line in [b'M',b'MO',b'MOV',b'MOVE',b'MOVEE',b'MOVE',b'',b'TORCH',b'EEEE',b'MMMM',b'X'*31,b'']:
   self.lib.game_render_input(cached,line,underlay)
   self.lib.game_render(g,fresh,line,b'OK')
   self.assertEqual(cached.raw,fresh.raw,line)
 def test_dark(self):self.compare([])
 def test_lit(self):self.compare(['PULL LEFT TORCH','USE LEFT'])
 def test_moved(self):self.compare(['PULL LEFT TORCH','USE LEFT','MOVE'])
 def test_turned(self):self.compare(['PULL LEFT TORCH','USE LEFT','MOVE','TURN RIGHT'])
if __name__=='__main__':unittest.main()
