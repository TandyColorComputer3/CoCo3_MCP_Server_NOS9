#include "fileio.h"
/* NitrOS-9 upstream level1/cmds/copy.asm Open/Create/Read/Close, defs/os9.d.
 * Create A=WRITE(2), B=owner READ|WRITE(3); Open A=READ(1). No execute/public
 * write flags, replacement or delete. Preserve CMOC Y/U calling convention. */
Byte file_open(const char *path, Byte create, Byte *fd)
{
    Byte error, number;
    asm {
        ldx :path
        tst :create
        beq @read
        lda #2
        ldb #3
        os9 $83
        bra @done
@read
        lda #1
        os9 $84
@done
        bcs @failed
        sta :number
        clrb
@failed
        stb :error
    }
    if (!error) *fd=number;
    return error;
}
Byte file_read(Byte fd, void *buffer, Word size, Word *got)
{
    Byte error;
    Word count;
    asm {
        lda :fd
        ldx :buffer
        pshs y
        ldy :size
        os9 $89
        bcs @failed
        clrb
@failed
        sty :count
        puls y
        stb :error
    }
    *got=error ? 0 : count;
    return error;
}
Byte file_close(Byte fd)
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
