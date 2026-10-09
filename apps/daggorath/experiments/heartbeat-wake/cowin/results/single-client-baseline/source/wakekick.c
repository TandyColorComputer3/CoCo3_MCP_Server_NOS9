#include <cmoc.h>
#include "platform.h"
#include "ipc.h"
#include "graphics.h"
#define MAGIC 0xb6
#define VERSION 1
#define FRAME 16
#define OP_REQUEST 5
#define OP_SHUTDOWN 7
extern Byte sleep_signal(void);
static Byte cmdPath=255,replyPath=255;
static Byte graphicsPath=255;
static Word requestCount,ackCount;
static Word sceneCount,statusSceneCount,staleCount;
static Byte set(Byte path,Byte op,Word *x,Word y){Registers r;Byte e;r.x=*x;r.y=y;e=os_setstat(path,op,&r);*x=r.x;return e;}
static Byte snap(Byte path,unsigned long *generation,Byte *phase){Registers r;Byte e;r.x=r.y=0;r.b=0;e=os_getstat(path,0x96,&r);if(e)return e;*generation=((unsigned long)r.y<<16)|r.x;*phase=r.b;return 0;}
static Byte ticks(Byte path,Word *value){Registers r;Byte e;r.x=r.y=0;e=os_getstat(path,0x95,&r);if(e)return e;*value=r.x;return 0;}
static Byte claim(Byte path,Word *pending){*pending=0;return set(path,0x9b,pending,0);}
static Byte reconcile_final(Byte path,unsigned long *generation,Byte *phase){unsigned long next,check;Byte p,q,e;Word x;
 e=snap(path,&next,&p);if(e)return e;
 if(next!=*generation){if(next-*generation!=1||p==*phase)return 188;x=(Word)next;e=set(path,0x98,&x,0);if(e)return e;*generation=next;*phase=p;}
 e=snap(path,&check,&q);if(e)return e;return check==*generation&&q==*phase?0:188;}
static Byte reply(const Byte *request,Byte result){Byte ack[FRAME],e;memset(ack,0,FRAME);ack[0]=MAGIC;ack[1]=VERSION;ack[2]=request[2];ack[3]=request[3];ack[4]=result;cowinDiag[25]=1;cowinDiag[46]=replyPath;
 e=ipc_write(replyPath,ack,7);cowinDiag[26]=e;cowinDiag[6]=21;cowinDiag[27]=(Byte)(e!=0);cowinDiag[28]=e;if(e)return e;e=ipc_write(replyPath,ack+7,FRAME-7);cowinDiag[47]=e;cowinDiag[6]=22;cowinDiag[27]=(Byte)(e!=0);cowinDiag[28]=e;if(!e)ackCount++;return e;}
static Byte service_request(Byte path,unsigned long *generation,Byte *phase,Byte *worked,Byte *done){Byte frame[FRAME],e,stale=0,p;Word pending,x;unsigned long requested,next;
 e=claim(path,&pending);if(e)return e;if(!pending)return 0;
 e=ipc_read(cmdPath,frame,FRAME);if(e)return e;requestCount++;
 if(frame[0]!=MAGIC||frame[1]!=VERSION||(frame[2]!=OP_REQUEST&&frame[2]!=OP_SHUTDOWN))return 187;
 if(frame[2]==OP_REQUEST){requested=((unsigned long)frame[6]<<24)|((unsigned long)frame[7]<<16)|((Word)frame[8]<<8)|frame[9];
  cowinDiag[8]=frame[3];
  e=snap(path,&next,&p);if(e)return e;
  if(frame[4]&&(requested!=next||frame[10]!=p)){stale=1;staleCount++;}
  else {
#ifdef RETURN_DIAG
   cowinDiag[63]=0xa0;
#endif
   e=graphics_scene(graphicsPath,frame[5],frame[4],p);
#ifdef RETURN_DIAG
   cowinDiag[62]=e;
   cowinDiag[63]=0xc1;
#endif
   if(e)return e;
#ifdef RETURN_DIAG
   cowinDiag[63]=0xc2;
#endif
   sceneCount++;
#ifdef RETURN_DIAG
   cowinDiag[63]=0xc3;
#endif
   if(frame[4])statusSceneCount++;
#ifdef RETURN_DIAG
   cowinDiag[63]=0xc4;
#endif
  }
  cowinDiag[6]=16;e=snap(path,&next,&p);cowinDiag[54]=e;if(e)return e;
  cowinDiag[55]=(Byte)(next>>8);cowinDiag[56]=(Byte)next;
  cowinDiag[57]=(Byte)(*generation>>8);cowinDiag[58]=(Byte)*generation;
  cowinDiag[59]=p;cowinDiag[60]=*phase;
  if(next!=*generation){if(next-*generation!=1||p==*phase)return 188;
   cowinDiag[6]=17;e=graphics_heart(graphicsPath,p);cowinDiag[61]=e;if(e)return e;
   x=(Word)next;e=set(path,0x98,&x,0);cowinDiag[6]=18;cowinDiag[62]=e;if(e)return e;*generation=next;*phase=p;*worked|=1;}
 }
 e=reply(frame,stale);if(e)return e;*worked|=2;if(frame[2]==OP_SHUTDOWN)*done=1;return 0;}
static Byte reconcile(Byte path,unsigned long *generation,Byte *phase,Byte slept,Byte *worked,Byte *done){unsigned long next;Byte p,e;Word x;
 e=snap(path,&next,&p);if(e)return e;
 if(next!=*generation){if(next-*generation!=1||p==*phase)return 188;
  e=graphics_heart(graphicsPath,p);if(e)return e;
  x=(Word)next;e=set(path,0x98,&x,0);if(e)return e;*generation=next;*phase=p;*worked|=1;}
 e=service_request(path,generation,phase,worked,done);if(e)return e;
 if(slept){x=0;e=set(path,0x9c,&x,0);if(e)return e;}return 0;}
static Byte waitwork(Byte path,unsigned long *generation,Byte *phase,Byte *kind,Byte *done){Byte e,worked;Word arm;*kind=0;
 for(;;){worked=0;e=reconcile(path,generation,phase,0,&worked,done);if(e)return e;if(worked){*kind=worked;return 0;}
  arm=(Word)*generation;e=set(path,0x97,&arm,0);if(e)return e;
  worked=0;e=reconcile(path,generation,phase,0,&worked,done);if(e)return e;if(worked){*kind=worked;return 0;}if(arm)continue;
  e=sleep_signal();if(e)return e;worked=0;e=reconcile(path,generation,phase,1,&worked,done);if(e)return e;if(worked){*kind=worked;return 0;}
 }}
static Byte bind_children(Byte self,Byte child[2]){Byte e,n,saved0=255,saved1=255;
 e=ipc_open("/pipe",&cmdPath);if(e)return e;e=ipc_open("/pipe",&replyPath);if(e)return e;
 e=ipc_dup(0,&saved0);if(e)return e;e=ipc_dup(1,&saved1);if(e)return e;
 e=ipc_close(0);if(e)return e;e=ipc_dup(cmdPath,&n);if(e||n!=0)return e?e:187;
 e=ipc_close(1);if(e)return e;e=ipc_dup(replyPath,&n);if(e||n!=1)return e?e:187;
 e=ipc_priority(self,128);if(e)return e;
 e=ipc_fork("kickchild","1 26 4 5\r",&child[0]);if(e)return e;
 e=ipc_fork("kickchild","1 1 0 7\r",&child[1]);if(e)return e;
 e=os_sleep(2);if(e)return e;e=ipc_priority(self,192);if(e)return e;
 e=ipc_close(0);if(e)return e;e=ipc_dup(saved0,&n);if(e||n!=0)return e?e:187;
 e=ipc_close(1);if(e)return e;e=ipc_dup(saved1,&n);if(e||n!=1)return e?e:187;
 e=ipc_close(saved0);if(e)return e;return ipc_close(saved1);
}
int main(void){Byte path=255,e,phase,pid,self,status,kind,child[2],done=0;Word x,delta,waited;unsigned long generation;unsigned children=2;
 e=ipc_self_pid(&self);if(e)return e;e=ipc_priority(self,192);if(e)return e;e=ipc_open("/da63",&path);if(e)return e;
 x=0;e=set(path,0x90,&x,0);if(e)return e;x=3;e=set(path,0x92,&x,0);if(e)return e;
 e=bind_children(self,child);if(e)return e;e=graphics_open(&graphicsPath);if(e)return e;x=0;e=set(path,0x93,&x,0);if(e)return e;e=snap(path,&generation,&phase);if(e)return e;
 e=ticks(path,&delta);if(e)return e;waited=0;do{e=waitwork(path,&generation,&phase,&kind,&done);if(e)return e;e=ticks(path,&delta);if(e)return e;if((Word)(65535-waited)<delta)return 188;waited+=delta;}while(waited<81);
#ifdef HEARTBEAT_ONLY_GATE
 while(generation<220){e=waitwork(path,&generation,&phase,&kind,&done);if(e)return e;}
#endif
 e=ipc_signal(child[0],1);if(e)return e;while(requestCount<26){e=waitwork(path,&generation,&phase,&kind,&done);if(e)return e;}
#ifndef HEARTBEAT_ONLY_GATE
 while(generation<220){e=waitwork(path,&generation,&phase,&kind,&done);if(e)return e;}
#endif
 e=ipc_signal(child[1],1);if(e)return e;while(!done){e=waitwork(path,&generation,&phase,&kind,&done);if(e)return e;}
 while(children--){e=ipc_wait(&pid,&status);if(e||status)return e?e:status;}
 if(requestCount!=27||ackCount!=27||sceneCount+staleCount!=26||!sceneCount||!statusSceneCount||!staleCount)return 188;
 x=0;e=set(path,0x94,&x,0);if(e)return e;
 e=reconcile_final(path,&generation,&phase);if(e)return e;
 x=0;e=set(path,0x99,&x,0);if(e)return e;
 e=os_sleep(1);if(e)return e;
 x=0;e=set(path,0x91,&x,0);if(e)return e;
 e=ipc_close(cmdPath);if(e)return e;e=ipc_close(replyPath);if(e)return e;e=ipc_close(path);if(e)return e;return graphics_close(graphicsPath);
}
