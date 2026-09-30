/* Resident lifecycle half of the mandatory source-scheduler module.  Unlike
 * optional dodaudio, any load/link/call failure is returned to the caller. */
#include "scheduler-api.h"
typedef struct { Word header,entry;Byte retained; } SchedulerLink;
extern Byte scheduler_preload(void);
extern Byte scheduler_release(void);
extern Byte scheduler_link(SchedulerLink *);
extern Byte scheduler_call(SchedulerLink *,DagSchedulerContextV1 *);
extern Byte scheduler_unlink(SchedulerLink *);
static SchedulerLink linkState;
Byte game_scheduler_open(void){Byte e;
 if(linkState.retained)return 0;
 e=scheduler_preload();if(e)return e;linkState.retained=1;return 0;
}
Byte game_scheduler_close(void){Byte e;
 if(!linkState.retained)return 0;
 e=scheduler_release();if(!e)linkState.retained=0;return e;
}
Byte game_scheduler_call(DagSchedulerContextV1 *context){Byte e,u;
 if(!linkState.retained)return 221;
 e=scheduler_link(&linkState);if(e)return e;
 e=scheduler_call(&linkState,context);
 u=scheduler_unlink(&linkState);
 if(!e)e=u;
 return e;
}
