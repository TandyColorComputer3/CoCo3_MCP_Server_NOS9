/* Probe coordinator owns only its direct children; each service probe owns
 * its own helper children. Reuses the M2 observer unchanged. */
#include <cmoc.h>
#include "ipc.h"
int main(int argc,char **argv){Byte e,r,pid,status,children=0,i;const char *target,*args;
 if(argc!=2)return AUDIO_BAD;
 target=!strcmp(argv[1],"tick")||!strcmp(argv[1],"wizard")?"/d1/tickgate":"/d1/m3gate";
 args=!strcmp(target,"/d1/tickgate")?"\r":"natural\r";
 e=ipc_fork("/d1/dodsnd","observer-long\r",&pid);if(e)return e;++children;
 e=ipc_fork(target,args,&pid);if(!e)++children;
 if(!e&&!strcmp(argv[1],"wizard")){e=ipc_fork("/d1/dodwiz","\r",&pid);if(!e)++children;}
 for(i=0;i<children;i++){r=ipc_wait(&pid,&status);printf("GATE CHILD pid=%u status=%u wait=%u\r",pid,status,r);if(r)e=r;else if(status)e=status;}
 return e;
}
