/* Original-derived CRETUR.ASM:CMOVE/STEPOK/CWALK and COMMON.ASM:QUESCN.
 * M6 stops at the CMOV20 attack boundary: it schedules that boundary but
 * performs no sound, shield, attack, damage, or player-state operation. */
#include "game.h"

#define OBASE 0x0b15
static const signed char stepRow[4]={-1,0,1,0};
static const signed char stepCol[4]={0,1,0,-1};
static Word getword(const Byte *p){return (Word)p[0]*256+p[1];}
static void putword(Byte *p,Word w){p[0]=(Byte)(w>>8);p[1]=(Byte)w;}
static Byte cell(const Game *g,Byte row,Byte col){return g->maze[(Word)row*32+col];}
static Word delay(Byte value){return value?value:256;}
typedef struct { Byte (*progress)(void *);void *context;Byte error; } CreatureProgress;
static Byte checkpoint(CreatureProgress *p){
 if(p&&p->progress&&!p->error)p->error=p->progress(p->context);
 return p?p->error:0;
}

void game_creature_init(Game *g,CreatureScheduler *s){Byte i;
 s->framePhase=0;
 for(i=0;i<32;i++){
  s->combatPending[i]=0;
  s->countdown[i]=g->creatures[i][12]?delay(g->creatures[i][6]):0;
 }
}

static Byte occupied(const Game *g,Byte row,Byte col,Byte except,CreatureProgress *p){Byte i;const Byte *c;
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
 /* CMOV10 excludes Scorpion (6) and Wizard (10+); OFIND scans OCB order. */
 if(c[13]>=6)return 0;
 for(i=0;i<g->count;i++){
  o=g->objects[i];
  if(o[4]==0&&o[5]==0&&o[2]==c[15]&&o[3]==c[16]){
   old=getword(c+8);putword(o,old);putword(c+8,(Word)(OBASE+(Word)i*14));--o[5];
   return 1;
  }
  if(!(i&7)&&checkpoint(p))return 0;
 }
 return 0;
}

static Byte move_one(Game *g,CreatureScheduler *s,Byte index,CreatureProgress *p){Byte *c=g->creatures[index],dir,order,offset,tries,relative,dirty=0;
 static const Byte movtab[7]={0,3,1,0,1,3,0};
 if(!c[12]){s->countdown[index]=0;s->combatPending[index]=0;return 0;}
 /* CMOV10..12: one unowned object pickup consumes this turn. */
 if(pickup(g,index,p)){s->countdown[index]=delay(c[6]);return 1;}if(checkpoint(p))return 0;
 /* This is the exact M6 hard gate before original CMOV20. The original
  * attack scheduler period is retained, but gameplay/combat is not run. */
 if(c[15]==g->row&&c[16]==g->col){s->combatPending[index]=1;s->countdown[index]=delay(c[7]);return 0;}
 s->combatPending[index]=0;
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

Byte game_creature_advance_progress(Game *g,CreatureScheduler *s,Word videoTicks,
                                    Byte (*progress)(void *),void *context,Byte *dirty){Byte i;Word n=videoTicks;CreatureProgress work={progress,context,0};
 /* COMMON:CLK40 visits JIFQUE each 1/60 s; ROLTAB rolls JIFFY at 6,
  * then visits Q.TEN. Creature CCTMV/CCTAT are therefore 1/10-second
  * units, matching the original queued CMOVE cadence. */
 *dirty=0;
 while(n--){
  if(++s->framePhase>=6){
   s->framePhase=0;
   for(i=0;i<32;i++)if(s->countdown[i]){
    if(--s->countdown[i]==0)*dirty|=move_one(g,s,i,&work);
    if(work.error)return work.error;
    if(progress&&!(i&3)){Byte e=checkpoint(&work);if(e)return e;}
   }
  }
  /* A video tick is the bounded presentation-service boundary. Q.TEN
   * simulation still runs only after the sixth tick, as in COMMON:CLK40. */
  if(progress){Byte e=progress(context);if(e)return e;}
 }
 return 0;
}
Byte game_creature_advance(Game *g,CreatureScheduler *s,Word videoTicks){Byte dirty;
 (void)game_creature_advance_progress(g,s,videoTicks,0,0,&dirty);
 return dirty;
}
