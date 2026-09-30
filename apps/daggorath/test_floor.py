#!/usr/bin/env python3
"""PGET/PDROP, COMCRE:OFIND and VIEW52 source-derived invariants."""
import ctypes,subprocess,tempfile,unittest,json,hashlib
from pathlib import Path
from import_gameplay import generate
APP=Path(__file__).resolve().parent
class Floor(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  cls.tmp=tempfile.TemporaryDirectory();cls.out=Path(cls.tmp.name);generate(cls.out)
  subprocess.run(['cc','-O2','-shared','-fPIC','-I'+str(APP/'src'),'-I'+str(cls.out),str(APP/'src/gameplay/game.c'),str(APP/'src/original/logical.c'),'-o',str(cls.out/'game.so')],check=True)
  cls.lib=ctypes.CDLL(str(cls.out/'game.so'));cls.lib.game_message.restype=ctypes.c_char_p
 @classmethod
 def tearDownClass(cls):cls.tmp.cleanup()
 def setUp(self):self.g=ctypes.create_string_buffer(2608);self.lib.game_init(self.g,0)
 def word(self,o):return int.from_bytes(self.g.raw[o:o+2],'little')
 def cmd(self,s,status=0):self.assertEqual(self.lib.game_command(self.g,s.encode()),status)
 def obj(self,n):return self.g.raw[1024+n*14:1024+(n+1)*14]
 def setobj(self,n,b):ctypes.memmove(ctypes.addressof(self.g)+1024+n*14,bytes(b),14)
 def frame(self):
  f=ctypes.create_string_buffer(6144);self.lib.game_render(self.g,f,b'',b'');return f.raw
 def light(self):self.cmd('P L TORCH');self.cmd('USE LEFT')
 def test_drop_location_owner_weight_links(self):
  self.cmd('P R SWORD');before=self.obj(63);self.cmd('D R')
  expected=bytearray(before);expected[2:6]=bytes([16,11,0,0])
  self.assertEqual(self.obj(63),expected);self.assertEqual(self.word(2604),0);self.assertEqual(self.word(2592),10)
  self.assertEqual(self.word(2594),0xe95);self.assertEqual(self.word(2590),0)
 def test_get_owner_weight_preserves_location_and_link(self):
  self.cmd('P R SWORD');self.cmd('D R');before=self.obj(63);self.cmd('G L WOODEN SWORD')
  expected=bytearray(before);expected[5]=1;self.assertEqual(self.obj(63),expected)
  self.assertEqual(self.word(2596),0xe87);self.assertEqual(self.word(2592),35)
 def test_round_trips_no_loss_or_duplication(self):
  self.cmd('P R SWORD');self.cmd('D R');before=self.g.raw
  for _ in range(20):
   self.cmd('G L SW');self.cmd('S L');self.cmd('P R W SW');self.cmd('D R')
   self.assertEqual(self.g.raw,before)
 def test_invalid_operations_atomic(self):
  self.light();self.cmd('P R SWORD');self.cmd('D R');before=self.g.raw
  for cmd in ['G UP SWORD','G R PINE SWORD','G R SHIELD','G R S','D L']:
   self.cmd(cmd,2);self.assertEqual(self.g.raw,before)
  self.cmd('G R SWORD');before=self.g.raw;self.cmd('G R SWORD',2);self.assertEqual(self.g.raw,before)
 def test_current_room_level_owner_and_allocated_limit(self):
  self.cmd('P R SWORD');self.cmd('D R');original=self.obj(63)
  for offset,value in [(2,15),(3,12),(4,1),(5,255),(5,1)]:
   b=bytearray(original);b[offset]=value;self.setobj(63,b);before=self.g.raw;self.cmd('G L SWORD',2);self.assertEqual(self.g.raw,before)
  self.setobj(65,original);self.cmd('G L SWORD',2) # unused OCB slot is not allocated
 def test_first_ocb_match_not_link_order(self):
  self.cmd('P R SWORD');self.cmd('D R');b=bytearray(self.obj(63));self.setobj(0,b)
  self.cmd('G L SWORD');self.assertEqual(self.word(2596),0xb15);self.assertEqual(self.obj(63)[5],0)
 def test_dark_get_permitted_and_no_rng_damage(self):
  before=self.g.raw[2576:2579];self.cmd('P R SWORD');self.cmd('D R');self.cmd('G L SWORD')
  self.assertEqual(self.g.raw[2576:2579],before);self.assertEqual(self.word(2590),0);self.assertEqual(self.g.raw[2586],0)
 def test_light_floor_appears_get_erases_drop_restores(self):
  self.light();self.cmd('P R SWORD');empty=self.frame()[:4864];self.cmd('D R');floor=self.frame()[:4864]
  self.assertNotEqual(empty,floor);self.cmd('G R SWORD');self.assertEqual(self.frame()[:4864],empty)
  self.cmd('D R');self.assertEqual(self.frame()[:4864],floor)
 def test_dark_objects_not_visible(self):
  self.cmd('P L SWORD');self.cmd('D L');self.assertEqual(self.frame()[:4864],bytes(4864))
 def test_torch_drop_requires_real_hand_no_light_convenience(self):
  self.light();self.cmd('D L',2);self.assertEqual(self.word(2598),0xe95)
  self.cmd('P R TORCH');self.cmd('D R');self.assertEqual(self.word(2598),0);self.assertEqual(self.obj(64)[5],0)
  self.cmd('G L TORCH');self.assertEqual(self.word(2598),0);self.cmd('USE LEFT');self.assertEqual(self.word(2598),0xe95)
 def test_floor_messages(self):
  self.assertEqual(self.lib.game_message(b'GET LEFT SWORD',2),b'???')
  self.assertEqual(self.lib.game_message(b'D R',0),b'')
  self.assertEqual(self.lib.game_message(b'G R SW',0),b'')
 def test_input_underlay_with_floor(self):
  self.light();self.cmd('P R SWORD');self.cmd('D R');f=ctypes.create_string_buffer(self.frame(),6144);under=ctypes.create_string_buffer(f.raw[5888:6112],224)
  for text in [b'GET',b'GET LEFT SWORD',b'GET',b'']:
   self.lib.game_render_input(f,text,under);fresh=ctypes.create_string_buffer(6144);self.lib.game_render(self.g,fresh,text,b'');self.assertEqual(f.raw,fresh.raw)
 def test_word_weight_wrap(self):
  self.cmd('P R SWORD');ctypes.memmove(ctypes.addressof(self.g)+2592,(10).to_bytes(2,'little'),2)
  self.cmd('D R');self.assertEqual(self.word(2592),65521)
  self.cmd('G R SWORD');self.assertEqual(self.word(2592),10)
 def reference(self):
  r=json.loads((APP/'test/fixtures/floor-original.json').read_text())
  self.assertEqual(hashlib.sha256((self.out/'original.rom').read_bytes()).hexdigest(),r['romSha256'])
  for key,value in r['blobs'].items():self.assertEqual(hashlib.sha256(bytes.fromhex(value)).hexdigest(),key)
  return r
 def test_original_state_round_trip(self):
  r=self.reference()
  for state in r['states']:
   if state['command']:self.cmd(state['command'])
   actual=b''.join(self.word(o).to_bytes(2,'big') for o in [2594,2596,2604,2598,2592])+self.g.raw[1024+63*14:1024+65*14]
   self.assertEqual(actual.hex(),state['playerStateHex'],state['name'])
 def test_original_complete_scene_and_status(self):
  r=self.reference()
  for state in r['states']:
   if state['command']:self.cmd(state['command'])
   # Copy independently observed original world for rendering only. This does
   # not implement or simulate original autonomous creature movement in M4.
   model=ctypes.create_string_buffer(self.g.raw,2608)
   world=bytes.fromhex(r['blobs'][state['world']]);self.assertEqual(len(world),2576)
   ctypes.memmove(model,world,2576);frame=ctypes.create_string_buffer(6144)
   self.lib.game_render(model,frame,b'',b'');self.lib.game_render_status(model,frame,state['heartPhase'])
   self.assertEqual(frame.raw[:5120].hex(),r['blobs'][state['frame']],state['name'])
if __name__=='__main__':unittest.main()
