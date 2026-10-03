/* Adapted from VECTOR.ASM VECTOR/INCRE/DIVIDE/VECT30..60, VCTLST.ASM
 * VCTLSX/VCTREL at unity scale, MISC.ASM WIZZES; pinned provenance.json.
 * Original Dyna Micro copyright MCMLXXXII. No OS-9 or physical screen offsets. */
#ifdef _CMOC_VERSION_
#include <cmoc.h>
#else
#include <string.h>
#endif
#include "logical.h"
#include "data.h"
unsigned char wizard_line_progress(unsigned char *frame,int x0,int y0,int x1,int y1,
                                   unsigned char fade,unsigned char (*progress)(void *),void *context)
{
    int dx=x1-x0,dy=y1-y0,n,ay,i;
    long xx=(long)x0*256+128,yy=(long)y0*256+128,sx,sy;
    unsigned int period=(unsigned int)fade+1,count=period;
    n=dx<0?-dx:dx;ay=dy<0?-dy:dy;if(ay>n)n=ay;
    if(!n||fade==255)return 0;
    /* Line setup below performs two generated 32-bit divides before the
     * pixel-loop callback.  Service once at this natural vector boundary so
     * coordinate transforms and fixed-point setup are separately bounded. */
    if(progress&&progress(context))return 1;
    /* INCRE truncates magnitude before restoring sign (signed C division).
     * 24-bit accumulators represented by long; selected geometry cannot overflow. */
    sx=(long)dx*256/n;sy=(long)dy*256/n;
    for(i=0;i<n;++i){
        /* Cooperative foreground service; this changes no VECTOR coordinates,
         * fade samples or pixels. No graphics operation runs in VIRQ context. */
        /* The native PB1 transition is a cheap foreground edge hint; the
         * callback still obtains authoritative phase/generation atomically.
         * Two pixels bound the measured lit-vector gap without changing any
         * original coordinate, fade sample or framebuffer bit. */
        if(progress&&!(i&1)&&progress(context))return 1;
        if(--count==0){
            /* VECTOR reads the integer bytes directly; avoid general long
             * division for fixed-point extraction on the 6809. */
            int x=(int)(xx>>8),y=(int)(yy>>8);count=period;
            if(xx>=0&&yy>=0&&x<256&&y<152)
                frame[y*32+x/8]|=(unsigned char)(128>>(x&7));
        }
        xx+=sx;yy+=sy;
    }
    return 0;
}
void wizard_line(unsigned char *frame,int x0,int y0,int x1,int y1,unsigned char fade)
{
    (void)wizard_line_progress(frame,x0,y0,x1,y1,fade,0,0);
}
/* EXPAND GETFIV: MSB-first five-bit stream. SWCTAB stores 7 rows following
 * a five-bit length. COMTXT NDPB10 centers the glyph by shifting two bits. */
static unsigned char five(const unsigned char *p,unsigned int bit)
{
    unsigned char n=0,i;
    for(i=0;i<5;++i,++bit)n=(n<<1)|((p[bit/8]>>(7-bit%8))&1);
    return n;
}
static void text(unsigned char *frame,const unsigned char *s,unsigned int size,
    unsigned int base,unsigned char inverse)
{
    unsigned int cursor=0,i,row;
    for(i=0;i<size;++i){
        unsigned char c=s[i];
        if(c==31){cursor=(cursor+32)&~31;continue;}
        if(c>=31||cursor>=128)continue;
        for(row=0;row<7;++row)
            frame[base+(cursor/32)*256+cursor%32+row*32]=(five(packed_font+c*5,5+row*5)<<2)^inverse;
        ++cursor;
    }
}
void wizard_copyright(unsigned char *frame)
{
    memset(frame+152*32,255,8*32);
    text(frame,status_text,sizeof(status_text),152*32,255);
}
void wizard_frame(unsigned char *frame,unsigned char fade,unsigned char messages)
{
    unsigned int i;
    memset(frame,0,FRAME_BYTES);wizard_copyright(frame);
    for(i=0;i<sizeof(wizard_segments)/sizeof(wizard_segments[0]);++i){
        const unsigned char *p=wizard_segments[i];wizard_line(frame,p[0],p[1],p[2],p[3],fade);
    }
    if(messages){
        /* TXTPRI cursor is preserved across OUTSTI calls; first message starts
         * with CR and ends with CR, placing messages on rows 168 and 176. */
        text(frame,intro1,sizeof(intro1),160*32,0);
        text(frame,intro2,sizeof(intro2),176*32,0);
    }
}
