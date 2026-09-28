/* Disposable, named playback of the production SSC source-ID catalog.
 * The game and dodaudio do not require this command to be installed. */
#include <cmoc.h>
#include "backend.h"
#include "ssc_catalog.h"
static Byte cancelled;
static Byte play(Byte id){Byte e=backend_play(id,255);
 if(!e)e=backend_drain();
 printf("DODCAT %u %s STATUS %u\r",id,ssc_catalog_name(id),e);
 return e;
}
int main(int argc,char **argv){Byte id,e=0,r,all=0;
 if(argc!=2)return AUDIO_BAD;
 if(!strcmp(argv[1],"list")){
  for(id=0;id<=AUDIO_LAST_SOURCE_ID;id++)printf("%u %s\r",id,ssc_catalog_name(id));
  return 0;
 }
 if(!strcmp(argv[1],"all"))all=1;
 else{
  for(id=0;id<=AUDIO_LAST_SOURCE_ID;id++)
   if(!strcmp(argv[1],ssc_catalog_name(id)))break;
  if(id>AUDIO_LAST_SOURCE_ID)return AUDIO_BAD;
 }
 e=os_intercept(&cancelled);if(e)return e;
 e=backend_open("ssc-mame-fast",&cancelled);
 if(e){backend_close();return e;}
 if(all){for(id=0;id<=AUDIO_LAST_SOURCE_ID;id++){
  e=play(id);if(e)break;
  e=os_sleep(12);if(e)break; /* yield and separate captured events */
 }}else e=play(id);
 r=backend_close();if(!e)e=r;
 if(os_signal_value(&cancelled))e=os_signal_value(&cancelled);
 return e;
}
