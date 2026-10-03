#include <cmoc.h>
#include "presentation.h"
#include "logical.h"
#include "phase-chain.h"
void playback_frame(Byte *frame,Byte fade,Byte messages);
static Byte frame[FRAME_BYTES],signalFlag;
/* Original cartridge frame-notifier trace, first visible frame=0.
 * Measured on MAME 0.289 coco3h; 912 video IRQs to final blank.
 * See docs/apps/DAGGORATH_WIZARD_M1.md, original MISC/ONCE/SOUNDS labels.
 * Absolute deadlines include original drawing and silent sound durations. */
static const Word deadlines[]={0,18,36,54,72,90,108,126,144,162,180,198,216,234,252,271,293,
    377,539,658,676,694,712,730,748,766,784,802,820,838,856,874,892,910,912};
static Word previous,elapsed;
static void sound_event(Byte event){(void)event;}
static Byte sample_clock(void)
{
    Word now,delta;Byte e=os_clock(&now,0);if(e)return e;
    delta=now>=previous?now-previous:3600-previous+now;
    /* EOU Clock2 refreshes calendar time at the minute boundary. A restored
     * RTC can jump there; neither that jump nor a stalled process is animation
     * time. Resume from the new baseline instead of ending the Wizard. */
    if(delta>300){previous=now;return 0;}
    elapsed+=delta;previous=now;return 0;
}
static Byte wait_until(Word deadline)
{
    Byte e;
    for(;;){
        e=os_signal_value(&signalFlag);if(e)return e;
        e=sample_clock();if(e)return e;
        if(elapsed>=deadline)return 0;
        /* Timed sleep yields CPU; always remeasure, never add requested sleeps. */
        e=os_sleep(1);if(e)return e;
    }
}
int main(int argc,char **argv)
{
    Byte error=0,cleanup,index,fade,messages;int cancel=0,opening=0;
    if(argc>2)return ERR_ARGUMENT;
    if(argc==2){cancel=!strcmp(argv[1],"cancel");opening=!strcmp(argv[1],"opening");
        if(!cancel&&!opening)return ERR_ARGUMENT;}
    signalFlag=0;error=os_intercept(&signalFlag);if(error)return error;
    error=os_clock(&previous,1);if(error)return error;
    error=screen_open();if(error)goto done;
    elapsed=0;
    for(index=0;index<35;index++){
        messages=index==17;
        fade=index<17?32-index*2:(index==17?0:(index==34?255:(index-18)*2));
        playback_frame(frame,fade,messages);
        error=screen_prepare(frame);if(error)goto done;
        /* Preparation precedes the deadline; presentation cost is measured live. */
        if(index){error=wait_until(deadlines[index]);if(error)goto done;}
        error=screen_flip();if(error)goto done;
        if(!index){error=os_clock(&previous,0);if(error)goto done;sound_event(1);}
        if(index==16)sound_event(2);
        if(index==18)sound_event(3);
        if(cancel&&index==8){error=os_cancel_self();if(error)goto done;}
        error=os_signal_value(&signalFlag);if(error)goto done;
    }
    error=wait_until(915); /* observable final blank, original blank lasts 3 IRQs */
    if(!error&&opening){
        Byte path=screen_path();char args[5];Word n=0;
        /* No process-local pointer survives Chain. Pass only the inherited
         * graphics path number; the next phase remaps GP 196/1 itself. */
        if(path>=100)args[n++]='0'+path/100;
        if(path>=10)args[n++]='0'+(path/10)%10;
        args[n++]='0'+path%10;args[n++]='\r';
        error=screen_handoff();if(error)goto done;
        error=dod_chain("/d1/dodintro",args,n);
    }
done:
    sound_event(0);cleanup=screen_close();if(cleanup)return cleanup;
    {const char *s="DODWIZ TERM RESTORED\r";Byte e=os_write(1,s,strlen(s));if(e)return e;}
    return error;
}
