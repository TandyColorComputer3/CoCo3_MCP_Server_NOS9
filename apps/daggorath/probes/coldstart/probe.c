#include <cmoc.h>
#include "presentation.h"
#include "logical.h"
void phase(Byte value);
int wizard_main(int argc,char **argv);
Byte limit;
static Byte frame[FRAME_BYTES];
static Byte load_file(void){const char *name="/d1/dodwiz";Byte p,e;Word n;Byte data[256];
 asm {
 ldx :name
 lda #1
 os9 $84
 bcs @bad
 sta :p
 clrb
@bad
 stb :e
 }
 if(e)return e;
 do{phase(40);asm {
 lda :p
 leax :data
 ldy #256
 os9 $89
 bcs @bad
 sty :n
 clrb
@bad
 stb :e
 }phase(41);}while(!e&&n);
 {Byte close;asm {
 lda :p
 os9 $8f
 bcs @bad
 clrb
@bad
 stb :close
 }return e==211?close:e;}
}
int main(int argc,char **argv){Byte e=0,c,i;phase(1);limit=argc>1?atoi(argv[1]):0;
 if(limit==8){e=load_file();phase(2);return e;}
 if(limit==9){e=wizard_main(1,argv);phase(2);return e;}
 if(limit){e=screen_open();if(e)goto done;}
 if(limit>=6){phase(30);e=screen_prepare(frame);phase(31);if(e)goto done;
 for(i=0;i<(limit==7?35:1);i++){phase(32);e=screen_flip();phase(33);if(e)goto done;os_sleep(1);}}
 phase(3);os_sleep(120);
done:phase(34);c=screen_close();phase(35);return e?e:c;
}
