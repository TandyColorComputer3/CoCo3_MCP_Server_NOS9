#include <cmoc.h>
#include "game.h"
#include "presentation.h"
#include "logical.h"
#include "native_heartbeat.h"
Byte game_input(Byte path,Byte *key);
static Game game;
static Byte frame[FRAME_BYTES],signalFlag;
static Byte inputUnderlay[GAME_INPUT_BYTES];
static NativeHeartbeat heartbeat={255,0};
static NativeHeartbeatState heartbeatState;
/* PULL/STOW/GET/DROP use original token rules; other commands retain M1 adapters.
 * EXIT remains an isolated OS-9-only lifecycle command. */
int main(int argc,char **argv){Byte e=0,r,key,n=0,dirty=1,result,oldrate,oldfaint,oldlight,shownPhase=255,view=GAME_VIEW_DUNGEON,nextView;Word previous,now,delta;char input[32];const char *message="TURN LEFT RIGHT AROUND  MOVE";
 if(argc>2||(argc==2&&strcmp(argv[1],"seed0")))return ERR_ARGUMENT;
 e=os_intercept(&signalFlag);if(e)return e;
 e=os_clock(&previous,1);if(e)return e;
 /* Production retains the original post-maze time perturbation. seed0 is
  * an explicit deterministic test mode; neither changes LVLTAB maze seeds. */
 game_init(&game,argc==2?0:previous/60);input[0]=0;
 e=screen_open();if(e)goto done;
 /* INIVUX: rate already computed; activation starts with remaining=1.
  * All module loads and graphics allocation precede the native claim. */
 e=os_clock(&previous,1);if(e)goto done;e=native_heartbeat_open(&heartbeat);if(e)goto done;
 e=native_heartbeat_rate(&heartbeat,game.rate);if(e)goto done;e=native_heartbeat_enable(&heartbeat);if(e)goto done;
 for(;;){
  e=os_signal_value(&signalFlag);if(e)break;
  e=os_clock(&now,0);if(e)break;delta=now>=previous?now-previous:3600-previous+now;previous=now;
  if(delta>300){e=ERR_ARGUMENT;break;}
  oldrate=game.rate;oldfaint=game.faint;oldlight=game.torch?game.objects[(game.torch-0x0b15)/14][7]:0;
  game_tick(&game,delta);if(game.rate!=oldrate){e=native_heartbeat_rate(&heartbeat,game.rate);if(e)break;}
  if(oldfaint!=game.faint||oldlight!=(game.torch?game.objects[(game.torch-0x0b15)/14][7]:0))dirty=1;
  if(game.dead){message="PLAYER DIED  EXITING";dirty=1;}
  e=game_input(screen_path(),&key);if(e)break;
  if(key){
   if(key==13){input[n]=0;if(!strcmp(input,"EXIT"))break;
    result=game_command(&game,input);nextView=game_display_command(input);if(result==GAME_OK&&nextView)view=nextView;message=game_message(input,result);
    e=native_heartbeat_rate(&heartbeat,game.rate);if(e)break;n=0;input[0]=0;
   }else if(key==8){if(n)input[--n]=0;}
   else if(key>=32&&key<=126&&n<31){if(key>='a'&&key<='z')key-=32;input[n++]=key;input[n]=0;}
   /* Editing changes only the port input row. Enter changes game/message
    * state and requires a full render. Keep any pending game_tick redraw. */
   if(key==13)dirty=1;else if(dirty!=1)dirty=2;
  }
  /* COMMON:CLK30 toggles HEARTS at the actual output edge. Query the
   * driver's latched phase; never derive a second heartbeat from wall time.
   * A heart-only change reuses every dungeon/input pixel. */
  e=native_heartbeat_query(&heartbeat,&heartbeatState);if(e)break;
  if(heartbeatState.fault){e=heartbeatState.fault;break;}
  if(!dirty&&shownPhase!=heartbeatState.phase)dirty=2;
  if(dirty){
   if(dirty==1){if(view==GAME_VIEW_EXAMINE)game_render_examine(&game,frame,"",message);else game_render(&game,frame,"",message);memcpy(inputUnderlay,frame+GAME_INPUT_OFFSET,GAME_INPUT_BYTES);}
   /* Full scene calculation may take several heartbeat periods. Sample
    * again at presentation, so redraw never reverts to a stale phase. */
   e=native_heartbeat_query(&heartbeat,&heartbeatState);if(e)break;
   if(heartbeatState.fault){e=heartbeatState.fault;break;}
   game_render_status(&game,frame,heartbeatState.phase);shownPhase=heartbeatState.phase;
   game_render_input(frame,input,inputUnderlay);e=screen_present(frame);if(e)break;dirty=0;
  }
  if(game.dead)break;
  e=os_sleep(1);if(e)break;
 }
 done:r=native_heartbeat_close(&heartbeat);if(!e)e=r;r=screen_close();if(!e)e=r;
 printf("DODGAME TERM RESTORED ROW %u COL %u DIR %u RATE %u STATUS %u\r",game.row,game.col,game.dir,game.rate,e);
 return e;
}
