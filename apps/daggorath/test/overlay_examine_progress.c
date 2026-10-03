#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "gameplay/game.h"
#include "gameplay/overlay-api.h"

Byte dod_overlay_execute(DagOverlayContextV1 *);
static Byte opaque,progressCalls,failAfter;
static void health(void *p,Game *g){assert(p==&opaque);game_health(g);}
static Byte name(void *p,Game *g,Word token,Byte *out){
 assert(p==&opaque);return game_object_name(g,token,out);
}
static void status(void *p,Game *g,Byte *frame,Byte phase){
 assert(p==&opaque);game_render_status(g,frame,phase);
}
static Byte progress(void *p){
 assert(p==&opaque);++progressCalls;return failAfter&&progressCalls>=failAfter?245:0;
}
static DagOverlayServices services={DOD_OVERLAY_ABI_V1,sizeof(DagOverlayServices),
 &opaque,health,name,status};

static Byte examine(Game *g,Byte *frame){DagOverlayContextV1 c;
 memset(&c,0,sizeof(c));c.abiVersion=DOD_OVERLAY_ABI_V1;c.contextSize=sizeof(c);
 c.game=g;c.services=&services;c.frame=frame;c.input="";c.message="";
 c.operation=DOD_OVERLAY_EXAMINE;c.progress=progress;c.progressContext=&opaque;
 return dod_overlay_execute(&c);
}
int main(void){Game g;Byte frame[6144],first;
 game_init_demo(&g,21);assert(examine(&g,frame)==0);first=progressCalls;
 assert(first>16);progressCalls=0;failAfter=3;assert(examine(&g,frame)==245);
 assert(progressCalls==3);puts("overlay EXAMINE progress and error propagation pass");return 0;
}
