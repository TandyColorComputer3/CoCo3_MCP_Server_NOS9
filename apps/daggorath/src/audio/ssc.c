/* Enhanced SSC interpretation, not the original DAC waveform.
 * Tandy SSC manual pp10-11,15-18,26-28: BUSY*, AF, buffer 98/D8, CF stop,
 * 8F timer base; each buffer sequence must end with a silence and FF.
 * PIA mux: EOU ASM/NITROS9/SCF/sspak.asm SpkOut.
 * Clock profile: live MAME0.289 fast AY=3579544, slow=1789772.
 * Original SOUNDS.ASM:SQUEAK/SNSQK1/SNSQK2/SNWAIT: 32 descending waits,
 * full-gain high/zero pulse pairs, no envelope. Four grouped rising tones
 * sample delay counts 28,20,12,4 (four groups of eight) to approximate that motion. No original-derived file contains these registers.
 */
#include <cmoc.h>
#include "backend.h"
#include "policy.h"
static AudioVoice voice;
static Byte owned,old1,old3,old23,*cancelled;
#include "ssc_io.h"
static Byte send(Byte b){Byte i,e;
 for(i=0;i<12;i++){
  if(os_signal_value(cancelled))return os_signal_value(cancelled);
  if(ssc_read_io(0xff7e)&128){ssc_write_io(0xff7e,b);return os_sleep(1);}
  e=os_sleep(1);if(e)return e;
 }return 246;
}
Byte backend_stop(void){Byte e=send(0xcf);
 voice.phase=VOICE_STOPPED;voice.decision=DEC_STOP;return e;
}
/* Manual pp10-11: sound-active bit is active low, delayed after execution.
 * Live MAME rejected the one-tick fence: activity was still idle. PLAY now
 * adds six yielding ticks (18 for consecutive-buffer PHASER) before ack.
 * Live CA startup took >0.20 s; six ticks prematurely stopped it.
 * Refresh follows that bounded startup fence; completion uses activity.
 * State is last-observed while blocked on IPC, reconciled before each request. */
static void refresh(void){
 if((voice.phase==VOICE_TRANSIENT||voice.phase==VOICE_REPEATING)&&
    (ssc_read_io(0xff7e)&0x20)){
  voice.phase=VOICE_IDLE;voice.decision=DEC_COMPLETE;
 }
}
Byte backend_phase(void){refresh();return voice.phase;}
Byte backend_decision(void){return voice.decision;}
static Byte tone(Word hz,Byte duration,Byte fast){Byte e;Word period;
 period=(Word)((fast?3579544UL:1789772UL)/(16UL*hz));
 e=send(12);if(e)return e;e=send((Byte)(period>>8));if(e)return e;
 e=send((Byte)period);if(e)return e;return send(duration);
}
static Byte silence(void){Byte e;
 e=send(0);if(e)return e;e=send(0);if(e)return e;e=send(1);if(e)return e;
 e=send(0);if(e)return e;return send(0xff);
}
Byte backend_open(const char *profile,Byte *signal){
 Byte i,e,fast;Word period;static const Word hz[4]={1535,1967,2737,4497};
 cancelled=signal;owned=0;voice.phase=VOICE_IDLE;voice.decision=0;
 if(!strcmp(profile,"ssc-mame-fast"))fast=1;
 else if(!strcmp(profile,"ssc-slow"))fast=0;
 else return AUDIO_UNSUPPORTED;
 if((ssc_read_io(0xff7e)&0xe0)!=0xe0)return AUDIO_BUSY;
 old1=ssc_read_io(0xff01);old3=ssc_read_io(0xff03);old23=ssc_read_io(0xff23);owned=1;
 /* Acquisition once; never reset between events. */
 ssc_write_io(0xff7d,1);ssc_write_io(0xff7d,0);
 /* I/O tracing found first configuration bytes posted during firmware reset.
  * Yield through startup before relying on BUSY* to accept configuration. */
 e=os_sleep(6);if(e)return e;
 ssc_write_io(0xff01,old1&0xf7);ssc_write_io(0xff03,old3|8);ssc_write_io(0xff23,old23|8);
 /* PIC7040 firmware F15C sets timer reload. MAME 0.289 tms7000.cpp
  * timer_run: (reload+1)*16*32/clock, prescaler=$1f from firmware F0BD.
  * 55 at fast clock (27 slow) gives 8.009 ms, avoiding base-1 IRQ overload. */
 e=send(0x8f);if(e)return e;e=send(fast?55:27);if(e)return e;
 e=send(0x98);if(e)return e;
 for(i=0;i<4;i++){
  period=(Word)((fast?3579544UL:1789772UL)/(16UL*hz[i]));
  e=send(12);if(e)return e;e=send((Byte)(period>>8));if(e)return e;
  e=send((Byte)period);if(e)return e;e=send(0);if(e)return e;
 }
 /* Manual pp15-18,26-28: buffer 1 for WHOOP; consecutive buffers 2..5
  * for PHASER's 60 tone events. 245 bytes including silence/terminator.
  * No buffer overlaps SQUEAK; all three use only channel A. */
 e=silence();if(e)return e;
 e=send(0x99);if(e)return e;
 {/* SOUNDS.ASM:WHOOP/SNSQK1: eight groups of 32 descending waits.
   * Source group cycle durations quantized to 18,15,13,11,9,6,4,2 timer ticks. */
  static const Word hzWhoop[8]={225,258,303,367,464,632,991,2289};
  static const Byte duration[8]={17,14,12,10,8,5,3,1};
  for(i=0;i<8;i++){e=tone(hzWhoop[i],duration[i],fast);if(e)return e;}}
 e=silence();if(e)return e;
 e=send(0x8a);if(e)return e;
 {/* SOUNDS.ASM:PHASER/PHAS1/MSQUEK: ten sweeps, each X=64..1. */
  Byte repeat;static const Word hzPhaser[6]={829,991,1231,1578,2289,4162};
  for(repeat=0;repeat<10;repeat++)for(i=0;i<6;i++){
   e=tone(hzPhaser[i],0,fast);if(e)return e;
  }}
 return silence();
}
Byte backend_play(Byte sound,Byte gain){Byte action,e,command;
 if(!owned)return AUDIO_BAD;
 if((sound!=AUDIO_SQUEAK&&sound!=AUDIO_WHOOP&&sound!=AUDIO_PHASER)||gain!=255)return AUDIO_UNSUPPORTED;
 refresh();action=audio_decide(&voice,sound);
 if(action==DEC_IGNORE||action==DEC_COALESCE)return 0;
 if(action==DEC_REPLACE||action==DEC_RETRIGGER){
  voice.phase=VOICE_REPLACING;e=backend_stop();if(e)return e;
 }
 command=sound==AUDIO_SQUEAK?0xd8:sound==AUDIO_WHOOP?0xd9:0xca;
 e=send(command);if(e)return e;
 /* Settling fence for the documented delayed sound-active detector, not a
  * waveform loop. CA traverses a longer command buffer than D8/D9.
 * The submitter and other OS-9 processes remain schedulable. */
 e=os_sleep(sound==AUDIO_PHASER?18:6);if(e)return e;
 audio_begin(&voice,sound);voice.decision=action;return 0;
}
Byte backend_drain(void){Byte e;Word ticks;
 /* Bounded completion polling, not waveform timing. Natural completion cannot
  * resurrect an interrupted event: there is no deferred software event queue. */
 for(ticks=0;ticks<180;ticks++){
  refresh();if(voice.phase!=VOICE_TRANSIENT&&voice.phase!=VOICE_REPEATING)return backend_stop();
  e=os_sleep(1);if(e)return e;
 }
 e=backend_stop();return e?e:246;
}

Byte backend_close(void){Byte e=0,flag;
 if(!owned)return 0;
 /* A handled signal must not prevent cleanup transmission. */
 flag=*cancelled;*cancelled=0;e=backend_stop();
 if(e){ssc_write_io(0xff7d,1);ssc_write_io(0xff7d,0);} /* bounded recovery only */
 ssc_write_io(0xff01,(ssc_read_io(0xff01)&0xf7)|(old1&8));
 ssc_write_io(0xff03,(ssc_read_io(0xff03)&0xf7)|(old3&8));
 ssc_write_io(0xff23,(ssc_read_io(0xff23)&0xf7)|(old23&8));
 owned=0;voice.phase=VOICE_SHUTDOWN;*cancelled=flag;return e;
}
