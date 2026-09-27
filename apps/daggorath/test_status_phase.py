#!/usr/bin/env python3
"""Gameplay M2 query-extension checks; prior heartbeat assertions unchanged."""
from pathlib import Path
import subprocess,tempfile
app=Path(__file__).resolve().parent
code=r'''
#include <assert.h>
#include "native_heartbeat.h"
static Byte phase,fail;
Byte ipc_open(const char *s,Byte *p){(void)s;*p=7;return 0;}
Byte ipc_close(Byte p){(void)p;return 0;}
Byte os_setstat(Byte p,Byte o,Registers*r){(void)p;(void)o;(void)r;return 0;}
Byte os_getstat(Byte p,Byte o,Registers*r){assert(p==7&&o==0x90);if(fail)return fail;r->a=1;r->b=phase;r->x=0x2e09;r->y=0x0100;return 0;}
int main(void){NativeHeartbeat h={7,1};NativeHeartbeatState s;
for(phase=0;phase<2;phase++){assert(!native_heartbeat_query(&h,&s));assert(s.phase==phase&&s.active==1&&s.enabled==1&&s.rateByte==46&&s.remainingByte==9&&!s.fault);}
fail=245;s.phase=7;assert(native_heartbeat_query(&h,&s)==245&&s.phase==7);return 0;}
'''
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp);(p/'phase.c').write_text(code)
 subprocess.run(['cc','-std=c99','-I'+str(app/'src'),'-I'+str(app/'src/audio'),str(p/'phase.c'),str(app/'src/audio/native_heartbeat.c'),'-o',str(p/'phase')],check=True)
 subprocess.run([str(p/'phase')],check=True)
print('3 native phase query checks passed')
