/*
 * Bounded autonomous demonstration of existing production Daggorath systems.
 * The command stream is TOKEN.ASM:AUTTAB entries 1..10, executed through the
 * retained dodcmd and dodsched modules.  It deliberately stops at Combat M1's
 * validated boundary; it is not a replacement player loop or a visual fixture.
 */
#include <cmoc.h>
#include "game.h"
#include "overlay-api.h"
#include "scheduler-api.h"
#include "scheduler-callbacks.h"
#include "presentation.h"
#include "logical.h"
#include "native_heartbeat.h"
#include "audio.h"

static Game game;
static GameTiming timing;
static CreatureScheduler creatures;
static DagSchedulerState schedulerState;
static DagSchedulerCallbackContext schedulerCallbacks;
static DagSchedulerContextV1 schedulerContext;
static DagSchedulerServices schedulerServices;
static Byte frame[FRAME_BYTES],signalBytes[2];
static NativeHeartbeat heartbeat={255,0};
static NativeHeartbeatState heartbeatState;
static unsigned long shownGeneration;
#ifdef _CMOC_VERSION_
static AudioQueue audioQueue;
#endif
static Byte heartbeatActive;

/* doddemo uses the same native-edge reconciliation as dodgame.  Rendering
 * and strip expansion are foreground work, so a progress point may replace
 * only the already-displayed two heart glyphs.  It never advances phase or
 * uploads an incomplete dungeon frame. */
static Byte refresh_heart(void);
static Byte render_heart_progress(Game *g,Byte *partial,void *context);
static Byte flip_heart_progress(void *context);

Byte scheduler_task_resident(void *opaque,Game *g,GameTiming *t,CreatureScheduler *q,
                           Byte task,Byte ccb,Byte *dirty){
 (void)opaque;(void)t;(void)q;(void)ccb;*dirty=0;
 if(task==DOD_TASK_HEALTH){game_health(g);return 0;}
 return 0;
}
Word scheduler_present_resident(void *opaque,Game *g,Byte mode){
 (void)opaque;(void)g;(void)mode;
 /* The portable scheduler intentionally does not charge CoWin wall time. */
 return 0;
}
/* DOD_SCHED_PLAYER_WAIT advances the recovered 81 CLOCK jiffies for each
 * AUTTAB word.  CLOCK updates HEARTS independently of PLAYER, so service the
 * VIRQ-owned generation through the same resident gateway on each scheduler
 * jiffy once the native heart is active. */
Byte scheduler_progress_resident(void *opaque){(void)opaque;return refresh_heart();}
static Byte refresh_heart(void){
 Byte e;if(!heartbeatActive)return 0;
 e=native_heartbeat_snapshot(&heartbeat,&heartbeatState);
 if(e)return e;if(heartbeatState.fault)return heartbeatState.fault;
 if(heartbeatState.edgeGeneration!=shownGeneration){
  game_render_heart(frame,heartbeatState.phase);
  e=screen_present_heart(frame);if(e)return e;
  shownGeneration=heartbeatState.edgeGeneration;
 }
#ifdef _CMOC_VERSION_
 audio_queue_progress(&audioQueue,os_signal_value(signalBytes+1));
#endif
 return 0;
}
static Byte render_heart_progress(Game *g,Byte *partial,void *context){
 (void)g;(void)partial;(void)context;
 return refresh_heart();
}
static Byte flip_heart_progress(void *context){(void)context;return refresh_heart();}
/* A completed logical scene has phase zero in its template.  Before a full
 * flip, overwrite its status cells from the authoritative driver snapshot;
 * then service new generations between the bounded strips and once after the
 * flip.  This prevents a late full-frame flip from restoring an old heart. */
static Byte present_scene(void){
 Byte e,phase;unsigned long generation;
 if(!heartbeatActive)return screen_present(frame);
 e=native_heartbeat_snapshot(&heartbeat,&heartbeatState);if(e)return e;
 if(heartbeatState.fault)return heartbeatState.fault;
 generation=heartbeatState.edgeGeneration;phase=heartbeatState.phase;
 e=game_render_status_progress(&game,frame,phase,render_heart_progress,0);if(e)return e;
 e=screen_prepare_progress(frame,flip_heart_progress,0);if(e)return e;
 e=screen_flip();if(e)return e;
 if(shownGeneration<generation)shownGeneration=generation;
 return refresh_heart();
}
/* Source logical waits are already represented by DOD_SCHED_PLAYER_WAIT. This
 * short visual dwell is only to make each completed state observable; it never
 * advances Game, scheduler or RNG state. */
static Byte dwell(Word ticks){Byte e;
 while(ticks--){e=os_signal_value(signalBytes);if(e)return e;e=os_sleep(1);if(e)return e;
  e=refresh_heart();if(e)return e;}
 return 0;
}
static Byte render_dungeon(const char *message){
 Byte e=game_render_with_progress(&game,frame,"",message,render_heart_progress,0);
 if(e)return e;
 return present_scene();
}
static Byte render_examine(const char *message){
 Byte e=game_overlay_examine(&game,frame,"",message);
 if(e)return e;
 return present_scene();
}
static void present_audio(const Byte *events,Byte count){
#ifdef _CMOC_VERSION_
 audio_queue_admit(&audioQueue,events,count);
 audio_queue_progress(&audioQueue,os_signal_value(signalBytes+1));
#else
 (void)events;(void)count;
#endif
}
static Byte scheduler(Byte operation,Word jiffies){
 schedulerContext.operation=operation;schedulerContext.logicalJiffies=jiffies;
 return game_scheduler_call(&schedulerContext);
}
static Byte command(const char *text,Byte words){
 GameCombat combat;Byte e,result,view;const char *message;Byte i;
 e=game_overlay_command(&game,text,&combat,&result,&view,&message);if(e)return e;
 if(result!=GAME_OK)return ERR_ARGUMENT;
 /* TOKEN.ASM:HUMAN waits once per completed word before the next source
  * scheduler boundary. This is the real portable AUTTAB contract. */
 for(i=0;i<words;i++){e=scheduler(DOD_SCHED_PLAYER_WAIT,0);if(e)return e;}
 e=scheduler(DOD_SCHED_BOUNDARY,0);if(e)return e;
 if(creatures.audioCount){present_audio(creatures.audio,creatures.audioCount);creatures.audioCount=0;}
 if(combat.eventCount)present_audio(combat.events,combat.eventCount);
 e=native_heartbeat_rate(&heartbeat,game.rate);if(e)return e;
 e=view==GAME_VIEW_EXAMINE?render_examine(message):render_dungeon(message);if(e)return e;
 return dwell(36);
}
static Byte attract_text(const char *a,const char *b,const char *c,const char *d,Word ticks){
 Byte e;game_render_attract(frame,a,b,c,d);e=screen_present(frame);if(e)return e;return dwell(ticks);
}
int main(void){Byte e=0,r;Byte heartPatterns[28];
 static const char *commands[]={
  "EXAMINE","PULL RIGHT TORCH","USE RIGHT","LOOK","MOVE",
  "PULL LEFT SHIELD","PULL RIGHT SWORD","MOVE","MOVE","ATTACK RIGHT"};
 static const Byte words[]={1,3,2,1,1,3,3,1,1,2};Byte i;
 e=os_intercept_audio(signalBytes);if(e)return e;
 game_init_demo(&game,21);
 schedulerServices.version=DOD_SCHEDULER_ABI_V1;schedulerServices.size=sizeof(schedulerServices);
 scheduler_callback_init(&schedulerCallbacks,0);schedulerServices.opaque=&schedulerCallbacks;
 schedulerServices.task=scheduler_task_gateway;
 schedulerServices.present=scheduler_present_gateway;schedulerServices.progress=0;
 memset(&schedulerContext,0,sizeof(schedulerContext));
 schedulerContext.abiVersion=DOD_SCHEDULER_ABI_V1;schedulerContext.contextSize=sizeof(schedulerContext);
 schedulerContext.game=&game;schedulerContext.timing=&timing;schedulerContext.creatures=&creatures;
 schedulerContext.state=&schedulerState;schedulerContext.services=&schedulerServices;
 game_heart_patterns(heartPatterns);screen_set_heart_patterns(heartPatterns);
 e=screen_open();if(e)goto done;
 e=game_overlay_open();if(e)goto done;
 e=game_scheduler_open();if(e)goto done;
 e=scheduler(DOD_SCHED_INIT,0);if(e)goto done;
#ifdef _CMOC_VERSION_
 audio_queue_open(&audioQueue,"/d1/dodaudio","ssc-mame-fast");
#endif
 /* ONCE.ASM: GAME30 -> PREPAR, then GAME40 demo initialization/map boundary.
  * These source strings occupy the source primary-text rows. MISC:PROMPX's
  * CR-dot is the separate interactive command prompt, not an intro prefix. */
 /* ONCE:DEMO10's first expanded message begins with CR, so it occupies
  * TXTPRI row 1.  The second continues on row 2; punctuation is SWCHAR data. */
 e=attract_text("","I DARE YE ENTER...","...THE DUNGEONS OF DAGGORATH!!!","",90);if(e)goto done;
 /* MISC:PREPAX writes TXTEXA cell (12,9), not the primary text area. */
 game_render_prepare(frame);e=screen_present(frame);if(e)goto done;e=dwell(60);if(e)goto done;
 e=scheduler(DOD_SCHED_BOUNDARY,0);if(e)goto done;
 e=render_dungeon(".");if(e)goto done;
 /* PLOOK:INIVUX enables the source heartbeat only after the first complete
  * demo dungeon state is available. The verified launcher preloads dhbpack,
  * exactly as it does for normal dodgame execution. */
 e=native_heartbeat_open(&heartbeat);if(e)goto done;
 e=native_heartbeat_rate(&heartbeat,game.rate);if(e)goto done;
 e=native_heartbeat_enable(&heartbeat);if(e)goto done;
 heartbeatActive=1;
 /* A mapped scheduler callback must enter the resident CMOC context through
  * the established gateway.  This is foreground reconciliation only: it
  * snapshots the one native VIRQ state and never advances a visual timer. */
 schedulerServices.progress=scheduler_progress_gateway;
 e=refresh_heart();if(e)goto done;
 for(i=0;i<sizeof(commands)/sizeof(commands[0]);i++){e=command(commands[i],words[i]);if(e)goto done;}
 /* Stable post-combat frame: the ordinary Combat M1 state, not a patched pose. */
 e=render_dungeon("STONE GIANT DEFEATED");if(e)goto done;
 e=dwell(120);
done:
 heartbeatActive=0;r=native_heartbeat_close(&heartbeat);if(!e)e=r;
 r=game_scheduler_close();if(!e)e=r;
 r=game_overlay_close();if(!e)e=r;
 r=screen_close();if(!e)e=r;
#ifdef _CMOC_VERSION_
 audio_queue_close(&audioQueue);
#endif
 return e;
}
