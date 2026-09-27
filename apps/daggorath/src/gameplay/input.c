#include "platform.h"
#include "ipc.h"
/* VTIO GSReady, upstream f470fa52: SS.Ready returns E$NotRdy ($f6)
 * while empty. Caller sleeps between polls so maintenance can run. I$Read
 * consumes exactly one available byte, not an unbounded line read. */
Byte game_input(Byte path,Byte *key){Registers r;Byte e;static Byte prepared=0;Byte options[32];
 /* Public SS.Opt; defs/scf.d PD.EKO-PD.OPT=4, PD.PAU-PD.OPT=7.
  * The path belongs to our new /w. Prevent SCF echo outside the logical
  * viewport and page pauses; preserve interrupt/quit and all other options.
  * Closing this owned path releases these options; Term is never changed. */
 if(!prepared){r.x=(Word)options;r.y=0;e=os_getstat(path,0,&r);if(e)return e;
  options[4]=0;options[7]=0;r.x=(Word)options;r.y=0;e=os_setstat(path,0,&r);if(e)return e;prepared=1;}
 r.x=0;r.y=0;*key=0;
 e=os_getstat(path,1,&r);if(e==246)return 0;if(e)return e;
 return ipc_read(path,key,1);
}
