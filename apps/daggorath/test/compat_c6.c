#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "compat/c5.h"
#include "gameplay/overlay-api.h"

#define OBASE 0x0b15
#define OADDR(n) (OBASE+(Word)(n)*14)

extern Byte dod_overlay_execute(DagOverlayContextV1 *);
static Byte opaque,clocks,boundaries,wordCalls,lastLength;
static char lastLine[33];

Byte dag_compat_text_cell(DagCompatCellContext *c){(void)c;return 0;}
Byte dag_compat_text_apply(DagCompatTextContext *c){(void)c;return 0;}
Byte dag_compat_present(const Byte *f,DagCompatPresentTiming *t){(void)f;(void)t;return 0;}
Byte dag_compat_present_cell(const Byte *f,Word c,DagCompatPresentTiming *t)
{(void)f;(void)c;(void)t;return 0;}
Byte os_sleep(Word ticks){(void)ticks;return 0;}

static void health(void *p,Game *g){(void)p;game_health(g);}
static Byte name(void *p,Game *g,Word token,Byte *out){(void)p;return game_object_name(g,token,out);}
static void status(void *p,Game *g,Byte *f,Byte phase){(void)p;game_render_status(g,f,phase);}
static DagOverlayServices overlayServices={DOD_OVERLAY_ABI_V1,sizeof(DagOverlayServices),&opaque,health,name,status};

static Byte command(Game *g,const char *text,GameCombat *combat,Byte *result,Byte *view){DagOverlayContextV1 c;
 memset(&c,0,sizeof(c));c.abiVersion=DOD_OVERLAY_ABI_V1;c.contextSize=sizeof(c);
 c.game=g;c.services=&overlayServices;c.command=text;c.combat=combat;c.operation=DOD_OVERLAY_COMMAND;
 assert(!dod_overlay_execute(&c));*result=c.result;*view=c.view;return 0;
}
static Byte scheduler(DagSchedulerContextV1 *c,Byte op,Word ticks){(void)c;(void)ticks;
 if(op==DOD_SCHED_CLOCK)++clocks;if(op==DOD_SCHED_BOUNDARY)++boundaries;return 0;
}
static Byte word(Game *g,Byte *frame,const char *line,Byte length)
{(void)g;(void)frame;++wordCalls;lastLength=length;memcpy(lastLine,line,length);lastLine[length]=0;return 0;}
static void enter(DagCompatC5 *c,const char *s){
 while(*s)assert(!dag_compat_c5_character(c,(Byte)*s++,DAG_COMPAT_C5_HUMAN));
 assert(!dag_compat_c5_character(c,13,DAG_COMPAT_C5_HUMAN));
}

int main(void){Game g;Byte state[DAG_COMPAT_STATE_BYTES],frame[DAG_COMPAT_FRAME_BYTES];
 DagSchedulerContextV1 schedulerContext;DagCompatC5 c6;
 DagCompatC5Services services={command,scheduler,word,0,0};Byte before;

 /* PTURN.ASM:PMOVE relative directions do not change the facing direction.
  * A four-command loop proves the connected HUMAN/parser path can continue
  * after each command rather than stopping at C5's first-human-command bound. */
 memset(&g,0,sizeof(g));memset(state,0,sizeof(state));memset(frame,0,sizeof(frame));
 memset(&schedulerContext,0,sizeof(schedulerContext));g.row=16;g.col=16;g.dir=0;g.level=2;g.power=6048;
 dag_compat_c5_init(&c6,&g,state,frame,&schedulerContext,&services);
 enter(&c6,"TURN RIGHT");assert(g.dir==1&&g.row==16&&g.col==16);
 enter(&c6,"MOVE");assert(g.dir==1&&g.row==16&&g.col==17);
 enter(&c6,"MOVE LEFT");assert(g.dir==1&&g.row==15&&g.col==17);
 enter(&c6,"MOVE BACKWARD");assert(g.dir==1&&g.row==15&&g.col==16);
 enter(&c6,"MOVE RIGHT");assert(g.dir==1&&g.row==16&&g.col==16);
 assert(g.damage==12&&clocks==5&&boundaries==5&&c6.commandCount==5);
 before=g.damage;g.maze[16*32+17]=255;enter(&c6,"MOVE");
 assert(c6.lastResult==GAME_BLOCKED&&g.row==16&&g.col==16&&g.damage==before+3);
 assert(clocks==6&&boundaries==6);

 /* The same persistent HUMAN path performs the existing source-derived bag,
  * hand, torch, EXAMINE, and LOOK transitions repeatedly. */
 clocks=boundaries=0;dag_compat_c4_init(&g,state,21);memset(frame,0,sizeof(frame));
 dag_compat_c5_init(&c6,&g,state,frame,&schedulerContext,&services);
 enter(&c6,"PULL LEFT SHIELD");assert(g.hand==OADDR(65));
 enter(&c6,"STOW LEFT");assert(!g.hand);
 enter(&c6,"PULL RIGHT TORCH");assert(g.rightHand==OADDR(64));
 enter(&c6,"USE RIGHT");assert(!g.rightHand&&g.torch==OADDR(64)&&g.lit);
 enter(&c6,"EXAMINE");assert(c6.lastView==GAME_VIEW_EXAMINE);
 enter(&c6,"LOOK");assert(c6.lastView==GAME_VIEW_DUNGEON);
 assert(clocks==6&&boundaries==6&&c6.commandCount==6);

 /* PATTK:PATT24 appends the source hit marker to the current command row.
  * This is driven by the real combat result, not by the command spelling. */
 memset(&g,0,sizeof(g));memset(state,0,sizeof(state));memset(frame,0,sizeof(frame));
 dag_compat_c4_init(&g,state,21);dag_compat_c5_init(&c6,&g,state,frame,&schedulerContext,&services);
 g.row=9;g.col=22;g.dir=0;g.rightHand=OADDR(0);g.objects[0][6]=2;
 g.objects[0][9]=19;g.objects[0][10]=4;g.objects[0][12]=1;g.objects[0][13]=1;
 g.creatures[18][0]=3;g.creatures[18][1]=0xe8;g.creatures[18][12]=1;
 g.creatures[18][13]=5;g.creatures[18][15]=9;g.creatures[18][16]=22;
 wordCalls=lastLength=0;
 enter(&c6,"ATTACK RIGHT");assert(c6.lastResult==GAME_OK);
 assert(wordCalls==1&&lastLength==17&&!strcmp(lastLine,"ATTACK RIGHT  !!!"));
 puts("compat C6 repeated HUMAN movement/inventory/view graph passed");return 0;
}
