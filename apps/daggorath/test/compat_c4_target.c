#include <cmoc.h>
#include "compat/c4.h"
#include "compat/opening.h"
#include "compat/present.h"
static Byte stateStorage[DAG_COMPAT_STATE_BYTES+255],frame[DAG_COMPAT_FRAME_BYTES];static Game game;
int main(void){Byte *state=(Byte*)(((Word)stateStorage+255)&0xff00),e;
 e=dag_compat_present_open();if(!e)e=dag_compat_opening_run(state,frame);if(!e)e=dag_compat_c4_run(&game,state,frame);
 if(!e)e=os_sleep(180);{Byte c=dag_compat_present_close();if(!e)e=c;}return e;}
