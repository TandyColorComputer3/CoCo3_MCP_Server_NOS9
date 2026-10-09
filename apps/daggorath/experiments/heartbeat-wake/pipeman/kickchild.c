#include <cmoc.h>
#include "platform.h"
#include "ipc.h"
#define MAGIC 0xb6
#define VERSION 1
#define FRAME 16
extern Byte sleep_signal(void);
static Byte set(Byte path,Byte op){Registers r;Byte e;r.x=r.y=0;e=os_setstat(path,op,&r);return e;}
static Byte send_frame(Byte path,const Byte *frame,Byte split){Byte e;
 if(!split)return ipc_write(path,frame,FRAME);
 e=ipc_write(path,frame,5);return e?e:ipc_write(path,frame+5,FRAME-5);
}
int main(int argc,char **argv){Byte path,e,pid,frame[FRAME],ack[FRAME],seq=0,op,split;unsigned delay,count,gap,i;
 if(argc!=5)return 187;e=ipc_self_pid(&pid);if(e)return e;e=ipc_priority(pid,128);if(e)return e;
 delay=atoi(argv[1]);count=atoi(argv[2]);gap=atoi(argv[3]);op=(Byte)atoi(argv[4]);
 e=ipc_open("/da63",&path);if(e)return e;e=sleep_signal();if(e)return e;
 if(delay){e=os_sleep(delay);if(e)return e;}
 for(i=0;i<count;i++){
  memset(frame,0,FRAME);frame[0]=MAGIC;frame[1]=VERSION;frame[2]=op;frame[3]=++seq;frame[5]=(Byte)i;
  split=(Byte)(i==0);e=send_frame(0,frame,split);if(e)return e;e=set(path,0x9a);if(e)return e;
  e=ipc_read(1,ack,FRAME);if(e)return e;
  if(ack[0]!=MAGIC||ack[1]!=VERSION||ack[2]!=op||ack[3]!=seq||ack[4])return 187;
  if(i+1<count&&gap){e=os_sleep(gap);if(e)return e;}
 }
 return ipc_close(path);
}
