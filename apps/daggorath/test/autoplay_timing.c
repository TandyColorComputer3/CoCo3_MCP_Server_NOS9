/* Deterministic source timing model for PLAYER:WAITX + COMPLR:HSLOW.
 * The commands execute through the ordinary resident/overlay command body;
 * only wall-clock pacing and the original scheduler boundary are modeled. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "gameplay/game.h"
#include "gameplay/overlay-api.h"

extern Byte dod_overlay_execute(DagOverlayContextV1 *);
static DagOverlayServices services={DOD_OVERLAY_ABI_V1,sizeof(DagOverlayServices),
 game_health,game_object_name,game_render_status};

static Byte command(Game *g,const char *text){
 DagOverlayContextV1 c;
 memset(&c,0,sizeof(c));c.abiVersion=DOD_OVERLAY_ABI_V1;c.contextSize=sizeof(c);
 c.game=g;c.services=&services;c.command=text;c.operation=DOD_OVERLAY_COMMAND;
 assert(dod_overlay_execute(&c)==0);return c.result;
}
/* HUMAN.ASM:PLAYER calls MISC.ASM:WAITX once per AUTTAB word. WAITX executes
 * 81 SYNC jiffies. COMMON.ASM moves HSLOW only to SCDQUE; it runs once when
 * PLAYER returns to the scheduler after the complete command. */
static void player_command(Game *g,GameTiming *timing,CreatureScheduler *creatures,const char *text,Byte words){
 Byte dirty;
 assert(command(g,text)==GAME_OK);
 game_timing_advance(g,timing,(Word)words*81);
 /* Q.TEN moves due CCB tasks to SCDQUE during the wait; dispatch happens only
  * after the complete command has returned to the foreground scheduler. */
 (void)game_creature_advance_progress(g,creatures,(Word)words*81,0,0,&dirty);
 game_timing_service(g,timing);
}
static void timing_queue(void){Game g;GameTiming timing;
 game_init_demo(&g,21);game_timing_init(&g,&timing);
 g.damage=7;g.recovery=3;
 game_timing_advance(&g,&timing,2);assert(g.damage==7&&!timing.recoveryPending);
 game_timing_advance(&g,&timing,1);assert(g.damage==7&&timing.recoveryPending);
 /* A due TCB remains pending; it cannot run repeatedly before SCD dispatch. */
 game_timing_advance(&g,&timing,200);assert(g.damage==7&&timing.recoveryPending);
 game_timing_service(&g,&timing);assert(g.damage==6&&!timing.recoveryPending);
 assert(g.recovery==g.rate);
}
static void auttab_prefix(void){Game g,before;GameTiming timing;CreatureScheduler creatures;Byte random,type,old;
 game_init_demo(&g,21);game_timing_init(&g,&timing);game_creature_init(&g,&creatures);
 assert(g.damage==0&&g.rate==46);
 player_command(&g,&timing,&creatures,"EXAMINE",1);
 /* TCBDAT places CREGEN after PLAYER, LUKNEW, HSLOW and BURNER in the
  * initial SCDQUE. It consumes one RANDOM and changes CMXLND only. */
 before=g;random=game_random(&before);type=(Byte)((random&7)+2);old=creatures.desired[type];
 game_creature_regenerate(&g,&creatures);
 assert(!memcmp(g.seed,before.seed,3));
 assert(creatures.desired[type]==(Byte)(old+1));
 player_command(&g,&timing,&creatures,"PULL RIGHT TORCH",3);
 player_command(&g,&timing,&creatures,"USE RIGHT",2);
 player_command(&g,&timing,&creatures,"LOOK",1);
 player_command(&g,&timing,&creatures,"MOVE",1);assert(g.damage==6);
 player_command(&g,&timing,&creatures,"PULL LEFT SHIELD",3);assert(g.damage==5);
 player_command(&g,&timing,&creatures,"PULL RIGHT SWORD",3);assert(g.damage==4);
 player_command(&g,&timing,&creatures,"MOVE",1);assert(g.damage==10);
 player_command(&g,&timing,&creatures,"MOVE",1);
 /* PATTK command 10's original entry state: HSLOW naturally reaches 16. */
 assert(g.level==2&&g.row==9&&g.col==22&&g.dir==0);
 assert(g.power==6048&&g.damage==16&&g.hand==0x0ea3&&g.rightHand==0x0e87);
 assert(g.torch==0x0e95);
 /* The Q.TEN queue path reaches the ordinary packed CCB 18 position without
  * fixture mutation.  Its remaining global-RNG trace is tracked separately. */
 assert(g.creatures[18][12]&&g.creatures[18][13]==5);
 assert(g.creatures[18][15]==9&&g.creatures[18][16]==22);
 assert(((Word)g.creatures[18][0]<<8|g.creatures[18][1])==704);
 assert(((Word)g.creatures[18][10]<<8|g.creatures[18][11])==0);
 /* CREGEN's one source transition changes later CMOVE branch choices, so the
  * complete modeled path reaches 189 rather than mechanically 188 calls.
  * The independent source capture needs 203; the remaining fourteen are not
  * synthesized by this bounded CMOVE model. */
 assert(g.seed[0]==0x48&&g.seed[1]==0x87&&g.seed[2]==0x8e);
}
int main(void){timing_queue();auttab_prefix();puts("HSLOW logical jiffy/AUTTAB prefix checks passed");return 0;}
