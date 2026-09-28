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
 def test_original_i_bar_cursor_is_present_after_input(self):
  g=ctypes.create_string_buffer(8192);self.lib.game_init(g,0)
  with_cursor=ctypes.create_string_buffer(6144);without=ctypes.create_string_buffer(6144)
  self.lib.game_render(g,with_cursor,b'A',b'OK');self.lib.game_render(g,without,b'',b'OK')
  cell=184*32+1
  self.assertNotEqual(with_cursor.raw[cell:cell+7*32],without.raw[cell:cell+7*32])
 def test_progress_heart_never_changes_completed_dungeon_frame(self):
  g=ctypes.create_string_buffer(8192);self.lib.game_init(g,0)
  for command in [b'PULL LEFT TORCH',b'USE LEFT']:
   self.assertEqual(self.lib.game_command(g,command),0)
  expected=ctypes.create_string_buffer(6144);observed=ctypes.create_string_buffer(6144)
  self.lib.game_render(g,expected,b'',b'OK')
  callback_type=ctypes.CFUNCTYPE(ctypes.c_ubyte,ctypes.c_void_p,ctypes.c_void_p,ctypes.c_void_p)
  calls=[]
  def progress(_game,partial,_context):
   calls.append(1)
   self.lib.game_render_heart(ctypes.c_void_p(partial),ctypes.c_ubyte(len(calls)&1))
   return 0
  callback=callback_type(progress)
  self.assertEqual(self.lib.game_render_with_progress(g,observed,b'',b'OK',callback,None),0)
  self.assertGreater(len(calls),0)
  self.assertEqual(observed.raw,expected.raw)
if __name__=='__main__':unittest.main()
