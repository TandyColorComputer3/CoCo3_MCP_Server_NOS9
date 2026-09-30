#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "gameplay/scheduler-api.h"
typedef struct { Word header,entry;Byte retained; } SchedulerLink;
static Byte sequence[8],n,fail;
Byte scheduler_preload(void){sequence[n++]=1;return fail==1?221:0;}
Byte scheduler_release(void){sequence[n++]=5;return fail==5?187:0;}
Byte scheduler_link(SchedulerLink *l){sequence[n++]=2;l->header=1;l->entry=2;return fail==2?237:0;}
Byte scheduler_call(SchedulerLink *l,DagSchedulerContextV1 *c){(void)l;(void)c;sequence[n++]=3;return fail==3?187:0;}
Byte scheduler_unlink(SchedulerLink *l){(void)l;sequence[n++]=4;return fail==4?187:0;}
int main(void){DagSchedulerContextV1 c;memset(&c,0,sizeof(c));
 assert(game_scheduler_call(&c)==221);assert(game_scheduler_open()==0);assert(n==1&&sequence[0]==1);
 assert(game_scheduler_call(&c)==0);assert(n==4&&sequence[1]==2&&sequence[2]==3&&sequence[3]==4);
 assert(game_scheduler_close()==0&&n==5&&sequence[4]==5);
 fail=1;assert(game_scheduler_open()==221);assert(game_scheduler_call(&c)==221);
 puts("dodsched mandatory retained-link lifecycle checks passed");return 0;}
