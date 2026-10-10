#include <cmoc.h>
#include "compat/present.h"
#include "compat/heartbeat-broker.h"
#include "gameplay/opening-heartbeat.h"
#include "audio/ipc.h"

#define REQUIRED_EDGES 220
typedef struct {Word generation,tick;Byte pb1,phase,wake,heart,latency,scene,pass;} SavedTrace;
static Byte frame[DAG_COMPAT_FRAME_BYTES];
static SavedTrace trace[240];
static Word traceSum,traceZero,traceOne,traceTwo,traceSceneEdges;
Byte compat_trace_create(const char *,Byte *);
Byte compat_trace_close(Byte);
Byte os_write(Byte,const void *,Word);
static Byte *decimal(Byte *p,Word value)
{Word divisor=10000;Byte started=0,digit;while(divisor){digit=(Byte)(value/divisor);
 if(digit||started||divisor==1){*p++=(Byte)('0'+digit);started=1;}value=(Word)(value%divisor);divisor/=10;}return p;}
static Byte write_summary(Word count,const DagHeartbeatBroker *b)
{Byte line[150],*p=line,path,e;const char *label="C6ST count=";while(*label)*p++=*label++;p=decimal(p,count);
 label=" wake=";while(*label)*p++=*label++;p=decimal(p,b->maxWakeTicks);
 label=" heart=";while(*label)*p++=*label++;p=decimal(p,b->maxHeartTicks);
 label=" scene=";while(*label)*p++=*label++;p=decimal(p,b->maxSceneTicks);
 label=" latency=";while(*label)*p++=*label++;p=decimal(p,b->maxLatencyTicks);
 label=" miss=";while(*label)*p++=*label++;p=decimal(p,b->deadlineMisses);
 label=" collapse=";while(*label)*p++=*label++;p=decimal(p,b->collapsed);
 label=" dup=";while(*label)*p++=*label++;p=decimal(p,b->duplicated);*p++=13;
 label="C6DIST sum=";while(*label)*p++=*label++;p=decimal(p,traceSum);
 label=" zero=";while(*label)*p++=*label++;p=decimal(p,traceZero);
 label=" one=";while(*label)*p++=*label++;p=decimal(p,traceOne);
 label=" two=";while(*label)*p++=*label++;p=decimal(p,traceTwo);
 label=" sceneEdges=";while(*label)*p++=*label++;p=decimal(p,traceSceneEdges);*p++=13;
 e=ipc_open("/term",&path);if(e)return e;e=os_write(path,line,(Word)(p-line));
 {Byte x=ipc_close(path);if(!e)e=x;}return e;}

static Byte save_trace(DagHeartbeatBroker *b,Word *count)
{Byte e;SavedTrace *t;
 for(;;){e=dag_broker_trace(b);if(e)return e;if(b->traceEmpty)return 0;
  if(*count>=240)return 188;t=&trace[(*count)++];t->generation=b->traceGeneration;
  t->tick=b->traceTick;t->pb1=b->tracePb1;t->phase=b->tracePhase;
  t->wake=b->traceWakeTicks;t->heart=b->traceHeartTicks;
  t->latency=b->traceLatencyTicks;t->scene=b->traceScene;t->pass=b->tracePass;}
}
static Byte write_trace(Word count,const DagHeartbeatBroker *b)
{Byte path,e,row[10],header[16];Word i;
 e=compat_trace_create("/dd/hbtrace",&path);if(e)return e;
 memset(header,0,sizeof(header));memcpy(header,"DHB1",4);header[4]=(Byte)(count>>8);header[5]=(Byte)count;
 header[6]=b->maxWakeTicks;header[7]=b->maxHeartTicks;header[8]=b->maxSceneTicks;
 header[9]=b->maxLatencyTicks;header[10]=b->deadlineMisses;header[11]=b->collapsed;header[12]=b->duplicated;
 e=os_write(path,header,sizeof(header));
 for(i=0;!e&&i<count;i++){row[0]=(Byte)(trace[i].generation>>8);row[1]=(Byte)trace[i].generation;
  row[2]=(Byte)(trace[i].tick>>8);row[3]=(Byte)trace[i].tick;row[4]=trace[i].pb1;
  row[5]=trace[i].phase;row[6]=trace[i].wake;row[7]=trace[i].heart;
  row[8]=trace[i].latency;row[9]=(Byte)((trace[i].scene<<1)|trace[i].pass);
  e=os_write(path,row,sizeof(row));}
 {Byte closeError=compat_trace_close(path);if(!e)e=closeError;}return e;
}
int main(void)
{DagHeartbeatBroker b;DagCompatPresentTiming timing;Byte hearts[28],e=0,r,stale;
 unsigned long first;Word count=0,expected;memset(&b,0,sizeof(b));memset(frame,0,sizeof(frame));
 memset(hearts,0,sizeof(hearts));hearts[3]=hearts[10]=0x3c;hearts[17]=hearts[24]=0x7e;
 dag_compat_present_set_hearts(hearts);e=dag_compat_present_open();if(e)goto done;
 e=opening_heartbeat_modules_open();if(e)goto done;
 e=dag_broker_start(&b,dag_compat_present_path(),"/d1/dodhbp",3);if(e)goto done;
 dag_compat_present_set_broker(&b);e=dag_broker_snapshot(&b);if(e)goto done;first=b.generation;
 while(b.generation-first<REQUIRED_EDGES){
  frame[(Word)((b.generation-first)&191)*32]=(Byte)b.generation;
  e=dag_compat_present(frame,&timing);if(e)goto done;
  /* Force an old status phase periodically; production must reject it. */
  if(!((Word)(b.generation-first)%17)){b.phase^=1;e=dag_broker_scene(&b,148,1,&stale);if(e)goto done;}
  e=dag_broker_snapshot(&b);if(e)goto done;
 }
 expected=(Word)(b.generation-first);e=dag_broker_stats(&b);if(e)goto done;
 if(b.deadlineMisses||b.collapsed||b.duplicated||b.maxWakeTicks>1||
    b.maxHeartTicks>1||b.maxSceneTicks>1||b.maxLatencyTicks>2){e=188;goto done;}
 e=save_trace(&b,&count);if(e)goto done;if(count!=expected){e=188;goto done;}
 {Word i;for(i=0;i<count;i++){traceSum+=trace[i].latency;if(!trace[i].latency)traceZero++;
   else if(trace[i].latency==1)traceOne++;else if(trace[i].latency==2)traceTwo++;
   if(trace[i].scene)traceSceneEdges++;
   if(!trace[i].pass||trace[i].pb1!=trace[i].phase||
    (i&&trace[i].generation!=(Word)(trace[i-1].generation+1))){e=188;goto done;}}}
done:
 if(b.opened){r=dag_broker_close(&b);if(!e)e=r;}else if(b.pid){r=dag_broker_cancel(&b);if(!e)e=r;}
 dag_compat_present_set_broker(0);r=opening_heartbeat_modules_close();if(!e)e=r;
 r=dag_compat_present_close();if(!e)e=r;if(!e){(void)write_summary(count,&b);(void)write_trace(count,&b);}return e;
}
