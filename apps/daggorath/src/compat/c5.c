/* HUMAN.ASM:PLAYER/HUMAN connected command path.  Cartridge keyboard and
 * AUTTAB input meet here as characters; only the hardware source of those
 * characters is adapted.  The existing callable command and scheduler
 * modules retain the source-derived PARSER/DISPAT and SCHED leaves. */
#ifdef _CMOC_VERSION_
#include <cmoc.h>
#else
#include <string.h>
#endif
#include "c5.h"

typedef struct { const char *text; Byte words; } AutoCommand;
static const AutoCommand auttab[]={
 {"EXAMINE",1},{"PULL RIGHT TORCH",3},{"USE RIGHT",2},{"LOOK",1},
 {"MOVE",1},{"PULL LEFT SHIELD",3},{"PULL RIGHT SWORD",3},{"MOVE",1},
 {"MOVE",1},{"ATTACK RIGHT",2},{"TURN RIGHT",2},{"MOVE",1},{"MOVE",1},
 {"MOVE",1},{"TURN RIGHT",2},{"MOVE",1},{"MOVE",1}
};

void dag_compat_c5_init(DagCompatC5 *c,Game *g,Byte *state,Byte *frame,
                        DagSchedulerContextV1 *scheduler,
                        const DagCompatC5Services *services){
 memset(c,0,sizeof(*c));c->game=g;c->originalState=state;
 c->logicalFrame=frame;c->scheduler=scheduler;c->services=services;
 c->text.logicalFrame=frame;c->text.cursor=33;c->text.inverse=0;
}

static Byte dispatch(DagCompatC5 *c){GameCombat combat;Byte e,result,view,commandLength=c->length;
 /* Cartridge HUMAN retains PLAY30's final separating space. The modern
  * callable command ABI accepts a normal C command without trailing blanks;
  * trim only at that demonstrated parser boundary, after source display and
  * line-buffer choreography have completed. */
 while(commandLength&&c->line[commandLength-1]==' ')--commandLength;
 c->line[commandLength]=0;
 e=c->services->command(c->game,c->line,&combat,&result,&view);if(e)return e;
 c->lastResult=result;c->lastView=view;++c->commandCount;
 /* HUMAN:PLAY99 returns PLAYER to Q.JIF. AUTTAB's WAIT calls have already
  * promoted source tasks; keyboard input advances the ordinary one jiffy. */
 if(c->origin==DAG_COMPAT_C5_HUMAN){e=c->services->scheduler(c->scheduler,DOD_SCHED_CLOCK,1);if(e)return e;}
 e=c->services->scheduler(c->scheduler,DOD_SCHED_BOUNDARY,0);if(e)return e;
 dag_compat_c4_sync(c->game,c->originalState);c->length=c->words=0;
 /* HUMAN:HMAN70 invokes PROMPT: CR, DOT, BAR, BS.  The compatibility
  * framebuffer keeps the visible dot; cursor phase belongs to presentation. */
 c->text.operation=DAG_COMPAT_TEXT_CHARACTER;c->text.value=31;
 e=dag_compat_text_apply(&c->text);if(e)return e;c->text.value=30;
 return dag_compat_text_apply(&c->text);
}

Byte dag_compat_c5_character(DagCompatC5 *c,Byte ch,Byte origin){
 if(!c||!c->services||!c->services->command||!c->services->scheduler)return ERR_ARGUMENT;
 c->origin=origin;
 if(ch==13)return dispatch(c);
 if(ch==8){if(c->length)--c->length;c->text.operation=DAG_COMPAT_TEXT_ERASE_CHARACTER;return dag_compat_text_apply(&c->text);}
 if(ch>='a'&&ch<='z')ch-=32;
 if(ch!=' '&&(ch<'A'||ch>'Z'))ch=' ';
 if(c->length<32)c->line[c->length++]=ch;
 c->text.operation=DAG_COMPAT_TEXT_CHARACTER;c->text.value=ch==' '?0:(Byte)(ch-'A'+1);
 return dag_compat_text_apply(&c->text);
}

Byte dag_compat_c5_autoplay(DagCompatC5 *c,Byte index){const char *p;Byte e;
 if(index>=sizeof(auttab)/sizeof(auttab[0]))return ERR_ARGUMENT;
 p=auttab[index].text;c->words=auttab[index].words;
 /* PLAY30 waits before each expanded word, then feeds that word and one
  * separating space through HUMAN. The cartridge writes directly into the
  * displayed bitmap, so the compatibility adapter presents after each word;
  * it must not defer all text until command completion. CLOCK promotions
  * remain queued until PLAYER returns after the terminating CR. */
 while(*p){e=c->services->scheduler(c->scheduler,DOD_SCHED_PLAYER_WAIT,0);if(e)return e;
  while(*p&&*p!=' '){e=dag_compat_c5_character(c,(Byte)*p++,DAG_COMPAT_C5_AUTOPLAY);if(e)return e;}
  e=dag_compat_c5_character(c,' ',DAG_COMPAT_C5_AUTOPLAY);if(e)return e;
  c->line[c->length]=0;if(c->services->word&&(e=c->services->word(c->game,c->logicalFrame,c->line,c->length)))return e;
  while(*p==' ')++p;
 }
 return dag_compat_c5_character(c,13,DAG_COMPAT_C5_AUTOPLAY);
}

Byte dag_compat_c5_type5_at(const Game *g,Byte range,Byte relative){
 static const signed char dr[]={-1,0,1,0},dc[]={0,1,0,-1};Byte i,dir=g->dir;
 Byte row=(Byte)(g->row+dr[dir]*range),col=(Byte)(g->col+dc[dir]*range);
 if(relative){dir=(Byte)((dir+relative)&3);row=(Byte)(row+dr[dir]);col=(Byte)(col+dc[dir]);}
 for(i=0;i<32;i++)if(g->creatures[i][12]&&g->creatures[i][13]==5&&
  g->creatures[i][15]==row&&g->creatures[i][16]==col)return 1;
 return 0;
}

void dag_compat_c5_prepare_view(const Game *g,Byte *s,Byte range){
 static const signed char rowStep[]={-1,0,1,0},colStep[]={0,1,0,-1};
 Byte row=(Byte)(g->row+rowStep[g->dir]*range);
 Byte col=(Byte)(g->col+colStep[g->dir]*range),cell,i,*o;
 dag_compat_c4_sync(g,s);s[0x05]=0;s[0x06]=128;s[0x07]=0;s[0x08]=76;
 s[0x23]=g->dir;s[0x7c]=row;s[0x7d]=col;s[0x8b]=range;
 s[0x6e]=s[0x6f]=0;
 if(g->torch>=0x0b15&&g->torch<0x0b15+(Word)g->count*14){
  o=(Byte *)g->objects[(g->torch-0x0b15)/14];s[0x6e]=o[7];s[0x6f]=o[8];
 }
 cell=g->maze[(Word)row*32+col];
 for(i=0;i<4;i++){s[0x7f4+i]=cell&3;s[0x7f4+4+i]=cell&3;cell>>=2;}
}
