#include "platform.h"
/* I$Open/I$Close: upstream defs/os9.d and level2/cmds/wcreate.asm.
 * /w allocates a new window; never repurpose Term or a numbered foreign window. */
Byte window_open(Byte *fd)
{
    const char *name="/w"; Byte error, number;
    asm {
        ldx :name
        lda #3
        os9 $84
        bcs @failed
        sta :number
        clrb
@failed
        stb :error
    }
    if (!error) *fd=number;
    return error;
}
Byte window_close(Byte fd)
{
    Byte error;
    asm {
        lda :fd
        os9 $8f
        bcs @failed
        clrb
@failed
        stb :error
    }
    return error;
}
