#include <cmoc.h>
#include "platform.h"
#include "ipc.h"
extern Byte sleep_signal(void);
static Byte set(Byte path,Byte op,Word *x,Word y){Registers r;Byte e;r.x=*x;r.y=y;e=os_setstat(path,op,&r);*x=r.x;return e;}
static Byte snap(Byte path,unsigned long *generation,Byte *phase){Registers r;Byte e;r.x=r.y=0;r.b=0;e=os_getstat(path,0x96,&r);if(e)return e;*generation=((unsigned long)r.y<<16)|r.x;*phase=r.b;return 0;}
static Byte kick(Byte path){Word x=0;return set(path,0x9a,&x,0);}
static Byte claim(Byte path,Word *pending){*pending=0;return set(path,0x9b,pending,0);}
static Byte reconcile(Byte path,unsigned long *generation,Byte *phase,Byte slept,Byte *worked){unsigned long next;Byte p,e;Word x;
 e=snap(path,&next,&p);if(e)return e;
 if(next!=*generation){if(next-*generation!=1||p==*phase)return 188;x=(Word)next;e=set(path,0x98,&x,0);if(e)return e;*generation=next;*phase=p;*worked|=1;}
 e=claim(path,&x);if(e)return e;if(x)*worked|=2;
 if(slept){x=0;e=set(path,0x9c,&x,0);if(e)return e;}
 return 0;
}
static Byte waitwork(Byte path,unsigned long *generation,Byte *phase,Byte *kind){Byte e,worked;Word arm;*kind=0;
 for(;;){worked=0;e=reconcile(path,generation,phase,0,&worked);if(e)return e;if(worked){*kind=worked;return 0;}
  arm=(Word)*generation;e=set(path,0x97,&arm,0);if(e)return e;
  worked=0;e=reconcile(path,generation,phase,0,&worked);if(e)return e;if(worked){*kind=worked;return 0;}
  if(arm)continue;
  e=sleep_signal();if(e)return e;
  worked=0;e=reconcile(path,generation,phase,1,&worked);if(e)return e;
  if(worked){*kind=worked;return 0;}
 }
}
int main(void){Byte path=255,e,phase,pid,self,status,kind,child[4];Word x;unsigned long generation;unsigned children=4;
 e=ipc_self_pid(&self);if(e)return e;e=ipc_priority(self,192);if(e)return e;
 e=ipc_open("/da63",&path);if(e)return e;x=0;e=set(path,0x90,&x,0);if(e)return e;x=3;e=set(path,0x92,&x,0);if(e)return e;
 e=ipc_priority(self,128);if(e)return e;
 e=ipc_fork("kickchild","1 1 0\r",&child[0]);if(e)return e;
 e=ipc_fork("kickchild","3 1 0\r",&child[1]);if(e)return e;
 e=ipc_fork("kickchild","2 1 0\r",&child[2]);if(e)return e;
 e=ipc_fork("kickchild","1 20 4\r",&child[3]);if(e)return e;
 e=os_sleep(2);if(e)return e;e=ipc_priority(self,192);if(e)return e;
 x=0;e=set(path,0x93,&x,0);if(e)return e;e=snap(path,&generation,&phase);if(e)return e;
 while(generation<1){e=waitwork(path,&generation,&phase,&kind);if(e)return e;}
 e=kick(path);if(e)return e;x=0;e=claim(path,&x);if(e||x!=1)return e?e:188;
 e=kick(path);if(e)return e;x=(Word)generation;e=set(path,0x97,&x,0);if(e||x!=2)return e?e:188;x=0;e=claim(path,&x);if(e||x!=1)return e?e:188;
 x=(Word)generation;e=set(path,0x97,&x,0);if(e||x)return e?e:188;e=kick(path);if(e)return e;x=0;e=claim(path,&x);if(e||x!=1)return e?e:188;
 e=ipc_signal(child[0],1);if(e)return e;do{e=waitwork(path,&generation,&phase,&kind);if(e)return e;}while(!(kind&2));
 e=ipc_signal(child[1],1);if(e)return e;do{e=waitwork(path,&generation,&phase,&kind);if(e)return e;}while(!(kind&2));
 e=ipc_signal(child[2],1);if(e)return e;do{e=waitwork(path,&generation,&phase,&kind);if(e)return e;}while(!(kind&2));
 e=ipc_signal(child[3],1);if(e)return e;while(generation<220){e=waitwork(path,&generation,&phase,&kind);if(e)return e;}
 while(children--){e=ipc_wait(&pid,&status);if(e||status)return e?e:status;}
 do{x=0;e=claim(path,&x);if(e)return e;}while(x);
 x=0;e=set(path,0x99,&x,0);if(e)return e;return sleep_signal();
}
