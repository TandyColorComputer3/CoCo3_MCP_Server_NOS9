#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "game.h"
#include "game_data.h"
#include "logical.h"

static Game g;
static CreatureScheduler s;
static Byte frame1[FRAME_BYTES],frame2[FRAME_BYTES];
static void blank_game(void){memset(&g,0,sizeof(g));memset(g.maze,0,sizeof(g.maze));
 g.row=16;g.col=16;g.seed[0]=0x73;g.seed[1]=0xc7;g.seed[2]=0x5d;
 g.creatureCount=1;g.creatures[0][12]=255;g.creatures[0][13]=10;
 g.creatures[0][14]=0;g.creatures[0][15]=16;g.creatures[0][16]=18;
 g.creatures[0][6]=1;g.creatures[0][7]=3;
 game_creature_init(&g,&s);
}
int main(void){Byte *c;Word damage;Byte changed;
 assert(sizeof(Game)==2608); /* M1B adds appended port-owned logical LEVEL. */
 blank_game();assert(g.creatures[0][15]==16&&g.creatures[0][16]==18);
 assert(!game_creature_advance(&g,&s,5));assert(g.creatures[0][16]==18);
 changed=game_creature_advance(&g,&s,1);c=g.creatures[0];
 assert(changed&&c[15]==16&&c[16]==17&&c[14]==3);
 assert(g.seed[0]==0x72&&g.seed[1]==0x73&&g.seed[2]==0xc7);
 assert(s.countdown[0]==1&&!s.combatPending[0]);
 damage=g.damage;assert(game_creature_advance(&g,&s,6));
 assert(c[15]==16&&c[16]==16&&s.combatPending[0]);
 assert(s.countdown[0]==3&&g.damage==damage); /* CMOV20 is deferred to dodsched. */

 /* Diagonal, blocked paths select the source MOVTAB and consume one RNG. */
 blank_game();c=g.creatures[0];c[15]=8;c[16]=8;g.row=16;g.col=16;
 assert(!game_creature_advance(&g,&s,6));
 assert(c[15]==7&&c[16]==8&&c[14]==0);
 assert(g.seed[0]==0x72&&g.seed[1]==0x73&&g.seed[2]==0xc7);
 blank_game();c=g.creatures[0];c[15]=8;c[16]=8;
 memset(g.maze,255,sizeof(g.maze));g.maze[8*32+8]=0;g.maze[16*32+16]=0;
 assert(!game_creature_advance(&g,&s,6));
 assert(c[15]==8&&c[16]==8&&g.seed[0]==0x72&&g.seed[1]==0x73&&g.seed[2]==0xc7);

 /* Original CMOV10 object pickup: OCB order, one action, no RNG. */
 blank_game();c=g.creatures[0];c[13]=0;c[15]=4;c[16]=5;c[6]=2;g.count=1;
 g.objects[0][2]=4;g.objects[0][3]=5;g.objects[0][4]=0;g.objects[0][5]=0;
 game_creature_init(&g,&s);assert(game_creature_advance(&g,&s,12));
 assert(g.objects[0][5]==255&&g.creatures[0][8]==0x0b&&g.creatures[0][9]==0x15);
 assert(s.countdown[0]==2&&g.seed[0]==0x73&&g.seed[1]==0xc7&&g.seed[2]==0x5d);

 /* Inactive CCB slots neither schedule nor render. */
 blank_game();c=g.creatures[0];c[12]=0;game_creature_init(&g,&s);
 assert(!s.countdown[0]&&!game_creature_advance(&g,&s,600));

 blank_game();g.row=16;g.col=11;g.dir=0;g.torch=0x0b15;
 g.objects[0][7]=31;g.creatures[0][13]=7;g.creatures[0][15]=15;g.creatures[0][16]=11;
 game_render(&g,frame1,"","");g.creatures[0][12]=0;game_render(&g,frame2,"","");
 assert(memcmp(frame1,frame2,FRAME_BYTES)!=0);
 game_render_examine(&g,frame1,"","");g.creatures[0][12]=255;game_render_examine(&g,frame2,"","");
 assert(memcmp(frame1,frame2,FRAME_BYTES)==0); /* off-player creatures are not named by M5 EXAMINE */
 puts("13 Gameplay M6 creature checks passed");return 0;
}
