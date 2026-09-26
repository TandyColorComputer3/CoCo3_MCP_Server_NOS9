/* Temporary research probe; no direct hardware writes or interrupt hooks.
 * SS.Tone: upstream level2/coco3/modules/snddrv_cc3.asm.
 * Fork/Wait: upstream kernel/ffork.asm, level1/cmds/deldir.asm.
 * Clock/sleep/signal adapters: unchanged apps/daggorath/src/os9.c. */
#include <cmoc.h>
#include "platform.h"
static Byte flag;
static Word delta(Word a,Word b){return a>=b?a-b:3600-b+a;}
static Byte fork_observer(void){
 Byte error;const char *name="audmt";char params[256];Word size=5;
 strcpy(params,"obs\r");
 asm {
  pshs y,u
  ldx :name
  ldy :size
  leau :params
  lda #$11
  clrb
  os9 $03
  bcs @bad
  clrb
@bad
  puls u,y
  stb :error
 }
 return error;
}
static Byte reap(void){Byte e;asm {
 os9 $04
 stb :e
 }return e;}
int main(int argc,char **argv){
 Word start,prev,now,elapsed=0,gap,maxgap=0,count=0,loops=0;Byte e=0,w;Registers r;
 if(argc!=2)return 187;
 e=os_intercept(&flag);if(e)return e;
 e=os_clock(&start,1);if(e)return e;prev=start;
 if(!strcmp(argv[1],"obs")){
  while(elapsed<300){
   e=os_sleep(1);if(e)return e;e=os_clock(&now,0);if(e)return e;
   gap=delta(now,prev);elapsed+=gap;prev=now;if(gap>maxgap)maxgap=gap;++count;
   if(os_signal_value(&flag))return 3;
  }
  printf("OBSERVER ticks=%u wakes=%u maxgap=%u\r",elapsed,count,maxgap);return 0;
 }
 if(strcmp(argv[1],"tone")&&strcmp(argv[1],"idle")&&strcmp(argv[1],"busy"))return 187;
 e=fork_observer();if(e)return e;e=os_sleep(10);if(e)goto done;
 e=os_clock(&start,0);if(e)goto done;
 if(!strcmp(argv[1],"tone")){r.x=(16<<8)|120;r.y=3800;e=os_setstat(1,0x98,&r);}
 else if(!strcmp(argv[1],"idle"))e=os_sleep(120);
 else {do {e=os_clock(&now,0);if(e)break;++loops;if(os_signal_value(&flag)){e=3;break;}}while(delta(now,start)<120);}
 if(os_clock(&now,0)&&!e)e=187;
 printf("PARENT %s ticks=%u loops=%u status=%u\r",argv[1],delta(now,start),loops,e);
done:
 w=reap();printf("REAP status=%u\r",w);return e?e:w;
}
