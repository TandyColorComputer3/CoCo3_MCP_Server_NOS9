/* Original-derived CRETUR.ASM:CMOVE/STEPOK/CWALK and COMMON.ASM:QUESCN.
 * CMOV20 is a scheduler-overlay operation: this resident primitive records a
 * due same-cell attack, then dodsched owns its source attack math and timing. */
#include "game.h"
#include "game_data.h"

#define OBASE 0x0b15
static const signed char stepRow[4]={-1,0,1,0};
static const signed char stepCol[4]={0,1,0,-1};
static Word getword(const Byte *p){return (Word)p[0]*256+p[1];}
static void putword(Byte *p,Word w){p[0]=(Byte)(w>>8);p[1]=(Byte)w;}
static Byte cell(const Game *g,Byte row,Byte col){return g->maze[(Word)row*32+col];}
static Word delay(Byte value){return value?value:256;}
typedef struct { Byte (*progress)(void *);void *context;Byte error; } CreatureProgress;
#ifdef DOD_SCHED_MODULE
/* Normal dodsched checks its bounded foreground service point once per source
 * CLOCK jiffy. The old resident-only fine-grained renderer callback is not
 * available in the module and must not pull a second presentation path in. */
#define checkpoint(p) 0
#else
static Byte checkpoint(CreatureProgress *p){
 if(p&&p->progress&&!p->error)p->error=p->progress(p->context);
 return p?p->error:0;
}
#endif

void game_creature_init(Game *g,CreatureScheduler *s){Byte i;
 s->framePhase=s->readyCount=s->audioCount=0;
 for(i=0;i<12;i++)s->desired[i]=level_cmt[(Word)g->level*12+i];
 for(i=0;i<32;i++){
  s->combatPending[i]=s->attackDue[i]=s->pending[i]=0;
  s->countdown[i]=g->creatures[i][12]?delay(g->creatures[i][6]):0;
 }
}

/* COMCRE.ASM:CREGEN sums CMXLND, not the live CCB population.  When its
 * source matrix has room it takes exactly one RANDOM result, selects type
 * 2..9, and increments that matrix entry.  CBIRTH is deliberately not called
 * here: the original task only changes a future birth allowance. */
void game_creature_regenerate(Game *g,CreatureScheduler *s){Byte i,total=0,type;
 for(i=0;i<12;i++)total=(Byte)(total+s->desired[i]);
 if(total>=32)return;
 type=(Byte)((game_random(g)&7)+2);
 ++s->desired[type];
}

static Byte occupied(const Game *g,Byte row,Byte col,Byte except,CreatureProgress *p){Byte i;const Byte *c;
#ifdef DOD_SCHED_MODULE
 (void)p;
#endif
 for(i=0;i<32;i++)if(i!=except){c=g->creatures[i];
  if(c[12]&&c[15]==row&&c[16]==col)return 1;
  if(!(i&7)&&checkpoint(p))return 0;
 }
 return 0;
}

/* STEPOK checks the 32x32 border and MAP32's solid-cell sentinel ($FF).
 * The original routine does not test the packed directional wall bits. */
static Byte step_ok(const Game *g,Byte row,Byte col,Byte dir,Byte *nr,Byte *nc){
 int r=(int)row+stepRow[dir&3],c=(int)col+stepCol[dir&3];
 if(r<0||r>31||c<0||c>31||cell(g,(Byte)r,(Byte)c)==255)return 0;
 *nr=(Byte)r;*nc=(Byte)c;return 1;
}

/* CWALK updates the original CCB only after STEPOK and CFIND both succeed.
 * Its near-player sound RANDOM call is retained even though M6 has no
 * creature-sound mapping; this preserves the source RNG stream. */
static Byte walk(Game *g,Byte index,Byte relative,Byte *visible,CreatureProgress *p){Byte *c=g->creatures[index],dir,nr,nc;
 Byte dr,dc,large,small;
 dir=(c[14]+relative)&3;
 if(!step_ok(g,c[15],c[16],dir,&nr,&nc)||occupied(g,nr,nc,index,p)||checkpoint(p))return 0;
 c[15]=nr;c[16]=nc;c[14]=dir;
 dr=nr>g->row?nr-g->row:g->row-nr;
 dc=nc>g->col?nc-g->col:g->col-nc;
 large=dr>dc?dr:dc;small=dr<dc?dr:dc;
 *visible=0;
 if(large<=8&&small<=2){(void)game_random(g);*visible=1;}
 return 1;
}

static Byte see_player(const Game *g,const Byte *c,Byte *direction,CreatureProgress *p){
#ifdef DOD_SCHED_MODULE
 (void)p;
#endif
 Byte row=c[15],col=c[16],nr,nc,steps=0;
 if(row==g->row){
  *direction=col<g->col?1:3;
  while(step_ok(g,row,col,*direction,&nr,&nc)){row=nr;col=nc;if(row==g->row&&col==g->col)return 1;if(!(++steps&3)&&checkpoint(p))return 0;}
 }else if(col==g->col){
  /* CMOV52's signed row subtraction selects the original absolute DIR. */
  *direction=row<g->row?2:0;
  while(step_ok(g,row,col,*direction,&nr,&nc)){row=nr;col=nc;if(row==g->row&&col==g->col)return 1;if(!(++steps&3)&&checkpoint(p))return 0;}
 }
 return 0;
}

static Byte pickup(Game *g,Byte index,CreatureProgress *p){Byte *c=g->creatures[index],i,*o;Word old;
#ifdef DOD_SCHED_MODULE
 (void)p;
#endif
 /* CMOV10 excludes Scorpion (6) and Wizard (10+); OFIND scans OCB order. */
 if(c[13]>=6)return 0;
 for(i=0;i<g->count;i++){
  o=g->objects[i];
  if(o[4]==g->level&&o[5]==0&&o[2]==c[15]&&o[3]==c[16]){
   old=getword(c+8);putword(o,old);putword(c+8,(Word)(OBASE+(Word)i*14));--o[5];
   return 1;
  }
  if(!(i&7)&&checkpoint(p))return 0;
 }
 return 0;
}

static Byte move_one(Game *g,CreatureScheduler *s,Byte index,CreatureProgress *p){Byte *c=g->creatures[index],dir,order,offset,tries,relative,dirty=0;
 static const Byte movtab[7]={0,3,1,0,1,3,0};
 if(!c[12]){s->countdown[index]=0;s->combatPending[index]=s->attackDue[index]=0;return 0;}
 /* CMOV10..12: one unowned object pickup consumes this turn. */
 if(pickup(g,index,p)){s->countdown[index]=delay(c[6]);return 1;}if(checkpoint(p))return 0;
 if(c[15]==g->row&&c[16]==g->col){s->combatPending[index]=s->attackDue[index]=1;s->countdown[index]=delay(c[7]);return 0;}
 s->combatPending[index]=s->attackDue[index]=0;
 if(see_player(g,c,&dir,p)){
  c[14]=dir;
  (void)walk(g,index,0,&dirty,p);
 }else{
  /* CMOV70..76: sign bit chooses table half; low two bits rotate start. */
  order=game_random(g);offset=(order&3)==0?1:0;
  order=(order&0x80)?0:3;
  for(tries=0;tries<3;tries++){
   relative=movtab[order+offset+tries];
   if(walk(g,index,relative,&dirty,p))break;
   if(checkpoint(p))return 0;
  }
  if(tries==3)(void)walk(g,index,2,&dirty,p);
 }
 if(c[15]==g->row&&c[16]==g->col){s->combatPending[index]=1;s->countdown[index]=delay(c[7]);return 1;}
 s->countdown[index]=delay(c[6]);
 return dirty;
}

/* COMMON.ASM:QUESCN moves an expired Q.TEN TCB to SCDQUE.  It cannot
 * immediately run and requeue itself while PLAYER is inside WAITX.  Preserve
 * the move-to-ready order (the Q.TEN linked-list order) separately from the
 * foreground CMOVE dispatch below. */
#ifndef DOD_SCHED_MODULE
static Byte creature_clock_advance(CreatureScheduler *s,Word videoTicks,CreatureProgress *work){Byte i;
 while(videoTicks--){
  if(++s->framePhase>=6){
   s->framePhase=0;
   for(i=0;i<32;i++)if(s->countdown[i]&&!s->pending[i]){
    if(!--s->countdown[i]){s->pending[i]=1;s->ready[s->readyCount++]=i;}
    if(work->progress&&!(i&3)&&checkpoint(work))return work->error;
   }
  }
  if(work->progress&&checkpoint(work))return work->error;
 }
 return 0;
}
static Byte creature_service(Game *g,CreatureScheduler *s,CreatureProgress *work,Byte *dirty){Byte n,index;
 for(n=0;n<s->readyCount;n++){
  index=s->ready[n];s->pending[index]=0;
  *dirty|=move_one(g,s,index,work);
  if(work->error)return work->error;
  if(work->progress&&checkpoint(work))return work->error;
 }
 s->readyCount=0;
 return 0;
}
/* Dynamic dodsched calls this only after its own CLOCK/QUESCN pass promoted
 * the CCB.  Keep CMOVE's mutation here in the resident owner of packed CCB
 * state; no module-global pointer or second queue is introduced. */
Byte game_creature_service_one(Game *g,CreatureScheduler *s,Byte index,Byte *dirty){
 CreatureProgress work={0,0,0};Byte row,col;
 if(index>=32)return 187;
 row=g->creatures[index][15];col=g->creatures[index][16];
 (void)move_one(g,s,index,&work);
 /* This API is for source scheduler state: CWLK90 requests LUKNEW whenever
  * the CCB actually changed cells.  The normal immediate renderer continues
  * to use move_one's visibility-oriented return through creature_service(). */
 *dirty=(Byte)(g->creatures[index][15]!=row||g->creatures[index][16]!=col);
 return work.error;
}
Byte game_creature_advance_progress(Game *g,CreatureScheduler *s,Word videoTicks,
                                    Byte (*progress)(void *),void *context,Byte *dirty){CreatureProgress work={progress,context,0};
 *dirty=0;
 if(creature_clock_advance(s,videoTicks,&work))return work.error;
 return creature_service(g,s,&work,dirty);
}
Byte game_creature_advance(Game *g,CreatureScheduler *s,Word videoTicks){Byte dirty;
 (void)game_creature_advance_progress(g,s,videoTicks,0,0,&dirty);
 return dirty;
}
#endif
