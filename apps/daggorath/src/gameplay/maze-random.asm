        section code
        export _game_random

* RANDOM.ASM:RANDOX, adapted to Game.seed at offset 2576. The original's
* feedback mask, eight parity shifts and 24-bit rotate sequence are retained.
* Default CMOC ABI: Game * at 2,S, Byte return in B. This leaf preserves
* X/Y/U (including resident CMOC Y), uses only caller-owned process data,
* and is position-independent/reentrant on both 6809 and 6309.
_game_random
        pshs    x,y,u
        ldu     8,s
        leau    2576,u
        ldx     #8
rnd_outer
        clrb
        ldy     #8
        lda     2,u
        anda    #$e1
rnd_bits
        lsla
        bcc     rnd_zero
        incb
rnd_zero
        leay    -1,y
        bne     rnd_bits
        lsrb
        rol     ,u
        rol     1,u
        rol     2,u
        leax    -1,x
        bne     rnd_outer
        ldb     ,u
        puls    x,y,u,pc
        endsection
