#include <cmoc.h>
#include "presentation.h"
#include "logical.h"
Byte window_open(Byte *fd);
Byte window_close(Byte fd);
static Byte fd=255,owned=0,mapped=0;
static Byte *pixels;
static Word mappedLength;
/* Public NitrOS-9 packets, CoWin L0027 and GrfDrv L08E1/L0B3F.
 * DefGPB fails if this buffer already exists: never replace a foreign buffer.
 * Namespace 196/1 is a requested reservation, not presumed private by convention. */
static const Byte setup[]={27,0x20,5,0,0,80,25,1,0,0,27,0x35,0,
    27,0x31,0,0,27,0x31,1,63,5,32}; /* cursor off, owned window only */
static const Byte define[]={27,0x29,196,1,48,0};
static const Byte capture[]={27,0x2c,196,1,0,0,0,0,2,0,0,192};
/* ONLY presentation knows the physical (+64,+4), with two physical pixels per logical bit translation. */
static const Byte put[]={27,0x2d,196,1,0,64,0,4};
static const Byte selectWindow[]={27,0x21},kill[]={27,0x2a,196,1};
Byte screen_open(void)
{
    Byte e;Registers r;owned=0;mapped=0;fd=255;
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
    if(mappedLength!=12288)return ERR_ARGUMENT;
    return os_write(fd,selectWindow,sizeof(selectWindow));
}
/* MAME 0.289 gime.cpp legacy emit_mc6847_samples<2>: preserve pixel aspect.
 * Nibble expansion duplicates bits, without resampling or altering logical data. */

static const Byte doubled[]={0,3,12,15,48,51,60,63,192,195,204,207,240,243,252,255};
Byte screen_prepare(const Byte *frame)
{
#ifdef _CMOC_VERSION_
    /* Bounded bit expansion, not an emulated delay. 6809 instructions only. */
    asm {
        pshs y,u
        ldx :frame
        ldu :pixels
        leay :doubled
        ldd #6144
        pshs d
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
    Word i;Byte *out=pixels;
    for(i=0;i<FRAME_BYTES;i++){
        Byte b=frame[i];*out++=doubled[b>>4];*out++=doubled[b&15];
    }
#endif
    return 0;
}
Byte screen_flip(void){return os_write(fd,put,sizeof(put));}
Byte screen_present(const Byte *frame)
{
    Byte e=screen_prepare(frame);return e?e:screen_flip();
}
Byte screen_close(void)
{
    Byte error=0,e;
    if(fd==255)return 0;
    error=os_write(1,selectWindow,sizeof(selectWindow));
    /* Release buffer through Term's parser even if interrupted upload left
     * the owned graphics parser waiting for payload. Close releases own window. */
    if(mapped){Registers r;memset(&r,0,sizeof(r));r.x=0xc401;
        e=os_map_buffer(fd,0xc401,0,&pixels,&mappedLength);if(!error)error=e;
        if(e){/* Do not free still-mapped storage. Process exit unmaps it. */
            e=window_close(fd);fd=255;return error;
        }mapped=0;
    }
    if(owned){e=os_write(1,kill,sizeof(kill));if(!error)error=e;owned=0;}
    e=window_close(fd);if(!error)error=e;fd=255;
    return error;
}
