#!/usr/bin/env python3
"""Source-state and cartridge-fixture tests; no emulator or media writes."""
import ctypes,hashlib,json,subprocess,tempfile,unittest
from pathlib import Path
from import_gameplay import generate
APP=Path(__file__).resolve().parent
class Gameplay(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  cls.temp=tempfile.TemporaryDirectory();cls.out=Path(cls.temp.name);generate(cls.out)
  flags=['cc','-O2','-I'+str(APP/'src'),'-I'+str(cls.out)]
  src=[str(APP/'src/gameplay/game.c'),str(APP/'src/original/logical.c')]
  subprocess.run(flags+['-shared','-fPIC']+src+['-o',str(cls.out/'game.so')],check=True)
  subprocess.run(flags+[str(APP/'test/gameplay.c')]+src+['-o',str(cls.out/'check')],check=True)
  cls.lib=ctypes.CDLL(str(cls.out/'game.so'));cls.state=ctypes.create_string_buffer(8192)
  cls.lib.game_init(cls.state,2);cls.fixture=json.loads((APP/'test/fixtures/gameplay-original.json').read_text())
 @classmethod
 def tearDownClass(cls):cls.temp.cleanup()
 def test_navigation_torch_recovery(self):subprocess.run([str(self.out/'check')],check=True)
 def test_original_rom_identity(self):self.assertEqual(hashlib.sha256((self.out/'original.rom').read_bytes()).hexdigest(),self.fixture['romSha256'])
 def test_original_maze(self):self.assertEqual(self.state.raw[:1024].hex(),self.fixture['tables']['MAZLND'])
 def test_original_objects(self):self.assertEqual(self.state.raw[1024:2032].hex(),self.fixture['tables']['OCBLND'])
 def test_original_creatures(self):self.assertEqual(self.state.raw[2032:2576].hex(),self.fixture['tables']['CCBLND'])
 def test_import_reproducible(self):
  before=(self.out/'game_data.h').read_bytes();generate(self.out);self.assertEqual(before,(self.out/'game_data.h').read_bytes())
if __name__=='__main__':unittest.main()
