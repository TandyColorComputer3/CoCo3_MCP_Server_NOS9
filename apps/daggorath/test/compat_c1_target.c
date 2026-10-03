#include <cmoc.h>
#include "compat/compat.h"
#include "compat/present.h"

extern const Byte fixtureState[];
extern const Byte fixtureFrame[];
extern const Byte fixtureChanges[];
extern const Word fixtureChangeCount;
extern const Byte fixtureBeat;
extern const Byte fixtureMode;

static Byte stateStorage[DAG_COMPAT_STATE_BYTES+255];
static Byte logicalFrame[DAG_COMPAT_FRAME_BYTES];
static Byte expandedStrip[DAG_COMPAT_STRIP_TARGET_BYTES];

static Byte doubled(Byte nibble)
{
    Byte value=0,bit;
    for(bit=0;bit<4;bit++){
        value<<=2;
        if(nibble&(0x08>>bit))value|=3;
    }
    return value;
}

static Word verify_expander(void)
{
    Word strip,sourceOffset,targetOffset,different=0;
    for(strip=0;strip<DAG_COMPAT_HEIGHT/DAG_COMPAT_STRIP_ROWS;strip++){
        const Byte *source=logicalFrame+strip*DAG_COMPAT_STRIP_SOURCE_BYTES;
        dag_compat_expand_strip(source,expandedStrip);
        for(sourceOffset=0,targetOffset=0;
            sourceOffset<DAG_COMPAT_STRIP_SOURCE_BYTES;
            sourceOffset++,targetOffset+=2){
            Byte packed=source[sourceOffset];
            if(expandedStrip[targetOffset]!=doubled(packed>>4))++different;
            if(expandedStrip[targetOffset+1]!=doubled(packed&15))++different;
        }
    }
    return different;
}

static Word ticks_between(Word before,Word after)
{
    return after>=before?after-before:(Word)(3600-before+after);
}

int main(int argc,char **argv)
{
    Byte *state=(Byte*)(((Word)stateStorage+255)&0xff00),e=0;
    Word i,change=0,next=0xffff,different=0,before=0,after=0;
    DagCompatRenderContext context;
    DagCompatPresentTiming timing;
    for(i=0;i<DAG_COMPAT_STATE_BYTES;i++)state[i]=fixtureState[i];
    for(i=0;i<DAG_COMPAT_FRAME_BYTES;i++)logicalFrame[i]=fixtureFrame[i];
    if(fixtureChangeCount)next=(Word)fixtureChanges[0]*256+fixtureChanges[1];
    context.state=state;context.logicalFrame=logicalFrame;context.mode=fixtureMode;
    e=dag_compat_present_open();
    if(!e)e=os_clock(&before,0);
    if(!e)e=dag_compat_render_giant(&context);
    if(!e)e=os_clock(&after,0);
    if(!e){
        for(i=0;i<DAG_COMPAT_FRAME_BYTES;i++){
            Byte expected=fixtureFrame[i];
            if(i==next){
                expected^=fixtureChanges[change*3+2];
                if(++change<fixtureChangeCount)
                    next=(Word)fixtureChanges[change*3]*256+fixtureChanges[change*3+1];
                else next=0xffff;
            }
            if(logicalFrame[i]!=expected)++different;
        }
        if(change!=fixtureChangeCount)different=0xffff;
    }
    if(!e&&!different)different=verify_expander();
    if(!e&&different)e=ERR_ARGUMENT;
    if(!e)e=dag_compat_present(logicalFrame,&timing);
    if(!e&&argc>1&&argv[1][0]=='m'){
        Byte renderTicks=(Byte)ticks_between(before,after),conversionTicks=(Byte)timing.conversionTicks;
        Byte transferTicks=(Byte)timing.transferTicks;
        for(i=0;i<DAG_COMPAT_FRAME_BYTES;i++)logicalFrame[i]=0;
        logicalFrame[0]=0xa5;logicalFrame[1]=renderTicks;
        logicalFrame[2]=conversionTicks;logicalFrame[3]=transferTicks;
        e=dag_compat_present(logicalFrame,&timing);
    }
    if(!e)e=os_sleep(120);
    {Byte closeError=dag_compat_present_close();if(!e)e=closeError;}
    if(!e&&argc>1&&argv[1][0]!='m'){
        if(argv[1][0]=='r')e=(Byte)ticks_between(before,after);
        else if(argv[1][0]=='c')e=(Byte)timing.conversionTicks;
        else if(argv[1][0]=='t')e=(Byte)timing.transferTicks;
        else e=ERR_ARGUMENT;
    }
    return e;
}
