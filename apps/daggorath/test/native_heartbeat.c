#include <assert.h>
#include <stdio.h>
#include "native_heartbeat.h"
static unsigned calls,closes;static Byte operation,fail,openfail,closefail,phaseValue,snapshotFault;static Word value,tickValue;static unsigned long edgeGeneration;
Byte ipc_open(const char *name,Byte *p){assert(name[0]=='/');if(openfail)return openfail;*p=7;return 0;}
Byte ipc_close(Byte p){assert(p==7);closes++;return closefail;}
Byte os_setstat(Byte p,Byte op,Registers *r){assert(p==7);calls++;operation=op;value=r->x;return fail;}
Byte os_getstat(Byte p,Byte op,Registers *r){assert(p==7);if(fail)return fail;if(op==0x90){r->a=1;r->x=0x2e03;r->y=0x0100;return 0;}if(op==0x96){r->a=snapshotFault;r->x=edgeGeneration;r->y=edgeGeneration>>16;r->b=phaseValue;return 0;}assert(op==0x95);r->x=tickValue;tickValue=0;return 0;}
int main(void){NativeHeartbeat h={255,0};NativeHeartbeatState s;unsigned n;Word ticks;
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
 n=calls;edgeGeneration=0x12345678UL;phaseValue=1;snapshotFault=0;assert(!native_heartbeat_snapshot(&h,&s)&&s.edgeGeneration==edgeGeneration&&s.phase==1&&!s.fault&&calls==n);
 edgeGeneration++;phaseValue=0;assert(!native_heartbeat_snapshot(&h,&s)&&s.edgeGeneration==edgeGeneration&&s.phase==0);
 snapshotFault=187;assert(!native_heartbeat_snapshot(&h,&s)&&s.fault==187);snapshotFault=0;
 tickValue=0x1234;assert(!native_heartbeat_take_ticks(&h,&ticks)&&ticks==0x1234&&!tickValue);
 tickValue=2;assert(!native_heartbeat_take_ticks(&h,&ticks)&&ticks==2&&!tickValue);
 fail=187;assert(native_heartbeat_take_ticks(&h,&ticks)==187);fail=0;
 fail=187;assert(native_heartbeat_disable(&h)==187&&h.opened);
 n=closes;assert(native_heartbeat_close(&h)==187&&h.opened&&closes==n);fail=0;
 assert(!native_heartbeat_close(&h)&&!h.opened&&operation==0x91);
 n=calls;assert(!native_heartbeat_close(&h)&&calls==n);
 assert(native_heartbeat_query(&h,&s)==187&&native_heartbeat_take_ticks(&h,&ticks)==187);
 assert(native_heartbeat_snapshot(&h,&s)==187);
 puts("20 native heartbeat client/ownership checks passed");return 0;
}
