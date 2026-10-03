#include <cmoc.h>
#include "presentation.h"
#include "logical.h"
Byte window_open(Byte *fd);
Byte window_close(Byte fd);
static Byte fd=255,owned=0,mapped=0,heartOwned=0,heartValid=0;
static Byte returnPath=255;
static Byte *pixels;
static Word mappedLength;
static const Byte *pendingFrame;
static Byte pendingStrips;
static Byte (*pendingProgress)(void *);
static void *pendingContext;
/* Public NitrOS-9 packets, CoWin L0027 and GrfDrv L08E1/L0B3F.
 * DefGPB fails if this buffer already exists: never replace a foreign buffer.
 * Namespace 196/1 is a requested reservation, not presumed private by convention. */
static const Byte setup[]={27,0x20,5,0,0,80,25,1,0,0,27,0x35,0,
    27,0x31,0,0,27,0x31,1,63,5,32}; /* cursor off, owned window only */
/* One mapped 512x32 strip keeps every foreground GFX2 operation below the
 * measured monolithic 512x192 bound. Reuse it serially; Level II process
 * pointers are never shared and no second mapped GP buffer is required. */
static const Byte define[]={27,0x29,196,1,8,0};
static const Byte capture[]={27,0x2c,196,1,0,0,0,0,2,0,0,32};
/* Original COMTXT:TXTDPB writes two eight-pixel heart columns over seven
 * rows. At the 2x presentation boundary this is 32x7 physical pixels.
 * CoWin GPLoad ($2B), GrfDrv L0B3F, initializes the small buffer metadata
 * and copies its 28 data bytes without mapping an extra 8K into the process. */
static Byte heartDefine[]={27,0x29,196,2,0,32};
static Byte heartLoad[]={27,0x2b,196,2,5,0,32,0,7,0,28,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static Byte heartPut[]={27,0x2d,196,2,1,48,0,156};
static Byte heartCache[2][28];
static Byte configuredHearts[28],heartsConfigured=0;
static const Byte doubled[]={0,3,12,15,48,51,60,63,192,195,204,207,240,243,252,255};
/* ONLY presentation knows the physical (+64,+4), with two physical pixels per logical bit translation. */
static Byte put[]={27,0x2d,196,1,0,64,0,4};
static const Byte selectWindow[]={27,0x21},kill[]={27,0x2a,196,1},heartKill[]={27,0x2a,196,2};
/* IOMan SS.DevNm copies a high-bit-terminated device name (at most 32
 * bytes); CoWin SS.ScTyp rejects non-window paths. Validate both inherited
 * paths before creating or selecting a new /w. This supports Term and an
 * ordinary numbered /w while rejecting redirected/mismatched stdio. */
static Byte invocation_window(void){
    char input[33],output[33];Registers r;Byte e,i;
    memset(input,0,sizeof(input));memset(output,0,sizeof(output));
    memset(&r,0,sizeof(r));e=os_getstat(0,SS_SCTYP,&r);if(e)return e;
    memset(&r,0,sizeof(r));e=os_getstat(1,SS_SCTYP,&r);if(e)return e;
    e=os_devname(0,input);if(e)return e;
    e=os_devname(1,output);if(e)return e;
    for(i=0;i<32;i++){
        Byte a=(Byte)input[i],b=(Byte)output[i];
        if((a&127)!=(b&127)||!a||!b)return ERR_ARGUMENT;
        if((a&128)||(b&128)){
            if((a&128)!=(b&128))return ERR_ARGUMENT;
            returnPath=1;return 0;
        }
    }
    return ERR_ARGUMENT;
}
static Byte preload_heart(Byte slot,const Byte *heart)
{
    Byte row,*target,e;
    heartLoad[3]=(Byte)(2+slot);
    for(row=0;row<7;row++){
        target=heartLoad+11+(Word)row*4;
        *target++=doubled[heart[row]>>4];*target++=doubled[heart[row]&15];
        *target++=doubled[heart[7+row]>>4];*target=doubled[heart[7+row]&15];
    }
    e=os_write(fd,heartLoad,sizeof(heartLoad));if(e)return e;
    memcpy(heartCache[slot],heartLoad+11,28);heartValid|=(Byte)(1<<slot);
    return 0;
}
void screen_set_heart_patterns(const Byte heartPatterns[28])
{
    memcpy(configuredHearts,heartPatterns,28);heartsConfigured=1;
}
Byte screen_open(void)
{
    Byte e;Registers r;owned=0;mapped=0;heartOwned=heartValid=0;fd=255;returnPath=255;
    pendingFrame=0;pendingStrips=0;pendingProgress=0;pendingContext=0;
    e=invocation_window();if(e)return e;
    e=window_open(&fd);if(e)return e;
    e=os_write(fd,setup,sizeof(setup));if(e)return e;
    memset(&r,0,sizeof(r));e=os_getstat(fd,SS_SCTYP,&r);if(e)return e;
    if(r.a!=5)return ERR_ARGUMENT;
    memset(&r,0,sizeof(r));e=os_getstat(fd,SS_SCSIZ,&r);if(e)return e;
    if(r.x!=80||r.y!=25)return ERR_ARGUMENT;
    e=os_write(fd,define,sizeof(define));if(e)return e;owned=1;
    /* GetBlk initializes the owned buffer metadata from the blank type-5 screen.
     * Public SS.MpGPB then exposes data, excluding its private buffer header.
     * Provenance: CoWin L0BD1; GrfDrv L0F31; archive/utils/view/view_gfx.a. */
    e=os_write(fd,capture,sizeof(capture));if(e)return e;
    memset(&r,0,sizeof(r));r.x=0xc401;r.y=1;
    e=os_map_buffer(fd,0xc401,1,&pixels,&mappedLength);if(e)return e;mapped=1;
    if(mappedLength!=2048)return ERR_ARGUMENT;
    /* Both immutable COMDAT:SPCTAB phases are resident before INIVUX enables
     * heartbeat delivery. Initialization performs no PutBlk and cannot invent
     * or consume a heartbeat generation. */
    if(heartsConfigured){
        heartDefine[3]=2;e=os_write(fd,heartDefine,sizeof(heartDefine));if(e)return e;heartOwned=1;
        e=preload_heart(0,configuredHearts);if(e)return e;
        heartDefine[3]=3;e=os_write(fd,heartDefine,sizeof(heartDefine));if(e)return e;heartOwned|=2;
        e=preload_heart(1,configuredHearts+14);if(e)return e;
    }
    return os_write(fd,selectWindow,sizeof(selectWindow));
}
/* F$Chain preserves path ownership and CoWin GP storage, but not SS.MpGPB's
 * process-local DAT mapping. See the live proba/probeb chain proof in
 * DAGGORATH_ATTRACT_M1.md and EOU CoWin SS.MpGPB. */
Byte screen_handoff(void)
{
    Byte e;
    if(fd==255||!mapped||!owned)return ERR_ARGUMENT;
    e=os_map_buffer(fd,0xc401,0,&pixels,&mappedLength);
    if(e)return e;
    mapped=0;pixels=0;pendingFrame=0;pendingStrips=0;
    return 0;
}
Byte screen_adopt(Byte inheritedPath)
{
    Registers r;Byte e;
    fd=255;owned=mapped=heartOwned=heartValid=0;returnPath=255;
    pendingFrame=0;pendingStrips=0;pendingProgress=0;pendingContext=0;
    e=invocation_window();if(e)return e;
    memset(&r,0,sizeof(r));e=os_getstat(inheritedPath,SS_SCTYP,&r);if(e)return e;
    if(r.a!=5)return ERR_ARGUMENT;
    memset(&r,0,sizeof(r));e=os_getstat(inheritedPath,SS_SCSIZ,&r);if(e)return e;
    if(r.x!=80||r.y!=25)return ERR_ARGUMENT;
    fd=inheritedPath;owned=1;
    e=os_map_buffer(fd,0xc401,1,&pixels,&mappedLength);if(e)return e;
    mapped=1;if(mappedLength!=2048)return ERR_ARGUMENT;
    if(heartsConfigured){
        heartDefine[3]=2;e=os_write(fd,heartDefine,sizeof(heartDefine));if(e)return e;heartOwned=1;
        e=preload_heart(0,configuredHearts);if(e)return e;
        heartDefine[3]=3;e=os_write(fd,heartDefine,sizeof(heartDefine));if(e)return e;heartOwned|=2;
        e=preload_heart(1,configuredHearts+14);if(e)return e;
    }
    return os_write(fd,selectWindow,sizeof(selectWindow));
}
/* MAME 0.289 gime.cpp legacy emit_mc6847_samples<2>: preserve pixel aspect.
 * Nibble expansion duplicates bits, without resampling or altering logical data. */

static void expand_chunk(const Byte *source,Byte *target,Word count)
{
#ifdef _CMOC_VERSION_
    /* Bounded bit expansion, not an emulated delay. 6809 instructions only. */
    asm {
        pshs y,u
        ldx :source
        ldd :count
        pshs d
        ldu :target
        leay :doubled
@expand
        lda ,x
        lsra
        lsra
        lsra
        lsra
        lda a,y
        sta ,u+
        ldb ,x+
        andb #15
        ldb b,y
        stb ,u+
        ldd ,s
        subd #1
        std ,s
        bne @expand
        leas 2,s
        puls u,y
    }
#else
    Word i;
    for(i=0;i<count;i++){
        Byte b=source[i];*target++=doubled[b>>4];*target++=doubled[b&15];
    }
#endif
}
Byte screen_prepare(const Byte *frame)
{
    pendingFrame=frame;pendingStrips=0x3f;pendingProgress=0;pendingContext=0;
    return 0;
}
Byte screen_prepare_progress(const Byte *frame,Byte (*progress)(void *),void *context)
{
    pendingFrame=frame;pendingStrips=0x3f;pendingProgress=progress;pendingContext=context;
    return 0;
}
Byte screen_prepare_ui(const Byte *frame)
{
    pendingFrame=frame;pendingStrips|=0x30;pendingProgress=0;pendingContext=0;return 0;
}
Byte screen_prepare_ui_progress(const Byte *frame,Byte (*progress)(void *),void *context)
{
    pendingFrame=frame;pendingStrips|=0x30;pendingProgress=progress;pendingContext=context;return 0;
}
static void reconcile_strip_heart(const Byte *frame)
{
    Byte row,col,b,*out;
    /* Heart rows 152..158 are rows 24..30 of strip four. A progress callback
     * may publish a newer authoritative phase after those rows were expanded;
     * patch them before PutBlk so the strip cannot restore an obsolete heart. */
    for(row=152;row<159;row++){
        out=pixels+(Word)(row-128)*64+30;
        for(col=15;col<17;col++){
            b=frame[(Word)row*32+col];*out++=doubled[b>>4];*out++=doubled[b&15];
        }
    }
}
Byte screen_flip(void)
{
    Byte strip,row,e,mask=pendingStrips;const Byte *frame=pendingFrame;
    if(fd==255||!mapped||!frame||!mask)return ERR_ARGUMENT;
    for(strip=0;strip<6;strip++)if(mask&(1<<strip)){
        if(pendingProgress){e=pendingProgress(pendingContext);if(e)return e;}
        for(row=0;row<32;row+=8){
            expand_chunk(frame+(Word)(strip*32+row)*32,pixels+(Word)row*64,256);
            if(pendingProgress){e=pendingProgress(pendingContext);if(e)return e;}
        }
        if(strip==4)reconcile_strip_heart(frame);
        put[7]=(Byte)(4+strip*32);
        e=os_write(fd,put,sizeof(put));if(e)return e;
        if(pendingProgress){e=pendingProgress(pendingContext);if(e)return e;}
    }
    pendingStrips=0;return 0;
}
Byte screen_present(const Byte *frame)
{
    Byte e=screen_prepare(frame);return e?e:screen_flip();
}
Byte screen_present_ui(const Byte *frame)
{
    Byte e=screen_prepare_ui(frame);return e?e:screen_flip();
}
Byte screen_present_ui_progress(const Byte *frame,Byte (*progress)(void *),void *context)
{
    Byte e=screen_prepare_ui_progress(frame,progress,context);
    return e?e:screen_flip();
}
Byte screen_present_heart(const Byte *frame)
{
    Byte row,col,b,*small,slot=0;
    Byte e;
    if(fd==255||!mapped||!heartOwned)return ERR_ARGUMENT;
    for(row=152;row<159;row++){
        small=heartLoad+11+(Word)(row-152)*4;
        for(col=15;col<17;col++){
            b=frame[(Word)row*32+col];*small++=doubled[b>>4];*small++=doubled[b&15];
        }
    }
    if((heartValid&1)&&!memcmp(heartCache[0],heartLoad+11,28))slot=0;
    else if((heartValid&2)&&!memcmp(heartCache[1],heartLoad+11,28))slot=1;
    else {
        slot=(heartValid&1)?1:0;
        if(slot&&!(heartOwned&2)){
            heartDefine[3]=3;e=os_write(fd,heartDefine,sizeof(heartDefine));if(e)return e;
            heartOwned|=2;
        }
        heartLoad[3]=(Byte)(2+slot);e=os_write(fd,heartLoad,sizeof(heartLoad));if(e)return e;
        memcpy(heartCache[slot],heartLoad+11,28);heartValid|=(Byte)(1<<slot);
    }
    heartPut[3]=(Byte)(2+slot);
    return os_write(fd,heartPut,sizeof(heartPut));
}
Byte screen_close(void)
{
    Byte error=0,e;
    if(fd==255)return 0;
    error=os_write(returnPath,selectWindow,sizeof(selectWindow));
    /* Release buffer through the invoking window's parser even if upload left
     * the owned graphics parser waiting for payload. Close releases own window. */
    if(mapped){Registers r;memset(&r,0,sizeof(r));r.x=0xc401;
        e=os_map_buffer(fd,0xc401,0,&pixels,&mappedLength);if(!error)error=e;
        if(e){/* Do not free still-mapped storage. Process exit unmaps it. */
            e=window_close(fd);fd=255;return error;
        }mapped=0;
    }
    if(heartOwned&2){Byte kill3[4]={27,0x2a,196,3};e=os_write(returnPath,kill3,sizeof(kill3));if(!error)error=e;}
    if(heartOwned&1){e=os_write(returnPath,heartKill,sizeof(heartKill));if(!error)error=e;}
    heartOwned=heartValid=0;
    if(owned){e=os_write(returnPath,kill,sizeof(kill));if(!error)error=e;owned=0;}
    e=window_close(fd);if(!error)error=e;fd=255;returnPath=255;
    pendingFrame=0;pendingStrips=0;pendingProgress=0;pendingContext=0;
    return error;
}
/* Owned path for interactive applications; never exposes/reconfigures Term. */
Byte screen_path(void){return fd;}
