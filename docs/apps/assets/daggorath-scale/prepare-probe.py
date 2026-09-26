from pathlib import Path
import shutil,json,hashlib
root=Path('/Volumes/SEDONA/Projects/CoCo3_MCP_Server_NOS9');p=Path('/private/tmp/daggorath-scale');app=p/'probe'
files=list((root/'apps/daggorath/src').rglob('*'))+[root/'media'/n for n in ('63SDC.VHD','63SDC-MCP-DEV.VHD','63EMU.DSK')]
(p/'before.json').write_text(json.dumps({str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in files if f.is_file()},indent=2))
shutil.copytree(root/'apps/daggorath',app,dirs_exist_ok=True)
header='/* Generated integer floor maps; logical renderer remains unchanged. */\n'
for name,w,h in [('a',512,192),('b',426,160),('c',384,144)]:
 for typ,label,vals in [('Byte','xb',[((x*256)//w)//8 for x in range(w)]),('Byte','xm',[128>>(((x*256)//w)%8) for x in range(w)]),('Word','yb',[(y*192//h)*32 for y in range(h)])]:
  header+=f'static const {typ} {label}_{name}[]={{'+','.join(map(str,vals))+'};\n'
(app/'src/maps.h').write_text(header)
(app/'src/main.c').write_text('''#include <cmoc.h>
#include "presentation.h"
#include "logical.h"
void scale_mode(Byte mode);
static Byte frame[FRAME_BYTES],signalFlag;
int main(int argc,char **argv){Byte e=0,c,i;Word t;
 if(argc!=2||strlen(argv[1])!=1||argv[1][0]<'a'||argv[1][0]>'c')return 187;
 scale_mode(argv[1][0]);e=os_intercept(&signalFlag);if(e)return e;
 e=screen_open();if(e)goto done;
 for(i=0;i<4;i++){
  wizard_frame(frame,i<2?0:i==2?14:32,i==1);
  e=screen_present(frame);if(e)goto done;
  for(t=0;t<240;t++){e=os_signal_value(&signalFlag);if(e)goto done;e=os_sleep(1);if(e)goto done;}
 }
 done:c=screen_close();return c?c:e;
}
''')
s=(root/'apps/daggorath/src/presentation.c').read_text()
s=s.replace('static const Byte define[]={27,0x29,196,1,24,0};','static const Byte define[]={27,0x29,196,1,62,128};')
s=s.replace('static const Byte upload[]={27,0x2b,196,1,5,1,0,0,192,24,0};','static const Byte upload[]={27,0x2b,196,1,5,2,128,0,200,62,128};')
s=s.replace('static const Byte put[]={27,0x2d,196,1,0,192,0,4};','static const Byte put[]={27,0x2d,196,1,0,0,0,0};')
s=s.replace('Byte screen_open(void)','''#include "maps.h"
static Byte canvas[16000];
static Word width,height,ox,oy;
static const Byte *xb,*xm;
static const Word *yb;
void scale_mode(Byte mode){
 if(mode=='a'){width=512;height=192;xb=xb_a;xm=xm_a;yb=yb_a;}
 else if(mode=='b'){width=426;height=160;xb=xb_b;xm=xm_b;yb=yb_b;}
 else {width=384;height=144;xb=xb_c;xm=xm_c;yb=yb_c;}
 ox=(640-width)/2;oy=(200-height)/2;
}
static void dot(Word x,Word y){canvas[y*80+x/8]|=128>>(x%8);}
static void bezel(void){Word x,y;
 for(x=0;x<640;x++){dot(x,0);dot(x,199);dot(x,oy-2);dot(x,oy+height+1);}
 for(y=0;y<200;y++){dot(0,y);dot(639,y);dot(ox-2,y);dot(ox+width+1,y);}
}
Byte screen_open(void)''')
a=s.index('    Byte e=os_write(fd,upload');b=s.index('    return os_write(fd,put',a)
s=s[:a]+'''    Word x,y;Byte bit,e,*dest;const Byte *source;
    memset(canvas,0,sizeof(canvas));bezel();
    for(y=0;y<height;y++){
      source=frame+yb[y];dest=canvas+(oy+y)*80+ox/8;bit=128>>(ox%8);
      for(x=0;x<width;x++){
        if(source[xb[x]]&xm[x])*dest|=bit;
        bit>>=1;if(!bit){bit=128;dest++;}
      }
    }
    e=os_write(fd,upload,sizeof(upload));if(e)return e;
    e=os_write(fd,canvas,sizeof(canvas));if(e)return e;
'''+s[b:]
(app/'src/presentation.c').write_text(s)
(app/'src/module.asm').write_text((app/'src/module.asm').read_text().replace('dodwiz','dodscale'))
