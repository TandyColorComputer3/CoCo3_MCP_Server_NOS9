#ifndef DAGGORATH_COMPAT_C5_H
#define DAGGORATH_COMPAT_C5_H
#include "c4.h"
#include "../gameplay/overlay-api.h"
#include "../gameplay/scheduler-api.h"
#include "present.h"

enum { DAG_COMPAT_C5_AUTOPLAY=1,DAG_COMPAT_C5_HUMAN=2 };

typedef struct {
 Byte (*command)(Game *,const char *,GameCombat *,Byte *,Byte *);
 Byte (*scheduler)(DagSchedulerContextV1 *,Byte,Word);
 Byte (*word)(Game *,Byte *,const char *,Byte);
#ifdef DAG_COMPAT_C6
 Byte (*progress)(void *);
 void *progressContext;
#endif
} DagCompatC5Services;

typedef struct {
 Game *game;
 Byte *originalState;
 Byte *logicalFrame;
 DagCompatTextContext text;
 DagSchedulerContextV1 *scheduler;
 const DagCompatC5Services *services;
 char line[33];
 Byte length,words,origin,commandCount,lastResult,lastView;
} DagCompatC5;

void dag_compat_c5_init(DagCompatC5 *,Game *,Byte *,Byte *,DagSchedulerContextV1 *,
                        const DagCompatC5Services *);
Byte dag_compat_c5_character(DagCompatC5 *,Byte character,Byte origin);
Byte dag_compat_c5_autoplay(DagCompatC5 *,Byte commandIndex);
void dag_compat_c5_prepare_view(const Game *,Byte *state,Byte range);
#ifdef DAG_COMPAT_C6
void dag_compat_c5_prepare_view_synced(const Game *,Byte *state,Byte range);
#endif
Byte dag_compat_c5_type5_at(const Game *,Byte range,Byte relativeDirection);

#endif
