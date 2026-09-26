#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "window.h"

int wm_main(int argc, char **argv);
static WindowColors original={15,1,4}, current;
static Byte *signalFlag;
static unsigned sets, ticks, verifiedMessages, colorQueries;
static int queryFail, partialFail, restoreFail, mismatch, outputFail, externalCancel;
static Byte screenType;

Byte os_getstat(Byte path, Byte function, Registers *r)
{
    if (path==255) return 201;
    if (queryFail) return 208;
    if (function==SS_SCTYP) r->a=screenType;
    else if (function==SS_SCSIZ) { r->x=80; r->y=25; }
    else if (function==SS_FBRGS) {
        ++colorQueries;
        r->a=current.foreground; r->b=current.background; r->x=current.border;
        if (mismatch && colorQueries==2) r->a^=1;
    } else assert(0);
    return 0;
}
Byte os_devname(Byte path, char *buffer)
{
    assert(path==1);
    memcpy(buffer,"Term",4); buffer[3]|=128;
    return 0;
}
Byte os_write(Byte path, const void *buffer, Word size)
{
    const Byte *p=buffer;
    assert(path==1);
    if (size==9 && p[0]==27) {
        ++sets;
        assert(p[1]==0x32 && p[3]==27 && p[4]==0x33 && p[6]==27 && p[7]==0x34);
        if (restoreFail && sets==2) return ERR_WRITE;
        current.foreground=p[2];
        if (partialFail && sets==1) return ERR_WRITE;
        current.background=p[5]; current.border=p[8];
    } else {
        if (size>=8 && !memcmp(p,"RESTORED",8)) ++verifiedMessages;
        if (outputFail && size>=11 && !memcmp(p,"DEMO ACTIVE",11)) return ERR_WRITE;
    }
    return 0;
}
Byte os_intercept(Byte *flag) { signalFlag=flag; return 0; }
Byte os_signal_value(Byte *flag) { return *flag; }
Byte os_sleep(Word count) {
    assert(count==1); ++ticks;
    if (externalCancel && ticks==10) *signalFlag=SIGNAL_ABORT;
    return 0;
}
Byte os_cancel_self(void) { *signalFlag=SIGNAL_INTERRUPT; return 0; }
static void reset(void)
{
    current=original; sets=ticks=verifiedMessages=colorQueries=0;
    queryFail=partialFail=restoreFail=mismatch=outputFail=externalCancel=0;
    screenType=2;
}
static int invoke(char *mode)
{
    char *args[]={"wmview",mode,0};
    return wm_main(mode ? 2 : 1,args);
}
static void restored(void)
{
    assert(window_same_colors(&current,&original));
    assert(sets==2 && verifiedMessages==1);
}
int main(void)
{
    reset(); assert(invoke(0)==0); assert(sets==0);
    reset(); assert(invoke("demo")==0); restored(); assert(ticks==600);
    reset(); assert(invoke("fault")==201); restored();
    reset(); assert(invoke("cancel")==3); restored();
    reset(); externalCancel=1; assert(invoke("demo")==2); restored(); assert(ticks==10);
    reset(); partialFail=1; assert(invoke("demo")==ERR_WRITE); restored();
    reset(); outputFail=1; assert(invoke("demo")==ERR_WRITE); restored();
    reset(); mismatch=1; assert(invoke("demo")==ERR_WRITE); restored();
    reset(); restoreFail=1; assert(invoke("demo")==ERR_WRITE); assert(verifiedMessages==0);
    reset(); queryFail=1; assert(invoke("demo")==208); assert(sets==0);
    reset(); screenType=5; assert(invoke("demo")==ERR_ARGUMENT); assert(sets==0);
    reset(); assert(invoke("unknown")==ERR_ARGUMENT); assert(sets==0);
    reset(); original.foreground=original.background; current=original; assert(invoke("demo")==0); restored();
    puts("13 lifecycle cases passed");
    return 0;
}
