/* Source-order CLOCK/QUESCN/SCHED orchestration for the callable dodsched
 * module.  Primitive game mutation and graphics remain caller-owned. */
#include "scheduler-api.h"
#include "audio/audio.h"

#define OBASE 0x0b15
/* The scheduler module has no import from dodgame. This is GAME:RANDOM's
 * exact three-byte state transition, kept local so the retained module stays
 * callable after the resident has unlinked every other overlay. */
static Byte random_byte(Game *g){Byte n,i,b,carry,next;
 for(n=0;n<8;n++){b=g->seed[2]&0xe1;carry=0;for(i=0;i<8;i++){carry^=b&1;b>>=1;}
  for(i=0;i<3;i++){next=g->seed[i]>>7;g->seed[i]=(g->seed[i]<<1)|carry;carry=next;}}
 return g->seed[0];
}
/* The normal creature scheduler is deliberately compiled into this callable
 * module. These aliases make the recovered implementation a private part of
 * dodsched; `random_byte` is its module-local GAME:RANDOM transition. */
#undef OBASE
#define getword creature_getword
#define putword creature_putword
#define game_random random_byte
#define game_creature_init scheduler_creature_init
#define game_creature_regenerate scheduler_creature_regenerate
#define DOD_SCHED_MODULE 1
#include "creature.c"
#undef DOD_SCHED_MODULE
#undef game_random
#undef game_creature_regenerate
#undef game_creature_init
#undef putword
#undef getword
#undef OBASE
#define OBASE 0x0b15
/* The extracted source contains this public resident wrapper inside the host
 * only section. The callable module uses the same underlying `move_one`. */
static Byte scheduler_service_one(Game *g,CreatureScheduler *s,Byte index,Byte *dirty){
 CreatureProgress work={0,0,0};Byte row,col;
 if(index>=32)return 187;
 row=g->creatures[index][15];col=g->creatures[index][16];
 (void)move_one(g,s,index,&work);
 *dirty=(Byte)(g->creatures[index][15]!=row||g->creatures[index][16]!=col);
 return work.error;
}
/* Hands are authoritative packed OCB tokens maintained by resident gameplay.
 * Scheduler CMOV20 only needs the source range check; a second divide-based
 * alignment validator would consume the last overlay bytes without adding a
 * source-visible behavior. */
static Byte valid_ocb(const Game *g,Word p){return p>=OBASE&&p<OBASE+(Word)g->count*14;}
static Word scale16(Word value,Byte radix){Word low=0,old;Byte high=0;
 while(radix--){old=low;low=(Word)(low+value);if(low<old)++high;}
 return (Word)(((Word)high<<9)|(low>>7));
}
static Word shield(const Game *g,Word current,Word hand){const Byte *o;Word candidate;
 if(!hand||!valid_ocb(g,hand))return current;
 o=g->objects[(hand-OBASE)/14];if(o[10]!=3)return current;
 candidate=creature_getword(o+6);return candidate<current?candidate:current;
}
static void sound(CreatureScheduler *q,Byte event){
 if(q->audioCount>=sizeof(q->audio))return;
 q->audio[q->audioCount++]=event;
}
/* CRETUR:CMOV20 → SHIELD → ATTACK → DAMAGE → HUPDAT. The scheduler module
 * owns this low-frequency code so dodgame remains below its resident policy
 * gate. Its only resident callback is DOD_TASK_HEALTH, which is HUPDAT's
 * authoritative player-rate/faint/death propagation. */
static Byte creature_attack(DagSchedulerContextV1 *c,Byte index){
 Byte *a,random,attackIndex=15;Word defense,remaining,value,power,magic,physical;int adjustment,score;Byte e;
 if(index>=32)return 187;a=c->game->creatures[index];
 if(!a[12]||a[15]!=c->game->row||a[16]!=c->game->col)return 187;
 sound(c->creatures,a[13]);
 defense=shield(c->game,0x8080,c->game->hand);defense=shield(c->game,defense,c->game->rightHand);
 power=creature_getword(a);remaining=(Word)(c->game->power-c->game->damage);value=(Word)(remaining<<2);
 do{Word old=value;value=(Word)(value-power);if(old<power)break;--attackIndex;}while(attackIndex);
 adjustment=attackIndex>=3?(attackIndex-3)*10:-(3-attackIndex)*25;
 random=random_byte(c->game);score=(int)random+adjustment-127;
 if(score>=0){
  sound(c->creatures,AUDIO_CLANK);
  magic=scale16(scale16(power,a[2]),(Byte)(defense>>8));
  physical=scale16(scale16(power,a[4]),(Byte)defense);
  c->game->damage=(Word)(c->game->damage+magic+physical);
 }
 /* CMOV30 calls HUPDAT after both hit and miss; it is mandatory state work. */
 if(!c->services->task)return 187;
 e=c->services->task(c->services->opaque,c->game,c->timing,c->creatures,DOD_TASK_HEALTH,0,&c->dirty);
 return e;
}

static Byte put(DagSchedulerState *s,Byte task){
 if(s->count>=36)return 187;
 s->fifo[s->tail]=task;if(++s->tail==36)s->tail=0;++s->count;return 0;
}
static Byte system_put(DagSchedulerState *s,Byte task){
 Byte bit=(Byte)(1<<task),e;
 if(s->queued&bit)return 0;
 e=put(s,task);if(!e)s->queued|=bit;return e;
}
static Byte get(DagSchedulerState *s,Byte *task){
 if(!s->count)return 0;
 *task=s->fifo[s->head];if(++s->head==36)s->head=0;--s->count;
 if(*task<4)s->queued&=(Byte)~(1<<*task);
 return 1;
}
static Byte clock_one(DagSchedulerContextV1 *c){
 DagSchedulerState *s=c->state;CreatureScheduler *q=c->creatures;Byte i,e;
 if(++s->sourceJiffy>=60){s->sourceJiffy=0;if(++s->sourceSecond==60)s->sourceSecond=0;}
 if(!c->timing->recoveryPending&&! --c->game->recovery)c->timing->recoveryPending=1;
 if(!c->timing->burnPending&&! --c->game->burn)c->timing->burnPending=1;
 if(c->timing->recoveryPending&&(e=system_put(s,DOD_TASK_HSLOW)))return e;
 if(c->timing->burnPending&&(e=system_put(s,DOD_TASK_BURNER)))return e;
 if(! --s->lookTicks){if((e=system_put(s,DOD_TASK_LUKNEW)))return e;s->lookTicks=18;}
 if(! --s->cregenTicks){if((e=system_put(s,DOD_TASK_CREGEN)))return e;s->cregenTicks=18000;}
 if(c->services->progress&&(e=c->services->progress(c->services->opaque)))return e;
 if(++q->framePhase<6)return 0;
 q->framePhase=0;if(++s->sourceTenth>=10)s->sourceTenth=0;
 for(i=0;i<32;i++)if(q->countdown[i]&&!q->pending[i]){
  if(!--q->countdown[i]){q->pending[i]=1;if((e=put(s,(Byte)(DOD_TASK_CMOVE|i))))return e;}
 }
 return 0;
}
static Byte clock(DagSchedulerContextV1 *c,Word ticks){Byte e;
 while(ticks--){e=clock_one(c);if(e)return e;}
 return 0;
}
static Byte presentation(DagSchedulerContextV1 *c){Word ticks;
 if(!c->services->present)return 187;
 ticks=c->services->present(c->services->opaque,c->game,c->state->presentationMode);
 return clock(c,ticks);
}
static Byte task(DagSchedulerContextV1 *c,Byte code,Byte ccb){
 Byte dirty=0,e=0;
 if(code==DOD_TASK_LUKNEW){
 if(c->state->newLook){c->state->newLook=0;
   if(c->services->present)e=presentation(c);
   dirty=1;
  }
 }else {
  /* The module owns normal queue semantics and creature mutation.  The
   * resident callback is retained only for HUPDAT and deterministic test
   * observation; it must never be the owner of scheduler state. */
  if(code==DOD_TASK_HSLOW&&c->timing->recoveryPending){
   c->game->damage=(Word)(c->game->damage-(c->game->damage+63)/64);
   c->timing->recoveryPending=0;c->game->recovery=c->game->rate?c->game->rate:256;
   if(!c->services->task)return 187;
   e=c->services->task(c->services->opaque,c->game,c->timing,c->creatures,DOD_TASK_HEALTH,0,&dirty);
  }else if(code==DOD_TASK_BURNER&&c->timing->burnPending){
   Byte *o;c->timing->burnPending=0;c->game->burn=3600;
   if(c->game->torch){o=c->game->objects[(c->game->torch-0x0b15)/14];
    if(o[6]){--o[6];if(o[6]<=5){o[9]=24;o[11]=0;}if(o[6]<o[7])o[7]=o[6];if(o[6]<o[8])o[8]=o[6];}}
  }else if(code==DOD_TASK_CREGEN)scheduler_creature_regenerate(c->game,c->creatures);
  else if(code==DOD_TASK_CMOVE){
   e=scheduler_service_one(c->game,c->creatures,ccb,&dirty);
   if(!e){
   c->creatures->pending[ccb]=0;
   if(c->creatures->attackDue[ccb]){
    c->creatures->attackDue[ccb]=0;e=creature_attack(c,ccb);dirty=1;
   }
   /* CRETUR.ASM:CWLK90 decrements NEWLUK after every successful walk.
    * The resident primitive reports that source update independently of
    * whether the creature happened to be visible in the current viewport. */
   if(dirty)c->state->newLook=1;
   }
  }
  /* CMOV30's HUPDAT is authoritative resident state propagation. */
  if(!e&&code==DOD_TASK_HEALTH){
   if(!c->services->task)return 187;
   e=c->services->task(c->services->opaque,c->game,c->timing,c->creatures,code,ccb,&dirty);
  }else if(!e&&c->services->task){
   Byte observed=0;e=c->services->task(c->services->opaque,c->game,c->timing,c->creatures,code,ccb,&observed);
   dirty|=observed;
  }
 }
 if(dirty)c->dirty=1;return e;
}
static Byte boundary(DagSchedulerContextV1 *c){Byte t,e;
 while(get(c->state,&t)){
  if((t&DOD_TASK_CMOVE)==DOD_TASK_CMOVE)e=task(c,DOD_TASK_CMOVE,(Byte)(t&31));
  else e=task(c,t,0);
  if(e)return e;
 }
 return 0;
}
Byte dodsched_execute(DagSchedulerContextV1 *c){Byte i,e;
 if(!c||c->abiVersion!=DOD_SCHEDULER_ABI_V1||c->contextSize!=sizeof(*c)||
    !c->game||!c->timing||!c->creatures||!c->state||!c->services||
    c->services->version!=DOD_SCHEDULER_ABI_V1||c->services->size!=sizeof(DagSchedulerServices))return 187;
 c->dirty=0;
 if(c->operation==DOD_SCHED_INIT){
  for(i=0;i<sizeof(*c->state);i++)((Byte *)c->state)[i]=0;
  c->state->lookTicks=18;c->state->cregenTicks=18000;
  c->timing->recoveryPending=c->timing->burnPending=0;
  if(!c->game->recovery)c->game->recovery=c->game->rate?c->game->rate:256;
  if(!c->game->burn)c->game->burn=3600;
  scheduler_creature_init(c->game,c->creatures);
  if((e=system_put(c->state,DOD_TASK_LUKNEW)))return e;
  if((e=system_put(c->state,DOD_TASK_HSLOW)))return e;
  if((e=system_put(c->state,DOD_TASK_BURNER)))return e;
  return system_put(c->state,DOD_TASK_CREGEN);
 }
 if(c->operation==DOD_SCHED_REQUEST_LOOK){c->state->newLook=1;return 0;}
 if(c->operation==DOD_SCHED_PLAYER_WAIT)return clock(c,81);
 if(c->operation==DOD_SCHED_NORMAL_TICKS){
  if((e=clock(c,c->logicalJiffies)))return e;
  return boundary(c);
 }
 if(c->operation==DOD_SCHED_CLOCK)return clock(c,c->logicalJiffies);
 if(c->operation==DOD_SCHED_BOUNDARY)return boundary(c);
 return 187;
}
