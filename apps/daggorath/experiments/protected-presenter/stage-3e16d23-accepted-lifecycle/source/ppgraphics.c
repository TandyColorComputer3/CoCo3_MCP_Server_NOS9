#include <cmoc.h>
#include "ppgraphics.h"

/* Exact setup sequence reused from the accepted Daggorath presentation path.
 * That path verifies SS.SCTYP=5 and SS.SCSIZ=80x25 before selecting /w. */
static const Byte setup[]={27,0x20,5,0,0,80,25,1,0,0,27,0x35,0,
    27,0x31,0,0,27,0x31,1,63,5,32};
static const Byte selectWindow[]={27,0x21};
static const Byte endWindow[]={27,0x24};
static const Byte define[]={27,0x29,196,4,2,0};
static const Byte capture[]={27,0x2c,196,4,0,0,0,0,2,0,0,8};
static const Byte kill[]={27,0x2a,196,4};
static Byte put[]={27,0x2d,196,4,0,0,0,0};
static Byte fd=255,owned=0,mapped=0;
static Byte *pixels;
static Word pixelsLength;

static Byte open_window(Byte *path){const char *name="/w";Byte e,n;asm{
 ldx :name
 lda #3
 os9 $84
 bcs @bad
 sta :n
 clrb
@bad
 stb :e
 }if(!e)*path=n;return e;}
static Byte close_window(Byte path){Byte e;asm{
 lda :path
 os9 $8f
 bcs @bad
 clrb
@bad
 stb :e
 }return e;}

Byte pp_graphics_open(Byte *path){Byte e,n;Registers r;fd=255;owned=0;mapped=0;pixels=0;pixelsLength=0;
 e=open_window(&fd);if(e)return e;
 e=os_write(fd,setup,sizeof(setup));if(e)goto fail;
 memset(&r,0,sizeof(r));e=os_getstat(fd,SS_SCTYP,&r);if(e)goto fail;
 if(r.a!=5){e=ERR_ARGUMENT;goto fail;}
 memset(&r,0,sizeof(r));e=os_getstat(fd,SS_SCSIZ,&r);if(e)goto fail;
 if(r.x!=80||r.y!=25){e=ERR_ARGUMENT;goto fail;}
 e=os_write(fd,define,sizeof(define));if(e)goto fail;owned=1;
 /* GetBlk initializes the persistent GPB geometry before process mapping. */
 e=os_write(fd,capture,sizeof(capture));if(e)goto fail;
 e=os_map_buffer(fd,0xc404,1,&pixels,&pixelsLength);if(e)goto fail;mapped=1;
 if(pixelsLength!=512){e=ERR_ARGUMENT;goto fail;}
 e=os_write(fd,selectWindow,sizeof(selectWindow));if(e)goto fail;
 *path=fd;return 0;
fail:
 if(mapped){os_map_buffer(fd,0xc404,0,&pixels,&pixelsLength);mapped=0;pixels=0;}
 if(owned){os_write(fd,kill,sizeof(kill));owned=0;}
 close_window(fd);fd=255;return e;
}

/* The proven 196/4 GPB is 512x8, one bit per pixel (512-byte mapped payload).
 * Each row has 64 bytes. Only its first byte contains the 8x8 checker; the
 * rest is background. The complete PutBlk rows are y=32 or y=64, away from
 * protected heart rows 156..162. */
Byte pp_graphics_draw(Byte path,Word x,Word y){Byte row,e;if(path!=fd||!mapped||!pixels||pixelsLength!=512)return ERR_ARGUMENT;
 memset(pixels,0,512);
 for(row=0;row<8;row++)pixels[(Word)row*64]=(row&1)?0x55:0xaa;
 put[4]=(Byte)(x>>8);put[5]=(Byte)x;put[6]=(Byte)(y>>8);put[7]=(Byte)y;
 e=os_write(path,put,sizeof(put));return e;
}

Byte pp_graphics_close(Byte path){Byte e,first=0;if(path!=fd)return ERR_ARGUMENT;
 if(mapped){e=os_map_buffer(path,0xc404,0,&pixels,&pixelsLength);if(e&&!first)first=e;else{mapped=0;pixels=0;}}
 if(owned){e=os_write(path,kill,sizeof(kill));if(e&&!first)first=e;else owned=0;}
 /* Restore the inherited command output before ending the owned window. */
 e=os_write(1,selectWindow,sizeof(selectWindow));if(e&&!first)first=e;
 e=os_write(path,endWindow,sizeof(endWindow));if(e&&!first)first=e;
 e=close_window(path);if(e&&!first)first=e;fd=255;return first;
}
