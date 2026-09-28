/* Production API acceptance harness. No SSC commands or addresses here. */
#include <cmoc.h>
#include "ipc.h"
static Byte interrupted;
static Byte request(AudioClient *c,Byte op,Byte sound,Byte gain){Byte e=audio_submit(c,op,sound,gain);return e?e:audio_receive(c);}
/* M2 acceptance paths use exactly the same client/IPC as the game will. */
static Byte m2case(AudioClient *c,const char *mode){Byte e=0,r;Word i;
 Byte event=(!strcmp(mode,"whoop")||!strcmp(mode,"immediate")||!strcmp(mode,"replace")||!strcmp(mode,"stop-active")||!strcmp(mode,"shutdown-active")||!strcmp(mode,"cancel-active")||!strcmp(mode,"error-active"))?AUDIO_WHOOP:AUDIO_PHASER;
 if(!strcmp(mode,"rapid")){
  e=audio_submit(c,AUDIO_PLAY,AUDIO_SQUEAK,255);if(e)return e;
  r=audio_submit(c,AUDIO_PLAY,AUDIO_SQUEAK,255);printf("BACKPRESSURE=%u expected=209\r",r);
  if(r!=AUDIO_BUSY)return AUDIO_BAD;e=audio_receive(c);if(e)return e;
  for(i=0;i<8;i++){e=request(c,AUDIO_PLAY,AUDIO_SQUEAK,255);if(e)return e;}
  return request(c,AUDIO_DRAIN,0,0);
 }
 for(i=0;i<(!strcmp(mode,"phaser-loop")?15:1);i++){
  e=request(c,AUDIO_PLAY,event,255);if(e)return e;
  if(!strcmp(mode,"whoop")||!strcmp(mode,"phaser")||!strcmp(mode,"phaser-loop")){
   e=request(c,AUDIO_DRAIN,0,0);if(e)return e;
   if(!strcmp(mode,"phaser-loop")){e=os_sleep(30);if(e)return e;}continue;
  }
  if(strcmp(mode,"immediate")){e=os_sleep(6);if(e)return e;}
  if(!strcmp(mode,"stop-active"))return request(c,AUDIO_STOP,0,0);
  if(!strcmp(mode,"shutdown-active"))return 0; /* caller finish sends shutdown now */
  if(!strcmp(mode,"cancel-active"))return audio_cancel(c);
  if(!strcmp(mode,"error-active")){
   Byte bad[8];audio_frame(bad,AUDIO_PLAY,event,255,++c->sequence);bad[1]=2;
   e=os_write(c->command,bad,8);c->pending=AUDIO_PLAY;return e?e:audio_receive(c);
  }
  if(!strcmp(mode,"replace")||!strcmp(mode,"immediate"))e=request(c,AUDIO_PLAY,AUDIO_PHASER,255);
  else if(!strcmp(mode,"coalesce"))e=request(c,AUDIO_PLAY,AUDIO_PHASER,255);
  else if(!strcmp(mode,"suppress"))e=request(c,AUDIO_PLAY,AUDIO_SQUEAK,255);
  else return AUDIO_BAD;
  if(e)return e;return request(c,AUDIO_DRAIN,0,0);
 }
 return e;
}
int main(int argc,char **argv){AudioClient c,second;Byte e,r,pid,status;Word i,start,now,gap,prev,elapsed=0,maxgap=0,wakes=0;const char *mode;
 if(argc!=2)return AUDIO_BAD;mode=argv[1];e=os_intercept(&interrupted);if(e)return e;
 if(!strcmp(mode,"observer")||!strcmp(mode,"observer-long")){
  e=os_clock(&prev,1);if(e)return e;
  while(elapsed<(!strcmp(mode,"observer-long")?900:300)){e=os_sleep(1);if(e)return e;e=os_clock(&now,0);if(e)return e;
   gap=now>=prev?now-prev:3600-prev+now;if(gap>300)return AUDIO_BAD;
   elapsed+=gap;prev=now;if(gap>maxgap)maxgap=gap;++wakes;
  }printf("OBSERVER ticks=%u wakes=%u maxgap=%u\r",elapsed,wakes,maxgap);return 0;
 }
 if(!strncmp(mode,"multi",5)||!strcmp(mode,"wizard-audio")){
  const char *child="repeat\r";Word children=2;
  if(!strcmp(mode,"multi-whoop"))child="whoop\r";
  else if(!strcmp(mode,"multi-phaser"))child="phaser\r";
  else if(!strcmp(mode,"multi-rapid"))child="rapid\r";
  else if(!strcmp(mode,"multi-replace"))child="replace\r";
  else if(!strcmp(mode,"wizard-audio"))child="phaser-loop\r";
  e=ipc_fork("dodsnd","observer-long\r",&pid);if(e)return e;
  e=ipc_fork("dodsnd",child,&pid);if(e)return e;
  if(!strcmp(mode,"wizard-audio")){e=ipc_fork("dodwiz","\r",&pid);if(e)return e;children=3;}
  for(i=0;i<children;i++){r=ipc_wait(&pid,&status);printf("CHILD pid=%u status=%u syscall=%u\r",pid,status,r);if(r)e=r;else if(status)e=status;}return e;
 }
 if(!strcmp(mode,"catalog")){
  e=audio_start(&c,"/d1/dodaudio","ssc-mame-fast");printf("START status=%u\r",e);if(e)return e;
  for(i=0;i<=AUDIO_LAST_SOURCE_ID;i++){
   e=request(&c,AUDIO_CATALOG_PLAY,(Byte)i,255);
   if(!e)e=request(&c,AUDIO_DRAIN,0,0);
   printf("CAT %u status=%u\r",i,e);
   if(e)break;
   e=os_sleep(12);if(e)break;
  }
  r=audio_finish(&c);if(!e)e=r;
  printf("FINISH status=%u\r",e);return e;
 }
 if(!strcmp(mode,"whoop")||!strcmp(mode,"phaser")||!strcmp(mode,"phaser-loop")||!strcmp(mode,"rapid")||!strcmp(mode,"immediate")||!strcmp(mode,"replace")||!strcmp(mode,"coalesce")||!strcmp(mode,"suppress")||!strcmp(mode,"stop-active")||!strcmp(mode,"shutdown-active")||!strcmp(mode,"cancel-active")||!strcmp(mode,"error-active")){
  e=audio_start(&c,"/d1/dodaudio","ssc-mame-fast");printf("START status=%u\r",e);if(e)return e;
  e=m2case(&c,mode);printf("CASE status=%u\r",e);
  if(c.opened){r=audio_finish(&c);if(!e)e=r;}
  printf("FINISH status=%u\r",e);return e;
 }

 if(strcmp(mode,"normal")&&strcmp(mode,"repeat")&&strcmp(mode,"stop")&&strcmp(mode,"cancel")&&strcmp(mode,"error")&&strcmp(mode,"owner"))return AUDIO_BAD;
 e=audio_start(&c,"/d1/dodaudio","ssc-mame-fast");printf("START status=%u\r",e);if(e)return e;
 if(!strcmp(mode,"owner")){r=audio_start(&second,"/d1/dodaudio","ssc-mame-fast");printf("SECOND status=%u expected=209\r",r);if(!r)audio_finish(&second);if(r!=AUDIO_BUSY)e=AUDIO_BAD;goto end;}
 for(i=0;i<(!strcmp(mode,"repeat")?5:1);i++){
  e=audio_submit(&c,AUDIO_PLAY,AUDIO_SQUEAK,255);if(e)break;
  if(!strcmp(mode,"stop")||!strcmp(mode,"cancel")){
   e=audio_receive(&c);if(e)break;
   if(!strcmp(mode,"cancel")){e=audio_cancel(&c);printf("CANCEL status=%u\r",e);return e;}
   e=request(&c,AUDIO_STOP,0,0);break;
  }
  /* Caller remains independently schedulable while the request is serviced. */
  os_clock(&start,0);e=os_sleep(1);if(e)break;os_clock(&now,0);
  printf("GAME progressed event=%u\r",i);
  e=audio_receive(&c);if(e)break;
  if(!strcmp(mode,"cancel")){e=audio_cancel(&c);printf("CANCEL status=%u\r",e);return e;}
  if(!strcmp(mode,"error")){
   Byte bad[8];audio_frame(bad,AUDIO_PLAY,0,255,++c.sequence);bad[1]=2;
   e=os_write(c.command,bad,8);c.pending=AUDIO_PLAY;if(!e)e=audio_receive(&c);
   printf("INJECTED status=%u\r",e);break;
  }
  e=request(&c,!strcmp(mode,"stop")?AUDIO_STOP:AUDIO_DRAIN,0,0);if(e)break;
  e=os_sleep(30);if(e)break;
  if(os_signal_value(&interrupted)){e=interrupted;break;}
 }
end:
 r=audio_finish(&c);if(!e)e=r;printf("FINISH status=%u\r",e);return e;
}
