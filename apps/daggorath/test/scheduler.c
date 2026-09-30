#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "gameplay/scheduler-api.h"

extern Byte dodsched_execute(DagSchedulerContextV1 *);
typedef struct { Byte calls[64],count;Word presentation; } Probe;
static Byte primitive(void *opaque,Game *g,GameTiming *t,CreatureScheduler *q,
                      Byte task,Byte ccb,Byte *dirty){Probe *p=opaque;
 (void)g;(void)t;(void)q;*dirty=task==DOD_TASK_CMOVE;
 p->calls[p->count++]=(Byte)(task==DOD_TASK_CMOVE?(DOD_TASK_CMOVE|ccb):task);
 return 0;
}
static Word present(void *opaque,Game *g,Byte mode){Probe *p=opaque;(void)g;(void)mode;return p->presentation;}
static DagSchedulerContextV1 context(Game *g,GameTiming *t,CreatureScheduler *q,
                                     DagSchedulerState *s,DagSchedulerServices *v,Byte op){
 DagSchedulerContextV1 c;memset(&c,0,sizeof(c));c.abiVersion=1;c.contextSize=sizeof(c);
 c.operation=op;c.game=g;c.timing=t;c.creatures=q;c.state=s;c.services=v;return c;
}
int main(void){Game g;GameTiming t;CreatureScheduler q;DagSchedulerState s;Probe p;
 DagSchedulerServices v={1,sizeof(v),&p,primitive,present,0};DagSchedulerContextV1 c;
 memset(&g,0,sizeof(g));memset(&t,0,sizeof(t));memset(&q,0,sizeof(q));memset(&p,0,sizeof(p));
 assert(sizeof(DagSchedulerState)==52);
 assert(dodsched_execute(0)==187);
 c=context(&g,&t,&q,&s,&v,255);assert(dodsched_execute(&c)==187);
 c=context(&g,&t,&q,&s,&v,DOD_SCHED_INIT);assert(dodsched_execute(&c)==0);
 /* TCBDAT order after PLAYER is LUKNEW, HSLOW, BURNER, CREGEN. */
 c.operation=DOD_SCHED_BOUNDARY;assert(dodsched_execute(&c)==0);
 assert(p.count==3&&p.calls[0]==DOD_TASK_HSLOW&&p.calls[1]==DOD_TASK_BURNER&&p.calls[2]==DOD_TASK_CREGEN);
 /* LUKNEW has no requested display yet. A later request is persistent across
  * the unlink/relink-equivalent separate calls and uses logical, not wall time. */
 c.operation=DOD_SCHED_REQUEST_LOOK;assert(dodsched_execute(&c)==0);p.presentation=17;
 c.operation=DOD_SCHED_CLOCK;c.logicalJiffies=18;assert(dodsched_execute(&c)==0);
 c.operation=DOD_SCHED_BOUNDARY;assert(dodsched_execute(&c)==0&&c.dirty);
 assert(s.sourceJiffy==35);
 /* One six-jiffy Q.TEN boundary promotes CCB 7. It runs only when the next
  * foreground boundary is explicitly entered. */
 q.framePhase=5;q.countdown[7]=1;c.operation=DOD_SCHED_CLOCK;c.logicalJiffies=1;
 assert(dodsched_execute(&c)==0&&s.count==2&&q.pending[7]);
 c.operation=DOD_SCHED_BOUNDARY;assert(dodsched_execute(&c)==0&&p.calls[p.count-1]==(DOD_TASK_CMOVE|7)&&!q.pending[7]);
 /* PLAYER's source wait is 81 distinct CLOCK jiffies and does not dispatch. */
 p.count=0;c.operation=DOD_SCHED_PLAYER_WAIT;assert(dodsched_execute(&c)==0&&p.count==0);
 puts("dodsched ABI, persistent-state, FIFO and source-boundary checks passed");
 return 0;
}
