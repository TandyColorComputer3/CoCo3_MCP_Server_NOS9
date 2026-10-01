#ifndef DOD_SCHEDULER_CALLBACKS_H
#define DOD_SCHEDULER_CALLBACKS_H

#include "scheduler-api.h"

/* The mapped scheduler invokes resident services through assembly gateways.
 * They restore CMOC's resident Y data base before tail-calling C. */
typedef struct { Word dataY,userOpaque; } DagSchedulerCallbackContext;
void scheduler_callback_init(DagSchedulerCallbackContext *,void *);
Byte scheduler_task_gateway(void *,Game *,GameTiming *,CreatureScheduler *,Byte,Byte,Byte *);
Word scheduler_present_gateway(void *,Game *,Byte);
Byte scheduler_progress_gateway(void *);

#endif
