#ifndef DAGGORATH_HEARTBEAT_BROKER_H
#define DAGGORATH_HEARTBEAT_BROKER_H
#include "../platform.h"

/* Fixed 16-byte pipe protocol. Request byte 3 is the sequence and byte 5 is
 * the operation argument; keeping them distinct is required for rate and Y.
 * No process-local address crosses this ABI.
 * The scene GP buffer is named in CoWin and cannot be refilled until ACK. */
#define DAG_BROKER_MAGIC 0xb6
#define DAG_BROKER_VERSION 1
#define DAG_BROKER_FRAME 16
#define DAG_BROKER_PRIORITY 192
#define DAG_BROKER_STATUS 1
#define DAG_BROKER_STALE 1
enum { DAG_BROKER_HELLO=1,DAG_BROKER_RATE,DAG_BROKER_ENABLE,
       DAG_BROKER_DISABLE,DAG_BROKER_SCENE,DAG_BROKER_SNAPSHOT,
       DAG_BROKER_SHUTDOWN,DAG_BROKER_STATS,DAG_BROKER_TRACE };

typedef struct {
 Byte command,reply,pid,opened,sequence,phase,fault;
 unsigned long generation;
 Word heartWrites,sceneWrites,staleRejected;
 Byte maxWakeTicks,maxHeartTicks,maxSceneTicks,maxLatencyTicks;
 Byte deadlineMisses,collapsed,duplicated;
 Word traceGeneration,traceTick;
 Byte tracePb1,tracePhase,traceWakeTicks,traceHeartTicks;
 Byte traceLatencyTicks,traceScene,tracePass,traceEmpty,kick;
} DagHeartbeatBroker;

Byte dag_broker_start(DagHeartbeatBroker *broker,Byte graphicsPath,
                      const char *service,Byte rate);
Byte dag_broker_rate(DagHeartbeatBroker *broker,Byte rate);
Byte dag_broker_snapshot(DagHeartbeatBroker *broker);
Byte dag_broker_stats(DagHeartbeatBroker *broker);
Byte dag_broker_trace(DagHeartbeatBroker *broker);
Byte dag_broker_scene(DagHeartbeatBroker *broker,Byte destinationY,Byte status,
                      Byte *stale);
Byte dag_broker_close(DagHeartbeatBroker *broker);
Byte dag_broker_cancel(DagHeartbeatBroker *broker);

#endif
