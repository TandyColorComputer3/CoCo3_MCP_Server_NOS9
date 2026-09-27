/* Counter-only driver ABI: ../virq/counter.asm. Fork/wait wrappers unchanged. */
#include <cmoc.h>
#include "ipc.h"
void phase(Byte value);
int main(int argc,char **argv){Byte p,e,child,status=0,r;Registers regs;
 if(argc!=3)return 187;
 e=ipc_open("/vc",&p);if(e)return e;
 regs.x=regs.y=0;e=os_setstat(p,0x90,&regs);if(e){ipc_close(p);return e;}
 phase(50);e=os_sleep(60);
 if(!e){phase(51);e=ipc_fork(argv[1],argv[2],&child);phase(52);}
 if(!e){e=ipc_wait(&child,&status);phase(53);}
 os_sleep(120);phase(54);r=ipc_close(p);
 printf("CHILD %u WAIT %u CLOSE %u\r",status,e,r);
 return e?e:(r?r:status);
}
