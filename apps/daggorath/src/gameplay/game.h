#ifndef DOD_GAME_H
#define DOD_GAME_H
#include "platform.h"
/* Original packed OCB/CCB bytes, not native C pointers. Link words retain
 * original OCBLND address tokens; only ocb() translates at the boundary. */
typedef struct {
 Byte maze[1024],objects[72][14],creatures[32][17],seed[3];
 Byte row,col,dir,count,creatureCount,rate,faint,lit,dead;
 Word power,damage,weight,bag,hand,torch,recovery,burn;
 Word rightHand;
} Game;
enum { GAME_OK=0,GAME_BLOCKED=1,GAME_INVALID=2,GAME_FAINT=3 };
void game_init(Game *g,Byte second);
Byte game_command(Game *g,const char *command);
void game_tick(Game *g,Word ticks);
void game_render(Game *g,Byte *frame,const char *input,const char *message);
/* STATUS/COMTXT in logical coordinates. phase: COMMON:HEARTS, 0 small. */
void game_render_status(Game *g,Byte *frame,Byte phase);
/* Port UI only: restore the seven input rows before drawing the next line.
 * The underlay preserves dungeon pixels when the edited line becomes shorter. */
#define GAME_INPUT_OFFSET (184*32)
#define GAME_INPUT_BYTES (7*32)
void game_render_input(Byte *frame,const char *input,const Byte *underlay);
Byte game_random(Game *g);
#endif
