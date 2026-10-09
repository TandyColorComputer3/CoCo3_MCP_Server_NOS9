#include <cmoc.h>
#include "platform.h"
#include "ipc.h"
extern Byte sleep_signal(void);
static Byte set(Byte path,Byte op){Registers r;Byte e;r.x=r.y=0;e=os_setstat(path,op,&r);return e;}
int main(int argc,char **argv){Byte path,e,pid;unsigned delay,count,gap,i;
 if(argc!=4)return 187;e=ipc_self_pid(&pid);if(e)return e;e=ipc_priority(pid,128);if(e)return e;
 delay=atoi(argv[1]);count=atoi(argv[2]);gap=atoi(argv[3]);e=ipc_open("/da63",&path);if(e)return e;
 e=sleep_signal();if(e)return e;
 if(delay){e=os_sleep(delay);if(e)return e;}for(i=0;i<count;i++){e=set(path,0x9a);if(e)return e;if(i+1<count&&gap){e=os_sleep(gap);if(e)return e;}}return ipc_close(path);}
