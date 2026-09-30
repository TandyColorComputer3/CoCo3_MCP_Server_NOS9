/* One Level II DAT slot is shared, never nested: an overlay command returns
 * before the next complete scheduler boundary temporarily links dodsched. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "gameplay/overlay-api.h"
#include "gameplay/scheduler-api.h"
typedef struct { Word header,entry;Byte retained,disabled; } OverlayLink;
typedef struct { Word header,entry;Byte retained; } SchedulerLink;
static Byte logbuf[16],n;
Byte overlay_preload(void){logbuf[n++]=1;return 0;} Byte overlay_release(void){logbuf[n++]=5;return 0;}
Byte overlay_link(OverlayLink *p){assert(!p->header);p->header=1;p->entry=2;logbuf[n++]=2;return 0;}
Byte overlay_unlink(OverlayLink *p){assert(p->header);p->header=0;logbuf[n++]=4;return 0;}
Byte overlay_call(OverlayLink *p,DagOverlayContextV1 *c){(void)p;logbuf[n++]=3;c->result=GAME_OK;c->view=GAME_VIEW_KEEP;c->outputMessage="";return 0;}
Byte scheduler_preload(void){logbuf[n++]=6;return 0;} Byte scheduler_release(void){logbuf[n++]=10;return 0;}
Byte scheduler_link(SchedulerLink *p){assert(!p->header);p->header=3;p->entry=4;logbuf[n++]=7;return 0;}
Byte scheduler_unlink(SchedulerLink *p){assert(p->header);p->header=0;logbuf[n++]=9;return 0;}
Byte scheduler_call(SchedulerLink *p,DagSchedulerContextV1 *c){(void)p;(void)c;logbuf[n++]=8;return 0;}
void game_health(Game *g){(void)g;} Byte game_object_name(Game *g,Word x,Byte *n){(void)g;(void)x;(void)n;return 0;}
void game_render_status(Game *g,Byte *f,Byte p){(void)g;(void)f;(void)p;}
int main(void){Game g;GameCombat combat;DagSchedulerContextV1 scheduler;Byte result,view;const char *message;
 memset(&g,0,sizeof(g));memset(&combat,0,sizeof(combat));memset(&scheduler,0,sizeof(scheduler));
 assert(game_overlay_open()==0&&game_scheduler_open()==0);
 assert(game_overlay_command(&g,"LOOK",&combat,&result,&view,&message)==0&&result==GAME_OK);
 assert(game_scheduler_call(&scheduler)==0);
 assert(game_scheduler_close()==0&&game_overlay_close()==0);
 assert(n==10&&memcmp(logbuf,(Byte[]){1,6,2,3,4,7,8,9,10,5},10)==0);
 puts("overlay-to-scheduler one-slot ordering checks passed");return 0;
}
