#ifdef _CMOC_VERSION_
#include <cmoc.h>
#else
#include <string.h>
#endif
#include "primary-text.h"
#include "game_data.h"
/* COMTXT.ASM:TXTXXX/TXTCR/TXTBS/TXTSCR; TXTSER.ASM:TXTCHR.
 * Retain characters and cursor until CLRPRI, rather than reconstructing one
 * current message from the latest command. This port-owned representation can
 * later be shared by normal interactive play. */
void primary_clear(DagPrimaryText *text){memset(text,0,sizeof(*text));}
void primary_character(DagPrimaryText *text,Byte code)
{
    if(code==36){text->cursor=text->cursor?text->cursor-1:127;return;}
    if(code==31)text->cursor=(text->cursor+32)&~31;
    else if(code<31){text->cells[text->cursor]=code;++text->cursor;}
    if(text->cursor>=128){
        memmove(text->cells,text->cells+32,96);
        memset(text->cells+96,0,32);text->cursor=96;
    }
}
void primary_prompt(DagPrimaryText *text)
{
    /* MISC.ASM:PROMPX uses CR, DOT, BAR, BS. BAR is an underline at the
     * following cell; BS leaves the cursor there for subsequent input. */
    primary_character(text,31);primary_character(text,30);
    primary_character(text,28);primary_character(text,36);
}
static Byte five(const Byte *p,Word bit)
{Byte n=0,i;for(i=0;i<5;i++,bit++)n=(n<<1)|((p[bit/8]>>(7-bit%8))&1);return n;}
void primary_render(const DagPrimaryText *text,Byte *frame)
{
    Byte row,col,y,code;
    memset(frame+160*32,0,32*32);
    for(row=0;row<4;row++)for(col=0;col<32;col++){
        code=text->cells[(Word)row*32+col];
        if(code>=31)continue;
        for(y=0;y<7;y++)frame[(Word)(160+row*8+y)*32+col]=five(font+(Word)code*5,5+y*5)<<2;
    }
}
