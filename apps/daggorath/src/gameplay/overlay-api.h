#ifndef DOD_OVERLAY_API_H
#define DOD_OVERLAY_API_H

#include "game.h"

/* Daggorath Level II command overlay ABI v1.  The assembly boundary is the
 * one live-proven in DAGGORATH_LEVEL2_MODULARIZATION.md: A=1, B=operation,
 * X=context.  The C structures below are only the extensible payload behind
 * X; neither side keeps one after F$UnLink. */
#define DOD_OVERLAY_ABI_V1 1
#define DOD_OVERLAY_COMMAND 1
#define DOD_OVERLAY_EXAMINE 2

typedef struct {
 Byte version;
 Byte size;
 void (*health)(Game *);
 Byte (*object_name)(Game *,Word,Byte *);
 void (*render_status)(Game *,Byte *,Byte);
} DagOverlayServices;

typedef struct {
 Byte abiVersion;
 Byte contextSize;
 Game *game;
 const DagOverlayServices *services;
 const char *command;
 GameCombat *combat;
 Byte *frame;
 const char *input;
 const char *message;
 Byte operation;
 Byte result;
 Byte view;
 const char *outputMessage;
} DagOverlayContextV1;

/* These host calls retain no overlay mapping between invocations. */
Byte game_overlay_open(void);
Byte game_overlay_close(void);
Byte game_overlay_command(Game *game,const char *command,GameCombat *combat,
                          Byte *result,Byte *view,const char **message);
Byte game_overlay_examine(Game *game,Byte *frame,const char *input,
                          const char *message);

#endif
