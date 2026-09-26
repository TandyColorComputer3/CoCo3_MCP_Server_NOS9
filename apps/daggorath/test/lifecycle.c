#include <assert.h>
#include <stdio.h>
#include "platform.h"
int wizard_main(int,char**);
static Byte *flag;static int frames,ticks,closes,failAt,openError,clockError;
static const int schedule[]={0,18,36,54,72,90,108,126,144,162,180,198,216,234,252,271,293,377,539,658,676,694,712,730,748,766,784,802,820,838,856,874,892,910,912};
Byte os_intercept(Byte *p){flag=p;return 0;}
Byte os_signal_value(Byte *p){return *p;}
Byte os_clock(Word *n,Byte validate){(void)validate;*n=(ticks+3500)%3600;return clockError;}
Byte os_sleep(Word n){assert(n==1);++ticks;return 0;}
Byte os_cancel_self(void){*flag=3;return 0;}
Byte screen_open(void){return openError;}
Byte screen_prepare(const Byte *p){(void)p;return 0;}
Byte screen_flip(void){assert(ticks==schedule[frames]);++frames;return frames==failAt?245:0;}
Byte screen_close(void){++closes;return 0;}
Byte os_write(Byte p,const void *b,Word n){(void)b;(void)n;assert(p==1);return 0;}
void playback_frame(Byte *p,Byte f,Byte m){(void)p;(void)f;(void)m;}
int main(void){
 char *normal[]={"dodwiz",0},*cancel[]={"dodwiz","cancel",0};
 assert(wizard_main(1,normal)==0);assert(frames==35&&ticks==915&&closes==1);
 frames=ticks=closes=0;assert(wizard_main(2,cancel)==3);assert(frames==9&&ticks==144&&closes==1);
 frames=ticks=closes=0;failAt=3;assert(wizard_main(1,normal)==245);assert(frames==3&&closes==1);
 frames=ticks=closes=0;openError=187;assert(wizard_main(1,normal)==187);assert(frames==0&&closes==1);
 frames=ticks=closes=0;openError=0;clockError=187;assert(wizard_main(1,normal)==187);assert(frames==0&&closes==0);
 puts("5 wizard lifecycle cases passed (35 deadlines, minute wrap, cancellation, write/open/clock failure)");return 0;
}
