#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "gameplay/game.h"
#include "audio/audio.h"

#define OBASE 0x0b15
#define OADDR(n) (OBASE+(Word)(n)*14)
static Game g;

static void word(Byte *p,Word value){p[0]=(Byte)(value>>8);p[1]=(Byte)value;}
static Byte *golden(void){Byte i,*c,*s,*t;
 memset(&g,0,sizeof(g));g.count=66;g.creatureCount=19;g.row=9;g.col=22;g.dir=0;
 g.power=6048;g.damage=16;g.rightHand=OADDR(63);g.torch=OADDR(64);
 g.seed[0]=0xf1;g.seed[1]=0x94;g.seed[2]=0xd4;
 s=g.objects[63];s[4]=0x3c;s[5]=1;s[9]=13;s[10]=4;s[12]=0;s[13]=40;
 t=g.objects[64];t[4]=0x3c;t[5]=1;t[6]=14;t[7]=7;t[9]=15;t[10]=5;t[13]=5;
 for(i=0;i<5;i++){c=g.creatures[i];word(c,704);c[3]=128;c[4]=128;c[5]=48;c[6]=17;c[7]=13;c[12]=255;c[13]=5;c[15]=(Byte)(i+1);c[16]=1;}
 c=g.creatures[18];word(c,704);c[3]=128;c[4]=128;c[5]=48;c[6]=17;c[7]=13;c[12]=255;c[13]=5;c[15]=9;c[16]=22;
 return c;
}

int main(void){GameCombat r;Byte *c,*o;Word right;
 /* PATTK substitutes COMDAT:EMPHND for an empty selected hand: sword
  * sound class 4, magic offense 0 and physical offense 5. At the initial
  * 160 power, (5 >> 3) yields zero energy; no target means no RANDOM. */
 memset(&g,0,sizeof(g));g.count=66;g.power=160;
 assert(game_command_combat(&g,"ATTACK LEFT",&r)==GAME_OK);
 assert(r.energy==0&&g.damage==0&&r.rngCalls==0&&!r.hit&&!r.killed);
 assert(r.eventCount==1&&r.events[0]==AUDIO_WHOOSH);

 c=golden();right=g.rightHand;
 assert(game_population(&g,5)==6);assert(game_command_combat(&g,"ATTACK RIGHT",&r)==GAME_OK);
 assert(r.energy==236&&g.damage==252&&r.rngCalls==1);
 assert(g.seed[0]==0x65&&g.seed[1]==0xf1&&g.seed[2]==0x94);
 assert(r.hitValue==94&&r.hit&&r.damage==708&&r.killed&&r.target==18);
 assert(!c[12]&&((Word)c[10]*256+c[11])==708&&game_population(&g,5)==5);
 assert(g.power==6136&&g.rate==41&&g.rightHand==right);
 assert(r.eventCount==3&&r.events[0]==AUDIO_WHOOSH&&r.events[1]==AUDIO_KLINK&&r.events[2]==AUDIO_BANG);
 assert(game_command(&g,"TURN RIGHT")==GAME_OK&&g.dir==1);

 /* General left-hand/ring path: guaranteed nonlethal hit, no RNG. */
 c=golden();g.hand=OADDR(65);o=g.objects[65];o[5]=1;o[6]=3;o[9]=19;o[10]=1;o[13]=5;
 assert(game_command_combat(&g,"ATTACK LEFT",&r)==GAME_OK);
 assert(!r.rngCalls&&r.hit&&!r.killed&&r.damage==88&&o[6]==2&&c[12]);
 assert(r.eventCount==2&&r.events[0]==AUDIO_PHASER&&r.events[1]==AUDIO_KLINK);

 /* A weak attacker reaches the source's negative miss result. */
 c=golden();g.power=160;word(c,800);
 assert(game_command_combat(&g,"ATTACK RIGHT",&r)==GAME_OK);
 assert(r.energy==6&&g.damage==22&&r.rngCalls==1&&!r.hit&&!r.killed);
 assert((signed short)r.hitValue==-101&&((Word)c[10]*256+c[11])==0);
 assert(r.eventCount==1&&r.events[0]==AUDIO_WHOOSH);

 /* Without a live torch, the otherwise successful hit consumes the second
  * RANDOM and this captured stream loses the one-in-four darkness test. */
 c=golden();g.torch=0;
 assert(game_command_combat(&g,"ATTACK RIGHT",&r)==GAME_OK);
 assert(r.rngCalls==2&&!r.hit&&!r.killed&&((Word)c[10]*256+c[11])==0);
 assert(g.seed[0]==0x96&&g.seed[1]==0x65&&g.seed[2]==0xf1);

 /* Death follows the packed carried-object chain and drops it in the room. */
 c=golden();word(c+8,OADDR(0));o=g.objects[0];word(o,0);o[5]=255;
 assert(game_command_combat(&g,"ATTACK RIGHT",&r)==GAME_OK&&r.killed);
 assert(o[5]==0&&o[2]==9&&o[3]==22);

 assert(game_command_combat(&g,"ATTACK UP",&r)==GAME_INVALID);
 puts("13 Combat M1 source-state checks passed");return 0;
}
