/* Disposable SSC listening diagnostic. It never runs from dodgame and does
 * not add a production semantic recipe. Source labels are from pinned
 * daggorath-reference/SOUNDS.ASM; these AY phrases are interpretations.
 * SSC command format: Tandy Speech/Sound Cartridge manual pp. 15-19, 25-28.
 */
#include <cmoc.h>
#include "audio.h"
#include "backend.h"
#include "ssc_io.h"
static Byte cancelled;
static Byte send(Byte value){Byte i,e;
 for(i=0;i<12;i++){
  if(os_signal_value(&cancelled))return os_signal_value(&cancelled);
  if(ssc_read_io(0xff7e)&128){ssc_write_io(0xff7e,value);return 0;}
  e=os_sleep(1);if(e)return e;
 }return 246;
}
static Byte bytes(const Byte *data,Byte count){Byte i,e;
 for(i=0;i<count;i++){e=send(data[i]);if(e)return e;}
 return 0;
}
/* Buffer 6 is unused by the established SQUEAK/WHOOP/PHASER backend.
 * Every test phrase terminates on an event boundary and ends in silence. */
static Byte phrase(const Byte *data,Byte count){Byte e;
 e=send(0x9e);if(e)return e;
 e=bytes(data,count);if(e)return e;
 return send(0xde);
}
/* RATTLE: ten noise pulses with intervening silence. Fixed AY period 12,
 * 8-ms event base, amplitude 12. This models pulse grouping, not the DAC
 * random sequence or original exact CPU-loop duration. */
static Byte snake(void){Byte data[61],i,n=0;
 for(i=0;i<10;i++){
  data[n++]=0x8c;data[n++]=12;data[n++]=1;
  data[n++]=0x80;data[n++]=12;data[n++]=1;
 }
 data[n++]=0xff;return phrase(data,n);
}
/* WHOOSH: source noise with a short rise then longer decay. The AY noise
 * generator remains autonomous while the caller sleeps; the shape is a
 * diagnostic approximation, not a cartridge-waveform match. */
static const Byte swordSwing[]={
  0x83,8,0, 0x86,9,0, 0x89,10,0, 0x8c,11,0,
  0x8b,13,1,0x89,15,1,0x86,17,1,0x83,19,1,
  0x80,19,0,0xff
};
/* KLINK: original mixes high tone and noise with a short decay. Separate
 * SSC A/B channels give a source-informed metallic diagnostic. */
static const Byte swordHit[]={
  0x0c,0,40,0, 0xac,4,0,
  0x08,0,43,1, 0xa8,5,1,
  0x04,0,47,1, 0xa4,6,1,
  0x00,0,47,0, 0xa0,6,0,0xff
};
/* CLANK: original detunes two tones; the diagnostic uses two AY channels
 * and a simple stepped decay. Original FREQ1/FREQ2 loop is not reproduced. */
static const Byte playerHit[]={
  0x0c,0,112,0, 0x2b,0,67,1,
  0x08,0,112,1,0x27,0,67,1,
  0x04,0,112,1,0x23,0,67,1,
  0x00,0,112,0,0x20,0,67,0,0xff
};
/* SOUNDS.ASM:SNDTAB assigns KLANK ($AF/$36) and KKLANK ($32/$12) to
 * knight types 1 and 2. SNCLK2 toggles two DAC components under SNCLK5's
 * common decay. This disposable AY translation preserves the two counter
 * ratios and descending envelope, but is not a cycle-accurate waveform.
 * SSC tone and 64-byte phrase format: Tandy SSC manual pp. 15-19, 25-28. */
static Byte knight(Byte second){
 static const Byte amplitude[]={12,10,8,6,4,2,0};
 Byte data[57],i,n=0,level;
 Word a=second?100:350,b=second?36:108;
 for(i=0;i<7;i++){
  level=amplitude[i];
  data[n++]=level;
  data[n++]=(Byte)(a>>8);data[n++]=(Byte)a;data[n++]=level?2:0;
  data[n++]=(Byte)(0x20|level);
  data[n++]=(Byte)(b>>8);data[n++]=(Byte)b;data[n++]=level?2:0;
 }
 data[n++]=0xff;
 return phrase(data,n);
}
int main(int argc,char **argv){Byte e,r,opened=0;
 if(argc!=2)return ERR_ARGUMENT;
 if(strcmp(argv[1],"spider")&&strcmp(argv[1],"snake")&&
    strcmp(argv[1],"sword-swing")&&strcmp(argv[1],"sword-hit")&&
    strcmp(argv[1],"player-hit")&&strcmp(argv[1],"knight-1")&&
    strcmp(argv[1],"knight-2"))return ERR_ARGUMENT;
 e=os_intercept(&cancelled);if(e)return e;
 e=backend_open("ssc-mame-fast",&cancelled);if(e)return e;opened=1;
 if(!strcmp(argv[1],"spider"))e=backend_play(AUDIO_SQUEAK,255);
 else if(!strcmp(argv[1],"snake"))e=snake();
 else if(!strcmp(argv[1],"sword-swing"))e=phrase(swordSwing,sizeof(swordSwing));
 else if(!strcmp(argv[1],"sword-hit"))e=phrase(swordHit,sizeof(swordHit));
 else if(!strcmp(argv[1],"player-hit"))e=phrase(playerHit,sizeof(playerHit));
 else e=knight(!strcmp(argv[1],"knight-2"));
 if(!e)e=os_sleep(90); /* Yield while hardware plays, then leave silence. */
 if(opened){r=backend_close();if(!e)e=r;}
 if(os_signal_value(&cancelled))e=os_signal_value(&cancelled);
 printf("DODLISTEN %s STATUS %u\r",argv[1],e);
 return e;
}
