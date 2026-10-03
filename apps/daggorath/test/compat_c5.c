#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "compat/c5.h"
#include "compat/present.h"
#include "gameplay/overlay-api.h"
#include "gameplay/scheduler-api.h"

extern Byte dod_overlay_execute(DagOverlayContextV1 *);
extern Byte dodsched_execute(DagSchedulerContextV1 *);
static Byte waits,boundaries,clocks,humanSeen,wordPresentations;
static Byte opaque;
static const char *wordFrames[]={"EXAMINE ","PULL ","PULL RIGHT ","PULL RIGHT TORCH ",
 "USE ","USE RIGHT ","LOOK ","MOVE ","PULL ","PULL LEFT ","PULL LEFT SHIELD ",
 "PULL ","PULL RIGHT ","PULL RIGHT SWORD ","MOVE "};
typedef struct {Byte row,col,dir,damage,r0,r1,r2,giantRow,giantCol,delay;} Checkpoint;
static const Checkpoint expected[8]={
 {12,22,0,0,0xA5,0xB1,0xC9,11,25,4},
 {12,22,0,0,0xF9,0x16,0xE8,10,25,17},
 {12,22,0,0,0x50,0xDC,0x8F,9,25,17},
 {12,22,0,0,0x8B,0x30,0xEE,9,25,4},
 {11,22,0,6,0x4B,0xDB,0x79,9,24,17},
 {11,22,0,5,0xC1,0xAA,0xE4,9,23,17},
 {11,22,0,4,0xD2,0xD4,0x58,9,22,17},
 {10,22,0,10,0x84,0xD4,0x3F,9,22,4}
};
Byte dag_compat_text_cell(DagCompatCellContext *c){(void)c;return 0;}
Byte dag_compat_text_apply(DagCompatTextContext *c){(void)c;return 0;}
Byte dag_compat_present(const Byte *f,DagCompatPresentTiming *t){(void)f;(void)t;return 0;}
Byte dag_compat_present_cell(const Byte *f,Word c,DagCompatPresentTiming *t){(void)f;(void)c;(void)t;return 0;}
Byte os_sleep(Word n){(void)n;return 0;}
static void health(void *p,Game *g){(void)p;game_health(g);}
static Byte name(void *p,Game *g,Word token,Byte *out){(void)p;return game_object_name(g,token,out);}
static void status(void *p,Game *g,Byte *f,Byte phase){(void)p;game_render_status(g,f,phase);}
static DagOverlayServices overlayServices={DOD_OVERLAY_ABI_V1,sizeof(DagOverlayServices),&opaque,health,name,status};
static Byte command(Game *g,const char *text,GameCombat *combat,Byte *result,Byte *view){DagOverlayContextV1 c;
 memset(&c,0,sizeof(c));c.abiVersion=DOD_OVERLAY_ABI_V1;c.contextSize=sizeof(c);
 c.game=g;c.services=&overlayServices;c.command=text;c.combat=combat;c.operation=DOD_OVERLAY_COMMAND;
 assert(dod_overlay_execute(&c)==0);if(!strcmp(text,"LOOK")&&humanSeen)humanSeen=2;
 *result=c.result;*view=c.view;return 0;
}
static Byte task(void *p,Game *g,GameTiming *t,CreatureScheduler *q,Byte n,Byte c,Byte *dirty){
 (void)p;(void)t;(void)q;(void)c;*dirty=0;if(n==DOD_TASK_HEALTH)game_health(g);return 0;
}
static Word present(void *p,Game *g,Byte mode){(void)p;(void)g;(void)mode;return 0;}
static Byte scheduler(DagSchedulerContextV1 *c,Byte op,Word ticks){
 if(op==DOD_SCHED_PLAYER_WAIT)++waits;else if(op==DOD_SCHED_BOUNDARY)++boundaries;else if(op==DOD_SCHED_CLOCK)++clocks;
 c->operation=op;c->logicalJiffies=ticks;return dodsched_execute(c);
}
static Byte word(Game *g,Byte *frame,const char *line,Byte length){(void)g;(void)frame;
 assert(wordPresentations<15&&length==strlen(wordFrames[wordPresentations]));
 assert(!strcmp(line,wordFrames[wordPresentations++]));return 0;
}
int main(void){Game g;Byte state[DAG_COMPAT_STATE_BYTES],frame[DAG_COMPAT_FRAME_BYTES];GameTiming timing;CreatureScheduler creatures;
 DagSchedulerState schedulerState;DagSchedulerServices schedulerServices={DOD_SCHEDULER_ABI_V1,sizeof(DagSchedulerServices),0,task,present,0};
 DagSchedulerContextV1 schedulerContext;DagCompatC5 c5;DagCompatC5Services services={command,scheduler,word};Byte i;
 dag_compat_c4_init(&g,state,21);memset(&schedulerContext,0,sizeof(schedulerContext));
 schedulerContext.abiVersion=DOD_SCHEDULER_ABI_V1;schedulerContext.contextSize=sizeof(schedulerContext);
 schedulerContext.game=&g;schedulerContext.timing=&timing;schedulerContext.creatures=&creatures;
 schedulerContext.state=&schedulerState;schedulerContext.services=&schedulerServices;
 assert(!scheduler(&schedulerContext,DOD_SCHED_INIT,0));assert(!scheduler(&schedulerContext,DOD_SCHED_BOUNDARY,0));
 memset(frame,0,sizeof(frame));dag_compat_c5_init(&c5,&g,state,frame,&schedulerContext,&services);
 for(i=0;i<8;i++){const Checkpoint *p=&expected[i];assert(!dag_compat_c5_autoplay(&c5,i));
  assert(g.row==p->row&&g.col==p->col&&g.dir==p->dir&&g.damage==p->damage);
  assert(g.seed[0]==p->r0&&g.seed[1]==p->r1&&g.seed[2]==p->r2);
  assert(g.creatures[18][12]&&g.creatures[18][13]==5);
  assert(g.creatures[18][15]==p->giantRow&&g.creatures[18][16]==p->giantCol);
  assert(creatures.countdown[18]==p->delay);
  if(i==5){assert(!dag_compat_c5_type5_at(&g,2,0));assert(dag_compat_c5_type5_at(&g,2,1));}
  if(i==6)assert(dag_compat_c5_type5_at(&g,2,0));
 }
 assert(c5.commandCount==8&&waits==15&&wordPresentations==15&&boundaries==9);
 assert(g.row==10&&g.col==22&&g.dir==0&&g.level==2);
 assert(g.creatures[18][12]&&g.creatures[18][13]==5&&g.creatures[18][15]==9&&g.creatures[18][16]==22);
 /* VIEWER:VIEW32/PDRAW saw this Giant at the right-hand adjacent cell after
  * command six and forward after the next source movement. */
 assert(dag_compat_c5_type5_at(&g,1,0));
 dag_compat_c5_prepare_view(&g,state,1);
 assert(state[0x23]==0&&state[0x7c]==9&&state[0x7d]==22&&state[0x8b]==1&&state[0x6e]==7);
 /* Real HUMAN entry: lowercase bytes arrive one at a time through the same
  * line buffer and parser dispatch, with PLAYER's one-jiffy reschedule. */
 humanSeen=1;for(i=0;i<4;i++)assert(!dag_compat_c5_character(&c5,(Byte)"look"[i],DAG_COMPAT_C5_HUMAN));
 assert(!dag_compat_c5_character(&c5,13,DAG_COMPAT_C5_HUMAN));
 assert(humanSeen==2&&c5.origin==DAG_COMPAT_C5_HUMAN&&c5.lastResult==GAME_OK&&clocks==1&&boundaries==10);
 assert(!memcmp(state+DAG_COMPAT_CCB_BASE,g.creatures,32*17));
 puts("compat C5 HUMAN/AUTTAB/parser/scheduler/Giant-state graph passed");return 0;
}
