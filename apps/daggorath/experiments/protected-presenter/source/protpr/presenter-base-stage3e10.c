#include <cmoc.h>
#include "heartbeat-broker.h"
#include "../audio/ipc.h"
#include "../audio/native_heartbeat.h"

Byte os_write(Byte,const void *,Word);
Byte os_sleep(Word);
Byte os_map_buffer(Byte,Word,Byte,Byte **,Word *);

static NativeHeartbeat heartbeat={255,0,0};
static Byte *sceneMap;
static Word sceneMapLength;
static volatile Byte mapStage,mapError;
static volatile Word mapPointer,mapLength;
static Byte *sideMap,*bottomMap;
static Word sideMapLength,bottomMapLength;
static Byte mapOwned,sideMapOwned,bottomMapOwned;
static volatile Byte sourceBlock,sideBlock,bottomBlock;
static volatile Byte sourceSlot,sideSlot,bottomSlot;
static volatile Byte mappingValid;
static Byte sideOwned,bottomOwned;
static const Byte sideDefine[]={27,0x29,196,4,0,210};
static const Byte bottomDefine[]={27,0x29,196,5,0,64};
static const Byte sideKill[]={27,0x2a,196,4};
static const Byte bottomKill[]={27,0x2a,196,5};
static Byte sideLoad[221]={27,0x2b,196,4,5,0,240,0,7,0,210};
static Byte bottomLoad[75]={27,0x2b,196,5,5,2,0,0,1,0,64};
static const Byte sideLeft[]={27,0x2d,196,4,0,64,0,156};
static const Byte sideRight[]={27,0x2d,196,4,1,80,0,156};
static const Byte bottomPut[]={27,0x2d,196,5,0,64,0,163};
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

/* Installed EOU GP header: payload follows a 32-byte metadata header.
 * SS.MpGPB's already-mapped path leaves X/Y unchanged, so only a freshly
 * mapped pointer or a unique validated header search may establish a GP. */
static Word gp_word(const Byte *p){return (Word)p[0]*256+p[1];}
static Byte dat_block(Word address)
{return *(volatile Byte *)(0xffa8+(address>>13));}
static Byte gp_header(Byte *payload,Byte id,Word capacity,Word width,Word height)
{Word a=(Word)payload;Byte *h;
 if(a<32||((a-32)&0xe000)!=(a&0xe000)||
    ((a&0x1fff)+capacity)>8192||dat_block(a)>=0x3e)return 0;
 h=payload-32;
 return h[3]==196&&h[4]==id&&gp_word(h+5)==capacity&&
        gp_word(h+7)==width&&gp_word(h+9)==height&&h[15]==1;
}
static Byte *gp_find(Byte *known,Byte id,Word capacity,Word width,Word height)
{Word base=(Word)known&0xe000,off;Byte *p,*found=0;
 for(off=0;off<=8192-32-capacity;off++){
  p=(Byte *)(base+off+32);
  if(gp_header(p,id,capacity,width,height)){
   if(found)return 0;
   found=p;
  }
 }
 return found;
}
static Byte distinct_payloads(void)
{Word a=(Word)sceneMap,b=(Word)sideMap,c=(Word)bottomMap;
 if(!a||!b||!c)return 0;
 if(sourceBlock==sideBlock&&a<b+sideMapLength&&b<a+sceneMapLength)return 0;
 if(sourceBlock==bottomBlock&&a<c+bottomMapLength&&c<a+sceneMapLength)return 0;
 if(sideBlock==bottomBlock&&b<c+bottomMapLength&&c<b+sideMapLength)return 0;
 return 1;
}
static Byte map_buffers(void)
{Byte e,*p;Word n;
 mapStage=1;e=os_map_buffer(2,0xc401,1,&sceneMap,&sceneMapLength);
 if(e)return e;mapOwned=1;
 if(sceneMapLength!=512||!gp_header(sceneMap,1,512,512,8))return ERR_ARGUMENT;
 sourceSlot=(Byte)((Word)sceneMap>>13);sourceBlock=dat_block((Word)sceneMap);
 mapStage=2;sideMap=gp_find(sceneMap,4,224,240,7);
 if(sideMap)sideMapLength=224;
 else {
  e=os_map_buffer(2,0xc404,1,&sideMap,&sideMapLength);
  if(e)return e;sideMapOwned=1;
  if(sideMapLength!=224||!gp_header(sideMap,4,224,240,7))return ERR_ARGUMENT;
 }
 sideSlot=(Byte)((Word)sideMap>>13);sideBlock=dat_block((Word)sideMap);
 mapStage=3;bottomMap=gp_find(sceneMap,5,64,512,1);
 if(!bottomMap&&sourceBlock!=sideBlock)
  bottomMap=gp_find(sideMap,5,64,512,1);
 if(bottomMap)bottomMapLength=64;
 else {
  e=os_map_buffer(2,0xc405,1,&p,&n);
  if(e)return e;bottomMapOwned=1;bottomMap=p;bottomMapLength=n;
  if(n!=64||!gp_header(bottomMap,5,64,512,1))return ERR_ARGUMENT;
 }
 bottomSlot=(Byte)((Word)bottomMap>>13);bottomBlock=dat_block((Word)bottomMap);
 if(!distinct_payloads())return ERR_ARGUMENT;
 mappingValid=1;mapStage=4;return 0;
}

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
/* Private Variant B: bracket the atomic countdown query with generation/phase
 * snapshots. A mismatch means a VIRQ edge occurred during the query. No
 * generation is discarded; the next loop reconciles it before another query.
 * At remaining=3, the next edge is more than two 60 Hz ticks away even if
 * the query happened just before a tick. The measured worst segment duration
 * is 10.445 ms; this leaves room for the final snapshot and one segment.
 */
static NativeHeartbeatState admissionQuery,admissionEnd;
static Word admissionDeferrals[3],admissionAccepts[3];
static Byte admit_status(Byte segment)
{Byte e,result;
 for(;;){
  e=service_heart();if(e)return e;
  e=native_heartbeat_snapshot(&heartbeat,&state);if(e)return e;
  e=native_heartbeat_query(&heartbeat,&admissionQuery);if(e)return e;
  e=native_heartbeat_snapshot(&heartbeat,&admissionEnd);if(e)return e;
  if(state.edgeGeneration!=admissionEnd.edgeGeneration||
     state.phase!=admissionEnd.phase||state.phase!=admissionQuery.phase||
     state.fault!=admissionQuery.fault||state.fault!=admissionEnd.fault)
   continue;
  if(admissionQuery.fault)return admissionQuery.fault;
  if(!admissionQuery.enabled||admissionQuery.remainingByte==0||
     admissionQuery.remainingByte>=3){admissionAccepts[segment]++;return 0;}
  admissionDeferrals[segment]++;
  e=native_heartbeat_wait(&heartbeat,shownGeneration,&result);if(e)return e;
 }
}
static Byte present_status(void)
{Byte row,e;
 e=admit_status(0);if(e)return e;
 for(row=0;row<7;row++)memcpy(sideMap+(Word)row*30,sceneMap+(Word)row*64,30);
 e=os_write(2,sideLeft,sizeof(sideLeft));if(e)return e;
 e=admit_status(1);if(e)return e;
 for(row=0;row<7;row++)memcpy(sideMap+(Word)row*30,sceneMap+(Word)row*64+34,30);
 e=os_write(2,sideRight,sizeof(sideRight));if(e)return e;
 e=admit_status(2);if(e)return e;
 memcpy(bottomMap,sceneMap+448,64);
 e=os_write(2,bottomPut,sizeof(bottomPut));if(e)return e;
 return service_heart();
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
{Byte request[DAG_BROKER_FRAME],pending,waitResult,e=0,r,pid,done=0,status=0;
 unsigned long requested,age;
 memset(&state,0,sizeof(state));r=ipc_self_pid(&pid);if(!e)e=r;
 if(!e)e=ipc_priority(pid,DAG_BROKER_PRIORITY);
 if(!e)e=native_heartbeat_open(&heartbeat);
 if(!e)e=snapshot();shownGeneration=state.edgeGeneration;
 if(!e){e=os_write(2,sideDefine,sizeof(sideDefine));if(!e)sideOwned=1;}
 if(!e){e=os_write(2,bottomDefine,sizeof(bottomDefine));if(!e)bottomOwned=1;}
 /* One-time GPLoad establishes stock EOU width/height metadata.  Status
  * requests use only validated direct GP writes and PutBlk. */
 if(!e)e=os_write(2,sideLoad,sizeof(sideLoad));
 if(!e)e=os_write(2,bottomLoad,sizeof(bottomLoad));
 if(!e){r=map_buffers();mapError=r;mapPointer=(Word)sceneMap;
  mapLength=sceneMapLength;if(r)e=r;}
 if(!e)e=os_clock(&lastPollTick,1);
 r=reply(DAG_BROKER_HELLO,0,e,0);if(!e)e=r;if(e)goto done;
 while(!done){
  e=service_heart();if(e)break;
  e=native_heartbeat_claim_request(&heartbeat,&pending);if(e)break;
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
     e=(request[4]&DAG_BROKER_STATUS)?present_status():os_write(2,putScene,sizeof(putScene));
     if(e)break;e=os_clock(&after,0);if(e)break;
     if(clip(elapsed(before,after))>maxSceneTicks)maxSceneTicks=clip(elapsed(before,after));
     sceneWrites++;e=service_heart();
     currentScene=0;break;}
   case DAG_BROKER_SHUTDOWN:e=native_heartbeat_disable(&heartbeat);done=1;break;
   default:e=ERR_ARGUMENT;
   }
   if(!e)e=service_heart();r=reply(request[2],request[3],e,status);if(!e)e=r;
   if(!e)e=service_heart();if(e)break;if(done)continue;
  }
idle:
  e=native_heartbeat_wait(&heartbeat,shownGeneration,&waitResult);if(e)break;
 }
done:if(bottomMapOwned){r=os_map_buffer(2,0xc405,0,&bottomMap,&bottomMapLength);if(!e)e=r;}
 if(sideMapOwned){r=os_map_buffer(2,0xc404,0,&sideMap,&sideMapLength);if(!e)e=r;}
 if(mapOwned){r=os_map_buffer(2,0xc401,0,&sceneMap,&sceneMapLength);if(!e)e=r;}
 if(bottomOwned){r=os_write(2,bottomKill,sizeof(bottomKill));if(!e)e=r;}
 if(sideOwned){r=os_write(2,sideKill,sizeof(sideKill));if(!e)e=r;}
 r=native_heartbeat_close(&heartbeat);if(!e)e=r;return e;
}
