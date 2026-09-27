/* Autonomous service wake proof using inherited R/W PipeMan path 0.
 * OS calls reuse verified M1 IPC and read-only EOU clock adapters.
 * Wake messages carry no audio event and do not advance time by message count.
 * No SS.Ready assumption, signal handler work, IRQ hook or pipe busy polling. */
#include <cmoc.h>
#include "ipc.h"
#include "heartbeat.h"
static Byte cancel;
static Word delta(Word a,Word b){return a>=b?a-b:3600-b+a;}
static Byte packet(Byte op,Word stamp){Byte f[8]={0xa3,0,0,0,0,0,0,0};f[1]=op;f[2]=stamp>>8;f[3]=stamp;return os_write(0,f,8);}
static Byte worker(Byte ticks){Word start,now,elapsed=0;Byte e,i;
 e=os_clock(&start,1);if(e)return e;
 if(!ticks){for(i=0;i<3;i++){e=os_sleep(60);if(e)return e;e=os_clock(&now,0);if(e)return e;e=packet(i+2,now);if(e)return e;}return 0;}
 while(elapsed<900&&!os_signal_value(&cancel)){
  e=os_sleep(2);if(e)return e;e=os_clock(&now,0);if(e)return e;
  elapsed=delta(now,start);e=packet(elapsed<900?1:255,now);if(e)return e;
 }return cancel;
}
int main(int argc,char **argv){Byte e,r,saved=255,pipe=255,n,ticker=0,commands=0,pid,status,f[8],done=0;Word last,now,dt,wakes=0,edges=0,maxgap=0,latency;HeartbeatState state;
 e=os_intercept(&cancel);if(e)return e;
 if(argc==2)return worker(!strcmp(argv[1],"tick"));
 /* Exercise the same source-derived countdown, with no heartbeat sound. */
 heartbeat_init(&state);heartbeat_update(&state,46);
 e=ipc_open("/pipe",&pipe);if(e)goto end;e=ipc_dup(0,&saved);if(e)goto end;
 e=ipc_close(0);if(e)goto end;e=ipc_dup(pipe,&n);if(e||n){if(!e)e=AUDIO_BAD;goto end;}
 e=ipc_fork("tickgate","tick\r",&ticker);if(e)goto end;
 e=ipc_fork("tickgate","commands\r",&commands);if(e)goto end;
 e=os_clock(&last,1);if(e)goto end;
 while(!done&&!e){
  e=ipc_read(0,f,8);if(e)break;
  e=os_clock(&now,0);if(e)break;dt=delta(now,last);last=now;
  if(dt>300||f[0]!=0xa3){e=AUDIO_BAD;break;}
  if(dt>maxgap)maxgap=dt;edges+=heartbeat_advance(&state,dt);++wakes;
  if(f[1]==1)continue;
  latency=delta(now,((Word)f[2]<<8)|f[3]);
  printf("TICK command=%u latency=%u elapsed-edges=%u remaining=%u\r",f[1],latency,edges,state.remaining);
  if(f[1]==2)heartbeat_update(&state,3);
  else if(f[1]==3)heartbeat_disable(&state);
  else if(f[1]==4)done=1;
  else e=246;
 }
 printf("TICK wakes=%u edges=%u maxgap=%u enabled=%u\r",wakes,edges,maxgap,state.enabled);
end:
 if(ticker)ipc_signal(ticker,3);
 if(e&&commands)ipc_signal(commands,3);
 if(ticker){r=ipc_wait(&pid,&status);if(!e&&r)e=r;}
 if(commands){r=ipc_wait(&pid,&status);if(!e&&r)e=r;}
 if(saved!=255){ipc_close(0);r=ipc_dup(saved,&n);if(!e)e=r;ipc_close(saved);}
 if(pipe!=255)ipc_close(pipe);
 return e;
}
