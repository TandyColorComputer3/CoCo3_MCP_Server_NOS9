/* Production API acceptance harness. No SSC commands or addresses here. */
#include <cmoc.h>
#include "ipc.h"
static Byte interrupted;
static Byte request(AudioClient *c,Byte op,Byte sound,Byte gain){Byte e=audio_submit(c,op,sound,gain);return e?e:audio_receive(c);}
int main(int argc,char **argv){AudioClient c,second;Byte e,r,pid,status;Word i,start,now,gap,prev,elapsed=0,maxgap=0,wakes=0;const char *mode;
 if(argc!=2)return AUDIO_BAD;mode=argv[1];e=os_intercept(&interrupted);if(e)return e;
 if(!strcmp(mode,"observer")){
  e=os_clock(&prev,1);if(e)return e;
  while(elapsed<300){e=os_sleep(1);if(e)return e;e=os_clock(&now,0);if(e)return e;
   gap=now>=prev?now-prev:3600-prev+now;if(gap>300)return AUDIO_BAD;
   elapsed+=gap;prev=now;if(gap>maxgap)maxgap=gap;++wakes;
  }printf("OBSERVER ticks=%u wakes=%u maxgap=%u\r",elapsed,wakes,maxgap);return 0;
 }
 if(!strcmp(mode,"multi")){
  e=ipc_fork("dodsnd","observer\r",&pid);if(e)return e;
  e=ipc_fork("dodsnd","repeat\r",&pid);if(e)return e;
  for(i=0;i<2;i++){r=ipc_wait(&pid,&status);printf("CHILD pid=%u status=%u syscall=%u\r",pid,status,r);if(r)e=r;else if(status)e=status;}return e;
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
