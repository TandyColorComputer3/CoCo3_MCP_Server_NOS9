#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "platform.h"
int probe_main(int,char**);
static int selected,closed,ended,sleeps,failSetup; static Byte *flag;
Byte window_open(Byte *p){*p=3;return 0;}
Byte window_close(Byte p){assert(p==3);++closed;return 0;}
Byte os_intercept(Byte *p){flag=p;return 0;}
Byte os_signal_value(Byte *p){return *p;}
Byte os_cancel_self(void){*flag=3;return 0;}
Byte os_sleep(Word n){assert(n==1);++sleeps;return 0;}
Byte os_getstat(Byte p,Byte fn,Registers *r){assert(p==3);if(fn==SS_SCTYP)r->a=5;else{assert(fn==SS_SCSIZ);r->x=80;r->y=25;}return 0;}
Byte os_write(Byte p,const void *data,Word n){
 const Byte *b=data;
 if(n>=10&&b[0]==27&&b[1]==0x20){assert(p==3);assert(b[2]==5&&b[5]==80&&b[6]==25);if(failSetup)return 245;}
 if(n==2&&b[0]==27&&b[1]==0x21)selected=p;
 if(n==2&&b[0]==27&&b[1]==0x24){assert(p==3);++ended;}
 return 0;
}
int main(void){
 char *normal[]={"gfxprobe",0},*cancel[]={"gfxprobe","cancel",0},*error[]={"gfxprobe","error",0};
 assert(probe_main(1,normal)==0);assert(selected==1&&closed==1&&ended==1&&sleeps==600);
 closed=ended=sleeps=0;assert(probe_main(2,cancel)==3);assert(selected==1&&closed==1&&ended==1&&sleeps==180);
 closed=ended=sleeps=0;assert(probe_main(2,error)==187);assert(selected==1&&closed==1&&ended==1&&sleeps==180);
 closed=ended=sleeps=0;failSetup=1;assert(probe_main(1,normal)==245);assert(selected==1&&closed==1&&sleeps==0);
 puts("4 graphics lifecycle cases passed");return 0;
}
