/* Disposable VIRQ lifecycle controller. Private Get/SetStat ABI is counter.asm.
 * Existing syscall wrappers: provenance in src/os9.c and audio/ipc.c.
 * Open path remains live across an injected stop failure; no unlink on failure. */
#include <cmoc.h>
#include "ipc.h"
static Byte signalFlag;
static Byte query(Byte p,Word *n){Registers r;Byte e;r.x=0;e=os_getstat(p,0x90,&r);if(!e){*n=r.x;printf("COUNTER %u STATIC %04x\r",r.x,r.y);}return e;}
static Byte control(Byte p,Byte op){Registers r;r.x=r.y=0;return os_setstat(p,op,&r);}
int main(int argc,char **argv){Byte p,q,e,r,child,status;Word a,b;const char *mode=argc>1?argv[1]:"normal";
 if(!strcmp(mode,"cpu")){Word t,last,dt,total=0,i;os_clock(&last,1);while(total<900){for(i=0;i<1000;i++)a=i;os_clock(&t,0);dt=(t+3600-last)%3600;if(dt>300)return 187;total+=dt;last=t;}printf("CPU DONE %u\r",total);return 0;}
 if(!strcmp(mode,"daemon")){e=ipc_fork("/d1/vctest","hold\r",&child);printf("DAEMON PID %u ERROR %u\r",child,e);return e;}
 if(!strcmp(mode,"terminate")){if(argc!=3)return 187;return ipc_signal(atoi(argv[2]),0);}
 e=os_intercept(&signalFlag);if(e)return e;
 e=ipc_open("/vc",&p);printf("OPEN %u\r",e);if(e)return e;
 e=control(p,0x90);printf("START %u\r",e);if(e)goto done;
 if(!strcmp(mode,"owner")){e=ipc_open("/vc",&q);if(e)goto done;r=control(q,0x90);printf("SECOND CLAIM %u\r",r);ipc_close(q);if(!r){e=187;goto done;}}
 if(!strcmp(mode,"invalid")){r=control(p,0x94);printf("INVALID OP %u\r",r);if(!r){e=187;goto done;}}
 if(!strcmp(mode,"unlink")){e=ipc_fork("unlink","VCounter\r",&child);if(e)goto done;e=ipc_wait(&child,&status);printf("ACTIVE UNLINK status=%u wait=%u\r",status,e);if(e)goto done;}
 if(!strcmp(mode,"duplicate")){r=control(p,0x90);printf("DUP START %u\r",r);if(!r){e=187;goto done;}}
 if(!strcmp(mode,"failure")){control(p,0x92);r=control(p,0x91);printf("INJECTED STOP %u PATH HELD\r",r);if(r!=187){e=187;goto done;}}
 if(!strcmp(mode,"duplicate-path")){e=ipc_dup(p,&q);if(e)goto done;e=ipc_close(p);p=q;printf("ORIGINAL CLOSED DUP HELD\r");}
 query(p,&a);
 if(!strcmp(mode,"wizard"))e=ipc_fork("/d1/dodwiz","\r",&child);
 else if(!strcmp(mode,"ssc"))e=ipc_fork("/d1/m3gate","natural\r",&child);
 else if(!strcmp(mode,"observer"))e=ipc_fork("/d1/dodsnd","observer-long\r",&child);
 else if(!strcmp(mode,"load"))e=ipc_fork("/d1/vctest","cpu\r",&child);
 if(e)goto done;
 e=os_sleep(!strcmp(mode,"hold")?9000:900);if(e)goto done;
 e=os_signal_value(&signalFlag);if(e)goto done;
 query(p,&b);printf("DELTA %u\r",b-a);
 if(!strcmp(mode,"cancel")){os_cancel_self();e=os_signal_value(&signalFlag);goto done;}
 if(!strcmp(mode,"error")){e=187;goto done;}
 if(!strcmp(mode,"kill")){Byte me;asm{ os9 $0c
 sta :me
 }ipc_signal(me,0);return 187;}
 if(!strcmp(mode,"term-retry")){control(p,0x93);goto done;}
 e=control(p,0x91);printf("STOP %u\r",e);if(e)goto done;
 r=control(p,0x91);printf("DUP STOP %u\r",r);if(r){e=r;goto done;}
 query(p,&a);
 if(!strcmp(mode,"post-wizard"))e=ipc_fork("/d1/dodwiz","\r",&child);
 if(e)goto done;
 e=os_sleep(1200);if(e)goto done;
 query(p,&b);printf("POST REMOVE DELTA %u\r",b-a);if(b!=a)e=187;
done:r=ipc_close(p);printf("CLOSE %u EXIT %u\r",r,e);return e?e:r;
}
