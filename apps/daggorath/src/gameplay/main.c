#include <cmoc.h>
#include "game.h"
#include "presentation.h"
#include "logical.h"
#include "native_heartbeat.h"
#include "audio.h"
#ifdef DOD_COMMAND_OVERLAY
#include "overlay-api.h"
#endif
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
static Byte phaseRefreshNeeded,audioBitValid,observedAudioBit;
#ifdef _CMOC_VERSION_
static AudioClient combatAudio;
static Byte combatAudioState;
#endif
typedef struct { Byte *shownPhase; } RenderProgressContext;
static Byte render_heart_progress(Game *g,Byte *partial,void *context);
static Byte creature_heart_progress(void *context);
/* Original COMMON.ASM:CLK30 and the native DHeartbeat VIRQ both toggle PIA
 * $FF22 bit 1 at the authoritative edge. Foreground code may cheaply notice
 * that transition, but it never derives phase/generation from the bit: a
 * changed bit only gates the atomic driver snapshot below. */
static Byte heartbeat_audio_bit(void){
#ifdef _CMOC_VERSION_
 return (*(volatile Byte *)0xff22)&2;
#else
 return 255; /* Host lifecycle tests retain the full snapshot path. */
#endif
}
/* A completed full PutBlk can race a VIRQ edge. Foreground reconciliation
 * always follows the newest atomic driver snapshot; intermediate phases are
 * not queued. It also patches the mapped full buffer, so the next full flip
 * cannot revive an older heart. COMMON.ASM:CLK30, COMTXT.ASM:TXTDPB. */
static Byte present_latest_heart(Byte *frame,Byte *shownPhase){Byte e,attempt,bit=heartbeat_audio_bit();
 if(audioBitValid&&bit!=255&&bit==observedAudioBit)return 0;
 for(attempt=0;attempt<3;attempt++){
  e=native_heartbeat_snapshot(&heartbeat,&heartbeatState);if(e)return e;
  if(heartbeatState.fault)return heartbeatState.fault;
  /* Phase changes only at an edge generation. The initial sentinel phase
   * does not request a heart PutBlk before the first complete scene. */
  if(heartbeatState.edgeGeneration==presentedGeneration){
   if(bit!=255){observedAudioBit=bit;audioBitValid=1;}return 0;
  }
  game_render_heart(frame,heartbeatState.phase);
  e=screen_present_heart(frame);if(e)return e;
  *shownPhase=heartbeatState.phase;presentedGeneration=heartbeatState.edgeGeneration;
  ++heartPresentations;
  bit=heartbeat_audio_bit();if(bit!=255){observedAudioBit=bit;audioBitValid=1;}
 }
 return 0; /* A newer edge is picked up at the next bounded service point. */
}
static Byte presentation_lag(Byte shownPhase){
 unsigned long lag;Byte e=native_heartbeat_snapshot(&heartbeat,&heartbeatState);if(e)return e;
 if(heartbeatState.fault)return heartbeatState.fault;
 lag=heartbeatState.edgeGeneration-presentedGeneration;
 if(lag>maximumPresentationLag)maximumPresentationLag=lag;
 phaseRefreshNeeded=(lag!=0||heartbeatState.phase!=shownPhase);return 0;
}
static Byte present_ui(Byte *frame,const char *input,const Byte *underlay,Byte *shownPhase){
 RenderProgressContext context={shownPhase};
 unsigned long frameGeneration;Byte framePhase;
 Byte e=native_heartbeat_snapshot(&heartbeat,&heartbeatState);if(e)return e;
 if(heartbeatState.fault)return heartbeatState.fault;
 frameGeneration=heartbeatState.edgeGeneration;framePhase=heartbeatState.phase;
 e=game_render_status_progress(&game,frame,heartbeatState.phase,
                               render_heart_progress,&context);if(e)return e;
 e=game_render_input_progress(&game,frame,input,underlay,
                              render_heart_progress,&context);if(e)return e;
 *shownPhase=framePhase;
 e=screen_present_ui_progress(frame,creature_heart_progress,&context);
 if(!e&&presentedGeneration<frameGeneration)presentedGeneration=frameGeneration;
 if(!e)e=present_latest_heart(frame,shownPhase);
 if(!e)e=presentation_lag(*shownPhase);
 return e;
}
/* COMMON.ASM:CLK30 changes HEARTS at the same native edge as PB1. A long
 * VIEWER redraw is foreground work; this callback only replaces the two
 * glyph columns in the last complete mapped screen. It never presents the
 * partial dungeon or advances an independent visual heartbeat. */
static Byte render_heart_progress(Game *g,Byte *partial,void *context){
 RenderProgressContext *p=(RenderProgressContext *)context;Byte e;
 (void)g;
 e=os_signal_value(&signalFlag);if(e)return e;
 e=present_latest_heart(partial,p->shownPhase);if(e)return e;
 return presentation_lag(*p->shownPhase);
}
static Byte creature_heart_progress(void *context){
 RenderProgressContext *p=(RenderProgressContext *)context;
 return render_heart_progress(&game,frame,p);
}
/* PULL/STOW/GET/DROP use original token rules; other commands retain M1 adapters.
 * EXIT remains an isolated OS-9-only lifecycle command. */
int main(int argc,char **argv){Byte e=0,r,key,n=0,dirty=1,result,oldrate,oldfaint,oldlight,shownPhase=255,framePhase,view=GAME_VIEW_DUNGEON,nextView,secondPhase=0,frameRemainder,drained,inputEmpty,heartPatterns[28];Word previous,videoTicks,seconds;unsigned long frameGeneration;char input[32];RenderProgressContext renderContext;GameCombat combat;const char *message="TURN LEFT RIGHT AROUND  MOVE";
 if(argc>2||(argc==2&&strcmp(argv[1],"seed0")))return ERR_ARGUMENT;
 presentedGeneration=maximumPresentationLag=heartPresentations=0;phaseRefreshNeeded=audioBitValid=0;
 memset(&heartbeatState,0,sizeof(heartbeatState));
 e=os_intercept(&signalFlag);if(e)return e;
 e=os_clock(&previous,1);if(e)return e;
 /* Production retains the original post-maze time perturbation. seed0 is
  * an explicit deterministic test mode; neither changes LVLTAB maze seeds. */
 game_init(&game,argc==2?0:previous/60);game_creature_init(&game,&creatureScheduler);input[0]=0;renderContext.shownPhase=&shownPhase;
 game_heart_patterns(heartPatterns);screen_set_heart_patterns(heartPatterns);e=screen_open();if(e)goto done;
#ifdef DOD_COMMAND_OVERLAY
 /* Retain the module before the heartbeat-critical interval.  Its non-mapped
  * reference leaves the eighth DAT slot free until a command actually links. */
 (void)game_overlay_open();
#endif
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
  e=game_creature_advance_progress(&game,&creatureScheduler,videoTicks,
                                   creature_heart_progress,&renderContext,&result);
  if(e)break;if(result)dirty=1;
  if(game.rate!=oldrate){e=native_heartbeat_rate(&heartbeat,game.rate);if(e)break;}
  if(oldfaint!=game.faint||oldlight!=(game.torch?game.objects[(game.torch-0x0b15)/14][7]:0))dirty=1;
  if(game.dead){message="PLAYER DIED  EXITING";dirty=1;}
  /* Present simulation/scene changes before accepting another text byte, so
   * every subsequently consumed character can use the fast UI-only path. */
  if(dirty==1){
   if(view==GAME_VIEW_EXAMINE){
#ifdef DOD_COMMAND_OVERLAY
    e=game_overlay_examine(&game,frame,"",message);if(e){view=GAME_VIEW_DUNGEON;message="COMMAND MODULE UNAVAILABLE";game_render(&game,frame,"",message);e=0;}
#else
    game_render_examine(&game,frame,"",message);
#endif
   }
   else {e=game_render_with_progress(&game,frame,"",message,render_heart_progress,&renderContext);if(e)break;}
   memcpy(inputUnderlay,frame+GAME_INPUT_OFFSET,GAME_INPUT_BYTES);game_render_input(frame,input,inputUnderlay);
   e=screen_prepare_progress(frame,creature_heart_progress,&renderContext);if(e)break;
   e=native_heartbeat_snapshot(&heartbeat,&heartbeatState);if(e)break;
   if(heartbeatState.fault){e=heartbeatState.fault;break;}
   frameGeneration=heartbeatState.edgeGeneration;framePhase=heartbeatState.phase;
   e=game_render_status_progress(&game,frame,heartbeatState.phase,
                                 render_heart_progress,&renderContext);if(e)break;
   e=game_render_input_progress(&game,frame,input,inputUnderlay,
                                render_heart_progress,&renderContext);if(e)break;
   e=screen_prepare_ui_progress(frame,creature_heart_progress,&renderContext);if(e)break;
   shownPhase=framePhase;
   e=present_latest_heart(frame,&shownPhase);if(e)break;
   e=screen_flip();if(e)break;
   if(presentedGeneration<frameGeneration)presentedGeneration=frameGeneration;
   e=present_latest_heart(frame,&shownPhase);if(e)break;
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
#ifdef DOD_COMMAND_OVERLAY
    e=game_overlay_command(&game,input,&combat,&result,&nextView,&message);
    if(e){result=GAME_INVALID;nextView=GAME_VIEW_KEEP;message="COMMAND MODULE UNAVAILABLE";e=0;}
#else
    result=game_command_combat(&game,input,&combat);nextView=game_display_command(input);message=game_message(input,result);
#endif
    if(result==GAME_OK&&nextView)view=nextView;
    e=native_heartbeat_rate(&heartbeat,game.rate);if(e)break;
#ifdef _CMOC_VERSION_
    if(result==GAME_OK)audio_present_optional(&combatAudio,&combatAudioState,
      combat.events,combat.eventCount,"/d1/dodaudio","ssc-mame-fast");
#endif
    n=0;input[0]=0;
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
   if(dirty==1){if(view==GAME_VIEW_EXAMINE){
#ifdef DOD_COMMAND_OVERLAY
    e=game_overlay_examine(&game,frame,"",message);if(e){view=GAME_VIEW_DUNGEON;message="COMMAND MODULE UNAVAILABLE";game_render(&game,frame,"",message);e=0;}
#else
    game_render_examine(&game,frame,"",message);
#endif
   }
    else {e=game_render_with_progress(&game,frame,"",message,render_heart_progress,&renderContext);if(e)break;}
    memcpy(inputUnderlay,frame+GAME_INPUT_OFFSET,GAME_INPUT_BYTES);game_render_input(frame,input,inputUnderlay);
    e=screen_prepare_progress(frame,creature_heart_progress,&renderContext);if(e)break;}
   /* Full scene rendering/packing may span heartbeat edges. Sample the
    * authoritative VIRQ phase after that work and refresh its rows last. */
   e=native_heartbeat_snapshot(&heartbeat,&heartbeatState);if(e)break;
   if(heartbeatState.fault){e=heartbeatState.fault;break;}
   frameGeneration=heartbeatState.edgeGeneration;framePhase=heartbeatState.phase;
   e=game_render_status_progress(&game,frame,heartbeatState.phase,
                                 render_heart_progress,&renderContext);if(e)break;
   shownPhase=framePhase;
   e=game_render_input_progress(&game,frame,input,inputUnderlay,
                                render_heart_progress,&renderContext);if(e)break;
   e=screen_prepare_ui_progress(frame,creature_heart_progress,&renderContext);if(e)break;
   e=present_latest_heart(frame,&shownPhase);if(e)break;
   e=screen_flip();if(e)break;
   if(presentedGeneration<frameGeneration)presentedGeneration=frameGeneration;
   e=present_latest_heart(frame,&shownPhase);if(e)break;
   e=presentation_lag(shownPhase);if(e)break;
   dirty=phaseRefreshNeeded?2:0;
  }
  if(game.dead)break;
  if(inputEmpty){e=os_sleep(1);if(e)break;}
 }
 done:
#ifdef _CMOC_VERSION_
 if(combatAudioState==AUDIO_OPTIONAL_READY){audio_finish(&combatAudio);combatAudioState=AUDIO_OPTIONAL_DISABLED;}
#endif
#ifdef DOD_COMMAND_OVERLAY
 r=game_overlay_close();if(!e)e=r;
#endif
 r=native_heartbeat_close(&heartbeat);if(!e)e=r;r=screen_close();if(!e)e=r;
 printf("DODGAME HEARTBEAT EDGE %lu PRESENTED %lu MAX_PHASE_LAG %lu HEART_PRESENTS %lu\r",heartbeatState.edgeGeneration,presentedGeneration,maximumPresentationLag,heartPresentations);
 printf("DODGAME TERM RESTORED ROW %u COL %u DIR %u RATE %u STATUS %u\r",game.row,game.col,game.dir,game.rate,e);
 return e;
}
