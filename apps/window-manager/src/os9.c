#include "platform.h"
/* CMOC 0.1.90 manual: "Making OS-9 system calls" and calling convention.
 * Y is the process data base; U is the C frame pointer. Restore both.
 * Numeric OS9 operands verified against defs/os9.d; no private OS offsets. */
Byte os_getstat(Byte path, Byte function, Registers *r)
{
    Byte error;
    asm {
        pshs y,u
        lda :path
        ldb :function
        ldx :r
        ldx 2,x
        os9 $8d
        bcs @failed
        ldu :r
        sta ,u
        stb 1,u
        stx 2,u
        sty 4,u
        clrb
@failed
        puls u,y
        stb :error
    }
    return error;
}

Byte os_write(Byte path, const void *buffer, Word size)
{
    Byte error;
    Word written;
    asm {
        lda :path
        ldx :buffer
        pshs y
        ldy :size
        os9 $8a
        bcs @failed
        clrb
@failed
        sty :written
        puls y
        stb :error
    }
    if (error) return error;
    return written == size ? 0 : ERR_WRITE;
}

/* F$Icpt gives the handler B=signal, U=registered data pointer.
 * EOU KERNEL/ficpt.asm and GSHELL SAVESGNL demonstrate flag + RTI.
 * No C prologue, global access, I/O or nested cleanup inside the handler. */
asm void signal_handler(void)
{
    asm {
        cmpb #2
        beq @cancel
        cmpb #3
        bne @done
@cancel
        stb ,u
@done
        rti
    }
}

Byte os_intercept(Byte *signal)
{
    Byte error;
    void (*handler)(void) = signal_handler;
    asm {
        ldx :handler
        pshs u
        ldu :signal
        os9 $09
        bcs @failed
        clrb
@failed
        puls u
        stb :error
    }
    return error;
}

Byte os_sleep(Word ticks)
{
    Byte error;
    /* kernel/fsleep.asm: X=1 only yields; timed sleep subtracts one before
     * queuing. This wrapper requests a positive timed polling interval. */
    if (!ticks || ticks == 65535) return ERR_ARGUMENT;
    ++ticks;
    asm {
        ldx :ticks
        os9 $0a
        bcs @failed
        clrb
@failed
        stb :error
    }
    return error;
}

Byte os_cancel_self(void)
{
    Byte error;
    asm {
        pshs y
        os9 $0c
        bcs @failed
        ldb #3
        os9 $08
        bcs @failed
        clrb
@failed
        puls y
        stb :error
    }
    return error;
}

/* CMOC 0.1.90 does not implement volatile. Explicit memory load on every poll. */
Byte os_signal_value(Byte *signal)
{
    Byte value;
    asm {
        ldx :signal
        ldb ,x
        stb :value
    }
    return value;
}

Byte os_devname(Byte path, char *buffer)
{
    Registers r;
    r.x=(Word)buffer;
    return os_getstat(path, SS_DEVNM, &r);
}
