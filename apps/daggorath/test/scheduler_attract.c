/* The AUTTAB prefix uses the callable scheduler's real operation boundary,
 * rather than the old aggregate advance/service helper.  The host executable
 * calls the same C body that is packaged in dodsched; retained-link mechanics
 * have their own ABI/lifecycle test. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "gameplay/game.h"
#include "gameplay/overlay-api.h"
#include "gameplay/scheduler-api.h"
#include "audio/audio.h"

extern Byte dod_overlay_execute(DagOverlayContextV1 *);
extern Byte dodsched_execute(DagSchedulerContextV1 *);

static Byte overlayOpaque;
static void overlay_health(void *opaque,Game *g){(void)opaque;game_health(g);}
static Byte overlay_object_name(void *opaque,Game *g,Word token,Byte *name){(void)opaque;return game_object_name(g,token,name);}
static void overlay_render_status(void *opaque,Game *g,Byte *frame,Byte phase){(void)opaque;game_render_status(g,frame,phase);}
static DagOverlayServices overlayServices={DOD_OVERLAY_ABI_V1,sizeof(DagOverlayServices),
 &overlayOpaque,overlay_health,overlay_object_name,overlay_render_status};

static Byte command(Game *g,const char *text,GameCombat *combat){DagOverlayContextV1 c;
 memset(&c,0,sizeof(c));c.abiVersion=DOD_OVERLAY_ABI_V1;c.contextSize=sizeof(c);
 c.game=g;c.services=&overlayServices;c.command=text;c.combat=combat;c.operation=DOD_OVERLAY_COMMAND;
 assert(dod_overlay_execute(&c)==0);return c.result;
}
static Byte primitive(void *opaque,Game *g,GameTiming *t,CreatureScheduler *q,
                      Byte task,Byte ccb,Byte *dirty){
 (void)opaque;(void)t;(void)q;(void)ccb;*dirty=0;
 if(task==DOD_TASK_HEALTH){game_health(g);return 0;}
 /* dodsched owns these source mutations; this callback only supplies
  * the shared resident HUPDAT operation and harmless observation. */
 return 0;
}
/* Portable policy: the scheduler models source waits, CLOCK promotion and
 * FIFO SCHED ordering.  It deliberately assigns no logical jiffies to the
 * cartridge's incidental bare-metal renderer duration. */
static Word present(void *opaque,Game *g,Byte mode){(void)opaque;(void)g;(void)mode;return 0;}
static Byte scheduler(DagSchedulerContextV1 *c,Byte operation,Word ticks){
 c->operation=operation;c->logicalJiffies=ticks;return dodsched_execute(c);
}
static void player(Game *g,DagSchedulerContextV1 *c,const char *text,Byte words){
 assert(command(g,text,0)==GAME_OK);
 while(words--){assert(scheduler(c,DOD_SCHED_PLAYER_WAIT,0)==0);}
 assert(scheduler(c,DOD_SCHED_BOUNDARY,0)==0);
}
int main(void){Game g,reference;GameTiming t;CreatureScheduler q;DagSchedulerState s;
 DagSchedulerServices services={DOD_SCHEDULER_ABI_V1,sizeof(DagSchedulerServices),0,primitive,present,0};
 DagSchedulerContextV1 c;GameCombat combat;Byte i;
 memset(&c,0,sizeof(c));c.abiVersion=DOD_SCHEDULER_ABI_V1;c.contextSize=sizeof(c);
 c.game=&g;c.timing=&t;c.creatures=&q;c.state=&s;c.services=&services;
 game_init_demo(&g,21);game_timing_init(&g,&t);game_creature_init(&g,&q);
 assert(scheduler(&c,DOD_SCHED_INIT,0)==0);
 assert(scheduler(&c,DOD_SCHED_BOUNDARY,0)==0); /* SYSTCB's first CREGEN. */
 player(&g,&c,"EXAMINE",1);
 /* Original autonomous capture: CMD D992 begins at A5B1C9.  The older
  * aggregate helper reached B1C9AD, one RANDOM transition too early. */
 assert(g.seed[0]==0xa5&&g.seed[1]==0xb1&&g.seed[2]==0xc9);
 player(&g,&c,"PULL RIGHT TORCH",3);
 player(&g,&c,"USE RIGHT",2);
 player(&g,&c,"LOOK",1);
 player(&g,&c,"MOVE",1);assert(g.damage==6);
 player(&g,&c,"PULL LEFT SHIELD",3);assert(g.damage==5);
 player(&g,&c,"PULL RIGHT SWORD",3);assert(g.damage==4);
 player(&g,&c,"MOVE",1);assert(g.damage==10);
 player(&g,&c,"MOVE",1);
 assert(g.level==2&&g.row==9&&g.col==22&&g.dir==0);
 assert(g.power==6048&&g.damage==16&&g.hand==0x0ea3&&g.rightHand==0x0e87&&g.torch==0x0e95);
 assert(g.creatures[18][12]&&g.creatures[18][13]==5&&g.creatures[18][15]==9&&g.creatures[18][16]==22);
 assert(((Word)g.creatures[18][0]<<8|g.creatures[18][1])==704);
 assert(((Word)g.creatures[18][10]<<8|g.creatures[18][11])==0);
 /* This is the portable policy's deterministic state, not a replacement for
  * the cartridge's renderer-latency-dependent 203-transition/F194D4 trace.
  * The final ordinary source boundary has CCB 18 due in the player's cell.
  * CMOV20 therefore consumes its one ATTACK RANDOM, misses, and requeues at
  * the CCB attack delay; it is not an attract-specific seed adjustment. */
 game_init_demo(&reference,21);for(i=0;i<185;i++)game_random(&reference);
 assert(reference.seed[0]==0xf2&&reference.seed[1]==0xa7&&reference.seed[2]==0x4c);
 /* CMOV20 uses the next source RANDOM before AUTTAB command 10. */
 assert(game_random(&reference)==0xb4);
 assert(g.seed[0]==reference.seed[0]&&g.seed[1]==reference.seed[1]&&g.seed[2]==reference.seed[2]);
 assert(g.seed[0]==0xb4&&g.seed[1]==0xf2&&g.seed[2]==0xa7);
 assert(q.combatPending[18]&&!q.attackDue[18]&&q.countdown[18]==13&&
        q.audioCount==1&&q.audio[0]==AUDIO_GRAWL);
 assert(command(&g,"ATTACK RIGHT",&combat)==GAME_OK);
 assert(g.seed[0]==0x8e&&g.seed[1]==0xb4&&g.seed[2]==0xf2);
 assert(g.damage==252&&g.power==6136&&!g.creatures[18][12]);
 assert(combat.rngCalls==1&&combat.hit&&combat.killed&&combat.target==18);
 assert(combat.energy==236&&combat.damage==708&&combat.hitValue==135);
 assert(combat.eventCount==3&&combat.events[0]==AUDIO_WHOOSH&&
        combat.events[1]==AUDIO_KLINK&&combat.events[2]==AUDIO_BANG);
 puts("dodsched AUTTAB portable-policy and command-10 checks passed");return 0;
}
