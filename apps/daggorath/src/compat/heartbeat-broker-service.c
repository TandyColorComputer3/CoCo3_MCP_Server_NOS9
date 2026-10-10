#include <cmoc.h>
#include "heartbeat-broker.h"
#include "../audio/ipc.h"
#include "../audio/native_heartbeat.h"

Byte os_write(Byte,const void *,Word);
Byte os_sleep(Word);

static NativeHeartbeat heartbeat={255,0};
static NativeHeartbeatState state;
static unsigned long shownGeneration;
static Word heartWrites,sceneWrites,staleRejected;
static Word lastPollTick;
static Byte maxWakeTicks,maxHeartTicks,maxSceneTicks,maxLatencyTicks;
static Byte deadlineMisses,collapsed,duplicated,currentScene;
#define TRACE_CAPACITY 240
typedef struct {Word generation,tick;Byte flags,wake,heart,latency,scene;} HeartTrace;
static HeartTrace traces[TRACE_CAPACITY];
static Byte traceHead,traceTail,traceCount;
static Byte putScene[]={27,0x2d,196,1,0,64,0,4};
static Byte putHeart[]={27,0x2d,196,2,1,48,0,156};

static void put32(Byte *p,unsigned long value)
{p[0]=(Byte)(value>>24);p[1]=(Byte)(value>>16);p[2]=(Byte)(value>>8);p[3]=(Byte)value;}
static unsigned long get32(const Byte *p)
{return ((unsigned long)p[0]<<24)|((unsigned long)p[1]<<16)|((unsigned long)p[2]<<8)|p[3];}
static Word elapsed(Word before,Word after){return after>=before?after-before:(Word)(65535-before+after+1);}
static Byte clip(Word value){return value>255?255:(Byte)value;}
static void record_trace(Word tick,Byte pb1,Byte phase,Byte wake,Byte heart,Byte latency,Byte scene)
{HeartTrace *t;if(traceCount==TRACE_CAPACITY){traceTail=(Byte)((traceTail+1)%TRACE_CAPACITY);traceCount--;}
 t=&traces[traceHead];t->generation=(Word)shownGeneration;t->tick=tick;
 t->flags=(Byte)((pb1&1)|((phase&1)<<1)|((latency<=2)<<2));t->wake=wake;
 t->heart=heart;t->latency=latency;t->scene=scene;
 traceHead=(Byte)((traceHead+1)%TRACE_CAPACITY);traceCount++;}

static Byte snapshot(void)
{Byte e=native_heartbeat_snapshot(&heartbeat,&state);return e?e:state.fault;}
static Byte service_heart(void)
{Byte e,wake,heart,latency,pb1;Word observed,start,done,difference;
 e=os_clock(&observed,0);if(e)return e;if(!state.enabled){lastPollTick=observed;return 0;}
 e=snapshot();if(e)return e;difference=(Word)(state.edgeGeneration-shownGeneration);
 if(difference>1){collapsed=(Byte)(collapsed+clip(difference-1));return 188;}
 if(state.edgeGeneration==shownGeneration){lastPollTick=observed;return 0;}
 wake=clip(elapsed(lastPollTick,observed));if(wake>maxWakeTicks)maxWakeTicks=wake;
 e=os_clock(&start,0);if(e)return e;putHeart[3]=(Byte)(2+state.phase);
 e=os_write(2,putHeart,sizeof(putHeart));if(e)return e;e=os_clock(&done,0);if(e)return e;
 heart=clip(elapsed(start,done));latency=clip((Word)wake+heart);
 if(heart>maxHeartTicks)maxHeartTicks=heart;if(latency>maxLatencyTicks)maxLatencyTicks=latency;
 if(latency>2)deadlineMisses++;shownGeneration=state.edgeGeneration;heartWrites++;
 pb1=(Byte)((*(volatile Byte *)0xff22>>1)&1);if(pb1!=state.phase)duplicated++;
 record_trace(done,pb1,state.phase,wake,heart,latency,currentScene);
 lastPollTick=done;return snapshot();
}
/* A status scene includes the heart cells.  If no native edge arrived while
 * the scene was in flight, restore the already-accounted phase without
 * advancing shownGeneration.  If an edge did arrive, service_heart() performs
 * the one authoritative transition instead. */
static Byte restore_status_heart(void)
{Byte e;unsigned long before=shownGeneration;
 e=service_heart();if(e||shownGeneration!=before)return e;
 putHeart[3]=(Byte)(2+state.phase);return os_write(2,putHeart,sizeof(putHeart));
}
static Byte reply(Byte op,Byte sequence,Byte error,Byte status)
{Byte frame[DAG_BROKER_FRAME];memset(frame,0,sizeof(frame));
 frame[0]=DAG_BROKER_MAGIC;frame[1]=DAG_BROKER_VERSION;frame[2]=op;frame[3]=sequence;
 frame[4]=error;frame[5]=status;
 if(op==DAG_BROKER_TRACE&&traceCount){HeartTrace *t=&traces[traceTail];
  frame[6]=(Byte)(t->generation>>8);frame[7]=(Byte)t->generation;
  frame[8]=(Byte)(t->tick>>8);frame[9]=(Byte)t->tick;frame[10]=t->flags;
  frame[11]=t->wake;frame[12]=t->heart;frame[13]=t->latency;frame[14]=t->scene;
  traceTail=(Byte)((traceTail+1)%TRACE_CAPACITY);traceCount--;
 }else if(op==DAG_BROKER_STATS){put32(frame+6,state.edgeGeneration);frame[10]=state.phase;
  frame[11]=maxWakeTicks;frame[12]=maxHeartTicks;frame[13]=maxSceneTicks;
  frame[14]=maxLatencyTicks;frame[15]=deadlineMisses;
 }else {put32(frame+6,state.edgeGeneration);frame[10]=state.phase;
  frame[11]=(Byte)(heartWrites>>8);frame[12]=(Byte)heartWrites;
  frame[13]=(Byte)(sceneWrites>>8);frame[14]=(Byte)sceneWrites;frame[15]=(Byte)staleRejected;}
 return ipc_write(1,frame,sizeof(frame));}

int main(void)
{Byte request[DAG_BROKER_FRAME],pending,claimed=0,armResult,e=0,r,pid,done=0,status=0;
 unsigned long requested,age,beforeArm;
 memset(&state,0,sizeof(state));r=ipc_self_pid(&pid);if(!e)e=r;
 if(!e)e=ipc_priority(pid,DAG_BROKER_PRIORITY);
 if(!e)e=native_heartbeat_open(&heartbeat);
 if(!e)e=snapshot();shownGeneration=state.edgeGeneration;
 if(!e)e=os_clock(&lastPollTick,1);
 r=reply(DAG_BROKER_HELLO,0,e,0);if(!e)e=r;if(e)goto done;
 while(!done){
  e=service_heart();if(e)break;
  if(claimed){pending=1;claimed=0;}
  else {e=native_heartbeat_claim_request(&heartbeat,&pending);if(e)break;}
  if(!pending)goto idle;
  if(pending){
   e=ipc_read(0,request,sizeof(request));if(e)break;
   if(request[0]!=DAG_BROKER_MAGIC||request[1]!=DAG_BROKER_VERSION){e=ERR_ARGUMENT;break;}
   status=0;
   switch(request[2]){
   case DAG_BROKER_RATE:e=native_heartbeat_rate(&heartbeat,request[5]);break;
   case DAG_BROKER_ENABLE:e=native_heartbeat_enable(&heartbeat);if(!e)e=snapshot();
    if(!e)e=os_clock(&lastPollTick,0);if(!e)state.enabled=1;shownGeneration=state.edgeGeneration;break;
   case DAG_BROKER_DISABLE:e=native_heartbeat_disable(&heartbeat);if(!e)state.enabled=0;break;
   case DAG_BROKER_SNAPSHOT:e=snapshot();break;
   case DAG_BROKER_STATS:e=snapshot();status=(Byte)((collapsed||duplicated)?1:0);break;
   case DAG_BROKER_TRACE:e=snapshot();status=(Byte)(traceCount?0:1);break;
   case DAG_BROKER_SCENE:
    e=service_heart();if(e)break;requested=get32(request+6);
    age=state.edgeGeneration-requested;
    /* A legitimate snapshot remains valid after native heartbeat advancement:
     * its phase must differ by the generation age parity.  A future snapshot
     * or a generation/phase mismatch remains stale. */
    if((request[4]&DAG_BROKER_STATUS)&&
       ((age&0x80000000UL)||
        ((Byte)(request[10]^state.phase)!=(Byte)(age&1)))){
      staleRejected++;status=DAG_BROKER_STALE;break;
    }
    {Word before,after;currentScene=(Byte)((request[4]&DAG_BROKER_STATUS)?2:1);
     putScene[7]=request[5];e=os_clock(&before,0);if(e)break;
     e=os_write(2,putScene,sizeof(putScene));if(e)break;e=os_clock(&after,0);if(e)break;
     if(clip(elapsed(before,after))>maxSceneTicks)maxSceneTicks=clip(elapsed(before,after));
     sceneWrites++;e=(request[4]&DAG_BROKER_STATUS)?restore_status_heart():service_heart();
     currentScene=0;break;}
   case DAG_BROKER_SHUTDOWN:e=native_heartbeat_disable(&heartbeat);done=1;break;
   default:e=ERR_ARGUMENT;
   }
   if(!e)e=service_heart();r=reply(request[2],request[3],e,status);if(!e)e=r;
   if(!e)e=service_heart();if(e)break;if(done)continue;
  }
idle:
  beforeArm=shownGeneration;
  e=native_heartbeat_arm(&heartbeat,shownGeneration,&armResult);if(e)break;
  if(armResult!=NATIVE_HEARTBEAT_ARMED){e=native_heartbeat_disarm(&heartbeat);if(e)break;continue;}
  e=service_heart();if(e)break;
  if(shownGeneration!=beforeArm){e=native_heartbeat_disarm(&heartbeat);if(e)break;continue;}
  e=native_heartbeat_claim_request(&heartbeat,&pending);if(e)break;
  if(pending){claimed=1;e=native_heartbeat_disarm(&heartbeat);if(e)break;continue;}
  e=os_sleep(2);r=native_heartbeat_disarm(&heartbeat);if(!e)e=r;if(e)break;
 }
done:r=native_heartbeat_close(&heartbeat);if(!e)e=r;return e;
}
