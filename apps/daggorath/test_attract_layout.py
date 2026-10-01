#!/usr/bin/env python3
"""Source-coordinate checks for ONCE:DEMO10 and MISC:PREPAX presentation."""
import ctypes,re,subprocess,tempfile,unittest
from pathlib import Path
from import_gameplay import generate

APP=Path(__file__).resolve().parent
REF=Path('/Volumes/SEDONA/Projects/daggorath-reference')

class AttractLayout(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  cls.tmp=tempfile.TemporaryDirectory();cls.out=Path(cls.tmp.name);generate(cls.out)
  subprocess.run(['cc','-O2','-shared','-fPIC','-I'+str(APP/'src'),'-I'+str(cls.out),
                  str(APP/'src/gameplay/game.c'),str(APP/'src/original/logical.c'),
                  '-o',str(cls.out/'game.so')],check=True)
  cls.lib=ctypes.CDLL(str(cls.out/'game.so'))
  cls.font=bytes(map(int,re.search(r'font\[\]=\{([^}]+)',(cls.out/'game_data.h').read_text())[1].split(',')))
 @classmethod
 def tearDownClass(cls):cls.tmp.cleanup()
 def glyph(self,c):
  code=ord(c)-64 if 'A'<=c<='Z' else {' ':0,'!':27,'?':29,'.':30}[c]
  bits=''.join(f'{x:08b}' for x in self.font[code*5:code*5+5])
  return bytes(int(bits[5+y*5:10+y*5],2)<<2 for y in range(7))
 def cells(self,frame,row,col,text):
  for i,c in enumerate(text):
   self.assertEqual(bytes(frame[(row+y)*32+col+i] for y in range(7)),self.glyph(c),(row,col+i,c))
 def test_original_prepare_cursor_contract(self):
  source=(REF/'MISC.ASM').read_text()
  self.assertRegex(source,r'PREPAX[\s\S]*?EXAMIO[\s\S]*?#32\*9\+12[\s\S]*?P\.TXCUR')
  frame=ctypes.create_string_buffer(6144);self.lib.game_render_prepare(frame)
  self.cells(frame.raw,72,12,'PREPARE!')
  self.assertEqual(frame.raw[:72*32],bytes(72*32))
  self.assertEqual(frame.raw[79*32:],bytes(6144-79*32))
 def test_demo_welcome_cr_rows_and_punctuation(self):
  frame=ctypes.create_string_buffer(6144)
  self.lib.game_render_attract(frame,b'',b'I DARE YE ENTER...',b'...THE DUNGEONS OF DAGGORATH!!!',b'')
  self.cells(frame.raw,168,0,'I DARE YE ENTER...')
  self.cells(frame.raw,176,0,'...THE DUNGEONS OF DAGGORATH!!!')
  self.assertEqual(frame.raw[:168*32],bytes(168*32))
  self.assertEqual(frame.raw[183*32:],bytes(6144-183*32))
 def test_primary_text_area_has_four_source_rows_and_scroll_contract(self):
  data=(REF/'COMDAT.ASM').read_text(); text=(REF/'COMTXT.ASM').read_text(); service=(REF/'TXTSER.ASM').read_text()
  # COMDAT reserves TXTPRI at display row 20: exactly 32*4 cells.  COMTXT
  # advances CR by one 32-cell row, and TXTSCR copies rows 1..3 upward then
  # clears row 3.  TXTCHR calls that scroll only when the cursor reaches 128.
  self.assertRegex(data,r'D0\$BAS\+\(256\*20\).*?TXTPRI\.TXBAS[\s\S]*?32\*4.*?TXTPRI\.TXCHR')
  self.assertRegex(text,r'TXTCR[\s\S]*?LEAX\s+32,X[\s\S]*?ANDB\s+#%11100000')
  self.assertRegex(text,r'TXTSCR\s+PSHS[\s\S]*?32\*8,X[\s\S]*?LDY\s+#32\*8.*?clear last line')
  self.assertRegex(service,r'CMPX\s+P\.TXCNT,U[\s\S]*?JSR\s+TXTSCR')
  frame=ctypes.create_string_buffer(6144)
  self.lib.game_render_attract(frame,b'ONE',b'TWO',b'THREE',b'FOUR')
  self.cells(frame.raw,160,0,'ONE');self.cells(frame.raw,168,0,'TWO')
  self.cells(frame.raw,176,0,'THREE');self.cells(frame.raw,184,0,'FOUR')
  self.assertEqual(frame.raw[192*32:],bytes(6144-192*32))

if __name__=='__main__':unittest.main()
