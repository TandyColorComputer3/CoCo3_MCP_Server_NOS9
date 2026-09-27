#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "audio/native_heartbeat.h"
int gameplay_main(int argc,char **argv);
static int mode,opened,closed,enabled,removed,presented,readcount,clockcount;
Byte os_intercept(Byte *s){*s=0;return 0;}
Byte os_signal_value(Byte *s){(void)s;return mode==1&&readcount>=2?3:0;}
Byte os_clock(Word *ticks,Byte validate){(void)validate;*ticks=clockcount++;return 0;}
Byte os_sleep(Word n){assert(n==1);return 0;}
Byte screen_open(void){opened++;return mode==3?250:0;}
Byte screen_close(void){closed++;return 0;}
Byte screen_path(void){return 3;}
Byte screen_present(const Byte *p){(void)p;presented++;return mode==2?245:0;}
Byte game_input(Byte path,Byte *key){assert(path==3);*key="EXIT\r"[readcount++];return 0;}
Byte native_heartbeat_open(NativeHeartbeat *h){h->opened=1;return 0;}
Byte native_heartbeat_rate(NativeHeartbeat *h,Word rate){assert(h->opened&&rate==46);return 0;}
Byte native_heartbeat_enable(NativeHeartbeat *h){assert(h->opened);enabled++;return 0;}
Byte native_heartbeat_query(NativeHeartbeat *h,NativeHeartbeatState *state){
 assert(h->opened);memset(state,0,sizeof(*state));
 /* Lifecycle cases need a successful query, no fault and a stable phase.
  * Waveform/countdown behavior is tested separately. */
 state->active=1;state->enabled=enabled!=0;return 0;
}
Byte native_heartbeat_close(NativeHeartbeat *h){if(h->opened){removed++;h->opened=0;}return 0;}
int main(void){char *argv[]={"dodgame","seed0",0};int expected[]={0,3,245,250};
 for(mode=0;mode<4;mode++){opened=closed=enabled=removed=presented=readcount=clockcount=0;
  assert(gameplay_main(2,argv)==expected[mode]);assert(opened==1&&closed==1);
  assert(enabled==(mode==3?0:1));assert(removed==enabled);
  if(!mode)assert(readcount==5&&presented>0);
 }
 puts("4 gameplay lifecycle checks passed");return 0;}
