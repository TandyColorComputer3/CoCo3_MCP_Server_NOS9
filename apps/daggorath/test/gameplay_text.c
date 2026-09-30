#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "audio/native_heartbeat.h"
#include "gameplay/scheduler-api.h"
int gameplay_main(int argc,char **argv);
static const char keys[]="MOVE\rEXIT\r";
static unsigned readAt,uiPresents,fullPrepares,sleeps,schedulerOpens,schedulerCloses,schedulerCalls;
static Byte schedulerLive;
Byte os_intercept(Byte *s){*s=0;return 0;}
Byte os_signal_value(Byte *s){(void)s;return 0;}
Byte os_clock(Word *ticks,Byte validate){(void)validate;*ticks=0;return 0;}
Byte os_sleep(Word ticks){assert(ticks==1);assert(readAt>=sizeof(keys)-1);++sleeps;return 0;}
Byte screen_open(void){return 0;}
void screen_set_heart_patterns(const Byte *patterns){assert(patterns);}
Byte screen_close(void){assert(!schedulerLive);return 0;}
Byte screen_path(void){return 3;}
Byte screen_prepare(const Byte *frame){(void)frame;++fullPrepares;return 0;}
Byte screen_prepare_progress(const Byte *frame,Byte (*progress)(void *),void *context){Byte row,e;
 (void)frame;++fullPrepares;
 for(row=0;row<=192;row+=8){if(progress){e=progress(context);if(e)return e;}if(row==192)break;}
 return 0;}
Byte screen_prepare_ui(const Byte *frame){(void)frame;return 0;}
Byte screen_prepare_ui_progress(const Byte *frame,Byte (*progress)(void *),void *context){Byte row,e;
 (void)frame;
 for(row=152;row<=160;row+=2){if(progress){e=progress(context);if(e)return e;}}
 for(row=184;row<=190;row+=2){if(progress){e=progress(context);if(e)return e;}}
 return 0;}
Byte screen_flip(void){return 0;}
Byte screen_present(const Byte *frame){(void)frame;return 0;}
Byte screen_present_ui(const Byte *frame){(void)frame;++uiPresents;assert(readAt>uiPresents-1);return 0;}
Byte screen_present_ui_progress(const Byte *frame,Byte (*progress)(void *),void *context){
 Byte e=screen_prepare_ui_progress(frame,progress,context);
 return e?e:screen_present_ui(frame);}
Byte screen_present_heart(const Byte *frame){(void)frame;assert(!"stable mock heartbeat must not request a heart-only flip");return ERR_ARGUMENT;}
Byte game_input(Byte path,Byte *key){assert(path==3);*key=keys[readAt];if(*key)++readAt;return 0;}
Byte native_heartbeat_open(NativeHeartbeat *h){assert(schedulerLive);h->opened=1;return 0;}
Byte native_heartbeat_rate(NativeHeartbeat *h,Word rate){assert(h->opened&&rate>0&&rate<=255);return 0;}
Byte native_heartbeat_enable(NativeHeartbeat *h){assert(h->opened);return 0;}
Byte native_heartbeat_snapshot(NativeHeartbeat *h,NativeHeartbeatState *s){assert(h->opened);memset(s,0,sizeof(*s));return 0;}
Byte native_heartbeat_take_ticks(NativeHeartbeat *h,Word *ticks){assert(h->opened);*ticks=0;return 0;}
Byte native_heartbeat_close(NativeHeartbeat *h){h->opened=0;return 0;}
/* Mandatory scheduler lifecycle: the stable text fixture never reaches a
 * creature-attack call, but it still observes retained open before heartbeat
 * activation and final close before screen teardown. */
Byte game_scheduler_open(void){assert(!schedulerLive);schedulerLive=1;++schedulerOpens;return 0;}
Byte game_scheduler_call(DagSchedulerContextV1 *context){assert(schedulerLive&&context);++schedulerCalls;return 0;}
Byte game_scheduler_close(void){assert(schedulerLive);schedulerLive=0;++schedulerCloses;return 0;}
int main(void){char *argv[]={"dodgame","seed0",0};
 assert(gameplay_main(2,argv)==0);
 /* Four guest characters are presented before MOVE's CR; the later EXIT
  * characters are likewise visible before the command terminator is read. */
 assert(readAt==sizeof(keys)-1&&uiPresents==8&&sleeps==0);
 /* Only the initial scene and MOVE result use full scene rendering. */
 assert(fullPrepares==2);
 /* Initial scheduler setup plus one normal source boundary for each of the
  * two complete command iterations. */
 assert(schedulerOpens==1&&schedulerCloses==1&&schedulerCalls==3&&!schedulerLive);
 puts("8 immediate command-character presentation checks passed");return 0;
}
