#include <cmoc.h>
#include "present.h"

Byte window_open(Byte *fd);
Byte window_close(Byte fd);
Byte os_write(Byte path,const void *buffer,Word size);
Byte os_map_buffer(Byte path,Word groupBuffer,Byte map,Byte **buffer,Word *length);
Byte os_clock(Word *ticks,Byte validate);

#ifdef DAG_COMPAT_HEARTBEAT
static Byte fd=255,defined,mapped,heartOwned,heartValid,heartsConfigured;
#else
static Byte fd=255,defined,mapped;
#endif
static Byte *physicalStrip;
static Word physicalLength;
static const Byte setup[]={27,0x20,5,0,0,80,25,1,0,0,27,0x35,0,
    27,0x31,0,0,27,0x31,1,63,5,32};
#ifdef DAG_COMPAT_C6
static DagHeartbeatBroker *broker;
static const Byte define[]={27,0x29,196,1,2,0};
static const Byte capture[]={27,0x2c,196,1,0,0,0,0,2,0,0,8};
#else
static const Byte define[]={27,0x29,196,1,8,0};
static const Byte capture[]={27,0x2c,196,1,0,0,0,0,2,0,0,32};
#endif
static const Byte selectWindow[]={27,0x21};
static const Byte kill[]={27,0x2a,196,1};
static Byte put[]={27,0x2d,196,1,0,64,0,4};
#ifdef DAG_COMPAT_HEARTBEAT
static Byte heartDefine[]={27,0x29,196,2,0,32};
static Byte heartLoad[]={27,0x2b,196,2,5,0,32,0,7,0,28,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#ifndef DAG_COMPAT_C6
static Byte heartPut[]={27,0x2d,196,2,1,48,0,156};
#endif
static Byte configuredHearts[28],heartCache[2][28];
#endif
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

#ifdef DAG_COMPAT_HEARTBEAT
static Byte preload_heart(Byte slot,const Byte *heart)
{
    Byte row,*target,e;heartLoad[3]=(Byte)(2+slot);
    for(row=0;row<7;row++){
        target=heartLoad+11+(Word)row*4;
        *target++=doubled[heart[row]>>4];*target++=doubled[heart[row]&15];
        *target++=doubled[heart[7+row]>>4];*target=doubled[heart[7+row]&15];
    }
    e=os_write(fd,heartLoad,sizeof(heartLoad));if(e)return e;
    memcpy(heartCache[slot],heartLoad+11,28);heartValid|=(Byte)(1<<slot);return 0;
}

void dag_compat_present_set_hearts(const Byte heartPatterns[28])
{memcpy(configuredHearts,heartPatterns,28);heartsConfigured=1;}
#ifdef DAG_COMPAT_C6
void dag_compat_present_set_broker(DagHeartbeatBroker *value){broker=value;}

static void patch_status_heart(Byte firstRow,Byte phase)
{
    Byte row,*target,lastRow=(Byte)(firstRow+DAG_COMPAT_PRESENT_ROWS);
    row=firstRow>152?firstRow:152;if(lastRow>159)lastRow=159;
    for(;row<lastRow;row++){
        target=physicalStrip+(Word)(row-firstRow)*64+30;
        memcpy(target,heartCache[phase]+(Word)(row-152)*4,4);
    }
}

static Byte submit_strip(Byte destinationY,Byte status,Byte firstRow)
{
    Byte e,stale,attempt;
    if(!broker||!broker->opened)return ERR_ARGUMENT;
    for(attempt=0;attempt<3;attempt++){
        if(status){e=dag_broker_snapshot(broker);if(e)return e;patch_status_heart(firstRow,broker->phase);}
        e=dag_broker_scene(broker,destinationY,status,&stale);if(e)return e;
        if(!stale)return 0;
    }
    return 188;
}
#endif
#endif

Byte dag_compat_present_open(void)
{
#ifdef DAG_COMPAT_HEARTBEAT
    Byte e;fd=255;defined=mapped=heartOwned=heartValid=0;physicalStrip=0;physicalLength=0;
#else
    Byte e;fd=255;defined=mapped=0;physicalStrip=0;physicalLength=0;
#endif
    e=window_open(&fd);if(e)return e;
    e=os_write(fd,setup,sizeof(setup));if(e)return open_failed(e);
    e=os_write(fd,define,sizeof(define));if(e)return open_failed(e);defined=1;
    e=os_write(fd,capture,sizeof(capture));if(e)return open_failed(e);
    e=os_map_buffer(fd,0xc401,1,&physicalStrip,&physicalLength);if(e)return open_failed(e);mapped=1;
    if(physicalLength!=DAG_COMPAT_PRESENT_TARGET_BYTES)return open_failed(ERR_ARGUMENT);
#ifdef DAG_COMPAT_HEARTBEAT
    if(heartsConfigured){
        heartDefine[3]=2;e=os_write(fd,heartDefine,sizeof(heartDefine));if(e)return open_failed(e);heartOwned=1;
        e=preload_heart(0,configuredHearts);if(e)return open_failed(e);
        heartDefine[3]=3;e=os_write(fd,heartDefine,sizeof(heartDefine));if(e)return open_failed(e);heartOwned|=2;
        e=preload_heart(1,configuredHearts+14);if(e)return open_failed(e);
    }
#endif
    e=os_write(fd,selectWindow,sizeof(selectWindow));return e?open_failed(e):0;
}

Byte dag_compat_present_progress(const Byte logicalFrame[DAG_COMPAT_FRAME_BYTES],
                                 DagCompatPresentTiming *timing,
                                 Byte (*progress)(void *),void *context)
{
    Byte strip,e;Word before,after;
    if(fd==255||!mapped||!logicalFrame||!timing)return ERR_ARGUMENT;
    timing->conversionTicks=timing->transferTicks=0;
    timing->mappedLength=physicalLength;
    for(strip=0;strip<DAG_COMPAT_HEIGHT/DAG_COMPAT_PRESENT_ROWS;strip++){
        if(progress){e=progress(context);if(e)return e;}
        e=os_clock(&before,0);if(e)return e;
#ifdef DAG_COMPAT_C6
        dag_compat_expand_slice(logicalFrame+(Word)strip*DAG_COMPAT_PRESENT_SOURCE_BYTES,
                                physicalStrip);
#else
        dag_compat_expand_strip(logicalFrame+(Word)strip*DAG_COMPAT_PRESENT_SOURCE_BYTES,
                                physicalStrip);
#endif
        e=os_clock(&after,0);if(e)return e;
        timing->conversionTicks+=elapsed(before,after);
        /* Expansion and the CoWin PutBlk are independent pieces of host
         * presentation work.  Let the native-heartbeat reconciler run at
         * their established boundary so neither operation can hide a
         * complete source heartbeat interval. */
        if(progress){e=progress(context);if(e)return e;}
        put[7]=(Byte)(4+(Word)strip*DAG_COMPAT_PRESENT_ROWS);
        e=os_clock(&before,0);if(e)return e;
#ifdef DAG_COMPAT_C6
        e=submit_strip(put[7],(Byte)((Word)strip*DAG_COMPAT_PRESENT_ROWS<=158&&
                       (Word)(strip+1)*DAG_COMPAT_PRESENT_ROWS>152),
                       (Byte)((Word)strip*DAG_COMPAT_PRESENT_ROWS));
#else
        e=os_write(fd,put,sizeof(put));if(e)return e;
#endif
        if(e)return e;
        {
            Byte clockError=os_clock(&after,0);if(clockError)return clockError;
        }
        timing->transferTicks+=elapsed(before,after);
        if(progress){e=progress(context);if(e)return e;}
    }
    return 0;
}

Byte dag_compat_present(const Byte logicalFrame[DAG_COMPAT_FRAME_BYTES],
                        DagCompatPresentTiming *timing)
{return dag_compat_present_progress(logicalFrame,timing,0,0);}

Byte dag_compat_present_primary_progress(const Byte logicalFrame[DAG_COMPAT_FRAME_BYTES],
                                         DagCompatPresentTiming *timing,
                                         Byte (*progress)(void *),void *context)
{
    Byte e,strip;Word before,after;
    if(fd==255||!mapped||!logicalFrame||!timing)return ERR_ARGUMENT;
    timing->conversionTicks=timing->transferTicks=0;
    timing->mappedLength=physicalLength;
    for(strip=0;strip<32/DAG_COMPAT_PRESENT_ROWS;strip++){
        if(progress){e=progress(context);if(e)return e;}
        e=os_clock(&before,0);if(e)return e;
#ifdef DAG_COMPAT_C6
        dag_compat_expand_slice(logicalFrame+(Word)(160+(Word)strip*DAG_COMPAT_PRESENT_ROWS)*32,physicalStrip);
#else
        dag_compat_expand_strip(logicalFrame+160*DAG_COMPAT_STRIDE,physicalStrip);
#endif
        e=os_clock(&after,0);if(e)return e;timing->conversionTicks+=elapsed(before,after);
        if(progress){e=progress(context);if(e)return e;}
        put[7]=(Byte)(164+(Word)strip*DAG_COMPAT_PRESENT_ROWS);
        e=os_clock(&before,0);if(e)return e;
#ifdef DAG_COMPAT_C6
        e=submit_strip(put[7],0,(Byte)(160+(Word)strip*DAG_COMPAT_PRESENT_ROWS));if(e)return e;
#else
        e=os_write(fd,put,sizeof(put));if(e)return e;
#endif
        e=os_clock(&after,0);if(e)return e;timing->transferTicks+=elapsed(before,after);
        if(progress){e=progress(context);if(e)return e;}
    }
    return 0;
}

Byte dag_compat_present_primary(const Byte logicalFrame[DAG_COMPAT_FRAME_BYTES],
                                DagCompatPresentTiming *timing)
{return dag_compat_present_primary_progress(logicalFrame,timing,0,0);}

#ifdef DAG_COMPAT_HEARTBEAT
Byte dag_compat_present_heart(const Byte logicalFrame[DAG_COMPAT_FRAME_BYTES])
{
#ifdef DAG_COMPAT_C6
    (void)logicalFrame;return broker?dag_broker_snapshot(broker):ERR_ARGUMENT;
#else
    Byte row,col,b,*target,slot=0,e;
    if(fd==255||!mapped||!logicalFrame||!heartOwned)return ERR_ARGUMENT;
    for(row=152;row<159;row++){
        target=heartLoad+11+(Word)(row-152)*4;
        for(col=15;col<17;col++){
            b=logicalFrame[(Word)row*32+col];*target++=doubled[b>>4];*target++=doubled[b&15];
        }
    }
    if((heartValid&1)&&!memcmp(heartCache[0],heartLoad+11,28))slot=0;
    else if((heartValid&2)&&!memcmp(heartCache[1],heartLoad+11,28))slot=1;
    else {
        slot=(heartValid&1)?1:0;heartLoad[3]=(Byte)(2+slot);
        e=os_write(fd,heartLoad,sizeof(heartLoad));if(e)return e;
        memcpy(heartCache[slot],heartLoad+11,28);heartValid|=(Byte)(1<<slot);
    }
    heartPut[3]=(Byte)(2+slot);return os_write(fd,heartPut,sizeof(heartPut));
#endif
}
#endif

Byte dag_compat_present_cell(const Byte logicalFrame[DAG_COMPAT_FRAME_BYTES],
                             Word cursor, DagCompatPresentTiming *timing)
{
    Byte e,row,col,y,value;Word before,after,source,target;
#ifdef DAG_COMPAT_C6
    Byte strip;
#endif
    if(fd==255||!mapped||!logicalFrame||!timing||cursor>=128)return ERR_ARGUMENT;
    row=(Byte)(cursor/32);col=(Byte)(cursor%32);
    source=(Word)(160+(Word)row*8)*32+col;
    target=(Word)row*8*64+(Word)col*2;
    timing->conversionTicks=timing->transferTicks=0;timing->mappedLength=physicalLength;
#ifdef DAG_COMPAT_C6
    (void)source;(void)target;(void)y;(void)value;
    for(strip=0;strip<1;strip++){
        e=os_clock(&before,0);if(e)return e;
        dag_compat_expand_slice(logicalFrame+(Word)(160+(Word)row*8+(Word)strip*DAG_COMPAT_PRESENT_ROWS)*32,physicalStrip);
        e=os_clock(&after,0);if(e)return e;timing->conversionTicks+=elapsed(before,after);
        put[7]=(Byte)(164+(Word)row*8+(Word)strip*DAG_COMPAT_PRESENT_ROWS);
        e=os_clock(&before,0);if(e)return e;
        e=submit_strip(put[7],0,(Byte)(160+(Word)row*8+(Word)strip*DAG_COMPAT_PRESENT_ROWS));if(e)return e;
        e=os_clock(&after,0);if(e)return e;timing->transferTicks+=elapsed(before,after);
    }
    return 0;
#else
    e=os_clock(&before,0);if(e)return e;
    for(y=0;y<8;y++){
        value=logicalFrame[source+(Word)y*32];
        physicalStrip[target+(Word)y*64]=doubled[value>>4];
        physicalStrip[target+(Word)y*64+1]=doubled[value&15];
    }
    e=os_clock(&after,0);if(e)return e;timing->conversionTicks=elapsed(before,after);
    put[7]=164;
#endif
    e=os_clock(&before,0);if(e)return e;e=os_write(fd,put,sizeof(put));if(e)return e;
    e=os_clock(&after,0);if(e)return e;timing->transferTicks=elapsed(before,after);
    return 0;
}

Byte dag_compat_present_close(void)
{
    Byte e=0,x;
    if(fd==255)return 0;
    if(mapped){x=os_map_buffer(fd,0xc401,0,&physicalStrip,&physicalLength);if(!e)e=x;mapped=0;}
#ifdef DAG_COMPAT_HEARTBEAT
    if(heartOwned&2){Byte kill3[4]={27,0x2a,196,3};x=os_write(fd,kill3,sizeof(kill3));if(!e)e=x;}
    if(heartOwned&1){Byte heartKill[4]={27,0x2a,196,2};x=os_write(fd,heartKill,sizeof(heartKill));if(!e)e=x;}
    heartOwned=heartValid=0;
#endif
    if(defined){x=os_write(fd,kill,sizeof(kill));if(!e)e=x;defined=0;}
    x=window_close(fd);if(!e)e=x;
    fd=255;physicalStrip=0;physicalLength=0;
#ifdef DAG_COMPAT_C6
    broker=0;
#endif
    return e;
}
Byte dag_compat_present_path(void){return fd;}
