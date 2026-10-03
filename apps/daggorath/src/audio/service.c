#include <cmoc.h>
#include "ipc.h"
#include "backend.h"
static Byte cancelled;
/* Installed EOU F$Send has one pending process signal (fsend.asm). A full
 * reply must already be in the pipe before the completion signal is sent.
 * E$USigP=233 is retried by this worker, never by the foreground presenter. */
static Byte notify(Byte pid){Byte e;
 for(;;){
  if(os_signal_value(&cancelled))return os_signal_value(&cancelled);
  e=ipc_signal(pid,AUDIO_CREDIT_SIGNAL);
  if(e!=233)return e;
  e=os_sleep(1);if(e)return e;
 }
}
static Byte parse_pid(const char *s){Word n=0;
 if(!s||!*s)return 0;
 while(*s){if(*s<'0'||*s>'9')return 0;n=n*10+(Byte)(*s++-'0');if(n>255)return 0;}
 return (Byte)n;
}
int main(int argc,char **argv){Byte f[8],e,r,pid=0,credit;Byte done=0;
 e=os_intercept(&cancelled);if(e)return e;
 if(argc==3)pid=parse_pid(argv[2]);
 e=(argc==2||(argc==3&&pid))?backend_open(argv[1],&cancelled):AUDIO_BAD;
 audio_frame(f,0,0,e,0);r=os_write(1,f,8);if(!e)e=r;
 while(!e&&!done){
  e=ipc_read(0,f,8);if(e)break;
  e=audio_validate(f);
  credit=f[2]==AUDIO_CREDIT_PLAY;
  if(!e)switch(f[2]){
   case AUDIO_PLAY:case AUDIO_CATALOG_PLAY:e=backend_play(f[3],f[4]);break;
   case AUDIO_CREDIT_PLAY:
    e=pid?backend_play(f[3],f[4]):AUDIO_BAD;
    if(!e)e=backend_drain();
    break;
   case AUDIO_STOP:e=backend_stop();break;
   case AUDIO_DRAIN:e=backend_drain();break;
   case AUDIO_SHUTDOWN:done=1;e=backend_stop();break;
  }
  if(!e)e=os_signal_value(&cancelled);
  f[4]=e;r=os_write(1,f,8);if(!e)e=r;
  if(credit&&pid&&!r){r=notify(pid);if(!e)e=r;}
 }
 r=backend_close();if(!e)e=r;
 if(cancelled)e=cancelled;
 return e;
}
