#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "gameplay/game.h"
#include "gameplay/overlay-api.h"

Byte dod_overlay_execute(DagOverlayContextV1 *);
static Byte opaque,healthCalls,nameCalls,statusCalls;

static void health(void *context,Game *g){
 assert(context==&opaque);++healthCalls;game_health(g);
}
static Byte object_name(void *context,Game *g,Word token,Byte *name){
 assert(context==&opaque);++nameCalls;return game_object_name(g,token,name);
}
static void render_status(void *context,Game *g,Byte *frame,Byte phase){
 assert(context==&opaque);++statusCalls;game_render_status(g,frame,phase);
}
static DagOverlayServices services={DOD_OVERLAY_ABI_V1,sizeof(DagOverlayServices),
 &opaque,health,object_name,render_status};

static void call_examine(Game *g,Byte *frame){
 DagOverlayContextV1 c;
 memset(&c,0,sizeof(c));c.abiVersion=DOD_OVERLAY_ABI_V1;c.contextSize=sizeof(c);
 c.game=g;c.services=&services;c.frame=frame;c.input="";c.message="";
 c.operation=DOD_OVERLAY_EXAMINE;assert(dod_overlay_execute(&c)==0);
}
int main(void){
 Game g;Byte frame[6144];DagOverlayContextV1 c;
 game_init(&g,0);call_examine(&g,frame);call_examine(&g,frame);
 assert(nameCalls>=2&&statusCalls==2);
 memset(&c,0,sizeof(c));c.abiVersion=DOD_OVERLAY_ABI_V1;c.contextSize=sizeof(c);
 c.game=&g;c.services=&services;c.command="MOVE";c.operation=DOD_OVERLAY_COMMAND;
 assert(dod_overlay_execute(&c)==0&&c.result==GAME_OK&&healthCalls==1);
 services.opaque=0;assert(dod_overlay_execute(&c)==187);services.opaque=&opaque;
 puts("overlay repeated resident service callback contract checks passed");return 0;
}
