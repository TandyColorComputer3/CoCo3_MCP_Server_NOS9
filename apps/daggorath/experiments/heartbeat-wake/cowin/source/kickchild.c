#include <cmoc.h>
#include "platform.h"
#include "ipc.h"
#define MAGIC 0xb6
#define VERSION 1
#define FRAME 16
extern Byte sleep_signal(void);
static Byte set(Byte path,Byte op){Registers r;Byte e;r.x=r.y=0;e=os_setstat(path,op,&r);return e;}
static Byte snap(Byte path,unsigned long *generation,Byte *phase){Registers r;Byte e;r.x=r.y=0;r.b=0;e=os_getstat(path,0x96,&r);if(e)return e;*generation=((unsigned long)r.y<<16)|r.x;*phase=r.b;return 0;}
static Byte send_frame(Byte path,const Byte *frame,Byte split){Byte e;
 if(!split)return ipc_write(path,frame,FRAME);
 e=ipc_write(path,frame,5);return e?e:ipc_write(path,frame+5,FRAME-5);
}
static Byte exchange(Byte path,Byte *frame,Byte *ack,Byte op,Byte *seq,Byte split){Byte e;
 frame[0]=MAGIC;frame[1]=VERSION;frame[2]=op;frame[3]=++*seq;
 e=send_frame(0,frame,split);if(e)return e;e=set(path,0x9a);if(e)return e;
 e=ipc_read(1,ack,FRAME);if(e)return e;
 return ack[0]!=MAGIC||ack[1]!=VERSION||ack[2]!=op||ack[3]!=*seq||ack[4]>1?187:0;
}
int main(int argc,char **argv){Byte path,e,pid,frame[FRAME],ack[FRAME],seq=0,op,split,phase;unsigned delay,count,gap,i;unsigned long generation;
 if(argc!=5)return 187;e=ipc_self_pid(&pid);if(e)return e;e=ipc_priority(pid,128);if(e)return e;
 delay=atoi(argv[1]);count=atoi(argv[2]);gap=atoi(argv[3]);op=(Byte)atoi(argv[4]);
 e=ipc_open("/da63",&path);if(e)return e;e=sleep_signal();if(e)return e;
 if(delay){e=os_sleep(delay);if(e)return e;}
 for(i=0;i<count;i++){
  memset(frame,0,FRAME);
  if(op==5){e=snap(path,&generation,&phase);if(e)return e;frame[4]=(Byte)(i!=0&&(i%7)==0);frame[5]=frame[4]?152:(Byte)((i%19)*8);if(i==21){generation--;frame[12]=1;}frame[6]=(Byte)(generation>>24);frame[7]=(Byte)(generation>>16);frame[8]=(Byte)(generation>>8);frame[9]=(Byte)generation;frame[10]=phase;}
  split=(Byte)(i==0);e=exchange(path,frame,ack,op,&seq,split);if(e)return e;
  if(frame[12]&&ack[4]!=1)return 187;
  if(i+1<count&&gap){e=os_sleep(gap);if(e)return e;}
 }
 if(op==5){do{e=os_sleep(1);if(e)return e;e=snap(path,&generation,&phase);if(e)return e;}while(generation<220);memset(frame,0,FRAME);
  e=exchange(path,frame,ack,7,&seq,0);if(e)return e;if(ack[4])return 187;}
 return ipc_close(path);
}
