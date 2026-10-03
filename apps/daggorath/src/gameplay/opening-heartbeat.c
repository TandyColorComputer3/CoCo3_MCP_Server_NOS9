/* The opening is one public command. The restored EOU shell has no /dhb,
 * so load the existing driver/descriptor pack before opening that path.
 * Installed EOU accepts the concatenated pack through F$Load; loading its
 * members separately left I$Open failing with E$RAMFull (237).
 * See level1/cmds/load.asm and level2/modules/kernel/funload.asm. */
#include <cmoc.h>
#include "opening-heartbeat.h"

static Byte packLoaded;

static Byte unload_driver(void)
{
    Byte error;
    char name[]={'D','H','e','a','r','t','b','e','a',0xf4,0};
    asm {
        pshs y,u
        lda #$e1
        leax :name
        os9 $1d
        bcs @failed
        clrb
@failed
        puls u,y
        stb :error
    }
    return error;
}

Byte opening_heartbeat_modules_open(void)
{
    Byte error,type=0;
    char path[]="/d1/dhbpack";
    if(packLoaded)return ERR_ARGUMENT;
    asm {
        pshs y,u
        leax :path
        os9 $01
        puls u,y
        bcs @failed
        sta :type
        clrb
@failed
        stb :error
    }
    if(error)return error;
    packLoaded=1;
    return type==0xe1?0:ERR_ARGUMENT;
}

Byte opening_heartbeat_modules_close(void)
{
    Byte error;
    if(!packLoaded)return 0;
    error=unload_driver();
    /* After the last I$Close, installed EOU may already have removed the
     * packed driver. A missing module then means no retained reference. */
    if(error==216)error=0;
    if(!error)packLoaded=0;
    return error;
}
