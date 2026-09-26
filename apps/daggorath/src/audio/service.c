#include <cmoc.h>
#include "ipc.h"
#include "backend.h"
static Byte cancelled;
int main(int argc,char **argv){Byte f[8],e,r;Byte done=0;
 e=os_intercept(&cancelled);if(e)return e;
 e=argc==2?backend_open(argv[1],&cancelled):AUDIO_BAD;
 audio_frame(f,0,0,e,0);r=os_write(1,f,8);if(!e)e=r;
 while(!e&&!done){
  e=ipc_read(0,f,8);if(e)break;
  e=audio_validate(f);
  if(!e)switch(f[2]){
   case AUDIO_PLAY:e=backend_play(f[3],f[4]);break;
   case AUDIO_STOP:e=backend_stop();break;
   case AUDIO_DRAIN:e=backend_drain();break;
   case AUDIO_SHUTDOWN:done=1;e=backend_stop();break;
  }
  if(!e)e=os_signal_value(&cancelled);
  f[4]=e;r=os_write(1,f,8);if(!e)e=r;
 }
 r=backend_close();if(!e)e=r;
 if(cancelled)e=cancelled;
 return e;
}
