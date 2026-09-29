#!/usr/bin/env python3
"""PGET:PPULL/PSTOW + PARSER token semantics; original OCB IDs/links."""
import ctypes,subprocess,tempfile,unittest,json,hashlib
from pathlib import Path
from import_gameplay import generate
APP=Path(__file__).resolve().parent
class Bag(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  cls.tmp=tempfile.TemporaryDirectory();cls.out=Path(cls.tmp.name);generate(cls.out)
  subprocess.run(['cc','-O2','-shared','-fPIC','-I'+str(APP/'src'),'-I'+str(cls.out),str(APP/'src/gameplay/game.c'),str(APP/'src/original/logical.c'),'-o',str(cls.out/'game.so')],check=True)
  cls.lib=ctypes.CDLL(str(cls.out/'game.so'))
 @classmethod
 def tearDownClass(cls):cls.tmp.cleanup()
 def setUp(self):self.g=ctypes.create_string_buffer(2606);self.lib.game_init(self.g,0)
 def word(self,offset):return int.from_bytes(self.g.raw[offset:offset+2],'little')
 def cmd(self,s,result=0):self.assertEqual(self.lib.game_command(self.g,s.encode()),result)
 def hands(self,l,r):self.assertEqual((self.word(2596),self.word(2604)),(l,r))
 def test_pull_sword_head_and_stow(self):
  self.cmd('PULL LEFT SWORD');self.hands(0xe87,0);self.assertEqual(self.word(2594),0xe95)
  self.cmd('STOW LEFT');self.hands(0,0);self.assertEqual(self.word(2594),0xe87)
  self.assertEqual(self.g.raw[1024+63*14:1024+63*14+2],b'\x0e\x95')
 def test_pull_torch_tail_preserves_removed_link(self):
  self.cmd('PULL RIGHT TORCH');self.hands(0,0xe95);self.assertEqual(self.word(2594),0xe87)
  self.assertEqual(self.g.raw[1024+63*14:1024+63*14+2],b'\x00\x00')
  self.cmd('STOW RIGHT');self.assertEqual(self.word(2594),0xe95)
  self.cmd('PULL LEFT TORCH');self.assertEqual(self.g.raw[1024+64*14:1024+64*14+2],b'\x0e\x87')
 def test_specific_adjective_class(self):
  self.cmd('PULL RIGHT WOODEN SWORD');self.hands(0,0xe87)
  self.cmd('PULL LEFT PINE TORCH');self.hands(0xe95,0xe87)
 def test_unique_abbreviations(self):
  self.cmd('P R W S W',2) # W adjective is WOODEN, but S generic is ambiguous.
  self.cmd('P R W SW');self.hands(0,0xe87);self.cmd('S R');self.hands(0,0)
  self.cmd('PULL L P TORCH');self.hands(0xe95,0)
 def test_ambiguous_generic_and_adjective(self):
  before=self.g.raw
  for s in ['PULL L S','PULL L I SWORD','PULL L PINE SWORD','PULL L TORCHES','PULL L','PULL','STOW']:
   self.cmd(s,2);self.assertEqual(self.g.raw,before)
 def test_empty_or_occupied_hand_atomic(self):
  self.cmd('STOW RIGHT',2);self.cmd('PULL LEFT SWORD');before=self.g.raw
  self.cmd('PULL LEFT TORCH',2);self.assertEqual(self.g.raw,before)
  self.cmd('PULL RIGHT SWORD',2);self.assertEqual(self.g.raw,before)
 def test_source_discards_trailing_tokens(self):
  self.cmd('  PULL  LEFT  SWORD IGNORED');self.hands(0xe87,0)
  self.cmd('STOW LEFT IGNORED');self.hands(0,0)
 def test_nonalpha_as_original_human_spaces(self):
  self.cmd('PULL-LEFT-WOODEN-SWORD');self.hands(0xe87,0)
 def test_wrong_direction_and_absent_objects(self):
  before=self.g.raw
  for s in ['PULL UP TORCH','PULL BACKWARD TORCH','PULL L SHIELD','PULL L ELVISH SWORD','DROP LEFT','GET RIGHT TORCH']:
   self.cmd(s,2);self.assertEqual(self.g.raw,before)
  # PATTK substitutes EMPHND for an empty selected hand. COMDAT gives it
  # sword class, magic offense 0 and physical offense 5; 5>>3 charges zero
  # energy at this power. With no creature at the initial cell, no RNG is used.
  before=self.g.raw;seed=self.g.raw[2576:2579];power=self.word(2588);damage=self.word(2590)
  self.cmd('ATTACK LEFT');self.assertEqual(self.g.raw,before)
  self.assertEqual((self.g.raw[2576:2579],self.word(2588),self.word(2590)),(seed,power,damage))
 def test_torch_selection_extinguished_by_pull(self):
  self.cmd('PULL LEFT TORCH');self.cmd('USE LEFT');self.assertEqual(self.word(2598),0xe95)
  self.cmd('P R PINE TORCH');self.assertEqual(self.word(2598),0);self.assertEqual(self.g.raw[2586],0)
  self.cmd('STOW RIGHT');self.assertEqual(self.word(2598),0)
 def test_cycle_preserves_unrelated_state(self):
  before=self.g.raw
  for _ in range(20):self.cmd('P L SWORD');self.cmd('STOW LEFT')
  self.assertEqual(self.g.raw,before)
 def test_faint_rejects_without_mutation(self):
  self.g[2585]=b'\x01';before=self.g.raw;self.cmd('P L SWORD',3);self.assertEqual(self.g.raw,before)
 def test_original_cartridge_hand_status(self):
  fixture=json.loads((APP/'test/fixtures/bag-original.json').read_text())
  self.assertEqual(hashlib.sha256((self.out/'original.rom').read_bytes()).hexdigest(),fixture['romSha256'])
  for state in fixture['states']:
   self.lib.game_init(self.g,0)
   for cmd in state['commands']:self.cmd(cmd)
   self.hands(state['left'],state['right'])
   actual=b''.join(self.word(o).to_bytes(2,'big') for o in [2594,2596,2604,2598])+self.g.raw[1024+63*14:1024+65*14]
   self.assertEqual(actual.hex(),state['stateHex'])
   frame=ctypes.create_string_buffer(6144)
   self.lib.game_render_status(self.g,frame,state['phase'])
   self.assertEqual(frame.raw[152*32:160*32].hex(),state['statusHex'])
if __name__=='__main__':unittest.main()
