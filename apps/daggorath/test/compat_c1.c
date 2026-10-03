#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "compat/present.h"

static Byte source[DAG_COMPAT_FRAME_BYTES];
static Byte mapped[DAG_COMPAT_STRIP_TARGET_BYTES];
static Byte expected[DAG_COMPAT_STRIP_TARGET_BYTES];
static unsigned writes, putCalls, maps, unmaps, closes;
static unsigned failAtWrite;
static Word clockValue;

Byte window_open(Byte *path) { *path=7; return 0; }
Byte window_close(Byte path) { assert(path==7); ++closes; return 0; }
Byte os_clock(Word *ticks,Byte validate) { assert(validate==0); *ticks=clockValue++; return 0; }
Byte os_map_buffer(Byte path,Word id,Byte map,Byte **buffer,Word *length)
{
    assert(path==7 && id==0xc401);
    if(map){ ++maps; *buffer=mapped; *length=sizeof(mapped); }
    else { ++unmaps; assert(*buffer==mapped); }
    return 0;
}
void dag_compat_expand_strip(const Byte *input,Byte *output)
{
    static const Byte expand[16]={0,3,12,15,48,51,60,63,192,195,204,207,240,243,252,255};
    unsigned i;
    for(i=0;i<DAG_COMPAT_STRIP_SOURCE_BYTES;i++){
        output[i*2]=expand[input[i]>>4];
        output[i*2+1]=expand[input[i]&15];
    }
}
Byte os_write(Byte path,const void *raw,Word length)
{
    const Byte *packet=raw;
    ++writes;
    if(writes==failAtWrite)return 42;
    if(length==8 && packet[0]==27 && packet[1]==0x2d){
        unsigned strip=(packet[7]-4)/DAG_COMPAT_STRIP_ROWS;
        unsigned i;
        assert(path==7 && packet[2]==196 && packet[3]==1);
        assert(packet[4]==0 && packet[5]==64 && packet[6]==0 && strip<6);
        dag_compat_expand_strip(source+strip*DAG_COMPAT_STRIP_SOURCE_BYTES,expected);
        for(i=0;i<sizeof(mapped);i++)assert(mapped[i]==expected[i]);
        ++putCalls;
    }
    if(length==4 && packet[0]==27 && packet[1]==0x2a)assert(path==1);
    return 0;
}

int main(void)
{
    DagCompatPresentTiming timing;
    unsigned i;
    for(i=0;i<sizeof(source);i++)source[i]=(Byte)(i^(i>>5));
    assert(dag_compat_present_open()==0);
    assert(maps==1);
    assert(dag_compat_present(source,&timing)==0);
    assert(putCalls==6 && timing.conversionTicks==6 && timing.transferTicks==6);
    assert(timing.mappedLength==DAG_COMPAT_STRIP_TARGET_BYTES);
    assert(dag_compat_present_close()==0);
    assert(unmaps==1 && closes==1 && writes==11);
    failAtWrite=14;
    assert(dag_compat_present_open()==42);
    assert(unmaps==1 && closes==2 && writes==15);
    assert(dag_compat_present_close()==0 && closes==2);
    puts("compat C1 conversion, presentation and ownership checks passed");
    return 0;
}
