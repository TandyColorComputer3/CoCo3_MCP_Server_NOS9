#ifndef DOD_SCHEDULER_API_H
#define DOD_SCHEDULER_API_H

#include "game.h"

/* Level II source-scheduler ABI v1.  The callable module has no mutable
 * globals: every value which survives F$UnLink belongs to this context. */
#define DOD_SCHEDULER_ABI_V1 1
enum {
 DOD_SCHED_INIT=1,DOD_SCHED_PLAYER_WAIT,DOD_SCHED_CLOCK,
 DOD_SCHED_BOUNDARY,DOD_SCHED_PRESENT,DOD_SCHED_REQUEST_LOOK,
 DOD_SCHED_NORMAL_TICKS
};
enum { DOD_TASK_LUKNEW=0,DOD_TASK_HSLOW=1,DOD_TASK_BURNER=2,
       DOD_TASK_CREGEN=3,DOD_TASK_HEALTH=4,DOD_TASK_CMOVE=0x80 };

/* Four system tasks plus at most 32 CCB Q.TEN tasks are eligible at a source
 * foreground boundary.  The layout is intentionally 52 bytes on the CMOC
 * target: no packed Game/OCB/CCB source table moves as a result. */
typedef struct {
 Byte fifo[36],head,tail,count;
 Byte queued,newLook,sourceJiffy,sourceTenth,sourceSecond;
 Word lookTicks,cregenTicks;
 Byte playerPhase,presentationMode,flags,reserved;
} DagSchedulerState;

typedef Byte (*DagSchedulerTask)(void *opaque,Game *game,GameTiming *timing,
                                 CreatureScheduler *creatures,Byte task,
                                 Byte ccb,Byte *sourceUpdate);
/* This returns source logical jiffies for the selected source presentation
 * operation. It must not measure CoWin/GFX2 or wall-clock duration. */
typedef Word (*DagSchedulerPresent)(void *opaque,Game *game,Byte mode);
/* This is a bounded foreground presentation service point. The scheduler
 * never performs GFX2 work; the resident callback may refresh the already
 * presented heartbeat state or report cancellation. */
typedef Byte (*DagSchedulerProgress)(void *opaque);

typedef struct {
 Byte version,size;
 void *opaque;
 DagSchedulerTask task;
 DagSchedulerPresent present;
 DagSchedulerProgress progress;
} DagSchedulerServices;

typedef struct {
 Byte abiVersion,contextSize,operation;
 Game *game;
 GameTiming *timing;
 CreatureScheduler *creatures;
 DagSchedulerState *state;
 Word logicalJiffies;
 const DagSchedulerServices *services;
 Byte dirty;
 Byte ccb;
} DagSchedulerContextV1;

/* Resident lifecycle; the actual F$Link mapping is temporary per call. */
Byte game_scheduler_open(void);
Byte game_scheduler_close(void);
Byte game_scheduler_call(DagSchedulerContextV1 *context);

#endif
