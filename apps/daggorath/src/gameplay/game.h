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
/* Separate from Game so the authentic 2,606-byte legacy state layout stays
 * byte-for-byte stable. countdown[] models the original Q.TEN TCB timers. */
typedef struct { Word countdown[32]; Byte framePhase; Byte combatPending[32]; } CreatureScheduler;
enum { GAME_OK=0,GAME_BLOCKED=1,GAME_INVALID=2,GAME_FAINT=3 };
/* PATTK/ATTACK/DAMAGE result facts. Event values are original SOUNDS.ASM
 * dispatch IDs, consumed by the existing semantic audio service. */
typedef struct {
 Byte events[3],eventCount,rngCalls,hit,killed,target;
 Word energy,hitValue,damage;
} GameCombat;
void game_init(Game *g,Byte second);
Byte game_command(Game *g,const char *command);
Byte game_command_combat(Game *g,const char *command,GameCombat *combat);
Byte game_population(const Game *g,Byte type);
const char *game_message(const char *command,Byte result);
/* DSPMOD is UI state, separate from authoritative Game inventory. */
enum { GAME_VIEW_KEEP=0,GAME_VIEW_DUNGEON=1,GAME_VIEW_EXAMINE=2 };
Byte game_display_command(const char *command);
void game_render_examine(Game *g,Byte *frame,const char *input,const char *message);
void game_tick(Game *g,Word ticks);
void game_render(Game *g,Byte *frame,const char *input,const char *message);
/* Optional foreground progress hook; the unfinished logical frame must not be
 * presented as a scene. A nonzero hook result aborts rendering. */
typedef Byte (*GameRenderProgress)(Game *g,Byte *frame,void *context);
Byte game_render_with_progress(Game *g,Byte *frame,const char *input,
                              const char *message,GameRenderProgress progress,void *context);
/* STATUS/COMTXT in logical coordinates. phase: COMMON:HEARTS, 0 small. */
void game_render_status(Game *g,Byte *frame,Byte phase);
Byte game_render_status_progress(Game *g,Byte *frame,Byte phase,
                                 GameRenderProgress progress,void *context);
void game_render_heart(Byte *frame,Byte phase);
void game_heart_patterns(Byte patterns[28]);
/* Port UI only: restore the seven input rows before drawing the next line.
 * The underlay preserves dungeon pixels when the edited line becomes shorter. */
#define GAME_INPUT_OFFSET (184*32)
#define GAME_INPUT_BYTES (7*32)
void game_render_input(Byte *frame,const char *input,const Byte *underlay);
Byte game_render_input_progress(Game *g,Byte *frame,const char *input,const Byte *underlay,
                                GameRenderProgress progress,void *context);
Byte game_random(Game *g);
/* Narrow resident services passed explicitly to the dynamic command overlay.
 * They preserve the existing packed-state interpretation; the overlay never
 * imports resident C symbols directly. */
void game_health(Game *g);
Byte game_object_name(Game *g,Word token,Byte *name);
void game_creature_init(Game *g,CreatureScheduler *scheduler);
/* Consume emulated 60 Hz video ticks. Returns nonzero when the visible scene
 * needs a redraw; same-cell attack entry is reported, never executed. */
Byte game_creature_advance(Game *g,CreatureScheduler *scheduler,Word videoTicks);
/* Identical Q.TEN advancement with optional bounded foreground service after
 * each completed tenth-second step. The callback may update only UI status. */
Byte game_creature_advance_progress(Game *g,CreatureScheduler *scheduler,Word videoTicks,
                                    Byte (*progress)(void *),void *context,Byte *dirty);
#endif
