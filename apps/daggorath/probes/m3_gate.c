/* Bounded SSC isolation probe, never linked into dodaudio.
 * Tandy SSC manual pp24-30: AF register access; 9F/DF sound buffer 7.
 * Reuses unchanged M2 transport/recipes to test actual queued-tone behavior. */
#include <cmoc.h>
#include "ipc.h"
#include "../src/audio/ssc.c"
static Byte signalFlag;
static Byte reg(Byte address,Byte value){Byte e=send(0xaf);if(e)return e;e=send(address);if(e)return e;e=send(value);if(e)return e;return send(0xff);}
static Byte report(const char *label){Word t;Byte e=os_clock(&t,0);printf("GATE %s t=%u status=%u\r",label,t,ssc_read_io(0xff7e));return e;}
int main(int argc,char **argv){Byte e=0,r;const char *mode=argc==2?argv[1]:"silence";
 e=os_intercept(&signalFlag);if(e)return e;
 e=backend_open("ssc-mame-fast",&signalFlag);if(e)goto done;
 /* A-only silence recipe in the unused buffer 7. */
 e=send(0x9f);if(e)goto done;e=silence();if(e)goto done;
 e=reg(2,200);if(e)goto done;e=reg(3,1);if(e)goto done;
 e=reg(7,0x3c);if(e)goto done;e=reg(9,12);if(e)goto done;
 e=report("B-on");if(e)goto done;e=os_sleep(60);if(e)goto done;
 e=backend_play(AUDIO_WHOOP,255);if(e)goto done;e=report("A-start");if(e)goto done;
 if(!strcmp(mode,"cancel")){os_cancel_self();e=os_signal_value(&signalFlag);if(!e)e=3;goto done;}
 if(!strcmp(mode,"silence"))e=send(0xdf);
 else if(!strcmp(mode,"register"))e=reg(8,0);
 else if(!strcmp(mode,"stop-b"))e=reg(9,0);
 else if(strcmp(mode,"natural"))e=AUDIO_BAD;
 if(e)goto done;e=report("operation");if(e)goto done;
 e=os_sleep(120);if(e)goto done;e=report("after");if(e)goto done;
 e=send(0xcf);if(e)goto done;e=report("all-silent");if(e)goto done;e=os_sleep(30);
done:r=backend_close();if(!e)e=r;return e;
}
