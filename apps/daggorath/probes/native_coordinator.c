/* Follow-up uses preloaded module names and starts heartbeat after launch settles.
 * Probe coordinator owns only its direct children; each service probe owns
 * its own helper children. Reuses the M2 observer unchanged. */
#include <cmoc.h>
#include "ipc.h"
int main(int argc,char **argv){Byte e,r,pid,status,children=0,i;const char *target,*args;
 if(argc!=2)return AUDIO_BAD;
 target="natbeat";
 args=!strcmp(argv[1],"slow")?"slow\r":"fast\r";
 e=ipc_fork("dodsnd","observer-long\r",&pid);if(e)return e;++children;
 if(!e&&!strcmp(argv[1],"ssc")){e=ipc_fork("m3gate","natural\r",&pid);if(!e)++children;}
 if(!e&&!strcmp(argv[1],"wizard")){e=ipc_fork("dodwiz","\r",&pid);if(!e)++children;}
 if(!e)e=os_sleep(!strcmp(argv[1],"wizard")?180:!strcmp(argv[1],"ssc")?120:1);
 if(!e){e=ipc_fork(target,args,&pid);if(!e)++children;}
 for(i=0;i<children;i++){r=ipc_wait(&pid,&status);printf("GATE CHILD pid=%u status=%u wait=%u\r",pid,status,r);if(r)e=r;else if(status)e=status;}
 return e;
}
