/* Resident half of the Daggorath command-overlay ABI v1.  F$NMLoad is the
 * Level II non-mapping form of the retained-load lifecycle: it gives dodcmd a
 * system link without consuming the one DAT slot reserved for a later F$Link.
 * See upstream ioman.asm:FNMLoad and funload.asm, cited in the architecture
 * report. */
#include "overlay-api.h"

typedef struct { Word header,entry;Byte retained,disabled; } OverlayLink;
extern Byte overlay_preload(void);
extern Byte overlay_release(void);
extern Byte overlay_link(OverlayLink *);
extern Byte overlay_call(OverlayLink *,DagOverlayContextV1 *);
extern Byte overlay_unlink(OverlayLink *);

static OverlayLink linkState;
static DagOverlayServices services={DOD_OVERLAY_ABI_V1,sizeof(DagOverlayServices),
 game_health,game_object_name,game_render_status};

Byte game_overlay_open(void){Byte e;
 if(linkState.retained||linkState.disabled)return linkState.disabled?221:0;
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
static Byte invoke(DagOverlayContextV1 *context){Byte e,u;
 if(!linkState.retained)return 221;
 e=overlay_link(&linkState);if(e)return e;
 e=overlay_call(&linkState,context);
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
 e=invoke(&context);if(e)return e;
 *result=context.result;*view=context.view;*message=context.outputMessage;return 0;
}
Byte game_overlay_examine(Game *game,Byte *frame,const char *input,const char *message){
 DagOverlayContextV1 context;Byte *p=(Byte *)&context;Byte i;
 for(i=0;i<sizeof(context);i++)p[i]=0;
 context.abiVersion=DOD_OVERLAY_ABI_V1;context.contextSize=sizeof(context);
 context.game=game;context.services=&services;context.frame=frame;
 context.input=input;context.message=message;context.operation=DOD_OVERLAY_EXAMINE;
 return invoke(&context);
}
