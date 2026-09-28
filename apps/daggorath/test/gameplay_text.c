#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "audio/native_heartbeat.h"
int gameplay_main(int argc,char **argv);
static const char keys[]="MOVE\rEXIT\r";
static unsigned readAt,uiPresents,fullPrepares,sleeps;
Byte os_intercept(Byte *s){*s=0;return 0;}
Byte os_signal_value(Byte *s){(void)s;return 0;}
Byte os_clock(Word *ticks,Byte validate){(void)validate;*ticks=0;return 0;}
Byte os_sleep(Word ticks){assert(ticks==1);assert(readAt>=sizeof(keys)-1);++sleeps;return 0;}
Byte screen_open(void){return 0;}
Byte screen_close(void){return 0;}
Byte screen_path(void){return 3;}
Byte screen_prepare(const Byte *frame){(void)frame;++fullPrepares;return 0;}
Byte screen_prepare_ui(const Byte *frame){(void)frame;return 0;}
Byte screen_flip(void){return 0;}
Byte screen_present(const Byte *frame){(void)frame;return 0;}
Byte screen_present_ui(const Byte *frame){(void)frame;++uiPresents;assert(readAt>uiPresents-1);return 0;}
Byte screen_present_heart(const Byte *frame){(void)frame;assert(!"stable mock heartbeat must not request a heart-only flip");return ERR_ARGUMENT;}
Byte game_input(Byte path,Byte *key){assert(path==3);*key=keys[readAt];if(*key)++readAt;return 0;}
Byte native_heartbeat_open(NativeHeartbeat *h){h->opened=1;return 0;}
Byte native_heartbeat_rate(NativeHeartbeat *h,Word rate){assert(h->opened&&rate>0&&rate<=255);return 0;}
Byte native_heartbeat_enable(NativeHeartbeat *h){assert(h->opened);return 0;}
Byte native_heartbeat_snapshot(NativeHeartbeat *h,NativeHeartbeatState *s){assert(h->opened);memset(s,0,sizeof(*s));return 0;}
Byte native_heartbeat_take_ticks(NativeHeartbeat *h,Word *ticks){assert(h->opened);*ticks=0;return 0;}
Byte native_heartbeat_close(NativeHeartbeat *h){h->opened=0;return 0;}
int main(void){char *argv[]={"dodgame","seed0",0};
 assert(gameplay_main(2,argv)==0);
 /* Four guest characters are presented before MOVE's CR; the later EXIT
  * characters are likewise visible before the command terminator is read. */
 assert(readAt==sizeof(keys)-1&&uiPresents==8&&sleeps==0);
 /* Only the initial scene and MOVE result use full scene rendering. */
 assert(fullPrepares==2);
 puts("8 immediate command-character presentation checks passed");return 0;
}
