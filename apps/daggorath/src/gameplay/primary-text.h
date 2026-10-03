#ifndef DOD_PRIMARY_TEXT_H
#define DOD_PRIMARY_TEXT_H
#include "platform.h"
/* COMDAT:TXTPRI is 32 columns by four rows. Codes are source SWCHAR codes,
 * separate from the packed Game/overlay ABI and from the bitmap scanout. */
typedef struct { Byte cells[128];Word cursor; } DagPrimaryText;
void primary_clear(DagPrimaryText *text);
void primary_character(DagPrimaryText *text,Byte code);
void primary_write(DagPrimaryText *text,const char *ascii);
void primary_prompt(DagPrimaryText *text);
void primary_render(const DagPrimaryText *text,Byte *frame);
Byte primary_render_progress(const DagPrimaryText *text,Byte *frame,
                            Byte (*progress)(void *),void *context);
#endif
