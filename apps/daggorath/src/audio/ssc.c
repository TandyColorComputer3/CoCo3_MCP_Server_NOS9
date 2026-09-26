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
static Byte owned,old1,old3,old23,*cancelled;
#include "ssc_io.h"
static Byte send(Byte b){Byte i,e;
 for(i=0;i<12;i++){
  if(os_signal_value(cancelled))return os_signal_value(cancelled);
  if(ssc_read_io(0xff7e)&128){ssc_write_io(0xff7e,b);return os_sleep(1);}
  e=os_sleep(1);if(e)return e;
 }return 246;
}
Byte backend_stop(void){return send(0xcf);}
Byte backend_open(const char *profile,Byte *signal){
 Byte i,e,fast;Word period;static const Word hz[4]={1535,1967,2737,4497};
 cancelled=signal;owned=0;
 if(!strcmp(profile,"ssc-mame-fast"))fast=1;
 else if(!strcmp(profile,"ssc-slow"))fast=0;
 else return AUDIO_UNSUPPORTED;
 if((ssc_read_io(0xff7e)&0xe0)!=0xe0)return AUDIO_BUSY;
 old1=ssc_read_io(0xff01);old3=ssc_read_io(0xff03);old23=ssc_read_io(0xff23);owned=1;
 /* Acquisition once; never reset between events. */
 ssc_write_io(0xff7d,1);ssc_write_io(0xff7d,0);
 ssc_write_io(0xff01,old1&0xf7);ssc_write_io(0xff03,old3|8);ssc_write_io(0xff23,old23|8);
 e=send(0x8f);if(e)return e;e=send(1);if(e)return e;
 e=send(0x98);if(e)return e;
 for(i=0;i<4;i++){
  period=(Word)((fast?3579544UL:1789772UL)/(16UL*hz[i]));
  e=send(12);if(e)return e;e=send((Byte)(period>>8));if(e)return e;
  e=send((Byte)period);if(e)return e;e=send(0);if(e)return e;
 }
 /* Mandatory final silence: channel A amplitude 0, period 1, duration 0. */
 e=send(0);if(e)return e;e=send(0);if(e)return e;e=send(1);if(e)return e;
 e=send(0);if(e)return e;return send(0xff);
}
Byte backend_play(Byte sound,Byte gain){if(!owned)return AUDIO_BAD;
 if(sound!=AUDIO_SQUEAK||gain!=255)return AUDIO_UNSUPPORTED;
 return send(0xd8);
}
Byte backend_drain(void){Byte e;
 /* Conservative bounded settle, then explicit stop. Calibrated live against
  * the firmware sequence; not an audio-rate waveform timer. */
 e=os_sleep(12);if(e)return e;return backend_stop();
}
Byte backend_close(void){Byte e=0,flag;
 if(!owned)return 0;
 /* A handled signal must not prevent cleanup transmission. */
 flag=*cancelled;*cancelled=0;e=backend_stop();
 if(e){ssc_write_io(0xff7d,1);ssc_write_io(0xff7d,0);} /* bounded recovery only */
 ssc_write_io(0xff01,(ssc_read_io(0xff01)&0xf7)|(old1&8));
 ssc_write_io(0xff03,(ssc_read_io(0xff03)&0xf7)|(old3&8));
 ssc_write_io(0xff23,(ssc_read_io(0xff23)&0xf7)|(old23&8));
 owned=0;*cancelled=flag;return e;
}
