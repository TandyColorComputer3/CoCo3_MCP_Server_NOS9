#ifndef DAGGORATH_COMPAT_PRESENT_H
#define DAGGORATH_COMPAT_PRESENT_H
#include "compat.h"
#ifdef DAG_COMPAT_C6
#include "heartbeat-broker.h"
#endif

typedef struct {
    Word conversionTicks;
    Word transferTicks;
    Word mappedLength;
} DagCompatPresentTiming;

Byte dag_compat_present_open(void);
#ifdef DAG_COMPAT_C6
void dag_compat_present_set_broker(DagHeartbeatBroker *broker);
#endif
Byte dag_compat_present(const Byte logicalFrame[DAG_COMPAT_FRAME_BYTES],
                        DagCompatPresentTiming *timing);
Byte dag_compat_present_progress(const Byte logicalFrame[DAG_COMPAT_FRAME_BYTES],
                                 DagCompatPresentTiming *timing,
                                 Byte (*progress)(void *),void *context);
Byte dag_compat_present_primary(const Byte logicalFrame[DAG_COMPAT_FRAME_BYTES],
                                DagCompatPresentTiming *timing);
Byte dag_compat_present_primary_progress(const Byte logicalFrame[DAG_COMPAT_FRAME_BYTES],
                                         DagCompatPresentTiming *timing,
                                         Byte (*progress)(void *),void *context);
void dag_compat_present_set_hearts(const Byte heartPatterns[28]);
Byte dag_compat_present_heart(const Byte logicalFrame[DAG_COMPAT_FRAME_BYTES]);
Byte dag_compat_present_cell(const Byte logicalFrame[DAG_COMPAT_FRAME_BYTES],
                             Word cursor, DagCompatPresentTiming *timing);
Byte dag_compat_present_close(void);
Byte dag_compat_present_path(void);

#endif
