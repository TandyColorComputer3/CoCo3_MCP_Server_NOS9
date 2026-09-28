/* Source: pinned daggorath-reference/SOUNDS.ASM:SNDTAB and generators.
 * These are bounded SSC/AY interpretations, NOT exact CoCo DAC waveforms.
 * Timing targets come from the source-built cartridge SOUNDX MAME capture
 * documented in docs/apps/DAGGORATH_GAMEPLAY_M8.md. Tandy SSC manual
 * pp. 15-19,25-28 defines buffer format, tone/noise postbytes and FF end.
 * No software audio-rate loop or interrupt ownership is introduced. */
#include "ssc_catalog.h"
typedef struct { Byte *data,n; } Phrase;
static Byte put(Phrase *p,Byte value){
 if(p->n>=SSC_CATALOG_CAPACITY)return AUDIO_BAD;
 p->data[p->n++]=value;return 0;
}
static Word clock_period(Word fastPeriod,Byte fast){
 Word slow=(Word)(fastPeriod/2);
 if(fast)return fastPeriod;
 return slow?slow:(Word)1;
}
static Byte tone(Phrase *p,Byte channel,Byte amplitude,Word period,Byte ticks){
 Byte e;
 e=put(p,(Byte)(channel|amplitude));if(e)return e;
 e=put(p,(Byte)(period>>8));if(e)return e;
 e=put(p,(Byte)period);if(e)return e;
 return put(p,ticks);
}
static Byte noise(Phrase *p,Byte amplitude,Byte period,Byte ticks){
 Byte e=put(p,(Byte)(0x80|amplitude));if(e)return e;
 e=put(p,period);if(e)return e;return put(p,ticks);
}
static Byte finish_tone(Phrase *p,Word period){Byte e=tone(p,0,0,period,0);return e?e:put(p,0xff);}
static Byte finish_noise(Phrase *p,Byte period){Byte e=noise(p,0,period,0);return e?e:put(p,0xff);}
static Byte rattle(Phrase *p,Byte pulses,Byte period){Byte i,e;
 /* RATTLE/PSSST/PSSHT share SNRAT1; only the pulse count differs. */
 for(i=0;i<pulses;i++){
  e=noise(p,12,period,3);if(e)return e;
  e=noise(p,0,period,2);if(e)return e;
 }return put(p,0xff);
}
static Byte roar(Phrase *p,Byte kind){
 static const Byte amplitude[]={3,5,7,9,11,13,14,12,10,8,5,2,0};
 Byte i,e,period=(Byte)(kind==AUDIO_SNARL?5:kind==AUDIO_GRAWL?9:12);
 Byte ticks=(Byte)(kind==AUDIO_SNARL?15:kind==AUDIO_GRAWL?12:11);
 /* GROWL/GRAWL/SNARL: source SNENVA attack then long SNENVN decay;
  * different attack increments become distinct envelope/noise colors. */
 for(i=0;i<13;i++){e=noise(p,amplitude[i],period,ticks);if(e)return e;}
 return put(p,0xff);
}
static Byte sweep(Phrase *p,Byte rising,Byte repetitions,Byte fast){
 static const Word rise[]={900,690,520,390,290,220,160,110};
 Byte i,r,e;
 /* BEOOP descends; GLUGLG repeats rising MSQUEQ. */
 for(r=0;r<repetitions;r++)for(i=0;i<8/repetitions;i++){
  Byte index=rising?(Byte)(i*(repetitions==4?7:1)):(Byte)(7-i);
  Word period=clock_period(rise[index],fast);
  e=tone(p,0,12,period,rising?9:6);if(e)return e;
 }
 return finish_tone(p,clock_period(110,fast));
}
static Byte metal(Phrase *p,Word a,Word b,Byte ticks,Byte fast){
 static const Byte amplitude[]={12,10,8,6,4,2,0};
 Byte i,e; a=clock_period(a,fast);b=clock_period(b,fast);
 /* CSETUP/SNCLK2: two counter-driven DAC components under SNCLK5 decay.
  * AY A/B approximate the two pitches; coupled phase is not reproduced. */
 for(i=0;i<7;i++){
  e=tone(p,0,amplitude[i],a,amplitude[i]?ticks:0);if(e)return e;
  e=tone(p,0x20,amplitude[i],b,amplitude[i]?ticks:0);if(e)return e;
 }return put(p,0xff);
}
static Byte wizard(Phrase *p,Byte fast){Byte i,e;
 /* BDLBDL: eight seed-dependent chirps followed by KABOOM. The two
  * wizard IDs share the original generator, so they share this recipe. */
 for(i=0;i<8;i++){
  e=tone(p,0,11,clock_period((Word)(750-80*i),fast),10);if(e)return e;
 }
 for(i=0;i<6;i++){
  e=noise(p,(Byte)(13-2*i),(Byte)(6+4*i),14);if(e)return e;
 }
 return finish_noise(p,30);
}
static Byte whoosh(Phrase *p){
 static const Byte amplitude[]={3,6,9,12,11,9,6,3,0};
 static const Byte period[]={8,9,10,11,13,15,17,19,19};
 Byte i,e;
 /* WHOOSH: retain the user-approved diagnostic noise rise/decay bytes. */
 for(i=0;i<9;i++){
  e=noise(p,amplitude[i],period[i],(Byte)(i>=4&&i<=7));if(e)return e;
 }return put(p,0xff);
}
static Byte klink(Phrase *p,Byte fast){
 static const Byte amp[]={12,8,4,0},np[]={4,5,6,6};Byte i,e;
 /* KLINK: retain the user-approved diagnostic high tone + noise shape. */
 for(i=0;i<4;i++){
  e=tone(p,0,amp[i],clock_period((Word)(40+i*3),fast),(Byte)(i==1||i==2));if(e)return e;
  e=put(p,(Byte)(0xa0|amp[i]));if(e)return e;
  e=put(p,np[i]);if(e)return e;
  e=put(p,(Byte)(i==1||i==2));if(e)return e;
 }return put(p,0xff);
}
static Byte short_noise(Phrase *p,Byte kind){Byte i,e,steps;
 steps=(Byte)(kind==AUDIO_CHUCK?5:kind==AUDIO_THUD?8:10);
 for(i=0;i<steps;i++){
  Byte amp=(Byte)(14-(i*12)/steps);
  Byte period=(Byte)(kind==AUDIO_CHUCK?8+3*i:kind==AUDIO_THUD?9+3*i:6+3*i);
  Byte ticks=(Byte)(kind==AUDIO_CHUCK?1:kind==AUDIO_THUD?2:14);
  e=noise(p,amp,period,ticks);if(e)return e;
 }return finish_noise(p,(Byte)(8+3*steps));
}
static Byte kaboom(Phrase *p){Byte i,e;
 /* KABOOM's two BOOMER bursts remain separate, with a held-silence gap. */
 for(i=0;i<12;i++){
  if(i==6){e=noise(p,0,24,11);if(e)return e;}
  e=noise(p,(Byte)(14-2*(i%6)),(Byte)(5+4*(i%6)),10);if(e)return e;
 }return finish_noise(p,30);
}
Byte ssc_catalog_build(Byte sound,Byte fast,Byte *data,Byte *length){
 Phrase p;Byte e=AUDIO_UNSUPPORTED;
 if(!data||!length||fast>1)return AUDIO_BAD;
 p.data=data;p.n=0;
 switch(sound){
 case AUDIO_RATTLE:e=rattle(&p,10,12);break;
 case AUDIO_GROWL:case AUDIO_GRAWL:case AUDIO_SNARL:e=roar(&p,sound);break;
 case AUDIO_BEOOP:e=sweep(&p,0,1,fast);break;
 case AUDIO_KLANK:e=metal(&p,350,108,10,fast);break;
 case AUDIO_PSSST:e=rattle(&p,2,16);break;
 case AUDIO_KKLANK:e=metal(&p,100,36,4,fast);break;
 case AUDIO_PSSHT:e=rattle(&p,1,19);break;
 case AUDIO_WIZARD_1:case AUDIO_WIZARD_2:e=wizard(&p,fast);break;
 case AUDIO_GLUGLG:e=sweep(&p,1,4,fast);break;
 case AUDIO_CLANG:e=metal(&p,200,72,7,fast);break;
 case AUDIO_WHOOSH:e=whoosh(&p);break;
 case AUDIO_CHUCK:e=short_noise(&p,AUDIO_CHUCK);break;
 case AUDIO_KLINK:e=klink(&p,fast);break;
 case AUDIO_CLANK:e=metal(&p,50,18,2,fast);break;
 case AUDIO_THUD:case AUDIO_BANG:e=short_noise(&p,sound);break;
 case AUDIO_KABOOM:e=kaboom(&p);break;
 }
 if(!e)*length=p.n;
 return e;
}
static const char *const catalogNames[]={
  "squeak","rattle","growl","beoop","klank","grawl","pssst",
  "kklank","pssht","snarl","wizard-1","wizard-2","gluglg",
  "phaser","whoop","clang","whoosh","chuck","klink","clank",
  "thud","bang","kaboom"
 };
const char *ssc_catalog_name(Byte sound){
 if(sound<=AUDIO_LAST_SOURCE_ID)return catalogNames[sound];
 return (const char *)0;
}
