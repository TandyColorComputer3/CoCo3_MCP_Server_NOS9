/* Private driver ABI: native/driver.asm. Public syscall wrappers already
 * verified against EOU IOMan/SCF; no application PIA access here.
 * COMMON:CLK30 byte-zero countdown/rate denotes 256 video ticks.
 */
#include "native_heartbeat.h"
#include "ipc.h"
static Byte control(NativeHeartbeat *h,Byte op,Word value){Registers r;
 if(!h->opened)return ERR_ARGUMENT;r.x=value;r.y=0;
 return os_setstat(h->path,op,&r);
}
Byte native_heartbeat_open(NativeHeartbeat *h){Byte e,p;Registers r;
 /* Caller initializes opened=0. Never overwrite a live ownership handle. */
 if(h->opened)return ERR_ARGUMENT;
 e=ipc_open("/dhb",&p);if(e)return e;r.x=r.y=0;
 e=os_setstat(p,0x90,&r);if(e){ipc_close(p);return e;}
 h->path=p;h->opened=1;return 0;
}
Byte native_heartbeat_rate(NativeHeartbeat *h,Word rateByte){
 if(rateByte>255)return ERR_ARGUMENT;return control(h,0x92,rateByte);
}
Byte native_heartbeat_enable(NativeHeartbeat *h){return control(h,0x93,0);}
Byte native_heartbeat_disable(NativeHeartbeat *h){return control(h,0x94,0);}
Byte native_heartbeat_query(NativeHeartbeat *h,NativeHeartbeatState *state){Registers r;Byte e;
 if(!h->opened)return ERR_ARGUMENT;r.x=0;e=os_getstat(h->path,0x90,&r);if(e)return e;
 state->active=r.a;state->rateByte=r.x>>8;state->remainingByte=r.x;
 state->enabled=r.y>>8;state->fault=r.y;return 0;
}
Byte native_heartbeat_close(NativeHeartbeat *h){Byte e;
 if(!h->opened)return 0;
 /* Explicit release first: duplicated/inherited references cannot keep PB1
  * running after a successful semantic shutdown by this owner. Kernel final
  * path close still supplies forced-termination cleanup for the last owner. */
 e=control(h,0x91,0);if(e)return e;e=ipc_close(h->path);
 if(!e){h->opened=0;h->path=255;}return e;
}
