#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "gameplay/scheduler-api.h"

Byte dodsched_execute(DagSchedulerContextV1 *);

static Byte taskCalls,presentCalls,progressCalls,taskError;

static Byte task(void *opaque,Game *g,GameTiming *timing,CreatureScheduler *creatures,
                 Byte code,Byte ccb,Byte *dirty){
 (void)opaque;(void)timing;(void)creatures;(void)ccb;
 ++taskCalls;*dirty=0;
 if(code==DOD_TASK_HEALTH)game_health(g);
 return taskError;
}
static Word present(void *opaque,Game *g,Byte mode){
 (void)opaque;(void)g;(void)mode;++presentCalls;
 return 0;
}
static Byte progress(void *opaque){(void)opaque;++progressCalls;return 0;}

int main(void){
 Game g;GameTiming timing;CreatureScheduler creatures;DagSchedulerState state;
 DagSchedulerServices services;DagSchedulerContextV1 c;
 memset(&timing,0,sizeof(timing));memset(&creatures,0,sizeof(creatures));
 memset(&services,0,sizeof(services));memset(&c,0,sizeof(c));
 game_init(&g,0);services.version=DOD_SCHEDULER_ABI_V1;
 services.size=sizeof(services);services.task=task;services.present=present;services.progress=progress;
 c.abiVersion=DOD_SCHEDULER_ABI_V1;c.contextSize=sizeof(c);c.game=&g;c.timing=&timing;
 c.creatures=&creatures;c.state=&state;c.services=&services;
 c.operation=DOD_SCHED_INIT;assert(dodsched_execute(&c)==0);
 state.newLook=1;timing.recoveryPending=1;timing.burnPending=1;
 c.operation=DOD_SCHED_BOUNDARY;assert(dodsched_execute(&c)==0);
 assert(taskCalls>=3&&presentCalls==1&&state.count==0);
 c.operation=DOD_SCHED_REQUEST_LOOK;assert(dodsched_execute(&c)==0);
 c.operation=DOD_SCHED_NORMAL_TICKS;c.logicalJiffies=18;assert(dodsched_execute(&c)==0);
 assert(presentCalls==2&&state.newLook==0);
 c.operation=DOD_SCHED_NORMAL_TICKS;c.logicalJiffies=2;assert(dodsched_execute(&c)==0);
 assert(progressCalls==20);
 state.head=state.tail=state.count=0;state.fifo[0]=DOD_TASK_HEALTH;state.tail=1;state.count=1;
 taskError=245;c.operation=DOD_SCHED_BOUNDARY;assert(dodsched_execute(&c)==245);
 puts("dodsched repeated task/present/progress callback contract checks passed");
 return 0;
}
