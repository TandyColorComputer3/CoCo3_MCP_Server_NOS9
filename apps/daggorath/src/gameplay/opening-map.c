#ifdef _CMOC_VERSION_
#include <cmoc.h>
#else
#include <string.h>
#endif
#include "opening-map.h"
/* MAPPER.ASM:MAPPER/MAPP10/MARK4: each of the 32 map rows occupies six
 * physical logical scanlines, and each cell one byte (eight pixels). A
 * $FF DGNGEN cell is solid; any occupied cell begins black. MAPFLG is set
 * in ONCE.ASM:GAME40, so unowned level objects and active CCBs are marked. */
static void mark(Byte *frame,Byte row,Byte col,Byte a,Byte b)
{
    Word at=(Word)row*192+col;
    frame[at+32]=a;frame[at+64]=b;frame[at+96]=b;frame[at+128]=a;
}
void opening_render_map(const Game *g,Byte *frame)
{
    Byte row,col,y,i;const Byte *o,*c;
    memset(frame,0,6144);
    for(row=0;row<32;row++)for(col=0;col<32;col++){
        Byte fill=g->maze[(Word)row*32+col]==255?255:0;
        for(y=0;y<6;y++)frame[(Word)row*192+(Word)y*32+col]=fill;
    }
    /* COMCRE.ASM:FNDOBJ filters by LEVEL before MAPPER tests ownership. */
    for(i=0;i<g->count;i++){o=g->objects[i];
        if(o[4]==g->level&&!o[5])mark(frame,o[2],o[3],0,8);
    }
    for(i=0;i<32;i++){c=g->creatures[i];
        if(c[12])mark(frame,c[15],c[16],0x10,0x54);
    }
    mark(frame,g->row,g->col,0x24,0x18);
    /* COMCRE.ASM:VFTTAB's level-two entry is a lone $80 terminator, so no
     * vertical-feature marks exist for this exact source demo map. */
}
