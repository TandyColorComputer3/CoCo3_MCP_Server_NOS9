#!/usr/bin/env python3
"""Original status-region fixtures plus state ownership/partial-redraw checks."""
import ctypes,json,subprocess,tempfile,unittest
from pathlib import Path
from import_gameplay import generate
APP=Path(__file__).resolve().parent
class Status(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  cls.tmp=tempfile.TemporaryDirectory();out=Path(cls.tmp.name);generate(out)
  subprocess.run(['cc','-shared','-fPIC','-O2','-I'+str(APP/'src'),'-I'+str(out),str(APP/'src/gameplay/game.c'),str(APP/'src/original/logical.c'),'-o',str(out/'game.so')],check=True)
  cls.lib=ctypes.CDLL(str(out/'game.so'))
 @classmethod
 def tearDownClass(cls):cls.tmp.cleanup()
 def setUp(self):
  self.g=ctypes.create_string_buffer(8192);self.f=ctypes.create_string_buffer(bytes([0x5a])*6144,6144);self.lib.game_init(self.g,0)
 def command(self,text):return self.lib.game_command(self.g,text.encode())
 def status(self,phase):
  self.lib.game_render_status(self.g,self.f,phase);return self.f.raw[152*32:160*32]
 def test_cartridge_regions(self):
  fixture=json.loads((APP/'test/fixtures/status-original.json').read_text())
  for name,commands in [('empty',[]),('left',['PULL LEFT TORCH']),('right',['USE LEFT','PULL RIGHT TORCH'])]:
   for cmd in commands:self.assertEqual(self.command(cmd),0)
   for phase in [0,1]:self.assertEqual(self.status(phase).hex(),fixture['regions'][name+'-'+str(phase)],(name,phase))
 def test_only_status_region_changes(self):
  self.status(1);self.assertEqual(self.f.raw[:152*32],bytes([0x5a])*(152*32));self.assertEqual(self.f.raw[160*32:],bytes([0x5a])*(32*32))
 def test_heart_changes_only_two_original_cells(self):
  a=self.status(0);b=self.status(1);changed=[i for i in range(256) if a[i]!=b[i]]
  self.assertTrue(changed);self.assertTrue(all(i%32 in [15,16] and i//32<7 for i in changed));self.assertEqual(a[-32:],bytes([255])*32)
 def test_single_torch_cannot_be_in_both_hands(self):
  self.assertEqual(self.command('PULL RIGHT TORCH'),0);before=self.status(0)
  self.assertEqual(self.command('PULL LEFT TORCH'),2);self.assertEqual(self.status(0),before)
 def test_use_right_returns_to_actual_bag(self):
  empty=self.status(0);self.assertEqual(self.command('PULL RIGHT TORCH'),0);self.assertNotEqual(self.status(0),empty)
  self.assertEqual(self.command('USE RIGHT'),0);self.assertEqual(self.status(0),empty);self.assertEqual(self.command('PULL LEFT TORCH'),0)
 def test_input_edit_preserves_status(self):
  self.status(1);before=self.f.raw;underlay=ctypes.create_string_buffer(before[184*32:191*32]);self.lib.game_render_input(self.f,b'MOVE',underlay)
  self.assertEqual(self.f.raw[:184*32],before[:184*32])
 def test_redraw_then_phase_restore(self):
  self.command('PULL RIGHT TORCH');expected=self.status(1);self.command('TURN RIGHT');self.lib.game_render(self.g,self.f,b'',b'OK');self.assertEqual(self.status(1),expected)
if __name__=='__main__':unittest.main()
