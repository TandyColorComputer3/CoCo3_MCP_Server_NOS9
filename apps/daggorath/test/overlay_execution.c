#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "gameplay/game.h"
#include "gameplay/overlay-api.h"

extern Byte dod_overlay_execute(DagOverlayContextV1 *);
static DagOverlayServices services={DOD_OVERLAY_ABI_V1,sizeof(DagOverlayServices),
 game_health,game_object_name,game_render_status};

static Byte run(Game *g,const char *text,GameCombat *combat,Byte *view,const char **message){
 DagOverlayContextV1 c;
 memset(&c,0,sizeof(c));c.abiVersion=DOD_OVERLAY_ABI_V1;c.contextSize=sizeof(c);
 c.game=g;c.services=&services;c.command=text;c.combat=combat;c.operation=DOD_OVERLAY_COMMAND;
 assert(dod_overlay_execute(&c)==0);*view=c.view;*message=c.outputMessage;return c.result;
}
static void compare_command(const char *text){Game resident,overlaid;GameCombat a,b;Byte va,vb,result;const char *ma,*mb;
 game_init(&resident,0);game_init(&overlaid,0);
 result=game_command_combat(&resident,text,&a);assert(result==run(&overlaid,text,&b,&vb,&mb));
 va=game_display_command(text);ma=game_message(text,result);
 assert(!memcmp(&resident,&overlaid,sizeof(Game)));assert(!memcmp(&a,&b,sizeof(a)));
 assert(va==vb&&!strcmp(ma,mb));
}
int main(void){Game direct,overlay,bad;Byte frameA[6144],frameB[6144],view;GameCombat combat;const char *message;
 compare_command("PULL LEFT TORCH");compare_command("USE LEFT");compare_command("TURN RIGHT");
 compare_command("MOVE");compare_command("ATTACK LEFT");compare_command("GET LEFT SWORD");
 game_init(&direct,0);overlay=direct;
 assert(game_command(&direct,"PULL LEFT TORCH")==run(&overlay,"PULL LEFT TORCH",&combat,&view,&message));
 assert(game_command(&direct,"USE LEFT")==run(&overlay,"USE LEFT",&combat,&view,&message));
 game_render_examine(&direct,frameA,"","OK");
 {DagOverlayContextV1 c;memset(&c,0,sizeof(c));c.abiVersion=1;c.contextSize=sizeof(c);c.game=&overlay;c.services=&services;c.frame=frameB;c.input="";c.message="OK";c.operation=DOD_OVERLAY_EXAMINE;assert(dod_overlay_execute(&c)==0);}
 assert(!memcmp(frameA,frameB,sizeof(frameA)));assert(!memcmp(&direct,&overlay,sizeof(Game)));
 bad=overlay;{DagOverlayContextV1 c;memset(&c,0,sizeof(c));c.abiVersion=2;c.contextSize=sizeof(c);c.game=&overlay;c.services=&services;c.operation=DOD_OVERLAY_COMMAND;assert(dod_overlay_execute(&c)==187);}
 assert(!memcmp(&bad,&overlay,sizeof(Game)));puts("overlay command/examine ABI checks passed");return 0;
}
