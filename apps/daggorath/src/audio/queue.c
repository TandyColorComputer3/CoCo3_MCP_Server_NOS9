#include <cmoc.h>
#include "ipc.h"

/* The game owns this entire FIFO and its one credit. F$Icpt only increments
 * the separate byte supplied to audio_queue_progress(). No 16-bit queue
 * operation is shared with the signal handler. */
static void disable(AudioQueue *q,Byte transportFailure){
 if(q->state==AUDIO_OPTIONAL_DISABLED)return;
 if(transportFailure)++q->transportFailures;
 q->state=AUDIO_OPTIONAL_DISABLED;
 q->count=0;q->credit=0;q->inflight=0;
 if(q->client.opened)(void)ipc_signal(q->client.pid,3);
}
void audio_queue_open(AudioQueue *q,const char *service,const char *profile){
 Byte e;memset(q,0,sizeof(*q));
 e=audio_start_credited(&q->client,service,profile);
 if(e){disable(q,1);return;}
 q->state=AUDIO_OPTIONAL_READY;q->credit=1;
}
void audio_queue_admit(AudioQueue *q,const Byte *events,Byte count){Byte i,tail;
 if(!count||q->state!=AUDIO_OPTIONAL_READY)return;
 /* A group that cannot fit is never partially played. Disable this optional
  * session deterministically instead of concealing a missing combat tail. */
 if(count>AUDIO_QUEUE_CAPACITY-q->count){++q->rejectedGroups;disable(q,0);return;}
 tail=(q->head+q->count)%AUDIO_QUEUE_CAPACITY;
 for(i=0;i<count;i++){q->slots[tail]=events[i];tail=(tail+1)%AUDIO_QUEUE_CAPACITY;}
 q->count+=count;if(q->count>q->highWater)q->highWater=q->count;
}
void audio_queue_progress(AudioQueue *q,Byte notice){Byte e,sound;Word now;
 if(q->state!=AUDIO_OPTIONAL_READY)return;
 if(q->credit){
  if(notice!=q->noticeSeen){disable(q,1);return;} /* stale/duplicate notice */
 }else{
  if(notice!=q->noticeSeen){
   e=audio_receive(&q->client);
   if(e){disable(q,1);return;}
   q->noticeSeen=notice;q->inflight=0;q->credit=1;++q->played;
  }else{
   e=os_clock(&now,0);
   if(e||(Word)(now-q->issuedTick)>=AUDIO_CREDIT_DEADLINE_TICKS)disable(q,1);
   return;
  }
 }
 if(!q->count)return;
 e=os_clock(&now,0);if(e){disable(q,1);return;}
 /* Snapshot before submission: a zero-delay worker may notify before the
  * foreground returns from its eight-byte I$Write. */
 q->noticeSeen=notice;
 sound=q->slots[q->head];e=audio_submit_credited(&q->client,sound);
 if(e){disable(q,1);return;}
 q->head=(q->head+1)%AUDIO_QUEUE_CAPACITY;--q->count;
 q->credit=0;q->inflight=1;q->issuedTick=now;
}
void audio_queue_cancel(AudioQueue *q){
 if(q->state==AUDIO_OPTIONAL_DISABLED)return;
 q->state=AUDIO_OPTIONAL_DISABLED;q->count=0;q->credit=0;q->inflight=0;
 if(q->client.opened)(void)ipc_signal(q->client.pid,3);
}
void audio_queue_close(AudioQueue *q){
 audio_queue_cancel(q);
 if(q->client.opened)(void)audio_cancel(&q->client);
}
