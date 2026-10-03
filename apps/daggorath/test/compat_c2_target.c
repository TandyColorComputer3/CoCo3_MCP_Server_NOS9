#include <cmoc.h>
#include "compat/compat.h"
#include "compat/present.h"
#include "compat-c2-data.h"

static Byte logicalFrame[DAG_COMPAT_FRAME_BYTES];
static Byte referenceFrame[DAG_COMPAT_FRAME_BYTES];
static Word referenceCursor;
static DagCompatTextContext textContext;
static DagCompatPresentTiming timing;

static Byte five(const Byte *p,Word bit)
{
    Byte value=0,i;
    for(i=0;i<5;i++,bit++)value=(value<<1)|((p[bit/8]>>(7-bit%8))&1);
    return value;
}

static void reference_character(Byte code)
{
    Word row,col,base;Byte y;
    if(code==0x24){referenceCursor=referenceCursor?referenceCursor-1:127;return;}
    if(code==0x1f)referenceCursor=(referenceCursor+32)&~31;
    else if(code<31){
        row=referenceCursor/32;col=referenceCursor%32;base=(160+row*8)*32+col;
        for(y=0;y<7;y++)referenceFrame[base+(Word)y*32]=
            five(compat_packed_font+(Word)code*5,5+(Word)y*5)<<2;
        ++referenceCursor;
    }
    if(referenceCursor>=128){
        for(base=0;base<24*32;base++)referenceFrame[160*32+base]=referenceFrame[168*32+base];
        for(base=184*32;base<192*32;base++)referenceFrame[base]=0;
        referenceCursor=96;
    }
}

static Byte verify(void)
{
    Word i;if(textContext.cursor!=referenceCursor)return ERR_ARGUMENT;
    for(i=0;i<DAG_COMPAT_FRAME_BYTES;i++)if(logicalFrame[i]!=referenceFrame[i])return ERR_ARGUMENT;
    return 0;
}

/* Preserve per-operation target checking without rescanning all 6 KiB after
 * every ordinary glyph.  A printable touches seven bytes unless it triggers
 * the source scroll; clears and scrolls still receive a complete comparison. */
static Byte verify_operation(Byte operation,Byte value,Word before)
{
    Word base;Byte y;
    if(textContext.cursor!=referenceCursor)return ERR_ARGUMENT;
    if(operation==DAG_COMPAT_TEXT_CLEAR_PRIMARY||
       ((operation==DAG_COMPAT_TEXT_CHARACTER||operation==DAG_COMPAT_TEXT_ERASE_CHARACTER)&&
        before==127))return verify();
    if((operation==DAG_COMPAT_TEXT_CHARACTER&&value<0x1f)||
       operation==DAG_COMPAT_TEXT_ERASE_CHARACTER){
        base=(Word)(160+(before/32)*8)*32+before%32;
        for(y=0;y<7;y++)if(logicalFrame[base+(Word)y*32]!=referenceFrame[base+(Word)y*32])
            return ERR_ARGUMENT;
    }
    return 0;
}

static Byte operate(Byte operation,Byte value,Byte present)
{
    Byte e;Word before=textContext.cursor;textContext.operation=operation;textContext.value=value;
    e=dag_compat_text_apply(&textContext);if(e)return e;
    if(operation==DAG_COMPAT_TEXT_CLEAR_PRIMARY){
        Word i;for(i=160*32;i<192*32;i++)referenceFrame[i]=0;referenceCursor=0;
    } else if(operation==DAG_COMPAT_TEXT_CHARACTER||operation==DAG_COMPAT_TEXT_ERASE_CHARACTER)
        reference_character(operation==DAG_COMPAT_TEXT_ERASE_CHARACTER?0:value);
    e=verify_operation(operation,value,before);if(e)return e;
    if(!present)return 0;
    if(operation==DAG_COMPAT_TEXT_CHARACTER&&value<0x1f&&textContext.cursor!=96)
        return dag_compat_present_cell(logicalFrame,before,&timing);
    return dag_compat_present_primary(logicalFrame,&timing);
}

static Byte write_source(const Byte *codes,Word count)
{
    Word i;Byte e;
    for(i=0;i<count;i++){
        e=operate(DAG_COMPAT_TEXT_CHARACTER,codes[i],0);
        if(e)return e;
    }
    return 0;
}

static Byte present_primary(void)
{
    return dag_compat_present_primary(logicalFrame,&timing);
}

static Byte hold(void){return os_sleep(60);}

static Byte verify_clear(Byte operation,Word first,Word last,Byte homes)
{
    Word i;Byte e;
    for(i=0;i<DAG_COMPAT_FRAME_BYTES;i++)logicalFrame[i]=0x5a;
    textContext.logicalFrame=logicalFrame;textContext.cursor=77;textContext.inverse=0;
    textContext.operation=operation;textContext.value=0;
    e=dag_compat_text_apply(&textContext);if(e)return e;
    if(textContext.cursor!=(homes?0:77))return ERR_ARGUMENT;
    for(i=0;i<DAG_COMPAT_FRAME_BYTES;i++)
        if(logicalFrame[i]!=(i>=first&&i<last?0:0x5a))return ERR_ARGUMENT;
    return 0;
}

static Byte verify_clear_contract(void)
{
    Word i;Byte e;
    e=verify_clear(DAG_COMPAT_TEXT_CLEAR_PRIMARY,160*32,192*32,1);if(e)return e;
    e=verify_clear(DAG_COMPAT_TEXT_CLEAR_STATUS,152*32,160*32,1);if(e)return e;
    e=verify_clear(DAG_COMPAT_TEXT_CLEAR_VIEWPORT,0,152*32,0);if(e)return e;
    e=verify_clear(DAG_COMPAT_TEXT_CLEAR_SCREEN,0,192*32,0);if(e)return e;
    for(i=0;i<DAG_COMPAT_FRAME_BYTES;i++){logicalFrame[i]=0xa5;referenceFrame[i]=0x5a;}
    textContext.logicalFrame=referenceFrame;textContext.cursor=77;textContext.inverse=0xff;
    textContext.operation=DAG_COMPAT_TEXT_CLEAR_SCREEN;textContext.value=0;
    e=dag_compat_text_apply(&textContext);if(e)return e;
    for(i=0;i<DAG_COMPAT_FRAME_BYTES;i++)
        if(logicalFrame[i]!=0xa5||referenceFrame[i]!=0xff)return ERR_ARGUMENT;
    textContext.logicalFrame=logicalFrame;textContext.inverse=0;textContext.cursor=0;
    for(i=0;i<DAG_COMPAT_FRAME_BYTES;i++)logicalFrame[i]=referenceFrame[i]=0;
    referenceCursor=0;
    return 0;
}

int main(void)
{
    Byte e=0,row;Word i;
    textContext.logicalFrame=logicalFrame;textContext.cursor=0;textContext.inverse=0;
    e=verify_clear_contract();
    if(!e)e=dag_compat_present_open();
    if(!e)e=operate(DAG_COMPAT_TEXT_CLEAR_PRIMARY,0,1);
    if(!e)e=verify();
    if(!e)e=hold();
    if(!e)e=operate(DAG_COMPAT_TEXT_CHARACTER,0x1f,0);
    if(!e)e=operate(DAG_COMPAT_TEXT_CHARACTER,0x1e,1);
    if(!e)e=verify();
    if(!e)e=hold();
    if(!e)e=operate(DAG_COMPAT_TEXT_CHARACTER,0x1c,1);
    if(!e)e=operate(DAG_COMPAT_TEXT_CHARACTER,0x24,0);
    if(!e)e=verify();
    if(!e)e=hold();
    if(!e)e=operate(DAG_COMPAT_TEXT_ERASE_CHARACTER,0,1);
    if(!e)e=verify();
    if(!e)e=hold();
    if(!e)e=operate(DAG_COMPAT_TEXT_CLEAR_PRIMARY,0,1);
    if(!e)e=write_source(compat_intro1,9);
    if(!e)e=verify();
    if(!e)e=present_primary();
    if(!e)e=write_source(compat_intro1+9,sizeof(compat_intro1)-9);
    if(!e)e=verify();
    if(!e)e=present_primary();
    if(!e)e=write_source(compat_intro2,8);
    if(!e)e=verify();
    if(!e)e=present_primary();
    if(!e)e=write_source(compat_intro2+8,sizeof(compat_intro2)-8);
    if(!e)e=verify();
    if(!e)e=present_primary();
    if(!e)e=hold();
    if(!e)e=operate(DAG_COMPAT_TEXT_CLEAR_PRIMARY,0,1);
    for(row=1;!e&&row<=4;row++)
        for(i=0;i<(row<4?32:31);i++)e=operate(DAG_COMPAT_TEXT_CHARACTER,row,0);
    if(!e)e=verify();
    if(!e)e=dag_compat_present_primary(logicalFrame,&timing);
    if(!e)e=hold();
    if(!e)e=operate(DAG_COMPAT_TEXT_CHARACTER,4,1);
    if(!e)e=verify();
    if(!e)e=hold();
    if(!e)e=operate(DAG_COMPAT_TEXT_CLEAR_PRIMARY,0,1);
    if(!e)e=verify();
    if(!e)e=hold();
    {Byte closeError=dag_compat_present_close();if(!e)e=closeError;}
    return e;
}
