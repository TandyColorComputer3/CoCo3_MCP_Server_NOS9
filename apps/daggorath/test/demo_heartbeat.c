/* Deterministic contract for the bounded doddemo choreography.  It replaces
 * only platform/module boundaries: demo.c itself owns AUTTAB iteration,
 * presentation ordering and its non-logical visual dwell. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "platform.h"
#include "gameplay/game.h"
#include "gameplay/overlay-api.h"
#include "gameplay/scheduler-api.h"
#include "gameplay/scheduler-callbacks.h"
#include "presentation.h"
#include "audio/native_heartbeat.h"
#include "audio/audio.h"

int demo_main(void);
Byte scheduler_progress_resident(void *opaque);
static const char *const expected[]={
 "EXAMINE","PULL RIGHT TORCH","USE RIGHT","LOOK","MOVE",
 "PULL LEFT SHIELD","PULL RIGHT SWORD","MOVE","MOVE","ATTACK RIGHT"};
static const Byte words[]={1,3,2,1,1,3,3,1,1,2};
static Byte commandAt,commandComplete,heartOpen,heartEnabled,schedulerLive;
static Word sleeps,schedulerCalls,dungeonRenders,examineRenders,presents;
static Byte cleanupAfterTenth,phase,renderedPhase,visiblePhase;
static unsigned long edgeGeneration;
static Word activeFlips,heartPresents,schedulerProgressCalls,postHeartbeatSleeps,dwellRefreshes;
static Byte sleepEdge;

Byte os_intercept(Byte *signal){*signal=0;return 0;}
Byte os_intercept_audio(Byte *signal){signal[0]=signal[1]=0;return 0;}
Byte os_signal_value(Byte *signal){(void)signal;return 0;}
Byte os_sleep(Word ticks){assert(ticks==1);++sleeps;
 if(heartEnabled){phase^=1;++edgeGeneration;++postHeartbeatSleeps;sleepEdge=1;}
 return 0;
}
Byte screen_open(void){return 0;}
Byte screen_close(void){assert(commandComplete==10);assert(!schedulerLive);cleanupAfterTenth=1;return 0;}
void screen_set_heart_patterns(const Byte patterns[28]){assert(patterns);}
Byte screen_prepare(const Byte *frame){(void)frame;return 0;}
Byte screen_prepare_progress(const Byte *f,Byte (*p)(void *),void *c){
 (void)f;if(heartEnabled){phase^=1;++edgeGeneration;}return p?p(c):0;
}
Byte screen_prepare_ui(const Byte *frame){(void)frame;return 0;}
Byte screen_prepare_ui_progress(const Byte *f,Byte (*p)(void *),void *c){(void)f;(void)p;(void)c;return 0;}
Byte screen_flip(void){++presents;if(heartEnabled){assert(renderedPhase==phase);++activeFlips;}return 0;}
Byte screen_present(const Byte *frame){(void)frame;++presents;return 0;}
Byte screen_present_ui(const Byte *frame){(void)frame;return 0;}
Byte screen_present_ui_progress(const Byte *f,Byte (*p)(void *),void *c){(void)f;(void)p;(void)c;return 0;}
Byte screen_present_heart(const Byte *frame){(void)frame;assert(renderedPhase==phase);visiblePhase=phase;++heartPresents;return 0;}
Byte screen_path(void){return 3;}

void game_init_demo(Game *g,Byte second){memset(g,0,sizeof(*g));assert(second==21);g->level=2;g->rate=46;g->row=12;g->col=22;g->power=6048;}
void game_health(Game *g){(void)g;}
void game_heart_patterns(Byte patterns[28]){memset(patterns,0,28);}
void game_render_attract(Byte *frame,const char *a,const char *b,const char *c,const char *d){(void)frame;(void)a;(void)b;(void)c;(void)d;}
void game_render_prepare(Byte *frame){(void)frame;}
Byte game_render_with_progress(Game *g,Byte *frame,const char *input,const char *message,GameRenderProgress progress,void *context){
 (void)g;(void)frame;(void)input;(void)message;++dungeonRenders;return progress?progress(g,frame,context):0;
}
void game_render_heart(Byte *frame,Byte p){(void)frame;renderedPhase=p;}
Byte game_render_status_progress(Game *g,Byte *frame,Byte phase,GameRenderProgress progress,void *context){
 (void)g;(void)frame;(void)phase;return progress?progress(g,frame,context):0;
}
Byte game_overlay_examine(Game *g,Byte *frame,const char *input,const char *message){
 (void)g;(void)frame;(void)input;(void)message;++examineRenders;return 0;
}

Byte game_overlay_open(void){return 0;}
Byte game_overlay_close(void){assert(commandComplete==10);return 0;}
Byte game_overlay_command(Game *g,const char *text,GameCombat *combat,Byte *result,Byte *view,const char **message){
 (void)g;memset(combat,0,sizeof(*combat));assert(commandAt<10);assert(!strcmp(text,expected[commandAt]));
 *result=GAME_OK;*view=commandAt==0?GAME_VIEW_EXAMINE:GAME_VIEW_DUNGEON;*message="OK";
 ++commandAt;++commandComplete;return 0;
}

Byte game_scheduler_open(void){assert(!schedulerLive);schedulerLive=1;return 0;}
Byte game_scheduler_close(void){assert(commandComplete==10);assert(schedulerLive);schedulerLive=0;return 0;}
Byte game_scheduler_call(DagSchedulerContextV1 *c){
 Byte expectedOperation=255;
 assert(schedulerLive&&c);
 if(!schedulerCalls)expectedOperation=DOD_SCHED_INIT;
 else if(schedulerCalls==1)expectedOperation=DOD_SCHED_BOUNDARY;
 else {
  Word at=(Word)(schedulerCalls-2),i,offset=0;
  for(i=0;i<10;i++){
   if(at<offset+words[i]){expectedOperation=DOD_SCHED_PLAYER_WAIT;break;}
   if(at==offset+words[i]){expectedOperation=DOD_SCHED_BOUNDARY;break;}
   offset+=(Word)words[i]+1;
  }
 }
 assert(expectedOperation!=255);assert(c->operation==expectedOperation);
 /* COMMON:CLK30 continues during HUMAN:WAIT. Model all 81 recovered source
  * jiffies and inject one native edge per completed AUTTAB word; the mapped
  * scheduler must use its resident-context progress gateway to publish it. */
 if(c->operation==DOD_SCHED_PLAYER_WAIT){
  Word j;assert(c->services->progress);
  for(j=0;j<81;j++){
   if(!(schedulerProgressCalls%81)){phase^=1;++edgeGeneration;}
   assert(c->services->progress(c->services->opaque)==0);
   ++schedulerProgressCalls;
  }
 }
 ++schedulerCalls;return 0;
}
void scheduler_callback_init(DagSchedulerCallbackContext *c,void *opaque){assert(c);(void)opaque;c->dataY=0;c->userOpaque=0;}
Byte scheduler_task_gateway(void *o,Game *g,GameTiming *t,CreatureScheduler *q,Byte task,Byte ccb,Byte *dirty){(void)o;(void)g;(void)t;(void)q;(void)task;(void)ccb;*dirty=0;return 0;}
Word scheduler_present_gateway(void *o,Game *g,Byte mode){(void)o;(void)g;(void)mode;return 0;}
Byte scheduler_progress_gateway(void *o){return scheduler_progress_resident(o);}

Byte native_heartbeat_open(NativeHeartbeat *h){assert(schedulerLive);h->opened=1;heartOpen=1;return 0;}
Byte native_heartbeat_rate(NativeHeartbeat *h,Word rate){assert(h->opened&&rate==46);return 0;}
Byte native_heartbeat_enable(NativeHeartbeat *h){assert(h->opened);heartEnabled=1;return 0;}
Byte native_heartbeat_snapshot(NativeHeartbeat *h,NativeHeartbeatState *state){
 assert(h->opened);if(sleepEdge){++dwellRefreshes;sleepEdge=0;}
 memset(state,0,sizeof(*state));state->phase=phase;state->edgeGeneration=edgeGeneration;return 0;
}
Byte native_heartbeat_close(NativeHeartbeat *h){assert(commandComplete==10);h->opened=0;return 0;}
void audio_present_optional(AudioClient *c,Byte *state,const Byte *events,Byte count,const char *service,const char *profile){
 (void)c;(void)state;(void)events;(void)count;(void)service;(void)profile;
}
Byte audio_finish(AudioClient *c){(void)c;return 0;}

int main(void){
 /* Two attract waits, ten post-command waits, and the bounded final hold.
  * These calls do not advance scheduler or Game state in demo.c. */
 assert(demo_main()==0);
 assert(commandAt==10&&commandComplete==10);
 assert(schedulerCalls==30); /* INIT + initial BOUNDARY + 18 waits + 10 boundaries */
 assert(dungeonRenders==11&&examineRenders==1&&presents==14);
 assert(sleeps==630); /* 90 + 60 + (10 * 36) + 120 */
 assert(heartOpen&&heartEnabled&&cleanupAfterTenth);
 /* Each active full flip receives an edge after status creation. The demo
  * must reconcile that generation before flipping instead of restoring the
  * older template heart. */
 assert(activeFlips==11&&heartPresents>=29&&visiblePhase==phase);
 assert(schedulerProgressCalls==18*81);
 /* Every post-heartbeat visual dwell tick may contain a VIRQ edge. It must
  * reach the same foreground reconcile path rather than wait for a scene. */
 assert(postHeartbeatSleeps==10*36+120&&dwellRefreshes==postHeartbeatSleeps);
 puts("doddemo AUTTAB 1-10 choreography contract checks passed");
 return 0;
}
