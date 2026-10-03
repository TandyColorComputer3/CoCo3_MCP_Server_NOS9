/* Resident half of the Daggorath command-overlay ABI v1.  F$NMLoad is the
 * Level II non-mapping form of the retained-load lifecycle: it gives dodcmd a
 * system link without consuming the one DAT slot reserved for a later F$Link.
 * See upstream ioman.asm:FNMLoad and funload.asm, cited in the architecture
 * report. */
#include "overlay-api.h"
#include "overlay-callbacks.h"

/* Host lifecycle tests exercise retained/link/unlink ordering without an
 * OS-9 CMOC Y register.  The semantic overlay tests provide direct service
 * implementations; production CMOC builds use the assembly gateways. */
#ifndef _CMOC_VERSION_
void overlay_callback_init(DagOverlayCallbackContext *context){context->dataY=0;}
void overlay_health_gateway(void *opaque,Game *game){(void)opaque;(void)game;}
Byte overlay_object_name_gateway(void *opaque,Game *game,Word token,Byte *name){
 (void)opaque;(void)game;(void)token;(void)name;return 0;
}
void overlay_render_status_gateway(void *opaque,Game *game,Byte *frame,Byte phase){
 (void)opaque;(void)game;(void)frame;(void)phase;
}
Byte overlay_progress_gateway(void *opaque){(void)opaque;return 0;}
#endif

typedef struct { Word header,entry;Byte retained,disabled; } OverlayLink;
extern Byte overlay_preload(void);
extern Byte overlay_release(void);
extern Byte overlay_link(OverlayLink *);
extern Byte overlay_call(OverlayLink *,DagOverlayContextV1 *);
extern Byte overlay_unlink(OverlayLink *);

static OverlayLink linkState;
static DagOverlayCallbackContext callbackContext;
/* dodcmd's result text resides in its temporary mapping.  Keep the completed
 * command result in resident storage before F$UnLink releases that mapping. */
static char commandMessage[32];
static DagOverlayServices services={DOD_OVERLAY_ABI_V1,sizeof(DagOverlayServices),
 &callbackContext,overlay_health_gateway,overlay_object_name_gateway,
 overlay_render_status_gateway};

void overlay_health_resident(void *opaque,Game *game){(void)opaque;game_health(game);}
Byte overlay_object_name_resident(void *opaque,Game *game,Word token,Byte *name){
 (void)opaque;return game_object_name(game,token,name);
}
void overlay_render_status_resident(void *opaque,Game *game,Byte *frame,Byte phase){
 (void)opaque;game_render_status(game,frame,phase);
}
#ifdef _CMOC_VERSION_
extern Byte gameplay_overlay_progress(void);
Byte overlay_progress_resident(void *opaque){(void)opaque;return gameplay_overlay_progress();}
#else
Byte overlay_progress_resident(void *opaque){(void)opaque;return 0;}
#endif

Byte game_overlay_open(void){Byte e;
 if(linkState.retained||linkState.disabled)return linkState.disabled?221:0;
 overlay_callback_init(&callbackContext);
 e=overlay_preload();
 if(e){linkState.disabled=1;return e;}
 linkState.retained=1;return 0;
}
Byte game_overlay_close(void){Byte e;
 if(!linkState.retained)return 0;
 e=overlay_release();
 if(!e)linkState.retained=0;
 return e;
}
static void copy_message(const char *source){Byte i;
 if(!source){commandMessage[0]=0;return;}
 for(i=0;i<sizeof(commandMessage)-1&&source[i];i++)commandMessage[i]=source[i];
 commandMessage[i]=0;
}
static Byte invoke(DagOverlayContextV1 *context,Byte retainMessage){Byte e,u;
 if(!linkState.retained)return 221;
 e=overlay_link(&linkState);if(e)return e;
 e=overlay_call(&linkState,context);
 /* The overlay owns outputMessage. Copy it while the mapped module remains
  * valid; callers must never retain pointers into an F$Link mapping. */
 if(!e&&retainMessage)copy_message(context->outputMessage);
 u=overlay_unlink(&linkState);
 /* An unlink error means the mapping lifetime is uncertain; disable future
 * commands rather than pretending the eighth slot was released. */
 if(u){linkState.disabled=1;if(!e)e=u;}
 return e;
}
Byte game_overlay_command(Game *game,const char *command,GameCombat *combat,
                          Byte *result,Byte *view,const char **message){
 DagOverlayContextV1 context;Byte *p=(Byte *)&context;Byte i,e;
 for(i=0;i<sizeof(context);i++)p[i]=0;
 context.abiVersion=DOD_OVERLAY_ABI_V1;context.contextSize=sizeof(context);
 context.game=game;context.services=&services;context.command=command;
 context.combat=combat;context.operation=DOD_OVERLAY_COMMAND;
 e=invoke(&context,1);if(e)return e;
 *result=context.result;*view=context.view;
 *message=commandMessage;return 0;
}
Byte game_overlay_examine(Game *game,Byte *frame,const char *input,const char *message){
 DagOverlayContextV1 context;Byte *p=(Byte *)&context;Byte i;
 for(i=0;i<sizeof(context);i++)p[i]=0;
 context.abiVersion=DOD_OVERLAY_ABI_V1;context.contextSize=sizeof(context);
 context.game=game;context.services=&services;context.frame=frame;
 context.input=input;context.message=message;context.operation=DOD_OVERLAY_EXAMINE;
 context.progress=overlay_progress_gateway;context.progressContext=&callbackContext;
 return invoke(&context,0);
}
