#include <cmoc.h>
#include "platform.h"
#include "ipc.h"
extern Byte sleep_signal(void);
static Byte set(Byte path,Byte op,Word *x,Word y){Registers r;Byte e;r.x=*x;r.y=y;e=os_setstat(path,op,&r);*x=r.x;return e;}
static Byte snap(Byte path,unsigned long *generation,Byte *phase){Registers r;Byte e;r.x=r.y=0;r.b=0;e=os_getstat(path,0x96,&r);if(e)return e;*generation=((unsigned long)r.y<<16)|r.x;*phase=r.b;return 0;}
int main(void){Byte path=255,e,phase,nextPhase,slept;Word value;unsigned long generation,nextGeneration;unsigned i;
 e=ipc_open("/da63",&path);if(e)return e;
 value=0;e=set(path,0x90,&value,0);if(e)return e;
 value=3;e=set(path,0x92,&value,0);if(e)return e;
 value=0;e=set(path,0x93,&value,0);if(e)return e;
 e=snap(path,&generation,&phase);if(e)return e;
 for(i=0;i<220;i++){
  value=(Word)generation;e=set(path,0x97,&value,0);if(e)return e;
  slept=0;
  if(i==0&&!value){do{e=snap(path,&nextGeneration,&nextPhase);if(e)return e;}while(nextGeneration==generation);}
  else {if(!value){e=sleep_signal();if(e)return e;slept=1;}e=snap(path,&nextGeneration,&nextPhase);if(e)return e;}
  if(nextGeneration-generation!=1||nextPhase==phase)return 188;
  value=(Word)nextGeneration;e=set(path,0x98,&value,slept);if(e)return e;
  generation=nextGeneration;phase=nextPhase;
 }
 value=0;e=set(path,0x99,&value,0);if(e)return e;
 return sleep_signal();
}
