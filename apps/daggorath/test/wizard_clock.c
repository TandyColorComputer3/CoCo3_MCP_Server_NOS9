/* Exercise the actual Wizard clock sampler with controlled OS-9 clock values. */
#include <assert.h>

#define main wizard_entry
#include "../src/main.c"
#undef main

static Word nextTick;

Byte os_clock(Word *ticks, Byte validate)
{
    (void)validate;
    *ticks=nextTick;
    return 0;
}

/* These satisfy the unused Wizard entry point in this focused host harness. */
Byte os_intercept(Byte *signal){(void)signal;return 0;}
Byte os_signal_value(Byte *signal){(void)signal;return 0;}
Byte os_sleep(Word ticks){(void)ticks;return 0;}
Byte os_cancel_self(void){return 0;}
Byte os_write(Byte path,const void *buffer,Word size)
{(void)path;(void)buffer;(void)size;return 0;}
Byte screen_open(void){return 0;}
Byte screen_prepare(const Byte *pixels){(void)pixels;return 0;}
Byte screen_flip(void){return 0;}
Byte screen_close(void){return 0;}
Byte screen_handoff(void){return 0;}
Byte screen_path(void){return 1;}
Byte dod_chain(const char *module,const char *parameters,Word length)
{(void)module;(void)parameters;(void)length;return 0;}
void playback_frame(Byte *pixels,Byte fade,Byte messages)
{(void)pixels;(void)fade;(void)messages;}

int main(void)
{
    previous=100;elapsed=10;nextTick=101;
    assert(sample_clock()==0 && previous==101 && elapsed==11);

    /* Ordinary calendar-minute wrap is one real tick. */
    previous=3599;elapsed=20;nextTick=0;
    assert(sample_clock()==0 && previous==0 && elapsed==21);

    /* Observed restored-RTC jump: 3599 -> 420 was 421 apparent ticks. */
    previous=3599;elapsed=30;nextTick=420;
    assert(sample_clock()==0 && previous==420 && elapsed==30);
    nextTick=421;
    assert(sample_clock()==0 && previous==421 && elapsed==31);

    /* Historical live capture: 3596 -> 1575 was 1579 apparent ticks. */
    previous=3596;elapsed=40;nextTick=1575;
    assert(sample_clock()==0 && previous==1575 && elapsed==40);
    nextTick=1576;
    assert(sample_clock()==0 && previous==1576 && elapsed==41);
    return 0;
}
