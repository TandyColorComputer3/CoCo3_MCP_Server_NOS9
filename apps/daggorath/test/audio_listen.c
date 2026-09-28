#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "audio/audio.h"
#define main listen_main
#include "../src/audio/listen.c"
#undef main
static Byte sent[96],count,played,closed,slept;
Byte os_intercept(Byte *signal){*signal=0;return 0;}
Byte os_signal_value(Byte *signal){return *signal;}
Byte os_sleep(Word ticks){assert(ticks==90);++slept;return 0;}
Byte ssc_read_io(Word address){assert(address==0xff7e);return 0xe0;}
void ssc_write_io(Word address,Byte value){assert(address==0xff7e);assert(count<sizeof(sent));sent[count++]=value;}
Byte backend_open(const char *profile,Byte *signal){assert(!strcmp(profile,"ssc-mame-fast")&&signal);return 0;}
Byte backend_play(Byte sound,Byte gain){assert(sound==AUDIO_SQUEAK&&gain==255);++played;return 0;}
Byte backend_close(void){++closed;return 0;}
static void run(const char *name){char *args[]={"dodlisten",(char *)name,0};
 count=played=closed=slept=0;assert(listen_main(2,args)==0);assert(closed==1&&slept==1);
}
int main(void){Byte i;
 run("spider");assert(played==1&&count==0);
 run("snake");assert(played==0&&count==63&&sent[0]==0x9e&&sent[62]==0xde);
 for(i=0;i<10;i++){
  assert(sent[1+i*6]==0x8c&&sent[2+i*6]==12&&sent[3+i*6]==1);
  assert(sent[4+i*6]==0x80&&sent[5+i*6]==12&&sent[6+i*6]==1);
 }assert(sent[61]==0xff);
 run("sword-swing");assert(count==sizeof(swordSwing)+2&&sent[0]==0x9e&&
  !memcmp(sent+1,swordSwing,sizeof(swordSwing))&&sent[count-1]==0xde);
 run("sword-hit");assert(count==sizeof(swordHit)+2&&sent[0]==0x9e&&
  !memcmp(sent+1,swordHit,sizeof(swordHit))&&sent[count-1]==0xde);
 run("player-hit");assert(count==sizeof(playerHit)+2&&sent[0]==0x9e&&
  !memcmp(sent+1,playerHit,sizeof(playerHit))&&sent[count-1]==0xde);
 run("knight-1");assert(count==59&&sent[0]==0x9e&&sent[57]==0xff&&sent[58]==0xde);
 for(i=0;i<7;i++){
  Byte level=(Byte)(12-2*i),offset=(Byte)(1+8*i);
  assert(sent[offset]==level&&sent[offset+1]==1&&sent[offset+2]==94);
  assert(sent[offset+3]==(level?2:0));
  assert(sent[offset+4]==(Byte)(0x20|level)&&sent[offset+5]==0&&sent[offset+6]==108);
  assert(sent[offset+7]==(level?2:0));
 }
 run("knight-2");assert(count==59&&sent[0]==0x9e&&sent[57]==0xff&&sent[58]==0xde);
 for(i=0;i<7;i++){
  Byte level=(Byte)(12-2*i),offset=(Byte)(1+8*i);
  assert(sent[offset]==level&&sent[offset+1]==0&&sent[offset+2]==100);
  assert(sent[offset+3]==(level?2:0));
  assert(sent[offset+4]==(Byte)(0x20|level)&&sent[offset+5]==0&&sent[offset+6]==36);
  assert(sent[offset+7]==(level?2:0));
 }
 puts("7 named SSC listening diagnostics passed");return 0;
}
