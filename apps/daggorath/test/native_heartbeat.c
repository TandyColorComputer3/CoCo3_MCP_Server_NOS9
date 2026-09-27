#include <assert.h>
#include <stdio.h>
#include "native_heartbeat.h"
static unsigned calls,closes;static Byte operation,fail,openfail,closefail;static Word value;
Byte ipc_open(const char *name,Byte *p){assert(name[0]=='/');if(openfail)return openfail;*p=7;return 0;}
Byte ipc_close(Byte p){assert(p==7);closes++;return closefail;}
Byte os_setstat(Byte p,Byte op,Registers *r){assert(p==7);calls++;operation=op;value=r->x;return fail;}
Byte os_getstat(Byte p,Byte op,Registers *r){assert(p==7&&op==0x90);if(fail)return fail;r->a=1;r->x=0x2e03;r->y=0x0100;return 0;}
int main(void){NativeHeartbeat h={255,0};NativeHeartbeatState s;unsigned n;
 assert(native_heartbeat_enable(&h)==187);assert(calls==0);
 openfail=214;assert(native_heartbeat_open(&h)==214);assert(!h.opened);openfail=0;
 fail=250;assert(native_heartbeat_open(&h)==250);assert(!h.opened&&closes==1);fail=0;
 assert(!native_heartbeat_open(&h)&&h.opened&&h.path==7&&operation==0x90);
 n=calls;assert(native_heartbeat_open(&h)==187&&calls==n);
 assert(!native_heartbeat_rate(&h,46)&&operation==0x92&&value==46);
 n=calls;assert(native_heartbeat_rate(&h,256)==187&&calls==n);
 assert(!native_heartbeat_rate(&h,0)&&value==0);
 assert(!native_heartbeat_enable(&h)&&operation==0x93);
 assert(!native_heartbeat_disable(&h)&&operation==0x94);
 assert(!native_heartbeat_enable(&h)&&operation==0x93);
 assert(!native_heartbeat_query(&h,&s)&&s.active==1&&s.enabled==1&&s.rateByte==46&&s.remainingByte==3&&!s.fault);
 fail=187;assert(native_heartbeat_disable(&h)==187&&h.opened);
 n=closes;assert(native_heartbeat_close(&h)==187&&h.opened&&closes==n);fail=0;
 assert(!native_heartbeat_close(&h)&&!h.opened&&operation==0x91);
 n=calls;assert(!native_heartbeat_close(&h)&&calls==n);
 assert(native_heartbeat_query(&h,&s)==187);
 puts("17 native heartbeat client/ownership checks passed");return 0;
}
