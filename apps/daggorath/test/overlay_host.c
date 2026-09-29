#include <assert.h>
#include <stdio.h>
#include "gameplay/overlay-api.h"
typedef struct { Word header,entry;Byte retained,disabled; } OverlayLink;
static int loads,links,calls,unlinks,unloads;static Byte failLink;
Byte overlay_preload(void){++loads;return 0;} Byte overlay_release(void){++unloads;return 0;}
Byte overlay_link(OverlayLink *r){++links;if(failLink)return failLink;r->header=0xc000;r->entry=0xc014;return 0;}
Byte overlay_call(OverlayLink *r,DagOverlayContextV1 *c){(void)r;++calls;assert(c->abiVersion==1&&c->operation==DOD_OVERLAY_COMMAND);c->result=GAME_OK;c->view=GAME_VIEW_KEEP;c->outputMessage="OK";return 0;}
Byte overlay_unlink(OverlayLink *r){++unlinks;assert(r->header==0xc000);return 0;}
void game_health(Game *g){(void)g;} Byte game_object_name(Game *g,Word n,Byte *out){(void)g;(void)n;(void)out;return 0;} void game_render_status(Game *g,Byte *f,Byte p){(void)g;(void)f;(void)p;}
int main(void){Game g;GameCombat c;Byte result,view;const char *message;
 assert(game_overlay_open()==0&&loads==1);assert(game_overlay_open()==0&&loads==1);
 assert(game_overlay_command(&g,"TURN LEFT",&c,&result,&view,&message)==0);assert(links==1&&calls==1&&unlinks==1&&result==GAME_OK);
 failLink=221;assert(game_overlay_command(&g,"TURN LEFT",&c,&result,&view,&message)==221);assert(links==2&&calls==1&&unlinks==1);
 assert(game_overlay_close()==0&&unloads==1);puts("overlay retained-link lifecycle checks passed");return 0;
}
