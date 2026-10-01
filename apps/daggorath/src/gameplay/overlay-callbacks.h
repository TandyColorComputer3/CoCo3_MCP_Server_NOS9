#ifndef DOD_OVERLAY_CALLBACKS_H
#define DOD_OVERLAY_CALLBACKS_H

#include "overlay-api.h"

/* Caller-owned state survives each temporary F$Link of dodcmd. */
typedef struct { Word dataY; } DagOverlayCallbackContext;

void overlay_callback_init(DagOverlayCallbackContext *);
void overlay_health_gateway(void *,Game *);
Byte overlay_object_name_gateway(void *,Game *,Word,Byte *);
void overlay_render_status_gateway(void *,Game *,Byte *,Byte);

/* Gateway targets retain normal resident CMOC stack checks. */
void overlay_health_resident(void *,Game *);
Byte overlay_object_name_resident(void *,Game *,Word,Byte *);
void overlay_render_status_resident(void *,Game *,Byte *,Byte);

#endif
