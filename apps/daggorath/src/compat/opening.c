#include "opening.h"
#include "present.h"
#include "opening-data.h"

static DagCompatTextContext text;
static DagCompatPresentTiming timing;

static Byte text_op(Byte operation,Byte value)
{text.operation=operation;text.value=value;return dag_compat_text_apply(&text);}
static Byte cell(Byte *frame,Word at,Byte inverse,Byte value)
{DagCompatCellContext c;c.logicalFrame=frame;c.cell=at;c.inverse=inverse;c.value=value;return dag_compat_text_cell(&c);}
static Byte present_cell(Byte *frame,Word cursor)
{return dag_compat_present_cell(frame,cursor,&timing);}
static Byte source_wait(void){return os_sleep(81);}
static void sound_event(Byte event){(void)event;/* C3 records source boundaries; backend integration is deferred. */}

Byte dag_compat_opening_run(Byte *state,Byte *frame)
{
    DagCompatWizardContext wizard;Byte e=0,fade,i;Word cursor,j;
    text.logicalFrame=frame;text.cursor=0;text.inverse=0;
    wizard.state=state;wizard.logicalFrame=frame;
    for(j=0;j<DAG_COMPAT_FRAME_BYTES;j++)frame[j]=0;
    text.inverse=0xff;e=text_op(DAG_COMPAT_TEXT_CLEAR_STATUS,0);if(e)return e;
    for(i=0;i<sizeof(compat_status);i++){e=cell(frame,19*32+i,0xff,compat_status[i]);if(e)return e;}
    text.inverse=0;
    sound_event(1); /* COMMON:CLK20 Wizard buzz begins. */
    for(fade=32;;fade-=2){
        e=text_op(DAG_COMPAT_TEXT_CLEAR_VIEWPORT,0);if(e)return e;
        wizard.fade=fade;e=dag_compat_render_wizard(&wizard);if(e)return e;
        e=dag_compat_present(frame,&timing);if(e)return e;
        e=os_sleep(1);if(e)return e; /* MISC:WIZZES SYNC visibility boundary. */
        if(!fade)break;
    }
    sound_event(2); /* MISC:WIZI20 A$EXP1. */
    for(i=0;i<sizeof(compat_opening_intro1);i++){
        cursor=text.cursor;e=text_op(DAG_COMPAT_TEXT_CHARACTER,compat_opening_intro1[i]);if(e)return e;
        if(compat_opening_intro1[i]<31){e=present_cell(frame,cursor);if(e)return e;}
    }
    for(i=0;i<sizeof(compat_opening_intro2);i++){
        cursor=text.cursor;e=text_op(DAG_COMPAT_TEXT_CHARACTER,compat_opening_intro2[i]);if(e)return e;
        if(compat_opening_intro2[i]<31){e=present_cell(frame,cursor);if(e)return e;}
    }
    e=source_wait();if(e)return e;e=source_wait();if(e)return e;
    e=text_op(DAG_COMPAT_TEXT_CLEAR_PRIMARY,0);if(e)return e;
    e=dag_compat_present_primary(frame,&timing);if(e)return e;
    sound_event(3); /* MISC:WIZOX second A$EXP1, then buzz. */
    for(fade=0;fade<32;fade+=2){
        e=text_op(DAG_COMPAT_TEXT_CLEAR_VIEWPORT,0);if(e)return e;
        wizard.fade=fade;e=dag_compat_render_wizard(&wizard);if(e)return e;
        e=dag_compat_present(frame,&timing);if(e)return e;
        e=os_sleep(1);if(e)return e;
    }
    sound_event(0);
    e=text_op(DAG_COMPAT_TEXT_CLEAR_VIEWPORT,0);if(e)return e;
    e=dag_compat_present(frame,&timing);if(e)return e;
    e=os_sleep(1);if(e)return e; /* ONCE:DEMO10 ZFLOP/UPDATE/SYNC. */
    /* GAME20 calls PREPAR before NEWLVL. Preserve status, clear only EXAMINE. */
    e=text_op(DAG_COMPAT_TEXT_CLEAR_VIEWPORT,0);if(e)return e;
    for(i=0;i<sizeof(compat_prepare);i++){e=cell(frame,9*32+12+i,0,compat_prepare[i]);if(e)return e;}
    return dag_compat_present(frame,&timing);
}
