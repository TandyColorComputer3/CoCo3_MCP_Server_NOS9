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
 /* NEWLVL's LEVEL. Appended with explicit padding so both host and CMOC's
  * target ABI are exactly 2,608 bytes; all source-derived table offsets stay
  * stable for existing Level II command-overlay ABI consumers. */
 Byte level,levelPadding;
} Game;
/* Separate from Game so the authentic 2,606-byte source-derived table prefix
 * stays byte-for-byte stable.  Game itself is 2,608 bytes after its appended
 * port-owned LEVEL metadata. countdown[] models the original Q.TEN TCB timers. */
typedef struct {
 Word countdown[32];
 /* COMCRE:CREGEN mutates CMXLND, which is scheduler state rather than a
  * source-derived Game-table byte. Keep the per-level desired population here
  * so later births can observe the source's five-minute regeneration. */
 Byte desired[12];
 Byte framePhase,readyCount;
 Byte combatPending[32],attackDue[32],pending[32],ready[32];
 /* CRETUR:CMOV20 invokes SOUNDS before ATTACK and A$KLK3 on a successful
  * player hit.  Keep those semantic events with the scheduler state so the
  * foreground owner can present them without making combat depend on audio. */
 /* One foreground loop drains the batch before another source scheduling
  * boundary can enqueue events. At most 32 CMOV20 tasks can contribute two
  * events each, so this fixed linear batch cannot wrap. */
 Byte audio[64],audioCount;
} CreatureScheduler;
/* COMPLR.ASM timing-queue state.  Timers live in Game; these flags model a
 * TCB already moved from a CLOCK queue to the scheduler queue, where it can
 * run once at the next foreground scheduling boundary. */
typedef struct { Byte recoveryPending,burnPending; } GameTiming;
/* Source scheduler task order for COMDAT.ASM:TCBDAT.  The normal NitrOS-9
 * loop may use the combined convenience APIs below; attract replay keeps
 * CLOCK promotion and each foreground dispatch distinct. */
enum { GAME_TIMING_HSLOW=0, GAME_TIMING_BURNER=1 };
enum { GAME_OK=0,GAME_BLOCKED=1,GAME_INVALID=2,GAME_FAINT=3 };
/* PATTK/ATTACK/DAMAGE result facts. Event values are original SOUNDS.ASM
 * dispatch IDs, consumed by the existing semantic audio service. */
typedef struct {
 Byte events[3],eventCount,rngCalls,hit,killed,target;
 Word energy,hitValue,damage;
} GameCombat;
void game_init(Game *g,Byte second);
/* ONCE:GAME20 demo path: LEVEL=2, DEMDAT and RAMDAT player coordinates. */
void game_init_demo(Game *g,Byte second);
Byte game_command(Game *g,const char *command);
Byte game_command_combat(Game *g,const char *command,GameCombat *combat);
Byte game_population(const Game *g,Byte type);
const char *game_message(const char *command,Byte result);
/* DSPMOD is UI state, separate from authoritative Game inventory. */
enum { GAME_VIEW_KEEP=0,GAME_VIEW_DUNGEON=1,GAME_VIEW_EXAMINE=2 };
Byte game_display_command(const char *command);
void game_render_examine(Game *g,Byte *frame,const char *input,const char *message);
void game_tick(Game *g,Word ticks);
void game_timing_init(Game *g,GameTiming *timing);
void game_timing_advance(Game *g,GameTiming *timing,Word jiffies);
void game_timing_service(Game *g,GameTiming *timing);
/* Run one source TCB only when CLOCK has made it eligible.  This is the
 * COMPLR:HSLOW/BURNER foreground boundary, never interrupt-context work. */
void game_timing_service_task(Game *g,GameTiming *timing,Byte task);
void game_render(Game *g,Byte *frame,const char *input,const char *message);
/* ONCE.ASM/COMTXT text area: four 32-column rows at logical y=160..184.
 * This is presentation-only; it does not modify packed game state. */
void game_render_attract(Byte *frame,const char *row0,const char *row1,
                         const char *row2,const char *row3);
/* MISC.ASM:PREPAX clears TXTEXA then writes PREPARE! at character (12,9).
 * It is deliberately separate from the four-row primary-text renderer. */
void game_render_prepare(Byte *frame);
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
/* COMCRE.ASM:CREGEN. Executes its genuine matrix mutation and reports the
 * source RANDOM result through Game.seed; it does not create a CCB itself. */
void game_creature_regenerate(Game *g,CreatureScheduler *scheduler);
/* Consume emulated 60 Hz video ticks.  Q.TEN expiry only queues a CCB task;
 * each queued task is serviced once at the following foreground boundary.
 * Returns nonzero when the visible scene needs a redraw; same-cell attack
 * entry is reported, never executed. */
Byte game_creature_advance(Game *g,CreatureScheduler *scheduler,Word videoTicks);
/* Identical Q.TEN advancement with optional bounded foreground service after
 * each completed tenth-second step. The callback may update only UI status. */
Byte game_creature_advance_progress(Game *g,CreatureScheduler *scheduler,Word videoTicks,
                                    Byte (*progress)(void *),void *context,Byte *dirty);
/* One already-promoted Q.TEN task at a foreground SCHED boundary.  CLOCK
 * promotion remains caller/module-owned; this narrow primitive performs no
 * additional tick advancement or queue scan. */
Byte game_creature_service_one(Game *g,CreatureScheduler *scheduler,Byte index,Byte *dirty);
#endif
