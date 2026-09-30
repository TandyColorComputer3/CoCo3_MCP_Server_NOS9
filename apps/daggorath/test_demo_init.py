#!/usr/bin/env python3
"""Source-capture checks for ONCE:GAME20 level-two DEMDAT initialization."""
import ctypes,hashlib,subprocess,tempfile,unittest
from pathlib import Path
from import_gameplay import generate

APP=Path(__file__).resolve().parent
OBASE=0x0b15

class Game(ctypes.Structure):
 _fields_=[('maze',ctypes.c_ubyte*1024),('objects',(ctypes.c_ubyte*14)*72),
          ('creatures',(ctypes.c_ubyte*17)*32),('seed',ctypes.c_ubyte*3),
          ('row',ctypes.c_ubyte),('col',ctypes.c_ubyte),('dir',ctypes.c_ubyte),
          ('count',ctypes.c_ubyte),('creatureCount',ctypes.c_ubyte),
          ('rate',ctypes.c_ubyte),('faint',ctypes.c_ubyte),('lit',ctypes.c_ubyte),
          ('dead',ctypes.c_ubyte),('power',ctypes.c_uint16),('damage',ctypes.c_uint16),
          ('weight',ctypes.c_uint16),('bag',ctypes.c_uint16),('hand',ctypes.c_uint16),
          ('torch',ctypes.c_uint16),('recovery',ctypes.c_uint16),('burn',ctypes.c_uint16),
          ('rightHand',ctypes.c_uint16),('level',ctypes.c_ubyte),
          ('levelPadding',ctypes.c_ubyte)]

class DemoInit(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  cls.temp=tempfile.TemporaryDirectory();out=Path(cls.temp.name);generate(out)
  subprocess.run(['cc','-O2','-shared','-fPIC','-I'+str(APP/'src'),'-I'+str(out),
                  str(APP/'src/gameplay/game.c'),str(APP/'src/original/logical.c'),
                  '-o',str(out/'game.so')],check=True)
  cls.lib=ctypes.CDLL(str(out/'game.so'))
  cls.lib.game_init.argtypes=[ctypes.POINTER(Game),ctypes.c_ubyte]
  cls.lib.game_init_demo.argtypes=[ctypes.POINTER(Game),ctypes.c_ubyte]
  cls.lib.game_command.argtypes=[ctypes.POINTER(Game),ctypes.c_char_p]
  cls.lib.game_command.restype=ctypes.c_ubyte
 @classmethod
 def tearDownClass(cls):cls.temp.cleanup()
 def setUp(self):self.g=Game();self.lib.game_init_demo(ctypes.byref(self.g),21)
 def test_port_abi_and_levels(self):
  normal=Game();self.lib.game_init(ctypes.byref(normal),0)
  self.assertEqual(ctypes.sizeof(Game),2608)
  self.assertEqual(normal.level,0);self.assertEqual(self.g.level,2)
 def test_exact_game40_source_tables(self):
  # Original cartridge GAME40 capture made after NEWLVL and GAME30, before
  # AUTTAB. These hashes cover only source-derived MAZLND/OCBLND/CCBLND, not
  # the appended port-owned Game.level byte.
  self.assertEqual(hashlib.sha256(bytes(self.g.maze)).hexdigest(),
                   '391f323f5a68ca330a2a7c0afa343121794c3d2e1f3bb4af9c66190b64cd90c9')
  self.assertEqual(hashlib.sha256(bytes(self.g.objects)).hexdigest(),
                   'dd160d070725056c3e2ae48378fbfe06e7e158cc0ff0a8217aaa8dd086b35526')
  self.assertEqual(hashlib.sha256(bytes(self.g.creatures)).hexdigest(),
                   '9b96c8c5c71869c3b90d8b26b44448ae8c652f9b9c018881b6d916dafdcce9ee')
  self.assertEqual(bytes(self.g.seed),bytes.fromhex('756519'))
  self.assertEqual((self.g.row,self.g.col,self.g.dir,self.g.power,self.g.damage),
                   (12,22,0,6048,0))
  self.assertEqual((self.g.count,self.g.creatureCount),(66,23))
 def test_demo_inventory_uses_normal_commands_and_drop_uses_level(self):
  # COMDAT:DEMDAT puts iron sword, pine torch and leather shield in the bag.
  self.assertEqual(self.lib.game_command(ctypes.byref(self.g),b'PULL LEFT SHIELD'),0)
  self.assertEqual(self.g.hand,OBASE+65*14)
  self.assertEqual(self.lib.game_command(ctypes.byref(self.g),b'DROP LEFT'),0)
  self.assertEqual(self.g.objects[65][4],2)

if __name__=='__main__':unittest.main()
