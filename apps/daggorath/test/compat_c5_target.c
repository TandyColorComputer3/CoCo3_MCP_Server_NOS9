#include <cmoc.h>
#include "compat/c5.h"
#include "compat/present.h"
#include "gameplay/overlay-api.h"
#include "gameplay/overlay-callbacks.h"
#include "gameplay/scheduler-api.h"
#include "gameplay/scheduler-callbacks.h"
#include "gameplay/opening-heartbeat.h"
#include "audio/native_heartbeat.h"

static Byte stateStorage[DAG_COMPAT_STATE_BYTES+255],frame[DAG_COMPAT_FRAME_BYTES];
static Byte primaryPixels[32*32],creatureUse[32];
static Game game;static GameTiming timing;static CreatureScheduler creatures;
static DagSchedulerState schedulerState;static DagSchedulerContextV1 schedulerContext;
static DagSchedulerCallbackContext callbackContext;static DagSchedulerServices schedulerServices;
static DagCompatC5 c5;static Byte currentView=GAME_VIEW_DUNGEON,presentationError;
static NativeHeartbeat heartbeat={255,0};static NativeHeartbeatState heartbeatState;
static unsigned long shownGeneration;static Byte heartbeatActive,heartPixelsReady,observedAudioBit,audioBitValid;
Byte game_input(Byte,Byte *);

/* COMMON.ASM:CLK30 and the native VIRQ toggle PIA $FF22 bit 1 at the same
 * edge. This verified production gate makes frequent renderer checkpoints
 * cheap; the atomic driver snapshot remains authoritative after a change. */
static Byte heartbeat_audio_bit(void){return (*(volatile Byte *)0xff22)&2;}
static Byte refresh_heart(void){Byte e,attempt,bit;
 if(!heartbeatActive)return 0;
 /* PB1 audio may begin while the first scene is being constructed, but a
  * heart-only PutBlk cannot precede the complete status strip that contains
  * it. The first full render installs both atomically from the viewer's
  * perspective; subsequent native edges use the bounded heart buffers. */
 if(!heartPixelsReady)return 0;
 bit=heartbeat_audio_bit();if(audioBitValid&&bit==observedAudioBit)return 0;
 /* The same native edge drives PB1 and the visual phase. An edge can occur
  * during the small synchronous PutBlk, so reconcile again before returning
  * rather than leaving sound and pixels one generation apart. */
 for(attempt=0;attempt<3;attempt++){
  e=native_heartbeat_snapshot(&heartbeat,&heartbeatState);if(e)return e;
  if(heartbeatState.fault)return heartbeatState.fault;
  if(heartbeatState.edgeGeneration==shownGeneration){
   observedAudioBit=heartbeat_audio_bit();audioBitValid=1;return 0;
  }
  game_render_heart(frame,heartbeatState.phase);
  e=dag_compat_present_heart(frame);if(e)return e;
  shownGeneration=heartbeatState.edgeGeneration;
 }
 /* A fourth edge is picked up by the next bounded checkpoint. Do not arm
  * the cheap bit gate until a stable generation has been observed. */
 audioBitValid=0;
 return 0;
}
static Byte render_progress(Game *g,Byte *partial,void *context)
{(void)g;(void)partial;(void)context;return refresh_heart();}
static Byte present_progress(void *context){(void)context;return refresh_heart();}
static Byte sleep_with_heart(Word ticks){Byte e;
 while(ticks--){e=os_sleep(1);if(e)return e;e=refresh_heart();if(e)return e;}
 return 0;
}

static Byte render_scene(void){DagCompatPresentTiming pt;DagCompatRenderContext rc;Byte i,e,*state=c5.originalState;
 memcpy(primaryPixels,frame+160*32,sizeof(primaryPixels));
 for(i=0;i<32;i++){creatureUse[i]=game.creatures[i][12];if(game.creatures[i][13]==5)game.creatures[i][12]=0;}
 e=game_render_compat_with_progress(&game,frame,"","",render_progress,0,state);
 for(i=0;i<32;i++)game.creatures[i][12]=creatureUse[i];if(e)return e;
 memcpy(frame+160*32,primaryPixels,sizeof(primaryPixels));
 rc.state=state;rc.logicalFrame=frame;
 for(i=0;i<10;i++){dag_compat_c5_prepare_view(&game,state,i);
  if(dag_compat_c5_type5_at(&game,i,0)){rc.mode=DAG_COMPAT_GIANT_FORWARD;e=dag_compat_render_giant(&rc);if(e)return e;}
  if(dag_compat_c5_type5_at(&game,i,1)){rc.mode=DAG_COMPAT_GIANT_RIGHT_PEEK;e=dag_compat_render_giant(&rc);if(e)return e;}
 }
 if(heartbeatActive){e=native_heartbeat_snapshot(&heartbeat,&heartbeatState);if(e)return e;
  if(heartbeatState.fault)return heartbeatState.fault;game_render_heart(frame,heartbeatState.phase);}
 e=dag_compat_present_progress(frame,&pt,present_progress,0);if(e)return e;
 if(heartbeatActive)shownGeneration=heartbeatState.edgeGeneration;
 return refresh_heart();
}
static Byte render_examine(void){DagCompatPresentTiming pt;Byte e;
 e=game_overlay_examine(&game,frame,"","");if(e)return e;
 memcpy(frame+160*32,primaryPixels,sizeof(primaryPixels));
 if(heartbeatActive){e=native_heartbeat_snapshot(&heartbeat,&heartbeatState);if(e)return e;
  if(heartbeatState.fault)return heartbeatState.fault;game_render_heart(frame,heartbeatState.phase);}
 e=dag_compat_present_progress(frame,&pt,present_progress,0);if(e)return e;
 if(heartbeatActive)shownGeneration=heartbeatState.edgeGeneration;
 return refresh_heart();
}
Byte scheduler_task_resident(void *p,Game *g,GameTiming *t,CreatureScheduler *q,Byte task,Byte ccb,Byte *dirty){
 (void)p;(void)t;(void)q;(void)ccb;*dirty=0;if(task==DOD_TASK_HEALTH)game_health(g);return 0;
}
Word scheduler_present_resident(void *p,Game *g,Byte mode){(void)p;(void)g;(void)mode;
 if(currentView==GAME_VIEW_DUNGEON&&!presentationError)presentationError=render_scene();return 0;
}
Byte scheduler_progress_resident(void *p){(void)p;return refresh_heart();}
Byte gameplay_overlay_progress(void){return refresh_heart();}
static Byte scheduler_call(DagSchedulerContextV1 *c,Byte op,Word ticks){Byte e;
 c->operation=op;c->logicalJiffies=ticks;e=game_scheduler_call(c);if(e)return e;
 e=presentationError;presentationError=0;if(e)return e;
 /* MISC.ASM:WAITX consumes 81 real 60-Hz jiffies as well as CLOCK work. */
 return op==DOD_SCHED_PLAYER_WAIT?sleep_with_heart(81):refresh_heart();
}
static Byte word_present(Game *g,Byte *f,const char *line,Byte length){DagCompatPresentTiming pt;
 (void)g;(void)line;(void)length;
 return dag_compat_present_primary_progress(f,&pt,present_progress,0);
}
static Byte command_call(Game *g,const char *text,GameCombat *combat,Byte *result,Byte *view){const char *message;Byte e;
 e=game_overlay_command(g,text,combat,result,view,&message);if(!e&&*result!=GAME_OK)return ERR_ARGUMENT;
 if(!e&&*view)currentView=*view;return e;
}
int main(void){DagCompatC5Services services={command_call,scheduler_call,word_present};Byte *state=(Byte*)(((Word)stateStorage+255)&0xff00),e=0,r,i,key,completed=0;
 Byte heartPatterns[28];
 dag_compat_c4_init(&game,state,21);dag_compat_c4_initial(&game,frame);dag_compat_c4_prompt(frame);
 game_heart_patterns(heartPatterns);dag_compat_present_set_hearts(heartPatterns);
 e=dag_compat_present_open();if(e)goto done;
 schedulerServices.version=DOD_SCHEDULER_ABI_V1;schedulerServices.size=sizeof(schedulerServices);
 scheduler_callback_init(&callbackContext,0);schedulerServices.opaque=&callbackContext;
 schedulerServices.task=scheduler_task_gateway;schedulerServices.present=scheduler_present_gateway;schedulerServices.progress=0;
 memset(&schedulerContext,0,sizeof(schedulerContext));schedulerContext.abiVersion=DOD_SCHEDULER_ABI_V1;
 schedulerContext.contextSize=sizeof(schedulerContext);schedulerContext.game=&game;schedulerContext.timing=&timing;
 schedulerContext.creatures=&creatures;schedulerContext.state=&schedulerState;schedulerContext.services=&schedulerServices;
 e=game_overlay_open();if(e)goto done;e=game_scheduler_open();if(e)goto done;
 e=scheduler_call(&schedulerContext,DOD_SCHED_INIT,0);if(e)goto done;
 e=scheduler_call(&schedulerContext,DOD_SCHED_BOUNDARY,0);if(e)goto done;
 dag_compat_c5_init(&c5,&game,state,frame,&schedulerContext,&services);
	 /* COMMON:CLK30 is already active when the cartridge constructs its first
	  * dungeon view. Start the native edge source before the EOU renderer so
	  * initial construction cannot create a silent, frozen-heart interval. */
	 e=opening_heartbeat_modules_open();if(e)goto done;
	 e=native_heartbeat_open(&heartbeat);if(e)goto done;
	 e=native_heartbeat_rate(&heartbeat,game.rate);if(e)goto done;
	 e=native_heartbeat_enable(&heartbeat);if(e)goto done;
	 heartbeatActive=1;schedulerServices.progress=scheduler_progress_gateway;
	 e=refresh_heart();if(e)goto done;
	 e=render_scene();if(e)goto done;
	 heartPixelsReady=1;e=refresh_heart();if(e)goto done;
 for(i=0;i<8;i++){e=dag_compat_c5_autoplay(&c5,i);if(e)goto done;
  e=native_heartbeat_rate(&heartbeat,game.rate);if(e)goto done;
  memcpy(primaryPixels,frame+160*32,sizeof(primaryPixels));
  e=currentView==GAME_VIEW_EXAMINE?render_examine():render_scene();if(e)goto done;
 }
 /* PLAYER's non-AUTFLG branch now owns the same private /w path as the
  * presenter. Characters reach HUMAN one at a time while CLOCK/SCHED keep
  * creatures alive; the first completed human command is C5's stop bound. */
 while(!completed){e=game_input(dag_compat_present_path(),&key);if(e)goto done;
  if(key){e=dag_compat_c5_character(&c5,key,DAG_COMPAT_C5_HUMAN);if(e)goto done;
   if(key==13){e=currentView==GAME_VIEW_EXAMINE?render_examine():render_scene();if(e)goto done;completed=1;}}
  else {e=scheduler_call(&schedulerContext,DOD_SCHED_NORMAL_TICKS,1);if(e)goto done;e=sleep_with_heart(1);if(e)goto done;}
 }
 e=sleep_with_heart(180);
done:heartPixelsReady=heartbeatActive=0;r=native_heartbeat_close(&heartbeat);if(!e)e=r;
 if(!heartbeat.opened){r=opening_heartbeat_modules_close();if(!e)e=r;}
 r=game_scheduler_close();if(!e)e=r;r=game_overlay_close();if(!e)e=r;
 r=dag_compat_present_close();if(!e)e=r;return e;
}
