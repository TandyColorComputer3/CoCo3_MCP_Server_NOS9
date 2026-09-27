/* Disposable native heartbeat investigation, not linked into dodaudio.
 * Original: COMMON.ASM:CLK30 toggles PIA1 PB1, no waveform busy loop.
 * Tandy Technical Reference III PIA/Sound: PB1 separate from analog mux.
 * Reject enabled CART interrupts: a port-B read acknowledges PIA flags.
 * Save CC only across short PIA read/modify/write, never across an OS call.
 */
#include <cmoc.h>
#include "ipc.h"
#include "heartbeat.h"
#include "../src/audio/ssc.c"
static Byte cancelledFlag,oldBit,oldDdr,ownedBit;
static Byte acquire(void){Byte e=0;
 asm {
 pshs cc
 orcc #$50
 lda $ff23
 anda #$3f
 cmpa #$34
 beq @ok
 cmpa #$3c
 beq @ok
 ldb #187
 stb :e
 bra @done
@ok
 ldb $ff22
 andb #2
 stb :oldBit
 anda #$fb
 sta $ff23
 ldb $ff22
 stb :oldDdr
 orb #2
 stb $ff22
 ora #4
 sta $ff23
@done
 puls cc
 }
 if(!e)ownedBit=1;return e;
}
/* Explicit desired phase avoids replaying late edges in a CPU burst. */
static void level(Byte value){asm {
 ldb :value
 andb #2
 pshs cc
 orcc #$50
 lda $ff23
 lda $ff22
 anda #$fd
 pshs b
 ora ,s+
 sta $ff22
 lda $ff23
 puls cc
 }}
static void release(void){if(!ownedBit)return;level(oldBit);asm {
 pshs cc
 orcc #$50
 lda $ff23
 anda #$fb
 sta $ff23
 ldb $ff22
 andb #$fd
 pshs a
 lda :oldDdr
 anda #2
 pshs b
 ora ,s+
 sta $ff22
 puls a
 ora #4
 sta $ff23
 puls cc
 }ownedBit=0;}
int main(int argc,char **argv){Byte e=0,r,ssc=0;Word last,now,dt,total=0,edges=0,maxLate=0,wait,n;HeartbeatState h;
 const char *mode=argc==2?argv[1]:"slow";
 Byte fast=!strcmp(mode,"fast")||!strcmp(mode,"ssc")||!strcmp(mode,"cancel")||!strcmp(mode,"error");
 if(strcmp(mode,"slow")&&strcmp(mode,"fast")&&strcmp(mode,"ssc")&&strcmp(mode,"cancel")&&strcmp(mode,"error"))return AUDIO_BAD;
 e=os_intercept(&cancelledFlag);if(e)return e;
 if(!strcmp(mode,"ssc")){e=backend_open("ssc-mame-fast",&cancelledFlag);ssc=1;if(e)goto done;}
 e=acquire();if(e)goto done;heartbeat_init(&h);heartbeat_update(&h,fast?3:46);h.remaining=h.interval;h.level=(oldBit!=0);
 e=os_clock(&last,1);if(e)goto done;
 printf("NATIVE begin interval=%u bit=%u ddr=%u\r",h.interval,oldBit,oldDdr);
 while(total<600){
  wait=h.remaining;e=os_sleep(wait);if(e)goto done;
  e=os_signal_value(&cancelledFlag);if(e)goto done;
  e=os_clock(&now,0);if(e)goto done;dt=(now+3600-last)%3600;last=now;
  if(dt>300){e=AUDIO_BAD;goto done;}total+=dt;
  if(dt>wait&&dt-wait>maxLate)maxLate=dt-wait;
  n=heartbeat_advance(&h,dt);edges+=n;if(n)level(h.level?2:0);
  if(ssc&&total>=60&&total-dt<60){e=backend_play(AUDIO_PHASER,255);if(e)goto done;}
  if(total>=90&&!strcmp(mode,"cancel")){os_cancel_self();e=os_signal_value(&cancelledFlag);if(!e)e=3;goto done;}
  if(total>=90&&!strcmp(mode,"error")){e=AUDIO_BAD;goto done;}
 }
 printf("NATIVE elapsed=%u edges=%u maxLate=%u\r",total,edges,maxLate);
done:release();if(ssc){r=backend_close();if(!e)e=r;}printf("NATIVE exit=%u\r",e);return e;
}
