#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "compat/opening.h"
#include "compat/present.h"
static unsigned full,primary,cells,renders,sleeps,wait81;static Byte fades[40];
Byte dag_compat_text_apply(DagCompatTextContext *c){if(c->operation==DAG_COMPAT_TEXT_CHARACTER)c->cursor++;else if(c->operation==DAG_COMPAT_TEXT_CLEAR_PRIMARY)c->cursor=0;return 0;}
Byte dag_compat_text_cell(DagCompatCellContext *c){assert(c->cell<768);return 0;}
Byte dag_compat_render_wizard(DagCompatWizardContext *c){fades[renders++]=c->fade;return 0;}
Byte dag_compat_present(const Byte *f,DagCompatPresentTiming *t){(void)f;(void)t;++full;return 0;}
Byte dag_compat_present_primary(const Byte *f,DagCompatPresentTiming *t){(void)f;(void)t;++primary;return 0;}
Byte dag_compat_present_cell(const Byte *f,Word c,DagCompatPresentTiming *t){(void)f;(void)c;(void)t;++cells;return 0;}
Byte os_sleep(Word n){++sleeps;if(n==81)++wait81;else assert(n==1);return 0;}
int main(void)
{Byte state[DAG_COMPAT_STATE_BYTES],frame[DAG_COMPAT_FRAME_BYTES];unsigned i;memset(state,0,sizeof(state));
 assert(!dag_compat_opening_run(state,frame));assert(renders==33);for(i=0;i<17;i++)assert(fades[i]==32-i*2);for(i=0;i<16;i++)assert(fades[17+i]==i*2);assert(wait81==2);assert(sleeps==36);assert(full==35);assert(primary==1);assert(cells==49);puts("compat C3 source choreography passed");return 0;}
