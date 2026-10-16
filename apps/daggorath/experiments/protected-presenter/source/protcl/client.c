/* Disposable presenter-ownership control. No CoWin path or GP pointer here. */
#include <cmoc.h>
#include "compat/heartbeat-broker.h"
#include "gameplay/opening-heartbeat.h"
#include "audio/ipc.h"

Byte os_sleep(Word);
static DagHeartbeatBroker broker;
static volatile Byte resultStage,resultError,requestCount,ackCount;

int main(void)
{
 Byte e=0,x,bootstrap=255,opened=0,started=0,stale=0;
 resultStage=1;
 e=opening_heartbeat_modules_open();if(e)goto done;
 opened=1;
 resultStage=2;
 /* This is a console descriptor used only as the child's temporary standard
  * output. The child independently I$Opens /w and owns its graphics path. */
 e=ipc_open("/term",&bootstrap);if(e)goto done;
 resultStage=3;
 e=dag_broker_start(&broker,bootstrap,"/d1/protpr",46);
 if(e)goto done;
 started=1;
 requestCount=ackCount=2; /* RATE and ENABLE after HELLO. */
 resultStage=4;
 e=ipc_close(bootstrap);bootstrap=255;if(e)goto done;
 e=dag_broker_snapshot(&broker);if(e)goto done;
 requestCount++;ackCount++;
 resultStage=5;
 e=dag_broker_scene(&broker,148,0,&stale);if(e)goto done;
 requestCount++;ackCount++;
 if(stale){e=188;goto done;}
 resultStage=6;
 e=dag_broker_snapshot(&broker);if(e)goto done;
 requestCount++;ackCount++;
 e=dag_broker_scene(&broker,156,1,&stale);if(e)goto done;
 requestCount++;ackCount++;
 if(stale){e=188;goto done;}
 /* Bounded client inactivity: heartbeat service belongs to the presenter. */
 resultStage=7;
 e=os_sleep(100);if(e)goto done;
 e=dag_broker_stats(&broker);if(e)goto done;
 requestCount++;ackCount++;
 if(broker.collapsed||broker.duplicated||broker.fault){e=188;goto done;}
done:
 resultError=e;resultStage=8;
 if(started){x=dag_broker_close(&broker);if(!x){requestCount++;ackCount++;}
  if(!e)e=x;
 }else if(broker.pid){x=dag_broker_cancel(&broker);if(!e)e=x;}
 if(bootstrap!=255){x=ipc_close(bootstrap);if(!e)e=x;}
 if(opened){x=opening_heartbeat_modules_close();if(!e)e=x;}
 resultError=e;resultStage=9;
 return e;
}
