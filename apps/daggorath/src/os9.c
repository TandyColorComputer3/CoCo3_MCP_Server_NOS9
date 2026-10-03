/* Reused from apps/window-manager/src/os9.c; same verified CMOC/OS-9 ABI. */
#include "platform.h"
/* CMOC 0.1.90 manual: "Making OS-9 system calls" and calling convention.
 * Y is the process data base; U is the C frame pointer. Restore both.
 * Public syscall operands verified against defs/os9.d; the target-specific
 * read-only clock layout exception is documented below. */
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

/* Installed EOU F$Icpt passes the registered data pointer in U. Keep the
 * existing cancellation byte at offset zero; completion signal 128 only
 * increments offset one. No C, Y-data access, pipe I/O or graphics in the
 * signal handler. The producer never clears the notice counter. */
asm void audio_signal_handler(void)
{
    asm {
        cmpb #128
        beq @notice
        cmpb #2
        beq @cancel
        cmpb #3
        bne @done
@cancel
        stb ,u
        bra @done
@notice
        inc 1,u
@done
        rti
    }
}

Byte os_intercept_audio(Byte *cancelAndNotice)
{
    Byte error;
    void (*handler)(void) = audio_signal_handler;
    asm {
        ldx :handler
        pshs u
        ldu :cancelAndNotice
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

/* CoWin SS.MpGPB ($84), level2/coco3/modules/cowin.asm L0BD1:
 * X=group/buffer, Y=1 map / 0 unmap; returns X=data address, Y=length. */
Byte os_setstat(Byte path, Byte function, Registers *r)
{
    Byte error;
    asm {
        pshs y,u
        lda :path
        ldb :function
        ldx :r
        ldy 4,x
        ldx 2,x
        os9 $8e
        bcs @failed
        ldu :r
        stx 2,u
        sty 4,u
        clrb
@failed
        puls u,y
        stb :error
    }
    return error;
}
/* Target-specific READ-ONLY clock adapter. EOU defs/os9.d Level II D.Time=$28,
 * D.Tick=$2e; Clock decrements/reloads 60 at vertical IRQ. Krn initializes its
 * first DAT block to physical block 0. fcpymem.asm implementation takes D as
 * a pointer to eight DAT words (its old header incorrectly says block number).
 * No mapped kernel pointer or writes. Validate against public F$Time first.
 * This is NOT a portable public high-resolution timer ABI. */
static const Word clockMap[8]={0,0,0,0,0,0,0,0};
Byte os_clock(Word *ticks, Byte validate)
{
    Byte packet[7],publicTime[6],error,i;
    const Word *map=clockMap;
    asm {
        pshs y,u
        ldd :map
        ldx #$28
        ldy #7
        leau :packet
        os9 $1b
        bcs @failed
        clrb
@failed
        puls u,y
        stb :error
    }
    if(error)return error;
    if(packet[5]>59||packet[6]<1||packet[6]>60)return ERR_ARGUMENT;
    if(validate){
        asm {
            leax :publicTime
            os9 $15
            bcs @failed
            clrb
@failed
            stb :error
        }
        if(error)return error;
        for(i=0;i<6;i++)if(packet[i]!=publicTime[i])return ERR_ARGUMENT;
    }
    *ticks=(Word)packet[5]*60+60-packet[6];return 0;
}

Byte os_map_buffer(Byte path, Word groupBuffer, Byte map, Byte **buffer, Word *length)
{
    Registers r;Byte e;
    if(map){
        r.x=groupBuffer;r.y=1;e=os_setstat(path,0x84,&r);if(e)return e;
        *buffer=(Byte *)r.x;*length=r.y;return 0;
    }else{
        /* EOU SS.MpGPB unmap returned E$BPAddr for this two-block buffer.
         * Public F$ClrBlk unmaps exactly the range returned by SS.MpGPB.
         * Provenance: kernel/fclrblk.asm, B=count, U=8K-aligned address.
         * It removes mappings, NOT underlying GP storage (KillBuff owns that). */
        Word address=(Word)*buffer;
        Word base=address&0xe000;
        Byte blocks=((address&0x1fff)+*length+8191)/8192;
        asm {
            pshs y,u
            ldb :blocks
            ldu :base
            os9 $50
            bcs @failed
            clrb
@failed
            puls u,y
            stb :e
        }
        return e;
    }
}
