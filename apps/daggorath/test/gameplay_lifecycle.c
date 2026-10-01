#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "audio/native_heartbeat.h"
#include "gameplay/scheduler-api.h"
#include "gameplay/scheduler-callbacks.h"
int gameplay_main(int argc,char **argv);
static int mode,opened,closed,enabled,removed,presented,readcount,clockcount;
static int schedulerOpened,schedulerClosed,schedulerCalls,schedulerLive;
Byte os_intercept(Byte *s){*s=0;return 0;}
Byte os_signal_value(Byte *s){(void)s;return mode==1&&readcount>=2?3:0;}
Byte os_clock(Word *ticks,Byte validate){(void)validate;*ticks=clockcount++;return 0;}
Byte os_sleep(Word n){assert(n==1);return 0;}
Byte screen_open(void){opened++;return mode==3?250:0;}
void screen_set_heart_patterns(const Byte *patterns){assert(patterns);}
Byte screen_close(void){assert(!schedulerLive);closed++;return 0;}
Byte screen_path(void){return 3;}
Byte screen_prepare(const Byte *p){(void)p;return 0;}
Byte screen_prepare_progress(const Byte *p,Byte (*progress)(void *),void *context){Byte row,e;(void)p;
 for(row=0;row<=192;row+=8){if(progress){e=progress(context);if(e)return e;}if(row==192)break;}
 return 0;}
Byte screen_prepare_ui(const Byte *p){(void)p;return 0;}
Byte screen_prepare_ui_progress(const Byte *p,Byte (*progress)(void *),void *context){Byte row,e;(void)p;
 for(row=152;row<=160;row+=2){if(progress){e=progress(context);if(e)return e;}}
 for(row=184;row<=190;row+=2){if(progress){e=progress(context);if(e)return e;}}
 return 0;}
Byte screen_flip(void){presented++;return mode==2?245:0;}
Byte screen_present(const Byte *p){(void)p;presented++;return mode==2?245:0;}
Byte screen_present_ui(const Byte *p){(void)p;return screen_flip();}
Byte screen_present_ui_progress(const Byte *p,Byte (*progress)(void *),void *context){
 Byte e=screen_prepare_ui_progress(p,progress,context);return e?e:screen_flip();}
Byte screen_present_heart(const Byte *p){(void)p;return screen_flip();}
Byte game_input(Byte path,Byte *key){assert(path==3);*key="EXIT\r"[readcount++];return 0;}
Byte native_heartbeat_open(NativeHeartbeat *h){assert(schedulerLive);h->opened=1;return 0;}
Byte native_heartbeat_rate(NativeHeartbeat *h,Word rate){assert(h->opened&&rate==46);return 0;}
Byte native_heartbeat_enable(NativeHeartbeat *h){assert(h->opened);enabled++;return 0;}
Byte native_heartbeat_query(NativeHeartbeat *h,NativeHeartbeatState *state){
 assert(h->opened);memset(state,0,sizeof(*state));
 /* Lifecycle cases need a successful query, no fault and a stable phase.
  * Waveform/countdown behavior is tested separately. */
 state->active=1;state->enabled=enabled!=0;return 0;
}
Byte native_heartbeat_snapshot(NativeHeartbeat *h,NativeHeartbeatState *state){assert(h->opened);state->edgeGeneration=1;state->phase=0;state->fault=0;return 0;}
Byte native_heartbeat_take_ticks(NativeHeartbeat *h,Word *videoTicks){assert(h->opened);*videoTicks=1;return 0;}
Byte native_heartbeat_close(NativeHeartbeat *h){if(h->opened){removed++;h->opened=0;}return 0;}
/* dodsched is mandatory. These mocks expose the same retained-open / temporary
 * call / final-close contract as production, while the existing four cases
 * continue to test main's screen, signal, flip and open failure paths. */
Byte game_scheduler_open(void){assert(opened==1&&!schedulerLive);schedulerOpened++;schedulerLive=1;return 0;}
Byte game_scheduler_call(DagSchedulerContextV1 *context){assert(schedulerLive&&context);schedulerCalls++;return 0;}
Byte game_scheduler_close(void){if(schedulerLive){schedulerClosed++;schedulerLive=0;}return 0;}
/* Host tests link main.c directly.  These deterministic stand-ins supply the
 * 6809 gateway symbols; scheduler lifecycle assertions remain unchanged. */
void scheduler_callback_init(DagSchedulerCallbackContext *c,void *opaque){
 assert(c);(void)opaque;c->dataY=0;c->userOpaque=0;
}
Byte scheduler_task_gateway(void *opaque,Game *g,GameTiming *t,CreatureScheduler *q,Byte task,Byte ccb,Byte *dirty){
 (void)opaque;(void)g;(void)t;(void)q;(void)task;(void)ccb;*dirty=0;return 0;
}
Word scheduler_present_gateway(void *opaque,Game *g,Byte mode){(void)opaque;(void)g;(void)mode;return 0;}
Byte scheduler_progress_gateway(void *opaque){(void)opaque;return 0;}
int main(void){char *argv[]={"dodgame","seed0",0};int expected[]={0,3,245,250},calls[]={2,2,2,0};
 for(mode=0;mode<4;mode++){opened=closed=enabled=removed=presented=readcount=clockcount=0;
  schedulerOpened=schedulerClosed=schedulerCalls=schedulerLive=0;
  assert(gameplay_main(2,argv)==expected[mode]);assert(opened==1&&closed==1);
  assert(enabled==(mode==3?0:1));assert(removed==enabled);
  assert(schedulerOpened==(mode==3?0:1));assert(schedulerClosed==schedulerOpened&&!schedulerLive);
  assert(schedulerCalls==calls[mode]);
  if(!mode)assert(readcount==5&&presented>0);
 }
 puts("4 gameplay lifecycle checks passed");return 0;}
