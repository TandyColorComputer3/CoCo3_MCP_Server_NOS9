#include <cmoc.h>
#include "present.h"

Byte window_open(Byte *fd);
Byte window_close(Byte fd);
Byte os_write(Byte path,const void *buffer,Word size);
Byte os_map_buffer(Byte path,Word groupBuffer,Byte map,Byte **buffer,Word *length);
Byte os_clock(Word *ticks,Byte validate);

static Byte fd=255,defined,mapped;
static Byte *physicalStrip;
static Word physicalLength;
static const Byte setup[]={27,0x20,5,0,0,80,25,1,0,0,27,0x35,0,
    27,0x31,0,0,27,0x31,1,63,5,32};
static const Byte define[]={27,0x29,196,1,8,0};
static const Byte capture[]={27,0x2c,196,1,0,0,0,0,2,0,0,32};
static const Byte selectWindow[]={27,0x21};
static const Byte kill[]={27,0x2a,196,1};
static Byte put[]={27,0x2d,196,1,0,64,0,4};
static const Byte doubled[]={0,3,12,15,48,51,60,63,192,195,204,207,240,243,252,255};

static Word elapsed(Word before,Word after)
{
    return after>=before?after-before:(Word)(3600-before+after);
}

static Byte open_failed(Byte error)
{
    dag_compat_present_close();
    return error;
}

Byte dag_compat_present_open(void)
{
    Byte e;fd=255;defined=mapped=0;physicalStrip=0;physicalLength=0;
    e=window_open(&fd);if(e)return e;
    e=os_write(fd,setup,sizeof(setup));if(e)return open_failed(e);
    e=os_write(fd,define,sizeof(define));if(e)return open_failed(e);defined=1;
    e=os_write(fd,capture,sizeof(capture));if(e)return open_failed(e);
    e=os_map_buffer(fd,0xc401,1,&physicalStrip,&physicalLength);if(e)return open_failed(e);mapped=1;
    if(physicalLength!=DAG_COMPAT_STRIP_TARGET_BYTES)return open_failed(ERR_ARGUMENT);
    e=os_write(fd,selectWindow,sizeof(selectWindow));return e?open_failed(e):0;
}

Byte dag_compat_present(const Byte logicalFrame[DAG_COMPAT_FRAME_BYTES],
                        DagCompatPresentTiming *timing)
{
    Byte strip,e;Word before,after;
    if(fd==255||!mapped||!logicalFrame||!timing)return ERR_ARGUMENT;
    timing->conversionTicks=timing->transferTicks=0;
    timing->mappedLength=physicalLength;
    for(strip=0;strip<6;strip++){
        e=os_clock(&before,0);if(e)return e;
        dag_compat_expand_strip(logicalFrame+(Word)strip*DAG_COMPAT_STRIP_SOURCE_BYTES,
                                physicalStrip);
        e=os_clock(&after,0);if(e)return e;
        timing->conversionTicks+=elapsed(before,after);
        put[7]=(Byte)(4+(Word)strip*DAG_COMPAT_STRIP_ROWS);
        e=os_clock(&before,0);if(e)return e;
        e=os_write(fd,put,sizeof(put));if(e)return e;
        {
            Byte clockError=os_clock(&after,0);if(clockError)return clockError;
        }
        timing->transferTicks+=elapsed(before,after);
    }
    return 0;
}

Byte dag_compat_present_primary(const Byte logicalFrame[DAG_COMPAT_FRAME_BYTES],
                                DagCompatPresentTiming *timing)
{
    Byte e;Word before,after;
    if(fd==255||!mapped||!logicalFrame||!timing)return ERR_ARGUMENT;
    timing->conversionTicks=timing->transferTicks=0;
    timing->mappedLength=physicalLength;
    e=os_clock(&before,0);if(e)return e;
    dag_compat_expand_strip(logicalFrame+160*DAG_COMPAT_STRIDE,physicalStrip);
    e=os_clock(&after,0);if(e)return e;
    timing->conversionTicks=elapsed(before,after);
    put[7]=164;
    e=os_clock(&before,0);if(e)return e;
    e=os_write(fd,put,sizeof(put));if(e)return e;
    e=os_clock(&after,0);if(e)return e;
    timing->transferTicks=elapsed(before,after);
    return 0;
}

Byte dag_compat_present_cell(const Byte logicalFrame[DAG_COMPAT_FRAME_BYTES],
                             Word cursor, DagCompatPresentTiming *timing)
{
    Byte e,row,col,y,value;Word before,after,source,target;
    if(fd==255||!mapped||!logicalFrame||!timing||cursor>=128)return ERR_ARGUMENT;
    row=(Byte)(cursor/32);col=(Byte)(cursor%32);
    source=(Word)(160+(Word)row*8)*32+col;
    target=(Word)row*8*64+(Word)col*2;
    timing->conversionTicks=timing->transferTicks=0;timing->mappedLength=physicalLength;
    e=os_clock(&before,0);if(e)return e;
    for(y=0;y<8;y++){
        value=logicalFrame[source+(Word)y*32];
        physicalStrip[target+(Word)y*64]=doubled[value>>4];
        physicalStrip[target+(Word)y*64+1]=doubled[value&15];
    }
    e=os_clock(&after,0);if(e)return e;timing->conversionTicks=elapsed(before,after);
    put[7]=164;
    e=os_clock(&before,0);if(e)return e;e=os_write(fd,put,sizeof(put));if(e)return e;
    e=os_clock(&after,0);if(e)return e;timing->transferTicks=elapsed(before,after);
    return 0;
}

Byte dag_compat_present_close(void)
{
    Byte e=0,x;
    if(fd==255)return 0;
    if(mapped){x=os_map_buffer(fd,0xc401,0,&physicalStrip,&physicalLength);if(!e)e=x;mapped=0;}
    if(defined){x=os_write(1,kill,sizeof(kill));if(!e)e=x;defined=0;}
    x=window_close(fd);if(!e)e=x;
    fd=255;physicalStrip=0;physicalLength=0;return e;
}
