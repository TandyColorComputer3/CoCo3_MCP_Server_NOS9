#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "gameplay/game.h"
#include "gameplay/scheduler-api.h"
#include "audio/audio.h"

#define OBASE 0x0b15

extern Byte dodsched_execute(DagSchedulerContextV1 *);

static void word(Byte *p,Word value){p[0]=(Byte)(value>>8);p[1]=(Byte)value;}
static Byte drain(CreatureScheduler *q,Byte *events){Byte i,n=q->audioCount;
 for(i=0;i<n;i++)events[i]=q->audio[i];q->audioCount=0;return n;
}
/* The dynamic module asks this narrow resident primitive to perform CMOV30's
 * HUPDAT. It is the same production callback main.c installs. */
static Byte primitive(void *opaque,Game *g,GameTiming *timing,CreatureScheduler *q,
                      Byte task,Byte ccb,Byte *dirty){
 (void)opaque;(void)timing;(void)q;(void)ccb;*dirty=0;
 if(task==DOD_TASK_HEALTH){game_health(g);return 0;}
 return 0;
}
static void setup(Game *g,CreatureScheduler *q,Byte type){Byte *c;
 memset(g,0,sizeof(*g));memset(g->maze,0,sizeof(g->maze));g->row=16;g->col=16;g->power=6048;g->creatureCount=1;
 c=g->creatures[0];c[12]=255;c[13]=type;c[15]=g->row;c[16]=g->col;c[6]=1;c[7]=3;
 if(type==5){word(c,704);c[2]=0;c[3]=128;c[4]=128;c[5]=48;}
 else {word(c,400);c[2]=255;c[3]=128;c[4]=255;c[5]=128;}
 game_creature_init(g,q);
}
/* Put an already-due CCB through the same DOD_TASK_CMOVE → CMOV20 path that
 * DODSCHED's normal FIFO boundary uses. No test-only combat entry exists. */
static void due(Game *g,GameTiming *timing,CreatureScheduler *q,DagSchedulerState *state){
 DagSchedulerServices services={DOD_SCHEDULER_ABI_V1,sizeof(DagSchedulerServices),0,primitive,0,0};
 DagSchedulerContextV1 c;
 memset(state,0,sizeof(*state));memset(&c,0,sizeof(c));
 c.abiVersion=DOD_SCHEDULER_ABI_V1;c.contextSize=sizeof(c);c.operation=DOD_SCHED_BOUNDARY;
 c.game=g;c.timing=timing;c.creatures=q;c.state=state;c.services=&services;
 state->fifo[0]=DOD_TASK_CMOVE;state->tail=1;state->count=1;q->pending[0]=1;
 assert(dodsched_execute(&c)==0&&c.dirty&&!q->pending[0]);
}
static void clock(Game *g,GameTiming *timing,CreatureScheduler *q,DagSchedulerState *state,Word ticks){
 DagSchedulerServices services={DOD_SCHEDULER_ABI_V1,sizeof(DagSchedulerServices),0,primitive,0,0};
 DagSchedulerContextV1 c;
 memset(&c,0,sizeof(c));c.abiVersion=DOD_SCHEDULER_ABI_V1;c.contextSize=sizeof(c);c.operation=DOD_SCHED_CLOCK;
 c.game=g;c.timing=timing;c.creatures=q;c.state=state;c.services=&services;c.logicalJiffies=ticks;
 assert(dodsched_execute(&c)==0);
 c.operation=DOD_SCHED_BOUNDARY;assert(dodsched_execute(&c)==0);
}
int main(void){Game g;CreatureScheduler q;GameTiming timing;DagSchedulerState state;Byte events[4];
 /* $000011 produces $F7. Stone Giant 2 has physical 128/magical 0, so the
  * source attack is a 704-point physical hit and emits GRAWL then CLANK. */
 setup(&g,&q,5);memset(&timing,0,sizeof(timing));g.seed[0]=g.seed[1]=0;g.seed[2]=0x11;due(&g,&timing,&q,&state);
 assert(g.seed[0]==0xf7&&g.seed[1]==0&&g.seed[2]==0&&g.damage==704&&g.rate==33);
 assert(q.combatPending[0]&&q.countdown[0]==3);
 assert(drain(&q,events)==2&&events[0]==AUDIO_GRAWL&&events[1]==AUDIO_CLANK);

 /* $000001 produces $87, below the same source ATTACK threshold: one sound,
  * one RANDOM, HUPDAT, and no DAMAGE. */
 setup(&g,&q,5);memset(&timing,0,sizeof(timing));g.seed[0]=g.seed[1]=g.seed[2]=0;g.seed[2]=1;due(&g,&timing,&q,&state);
 assert(g.seed[0]==0x87&&g.seed[1]==0&&g.seed[2]==0&&g.damage==0);
 assert(drain(&q,events)==1&&events[0]==AUDIO_GRAWL);

 /* Scorpion's $FF/$FF offense is the source-backed shield fixture. Leather
  * supplies P.OCXXX=$6C80: magical damage falls 796→671 while physical 796
  * remains, giving 1467 rather than the unshielded 1592. */
 setup(&g,&q,6);memset(&timing,0,sizeof(timing));g.seed[0]=g.seed[1]=0;g.seed[2]=0x11;due(&g,&timing,&q,&state);
 assert(g.damage==1592&&drain(&q,events)==2&&events[0]==AUDIO_PSSST&&events[1]==AUDIO_CLANK);
 setup(&g,&q,6);memset(&timing,0,sizeof(timing));g.count=1;g.hand=OBASE;g.objects[0][10]=3;g.objects[0][6]=108;g.objects[0][7]=128;g.seed[0]=g.seed[1]=0;g.seed[2]=0x11;due(&g,&timing,&q,&state);
 assert(g.damage==1467);

 /* CMOV30/HUPDAT defines the faint/death boundary after a normal source hit. */
 setup(&g,&q,6);memset(&timing,0,sizeof(timing));g.power=160;g.seed[0]=g.seed[1]=0;g.seed[2]=0x11;due(&g,&timing,&q,&state);
 assert(g.damage==1592&&g.faint&&g.dead);

 /* After CMOV20, P.CCTAT=3 is a three-Q.TEN delay. The second attack is
  * promoted only on the eighteenth logical jiffy and uses the same FIFO path. */
 setup(&g,&q,5);memset(&timing,0,sizeof(timing));g.seed[0]=g.seed[1]=g.seed[2]=0;g.seed[2]=1;due(&g,&timing,&q,&state);
 assert(q.countdown[0]==3);clock(&g,&timing,&q,&state,17);assert(q.countdown[0]==1&&q.audioCount==1);
 clock(&g,&timing,&q,&state,1);assert(q.countdown[0]==3&&q.audioCount==2);
 puts("15 source-state CRETUR CMOV20 scheduler checks passed");return 0;
}
