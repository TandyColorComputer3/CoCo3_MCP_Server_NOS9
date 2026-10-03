#ifndef DAGGORATH_COMPAT_C4_H
#define DAGGORATH_COMPAT_C4_H
#include "compat.h"
#include "../gameplay/game.h"
/* ONCE:GAME20..GAME50 compatibility boundary.  The caller owns both the
 * original-layout $0200..$0f04 image and the packed semantic state. */
void dag_compat_c4_init(Game *game,Byte *state,Byte second);
/* Keep the original-layout compatibility image authoritative at every
 * connected C5 semantic checkpoint.  This copies only established packed
 * source fields; presentation scratch is owned by the viewer. */
void dag_compat_c4_sync(const Game *game,Byte *state);
void dag_compat_c4_map(const Game *game,Byte *frame);
void dag_compat_c4_initial(const Game *game,Byte *frame);
void dag_compat_c4_prompt(Byte *frame);
Byte dag_compat_c4_run(Game *game,Byte *state,Byte *frame);
#ifdef DAG_COMPAT_TEST
unsigned long dag_compat_c4_random_calls(void);
#endif
#endif
