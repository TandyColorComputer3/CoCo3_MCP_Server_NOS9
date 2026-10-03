#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "audio/audio.h"

static Word now;
static Byte submitted[32],submittedCount,received,cancelled,failReceive,failWrite;
Byte audio_start_credited(AudioClient *c,const char *service,const char *profile){
 assert(!strcmp(service,"/d1/dodaudio")&&!strcmp(profile,"ssc-mame-fast"));
 memset(c,0,sizeof(*c));c->opened=1;c->pid=7;return 0;
}
Byte audio_submit_credited(AudioClient *c,Byte sound){
 assert(c->opened&&!c->pending);
 if(failWrite)return AUDIO_IO;
 submitted[submittedCount++]=sound;c->pending=AUDIO_CREDIT_PLAY;++c->sequence;
 return 0;
}
Byte audio_receive(AudioClient *c){
 assert(c->opened&&c->pending==AUDIO_CREDIT_PLAY);
 ++received;if(failReceive)return AUDIO_BAD;c->pending=0;return 0;
}
Byte ipc_signal(Byte pid,Byte signal){assert(pid==7&&signal==3);++cancelled;return 0;}
Byte audio_cancel(AudioClient *c){assert(c->opened);c->opened=0;return 3;}
Byte os_clock(Word *ticks,Byte validate){assert(!validate);*ticks=now;return 0;}
static void reset(void){now=submittedCount=received=cancelled=failReceive=failWrite=0;}
static void open_queue(AudioQueue *q){audio_queue_open(q,"/d1/dodaudio","ssc-mame-fast");assert(q->credit==1&&q->state==AUDIO_OPTIONAL_READY);}
int main(void){AudioQueue q;Byte group[3]={AUDIO_WHOOSH,AUDIO_KLINK,AUDIO_BANG};Byte i;
 reset();open_queue(&q);
 audio_queue_admit(&q,group,3);assert(q.count==3&&q.highWater==3);
 audio_queue_progress(&q,0);assert(!q.credit&&q.count==2&&submittedCount==1&&submitted[0]==AUDIO_WHOOSH);
 now=3;audio_queue_progress(&q,0);assert(submittedCount==1&&!received);
 audio_queue_progress(&q,1);assert(!q.credit&&received==1&&submittedCount==2&&submitted[1]==AUDIO_KLINK);
 audio_queue_progress(&q,2);assert(received==2&&submitted[2]==AUDIO_BANG);
 audio_queue_progress(&q,3);assert(q.credit&&q.count==0&&q.played==3&&received==3);
 audio_queue_close(&q);assert(!q.client.opened&&cancelled);

 reset();open_queue(&q);
 for(i=0;i<6;i++)audio_queue_admit(&q,group,1);
 assert(q.count==6&&q.highWater==6);
 audio_queue_admit(&q,group,2);assert(q.count==8&&q.highWater==8);
 audio_queue_admit(&q,group,3);assert(q.rejectedGroups==1&&q.transportFailures==0&&q.count==0&&q.state==AUDIO_OPTIONAL_DISABLED);
 audio_queue_close(&q);

 reset();open_queue(&q);
 for(i=0;i<6;i++)audio_queue_admit(&q,group,1);
 audio_queue_progress(&q,0);assert(!q.credit&&submittedCount==1);
 now=AUDIO_CREDIT_DEADLINE_TICKS-1;audio_queue_progress(&q,0);assert(q.state==AUDIO_OPTIONAL_READY);
 now=AUDIO_CREDIT_DEADLINE_TICKS;audio_queue_progress(&q,0);assert(q.state==AUDIO_OPTIONAL_DISABLED&&q.transportFailures==1&&q.count==0);
 audio_queue_progress(&q,1);assert(!q.credit&&received==0);audio_queue_close(&q);

 reset();open_queue(&q);audio_queue_admit(&q,group,3);audio_queue_progress(&q,0);
 audio_queue_cancel(&q);assert(q.count==0&&!q.credit);
 audio_queue_progress(&q,1);assert(!received);audio_queue_close(&q);

 reset();open_queue(&q);audio_queue_progress(&q,1);
 assert(q.state==AUDIO_OPTIONAL_DISABLED&&q.transportFailures==1);audio_queue_close(&q);

 reset();open_queue(&q);audio_queue_admit(&q,group,1);audio_queue_progress(&q,0);
 failReceive=1;audio_queue_progress(&q,1);
 assert(q.state==AUDIO_OPTIONAL_DISABLED&&!q.credit&&q.transportFailures==1);audio_queue_close(&q);

 reset();open_queue(&q);audio_queue_admit(&q,group,1);failWrite=1;audio_queue_progress(&q,0);
 assert(q.state==AUDIO_OPTIONAL_DISABLED&&q.transportFailures==1);audio_queue_close(&q);
 puts("6 credited audio queue contracts passed");return 0;
}
