/* Public opening continuation: real Game, dodcmd body, and dodsched body.
 * Only OS/path and module mapping boundaries are replaced on the host. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define DOD_OPENING_PHASE 1
#define main intro_main
#include "../src/gameplay/demo.c"
#undef main

Byte dod_overlay_execute(DagOverlayContextV1 *);
Byte dodsched_execute(DagSchedulerContextV1 *);
static Byte overlayLive,schedulerLive,mapped,commands,flips,firstExamine;
static Byte schedulerPresentActive,schedulerPresentRequests,schedulerFlips;
static Byte schedulerDungeonRequests,schedulerOverlayViewRequests;
static const char *expected[]={"EXAMINE","PULL RIGHT TORCH","USE RIGHT","LOOK","MOVE",
 "PULL LEFT SHIELD","PULL RIGHT SWORD","MOVE","MOVE","ATTACK RIGHT"};
static Byte opaque;
static void health(void *p,Game *g){(void)p;game_health(g);}
static Byte name(void *p,Game *g,Word token,Byte *out){(void)p;return game_object_name(g,token,out);}
static void status(void *p,Game *g,Byte *f,Byte phase){(void)p;game_render_status(g,f,phase);}
static DagOverlayServices service={DOD_OVERLAY_ABI_V1,sizeof(DagOverlayServices),&opaque,health,name,status};
Byte os_intercept_audio(Byte *s){s[0]=s[1]=0;return 0;}
Byte os_signal_value(Byte *s){return *s;}
Byte os_sleep(Word ticks){assert(ticks==1);return 0;}
Byte screen_adopt(Byte p){assert(p==3);return 0;}
Byte screen_close(void){assert(!mapped&&!overlayLive&&!schedulerLive);return 0;}
void screen_set_heart_patterns(const Byte p[28]){assert(p);}
Byte screen_present(const Byte *f){assert(f);return 0;}
Byte screen_present_heart(const Byte *f){assert(f);return 0;}
Byte screen_prepare_progress(const Byte *f,Byte (*p)(void *),void *c){assert(f);return p?p(c):0;}
Byte screen_flip(void){
 /* Source LUKNEW/PUPDAT may present through the resident callback while
  * dodsched alone owns the eighth DAT slot.  dodcmd remains forbidden here,
  * and every mapped flip must be enclosed by the established gateway. */
 assert(mapped!=1);assert(mapped==0||mapped==2);
 if(mapped==2){assert(schedulerPresentActive);++schedulerFlips;}
 ++flips;
 if(commands==1){
  Byte i;assert(primaryView==GAME_VIEW_EXAMINE);
  for(i=0;i<32;i++)assert(primary.cells[i]==0);
  assert(primary.cells[32]==30);
  for(i=0;i<7;i++)assert(primary.cells[33+i]==(Byte)("EXAMINE"[i]-'A'+1));
  assert(primary.cells[64]==30&&primary.cells[65]==28&&primary.cursor==65);
  firstExamine=1;
 }
 return 0;
}
Byte opening_heartbeat_modules_open(void){return 0;}
Byte opening_heartbeat_modules_close(void){return 0;}
Byte native_heartbeat_open(NativeHeartbeat *h){assert(overlayLive&&schedulerLive);h->opened=1;return 0;}
Byte native_heartbeat_rate(NativeHeartbeat *h,Word r){assert(h->opened&&r);return 0;}
Byte native_heartbeat_enable(NativeHeartbeat *h){assert(h->opened);return 0;}
Byte native_heartbeat_snapshot(NativeHeartbeat *h,NativeHeartbeatState *s){assert(h->opened);memset(s,0,sizeof(*s));return 0;}
Byte native_heartbeat_close(NativeHeartbeat *h){h->opened=0;return 0;}
Byte game_overlay_open(void){overlayLive=1;return 0;}
Byte game_overlay_close(void){assert(!mapped);overlayLive=0;return 0;}
Byte game_overlay_command(Game *g,const char *text,GameCombat *combat,Byte *result,Byte *view,const char **message){
 DagOverlayContextV1 c;Byte e;
 assert(overlayLive&&!mapped&&commands<10&&!strcmp(text,expected[commands]));++commands;
 memset(&c,0,sizeof(c));c.abiVersion=DOD_OVERLAY_ABI_V1;c.contextSize=sizeof(c);
 c.operation=DOD_OVERLAY_COMMAND;c.game=g;c.services=&service;c.command=text;c.combat=combat;
 mapped=1;e=dod_overlay_execute(&c);mapped=0;*result=c.result;*view=c.view;*message=c.outputMessage;return e;
}
Byte game_overlay_examine(Game *g,Byte *f,const char *input,const char *message){
 DagOverlayContextV1 c;Byte e;
 assert(overlayLive&&!mapped);memset(&c,0,sizeof(c));
 c.abiVersion=DOD_OVERLAY_ABI_V1;c.contextSize=sizeof(c);c.operation=DOD_OVERLAY_EXAMINE;
 c.game=g;c.services=&service;c.frame=f;c.input=input;c.message=message;
 mapped=1;e=dod_overlay_execute(&c);mapped=0;return e;
}
Byte game_scheduler_open(void){schedulerLive=1;return 0;}
Byte game_scheduler_close(void){assert(!mapped);schedulerLive=0;return 0;}
Byte game_scheduler_call(DagSchedulerContextV1 *c){Byte e;assert(schedulerLive&&!mapped);mapped=2;e=dodsched_execute(c);mapped=0;return e;}
void scheduler_callback_init(DagSchedulerCallbackContext *c,void *p){assert(!p);memset(c,0,sizeof(*c));}
Byte scheduler_task_gateway(void *p,Game *g,GameTiming *t,CreatureScheduler *q,Byte task,Byte ccb,Byte *dirty){return scheduler_task_resident(p,g,t,q,task,ccb,dirty);}
Word scheduler_present_gateway(void *p,Game *g,Byte mode){Word ticks;Byte before=schedulerFlips;
 assert(mapped==2&&!schedulerPresentActive);schedulerPresentActive=1;
 ++schedulerPresentRequests;
 if(primaryView==GAME_VIEW_DUNGEON)++schedulerDungeonRequests;
 else ++schedulerOverlayViewRequests;
 ticks=scheduler_present_resident(p,g,mode);
 if(primaryView==GAME_VIEW_DUNGEON)assert(schedulerFlips==(Byte)(before+1));
 else assert(schedulerFlips==before); /* dodcmd cannot be nested for EXAMINE. */
 schedulerPresentActive=0;return ticks;
}
Byte scheduler_progress_gateway(void *p){return scheduler_progress_resident(p);}
int main(void){
 char *args[]={"dodintro","3"};
 assert(intro_main(2,args)==0);assert(commands==10&&firstExamine);
 /* The ten command-completion flips remain, plus one resident flip for each
 * source LUKNEW/PUPDAT request whose selected view is resident. EXAMINE is
 * dodcmd-owned and therefore cannot be nested while dodsched owns the slot. */
 assert(schedulerPresentRequests==9&&schedulerDungeonRequests==7&&schedulerOverlayViewRequests==2);
 assert(schedulerPresentRequests==(Byte)(schedulerDungeonRequests+schedulerOverlayViewRequests));
 assert(schedulerFlips==schedulerDungeonRequests);
 assert(flips==(Byte)(10+schedulerDungeonRequests));
 assert(game.level==2&&game.row==9&&game.col==22&&game.dir==0);
 assert(game.power==6136&&!game.creatures[18][12]);
 assert(primaryView==GAME_VIEW_DUNGEON);
 printf("public M5: %u LUKNEW/PUPDAT requests, %u resident redraws, %u overlay-view deferrals\n",
        schedulerPresentRequests,schedulerDungeonRequests,schedulerOverlayViewRequests);
 puts("public M5: real AUTTAB 1-10, EXAMINE transcript, one-slot calls and cleanup pass");
 return 0;
}
