#!/usr/bin/env python3
"""PEXAM/COMTXT/CFIND source contracts; no host C-port raster oracle."""
import ctypes,subprocess,tempfile,unittest,json,hashlib
from pathlib import Path
from import_gameplay import generate
APP=Path(__file__).resolve().parent
class Examine(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  cls.tmp=tempfile.TemporaryDirectory();cls.out=Path(cls.tmp.name);generate(cls.out)
  subprocess.run(['cc','-O2','-shared','-fPIC','-I'+str(APP/'src'),'-I'+str(cls.out),str(APP/'src/gameplay/game.c'),str(APP/'src/original/logical.c'),'-o',str(cls.out/'game.so')],check=True)
  cls.lib=ctypes.CDLL(str(cls.out/'game.so'));cls.lib.game_message.restype=ctypes.c_char_p
  cls.rom=(cls.out/'original.rom').read_bytes()
  # Original packed SWCTAB comes from the same verified importer, not C lineage.
  import re
  cls.font=bytes(map(int,re.search(r'font\[\]=\{([^}]+)',(cls.out/'game_data.h').read_text())[1].split(',')))
 @classmethod
 def tearDownClass(cls):cls.tmp.cleanup()
 def setUp(self):self.g=ctypes.create_string_buffer(2608);self.lib.game_init(self.g,0)
 def cmd(self,s):self.assertEqual(self.lib.game_command(self.g,s.encode()),0)
 def frame(self):
  f=ctypes.create_string_buffer(6144);self.lib.game_render_examine(self.g,f,b'',b'');return f.raw
 def glyph(self,c):
  code=0 if c==' ' else 27 if c=='!' else ord(c)-64;b=self.font[code*5:code*5+5];bits=''.join(f'{x:08b}' for x in b)
  return bytes(int(bits[5+y*5:10+y*5],2)<<2 for y in range(7))
 def row(self,f,row,col,s,inverse=0):
  for i,c in enumerate(s):self.assertEqual(bytes(f[(row*8+y)*32+col+i] for y in range(7)),bytes(x^inverse for x in self.glyph(c)),(row,col+i,c))
 def test_initial_headers_bag_order(self):
  f=self.frame();self.row(f,0,10,'IN THIS ROOM');self.row(f,1,0,'!'*32);self.row(f,2,12,'BACKPACK');self.row(f,3,0,'WOODEN SWORD');self.row(f,3,16,'PINE TORCH')
 def test_no_qualifier_or_hand_selection(self):
  base=self.g.raw
  for c in ['EXAMINE','EXAMINE BAG','EXAMINE LEFT','EXAMINE SWORD','E','EX']:
   self.cmd(c);self.assertEqual(self.g.raw,base);self.assertEqual(self.lib.game_display_command(c.encode()),2);self.assertEqual(self.lib.game_message(c.encode(),0),b'')
 def test_look_and_unique_prefix(self):
  for c in ['LOOK','L','LOOK BAG']:self.cmd(c);self.assertEqual(self.lib.game_display_command(c.encode()),1)
  self.assertEqual(self.lib.game_command(self.g,b'EXAMINEX'),2);self.assertEqual(self.lib.game_display_command(b'EXAMINEX'),0)
 def test_no_hand_listing_empty_bag(self):
  self.cmd('P L SWORD');self.cmd('P R TORCH');f=self.frame();self.assertEqual(f[3*256:19*256],bytes(16*256))
 def test_floor_get_stow_pull_drop_roundtrip(self):
  self.cmd('P L SWORD');self.cmd('D L');floor=self.frame();self.row(floor,1,0,'WOODEN SWORD')
  self.cmd('G L SWORD');self.row(self.frame(),1,0,'!'*32);self.cmd('S L');self.row(self.frame(),3,0,'WOODEN SWORD')
  self.cmd('P R SWORD');self.cmd('D R');self.assertEqual(self.frame()[:4864],floor[:4864])
 def test_multiple_floor_ocb_order_not_drop_order(self):
  self.cmd('P L TORCH');self.cmd('D L');self.cmd('P L SWORD');self.cmd('D L');f=self.frame();self.row(f,1,0,'WOODEN SWORD');self.row(f,1,16,'PINE TORCH');self.row(f,2,0,'!'*32)
 def test_active_torch_inverse_seven_rows(self):
  self.cmd('P L TORCH');self.cmd('USE LEFT');f=self.frame();self.row(f,3,0,'PINE TORCH',255);self.row(f,3,16,'WOODEN SWORD');self.assertEqual(f[31*32:32*32],bytes(32))
 def test_all_ccb_slots_active_only_and_single_marker(self):
  # CFIND's full 32-record bound, not initialized creatureCount.
  base=2032+31*17;ctypes.memmove(ctypes.addressof(self.g)+base+12,bytes([255,0,0,16,11]),5)
  before=self.g.raw;f=self.frame();self.row(f,1,11,'!CREATURE!');self.assertEqual(self.g.raw,before)
  ctypes.memset(ctypes.addressof(self.g)+base+12,0,1);self.row(self.frame(),1,0,'!'*32)
 def test_render_read_only_no_random_damage_or_links(self):
  self.cmd('P L TORCH');self.cmd('USE LEFT');before=self.g.raw
  for _ in range(5):self.frame()
  self.assertEqual(self.g.raw,before)
 def test_scrolling_stays_inside_logical_frame(self):
  # Original linked records, no alternative inventory representation.
  for i in range(72):
   p=0xb15+(i+1)*14 if i<71 else 0;b=bytearray(14);b[:2]=p.to_bytes(2,'big');b[5]=1;b[9]=17;b[10]=4
   ctypes.memmove(ctypes.addressof(self.g)+1024+i*14,bytes(b),14)
  ctypes.memmove(ctypes.addressof(self.g)+2594,(0xb15).to_bytes(2,'little'),2)
  f=ctypes.create_string_buffer(b'Z'*6208,6208);before=self.g.raw;self.lib.game_render_examine(self.g,f,b'',b'')
  self.assertEqual(f.raw[6144:],b'Z'*64);self.assertEqual(self.g.raw,before);self.row(f.raw,17,0,'WOODEN SWORD')
 def test_original_cartridge_states_and_frames(self):
  r=json.loads((APP/'test/fixtures/examine-original.json').read_text())
  self.assertEqual(hashlib.sha256(self.rom).hexdigest(),r['romSha256'])
  for key,value in r['blobs'].items():self.assertEqual(hashlib.sha256(bytes.fromhex(value)).hexdigest(),key)
  for state in r['states']:
   self.lib.game_init(self.g,0)
   for cmd in state['commands']:self.cmd(cmd)
   words=b''.join(int.from_bytes(self.g.raw[i:i+2],'little').to_bytes(2,'big') for i in [2594,2596,2604,2598,2592])
   self.assertEqual(words.hex(),state['playerStateHex'],state['name'])
   self.assertEqual(self.g.raw[1906:1934].hex(),state['playerObjectsHex'],state['name'])
   # The original has running AI. Use its independently captured world solely
   # for the pixel oracle, after checking our player transitions separately.
   ctypes.memmove(ctypes.addressof(self.g),bytes.fromhex(r['blobs'][state['world']]),2576)
   self.assertEqual(self.frame()[:4864],bytes.fromhex(r['blobs'][state['frame']]),state['name'])
if __name__=='__main__':unittest.main()
