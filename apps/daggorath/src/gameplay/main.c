#include <cmoc.h>
#include "game.h"
#include "presentation.h"
#include "logical.h"
#include "native_heartbeat.h"
Byte game_input(Byte path,Byte *key);
static Game game;
static CreatureScheduler creatureScheduler;
static Byte frame[FRAME_BYTES],signalFlag;
static Byte inputUnderlay[GAME_INPUT_BYTES];
static NativeHeartbeat heartbeat={255,0};
static NativeHeartbeatState heartbeatState;
static unsigned long presentedGeneration;
static unsigned long maximumPresentationLag;
static unsigned long heartPresentations;
static Byte phaseRefreshNeeded;
static Byte presentation_lag(Byte shownPhase){
 unsigned long lag;Byte e=native_heartbeat_snapshot(&heartbeat,&heartbeatState);if(e)return e;
 if(heartbeatState.fault)return heartbeatState.fault;
 lag=heartbeatState.edgeGeneration-presentedGeneration;
 if(lag>maximumPresentationLag)maximumPresentationLag=lag;
 phaseRefreshNeeded=(lag!=0||heartbeatState.phase!=shownPhase);return 0;
}
static Byte present_ui(Byte *frame,const char *input,const Byte *underlay,Byte *shownPhase){
 Byte e=native_heartbeat_snapshot(&heartbeat,&heartbeatState);if(e)return e;
 if(heartbeatState.fault)return heartbeatState.fault;
 game_render_status(&game,frame,heartbeatState.phase);
 game_render_input(frame,input,underlay);
 e=screen_present_ui(frame);if(!e){*shownPhase=heartbeatState.phase;presentedGeneration=heartbeatState.edgeGeneration;}
 if(!e)e=presentation_lag(*shownPhase);
 return e;
}
typedef struct { Byte *shownPhase; } RenderProgressContext;
/* COMMON.ASM:CLK30 changes HEARTS at the same native edge as PB1. A long
 * VIEWER redraw is foreground work; this callback only replaces the two
 * glyph columns in the last complete mapped screen. It never presents the
 * partial dungeon or advances an independent visual heartbeat. */
static Byte render_heart_progress(Game *g,Byte *partial,void *context){
 RenderProgressContext *p=(RenderProgressContext *)context;Byte e;
 (void)g;
 e=os_signal_value(&signalFlag);if(e)return e;
 e=native_heartbeat_snapshot(&heartbeat,&heartbeatState);if(e)return e;
 if(heartbeatState.fault)return heartbeatState.fault;
 if(heartbeatState.edgeGeneration==presentedGeneration)return 0;
 game_render_heart(partial,heartbeatState.phase);
 e=screen_present_heart(partial);if(e)return e;
 *p->shownPhase=heartbeatState.phase;presentedGeneration=heartbeatState.edgeGeneration;
 ++heartPresentations;return presentation_lag(*p->shownPhase);
}
/* PULL/STOW/GET/DROP use original token rules; other commands retain M1 adapters.
 * EXIT remains an isolated OS-9-only lifecycle command. */
int main(int argc,char **argv){Byte e=0,r,key,n=0,dirty=1,result,oldrate,oldfaint,oldlight,shownPhase=255,view=GAME_VIEW_DUNGEON,nextView,secondPhase=0,frameRemainder,drained,inputEmpty;Word previous,videoTicks,seconds;char input[32];RenderProgressContext renderContext;const char *message="TURN LEFT RIGHT AROUND  MOVE";
 if(argc>2||(argc==2&&strcmp(argv[1],"seed0")))return ERR_ARGUMENT;
 presentedGeneration=maximumPresentationLag=heartPresentations=0;phaseRefreshNeeded=0;
 memset(&heartbeatState,0,sizeof(heartbeatState));
 e=os_intercept(&signalFlag);if(e)return e;
 e=os_clock(&previous,1);if(e)return e;
 /* Production retains the original post-maze time perturbation. seed0 is
  * an explicit deterministic test mode; neither changes LVLTAB maze seeds. */
 game_init(&game,argc==2?0:previous/60);game_creature_init(&game,&creatureScheduler);input[0]=0;renderContext.shownPhase=&shownPhase;
 e=screen_open();if(e)goto done;
 /* INIVUX: rate already computed; activation starts with remaining=1.
  * All module loads and graphics allocation precede the native claim. */
 e=os_clock(&previous,1);if(e)goto done;e=native_heartbeat_open(&heartbeat);if(e)goto done;
 e=native_heartbeat_rate(&heartbeat,game.rate);if(e)goto done;e=native_heartbeat_enable(&heartbeat);if(e)goto done;
 /* Begin a fresh simulation epoch after initialization and the existing
  * video-tick service is active; discard setup frames before this point. */
 e=native_heartbeat_take_ticks(&heartbeat,&videoTicks);if(e)goto done;
 for(;;){
  e=os_signal_value(&signalFlag);if(e)break;
  oldrate=game.rate;oldfaint=game.faint;oldlight=game.torch?game.objects[(game.torch-0x0b15)/14][7]:0;
  e=native_heartbeat_take_ticks(&heartbeat,&videoTicks);if(e)break;
  seconds=videoTicks/60;frameRemainder=videoTicks%60;secondPhase+=frameRemainder;
  if(secondPhase>=60){++seconds;secondPhase-=60;}
  if(seconds)game_tick(&game,seconds);
  if(game_creature_advance(&game,&creatureScheduler,videoTicks))dirty=1;
  if(game.rate!=oldrate){e=native_heartbeat_rate(&heartbeat,game.rate);if(e)break;}
  if(oldfaint!=game.faint||oldlight!=(game.torch?game.objects[(game.torch-0x0b15)/14][7]:0))dirty=1;
  if(game.dead){message="PLAYER DIED  EXITING";dirty=1;}
  /* Present simulation/scene changes before accepting another text byte, so
   * every subsequently consumed character can use the fast UI-only path. */
  if(dirty==1){
   if(view==GAME_VIEW_EXAMINE)game_render_examine(&game,frame,"",message);
   else {e=game_render_with_progress(&game,frame,"",message,render_heart_progress,&renderContext);if(e)break;}
   memcpy(inputUnderlay,frame+GAME_INPUT_OFFSET,GAME_INPUT_BYTES);game_render_input(frame,input,inputUnderlay);
   e=screen_prepare(frame);if(e)break;
   e=native_heartbeat_snapshot(&heartbeat,&heartbeatState);if(e)break;
   if(heartbeatState.fault){e=heartbeatState.fault;break;}
   game_render_status(&game,frame,heartbeatState.phase);game_render_input(frame,input,inputUnderlay);
   e=screen_prepare_ui(frame);if(e)break;e=screen_flip();if(e)break;
   shownPhase=heartbeatState.phase;presentedGeneration=heartbeatState.edgeGeneration;
   e=presentation_lag(shownPhase);if(e)break;dirty=phaseRefreshNeeded?2:0;
  }
  /* HUMAN/PLAYER echoes each character as it is consumed and only yields
   * one jiffy after the queue is empty. Reuse the current dungeon pixels and
   * refresh only the owned status/input rows per character; do not wait for
   * the host's entire MCP key burst before making text visible. */
  inputEmpty=0;
  for(drained=0;drained<32;drained++){
   e=os_signal_value(&signalFlag);if(e)break;
   e=game_input(screen_path(),&key);if(e||!key){inputEmpty=1;break;}
   if(key==13){input[n]=0;if(!strcmp(input,"EXIT"))break;
    result=game_command(&game,input);nextView=game_display_command(input);if(result==GAME_OK&&nextView)view=nextView;message=game_message(input,result);
    e=native_heartbeat_rate(&heartbeat,game.rate);if(e)break;n=0;input[0]=0;
   }else if(key==8){if(n)input[--n]=0;}
   else if(key>=32&&key<=126&&n<31){if(key>='a'&&key<='z')key-=32;input[n++]=key;input[n]=0;}
   /* Enter changes game/message state and requires a full scene render.
    * Ordinary accepted characters immediately update the small UI region. */
   if(key==13){dirty=1;break;}
   if(dirty!=1){dirty=2;e=present_ui(frame,input,inputUnderlay,&shownPhase);if(e)break;dirty=phaseRefreshNeeded?2:0;}
  }
  if(e)break;
  if(key==13&&!strcmp(input,"EXIT"))break;
  /* COMMON:CLK30 toggles HEARTS at the actual output edge. Query the
   * driver's latched phase; never derive a second heartbeat from wall time.
   * A heart-only change reuses every dungeon/input pixel. */
  e=native_heartbeat_snapshot(&heartbeat,&heartbeatState);if(e)break;
  if(heartbeatState.fault){e=heartbeatState.fault;break;}
  if(!dirty&&(shownPhase!=heartbeatState.phase||presentedGeneration!=heartbeatState.edgeGeneration))dirty=2;
  if(dirty){
   if(dirty==1){if(view==GAME_VIEW_EXAMINE)game_render_examine(&game,frame,"",message);
    else {e=game_render_with_progress(&game,frame,"",message,render_heart_progress,&renderContext);if(e)break;}
    memcpy(inputUnderlay,frame+GAME_INPUT_OFFSET,GAME_INPUT_BYTES);game_render_input(frame,input,inputUnderlay);e=screen_prepare(frame);if(e)break;}
   /* Full scene rendering/packing may span heartbeat edges. Sample the
    * authoritative VIRQ phase after that work and refresh its rows last. */
   e=native_heartbeat_snapshot(&heartbeat,&heartbeatState);if(e)break;
   if(heartbeatState.fault){e=heartbeatState.fault;break;}
   game_render_status(&game,frame,heartbeatState.phase);shownPhase=heartbeatState.phase;
   game_render_input(frame,input,inputUnderlay);
   e=screen_prepare_ui(frame);if(e)break;e=screen_flip();if(e)break;
   presentedGeneration=heartbeatState.edgeGeneration;e=presentation_lag(shownPhase);if(e)break;
   dirty=phaseRefreshNeeded?2:0;
  }
  if(game.dead)break;
  if(inputEmpty){e=os_sleep(1);if(e)break;}
 }
 done:r=native_heartbeat_close(&heartbeat);if(!e)e=r;r=screen_close();if(!e)e=r;
 printf("DODGAME HEARTBEAT EDGE %lu PRESENTED %lu MAX_PHASE_LAG %lu HEART_PRESENTS %lu\r",heartbeatState.edgeGeneration,presentedGeneration,maximumPresentationLag,heartPresentations);
 printf("DODGAME TERM RESTORED ROW %u COL %u DIR %u RATE %u STATUS %u\r",game.row,game.col,game.dir,game.rate,e);
 return e;
}
