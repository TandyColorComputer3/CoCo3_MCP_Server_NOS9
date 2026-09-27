/* Disposable activation-boundary surrogate, NOT gameplay or heartbeat.
 * Original ONCE:GAME50 -> PLOOK:INIVUX enables heartbeat before PUPDAT.
 * Finish intro and platform preparation, register a counter BEFORE first
 * subsequent PutBlk, then exercise resident presentation for 35 iterations.
 */
#include <cmoc.h>
#include "ipc.h"
#include "presentation.h"
#include "logical.h"
void phase(Byte value);
void playback_frame(Byte *,Byte,Byte);
Byte limit=10;
static Byte frame[FRAME_BYTES];
int main(int argc,char **argv){Byte e,c,p=255,child,status=0,i;Registers r;
 (void)argc;(void)argv;phase(70);
 e=ipc_fork("startprobe","9\r",&child);if(e)return e;
 e=ipc_wait(&child,&status);if(e||status)return e?e:status;phase(71);
 e=screen_open();if(e)goto done;
 playback_frame(frame,0,0);e=screen_prepare(frame);if(e)goto done;phase(72);
 e=ipc_open("/vc",&p);if(e)goto done;r.x=r.y=0;
 e=os_setstat(p,0x90,&r);if(e)goto done;phase(60);
 for(i=0;i<35;i++){phase(32);e=screen_flip();phase(33);if(e)goto done;e=os_sleep(30);if(e)goto done;}
 phase(61);
done:if(p!=255){c=ipc_close(p);if(!e)e=c;}
 c=screen_close();if(!e)e=c;phase(62);printf("RESIDENT BOUNDARY EXIT %u\r",e);return e;
}
