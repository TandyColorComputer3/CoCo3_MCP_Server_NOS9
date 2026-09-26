/* Disposable SSC architecture probe, NOT a Daggorath backend.
 * Tandy SSC Owner's Manual pp10-11 (FF7D/FF7E/reset/status), pp24-25
 * ($AF, AY register pairs, $FF terminator, channel-A example).
 * PIA routing: EOU SOURCECODE/ASM/NITROS9/SCF/sspak.asm:SpkOut.
 * No MPI selection changes: MAME mame0289 coco_ssc.cpp installs FF7D/E globally.
 * F$Fork/Wait ABI: upstream level1/modules/kernel/{ffork,fwait}.asm.
 */
#include <cmoc.h>
#include "platform.h"
#define HW(a) (*(volatile Byte *)(a))
static Byte interrupted,old1,old3,old23,owned;
static Word polls,writes;
static Word diff(Word a,Word b){return a>=b?a-b:3600-b+a;}
static Byte ready(void){
 Word i;Byte e;
 for(i=0;i<12;i++){
  e=os_signal_value(&interrupted);if(e)return e;
  ++polls;
  if(HW(0xff7e)&0x80)return 0;
  e=os_sleep(1);if(e)return e;
 }
 return 246; /* E$NotRdy; bounded controller wait, not a waveform loop */
}
static Byte send(Byte b){Byte e=ready();if(e)return e;HW(0xff7e)=b;++writes;
 /* Yield after every byte; conservative firmware pacing, not waveform timing. */
 return os_sleep(1);
}
static void close_sound(void){if(!owned)return;
 HW(0xff7d)=1;HW(0xff7d)=0; /* Manual p25 resets to silence, including error path. */
 /* Restore only the mux-control bit owned here; preserve unrelated current bits. */
 HW(0xff01)=(HW(0xff01)&0xf7)|(old1&8);
 HW(0xff03)=(HW(0xff03)&0xf7)|(old3&8);
 HW(0xff23)=(HW(0xff23)&0xf7)|(old23&8);
 owned=0;
}
static Byte open_sound(void){
 /* Explicit exclusive-use probe: refuse existing busy/speech/sound activity. */
 static const Byte packet[]={0xaf,7,63,8,0,9,0,10,0,0,254,1,0,7,62,8,12,0xff};
 Byte i,e;
 if((HW(0xff7e)&0xe0)!=0xe0)return 246;
 old1=HW(0xff01);old3=HW(0xff03);old23=HW(0xff23);owned=1;
 HW(0xff7d)=1;HW(0xff7d)=0;
 HW(0xff01)=old1&0xf7;HW(0xff03)=old3|8;HW(0xff23)=old23|8;
 for(i=0;i<sizeof(packet);i++){e=send(packet[i]);if(e)return e;}
 return ready();
}
static Byte fork_child(const char *name,const char *arg,Byte *child){
 char params[128];Word size;Byte e,pid;
 strcpy(params,arg);size=strlen(params);
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
  sta :pid
  stb :e
 }
 *child=pid;return e;
}
static Byte reap(void){Byte pid,e,cc;asm{
 os9 $04
 sta :pid
 stb :e
 tfr cc,a
 sta :cc
 }printf("WAIT pid=%u status=%u carry=%u\r",pid,e,cc&1);return e;}
int main(int argc,char **argv){
 Word start,prev,now,elapsed=0,gap,maxgap=0,count=0,active=0;
 Byte e=0,w,observer,helper,wizard,n=0;Registers r;const char *mode;
 if(argc!=2)return 187;mode=argv[1];
 e=os_intercept(&interrupted);if(e)return e;
 e=os_clock(&start,1);if(e)return e;prev=start;
 if(!strcmp(mode,"obs")){
  while(elapsed<300){
   e=os_sleep(1);if(e)return e;e=os_clock(&now,0);if(e)return e;
   gap=diff(now,prev);if(gap>300)return 187;
   elapsed+=gap;prev=now;if(gap>maxgap)maxgap=gap;++count;
  }
  printf("OBSERVER ticks=%u wakes=%u maxgap=%u\r",elapsed,count,maxgap);return 0;
 }
 if(!strcmp(mode,"hold")||!strcmp(mode,"cancel")||!strcmp(mode,"error")||!strcmp(mode,"idle")||!strcmp(mode,"busy")||!strcmp(mode,"tone")){
  e=os_sleep(10);if(e)return e;
  if(!strcmp(mode,"hold")||!strcmp(mode,"cancel")||!strcmp(mode,"error")){
   e=open_sound();if(e)goto done;
  }
  e=os_clock(&start,0);if(e)goto done;prev=start;
  if(!strcmp(mode,"tone")){r.x=(16<<8)|120;r.y=3800;e=os_setstat(1,0x98,&r);if(e)goto done;}
  else while(elapsed<120){
   if(strcmp(mode,"busy")){e=os_sleep(1);if(e)goto done;}
   e=os_clock(&now,0);if(e)goto done;gap=diff(now,prev);if(gap>300){e=187;goto done;}
   elapsed+=gap;prev=now;++count;
   if(owned&&!(HW(0xff7e)&32))++active;
   if(elapsed>=60&&!strcmp(mode,"cancel")){e=os_cancel_self();if(e)goto done;}
   if(elapsed>=60&&!strcmp(mode,"error")){e=187;goto done;}
   e=os_signal_value(&interrupted);if(e)goto done;
  }
 done:
  close_sound();w=os_clock(&now,0);if(w&&!e)e=w;
  printf("HELPER %s ticks=%u polls=%u writes=%u loops=%u active=%u status=%u\r",mode,diff(now,start),polls,writes,count,active,e);
  return e;
 }
 if(strcmp(mode,"ssc")&&strcmp(mode,"baseline")&&strcmp(mode,"load")&&strcmp(mode,"sstone")&&strcmp(mode,"canceltest")&&strcmp(mode,"errortest")&&strcmp(mode,"graphics"))return 187;
 e=fork_child("ssprobe","obs\r",&observer);if(e)return e;++n;
 {const char *arg="hold\r";
  if(!strcmp(mode,"baseline"))arg="idle\r";if(!strcmp(mode,"load"))arg="busy\r";if(!strcmp(mode,"sstone"))arg="tone\r";
  if(!strcmp(mode,"canceltest"))arg="cancel\r";if(!strcmp(mode,"errortest"))arg="error\r";
  e=fork_child("ssprobe",arg,&helper);if(e)goto join;++n;
 }
 if(!strcmp(mode,"graphics")){e=fork_child("dodwiz","\r",&wizard);if(e)goto join;++n;}
 join:
 while(n--){w=reap();if(w)e=w;}
 printf("COORD %s status=%u\r",mode,e);return e;
}
