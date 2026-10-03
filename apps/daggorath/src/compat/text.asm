        section code
        export  _dag_compat_text_apply,_dag_compat_text_cell

* PIC adaptation of TXTSER.ASM:TXTCHR and COMTXT.ASM:
* TXTXXX/TXTBS/TXTCR/TXTSCR/TXTDPB. The original direct video base becomes
* the caller-owned 256x192 logical framebuffer in the context at 2,S.
* Context: frame word, cursor word, inverse byte, operation byte, value byte.
* DP is never changed and is explicitly preserved for future mapped use.

CT_FRAME equ 0
CT_CURSOR equ 2
CT_INVERSE equ 4
CT_OPERATION equ 5
CT_VALUE equ 6

OP_CLEAR_PRIMARY equ 0
OP_CLEAR_STATUS equ 1
OP_CLEAR_VIEWPORT equ 2
OP_CLEAR_SCREEN equ 3
OP_CHARACTER equ 4
OP_ERASE equ 5

I_CR equ $1f
I_BS equ $24

_dag_compat_text_apply
        pshs    cc,dp,x,y,u
        ldx     10,s
        ldb     CT_OPERATION,x
        cmpb    #OP_CHARACTER
        lbeq    text_character
        cmpb    #OP_ERASE
        lbeq    text_erase
        ldu     CT_FRAME,x
        lbeq    text_bad
        cmpb    #OP_CLEAR_PRIMARY
        beq     clear_primary
        cmpb    #OP_CLEAR_STATUS
        beq     clear_status
        cmpb    #OP_CLEAR_VIEWPORT
        beq     clear_viewport
        cmpb    #OP_CLEAR_SCREEN
        beq     clear_screen
        bra     text_bad

clear_primary
        ldd     #160*32
        leau    d,u
        ldy     #32*32
        clr     CT_CURSOR,x
        clr     CT_CURSOR+1,x
        bra     clear_region
clear_status
        ldd     #152*32
        leau    d,u
        ldy     #8*32
        clr     CT_CURSOR,x
        clr     CT_CURSOR+1,x
        bra     clear_region
clear_viewport
        ldy     #152*32
        bra     clear_region
clear_screen
        ldy     #192*32
clear_region
        lda     CT_INVERSE,x
clear_loop
        sta     ,u+
        leay    -1,y
        bne     clear_loop
        bra     text_ok

text_erase
        clr     CT_VALUE,x
text_character
        ldu     CT_FRAME,x
        beq     text_bad
        lda     CT_VALUE,x
        cmpa    #I_BS
        beq     text_backspace
        cmpa    #I_CR
        beq     text_cr
        cmpa    #$24
        bhs     text_bad
        lbsr    deposit
        ldd     CT_CURSOR,x
        addd    #1
        std     CT_CURSOR,x
        bra     scroll_test
text_backspace
        ldd     CT_CURSOR,x
        subd    #1
        cmpd    #$ffff
        bne     store_cursor
        ldd     #127
store_cursor
        std     CT_CURSOR,x
        bra     text_ok
text_cr
        ldd     CT_CURSOR,x
        addd    #32
        andb    #$e0
        std     CT_CURSOR,x
scroll_test
        cmpd    #128
        blo     text_ok
        lbsr    scroll_primary
        ldd     #96
        std     CT_CURSOR,x
text_ok
        clrb
        bra     text_return
text_bad
        ldb     #187
text_return
        clra
        puls    cc,dp,x,y,u,pc

* C3 regional cell adapter. It deliberately shares C2's exact glyph table and
* source-order seven-byte deposit, while allowing ONCE:PSTS and MISC:PREPAX to
* select their original non-primary text regions. Context: frame, cell,
* inverse, value. This is presentation state only; no new text semantics.
_dag_compat_text_cell
        pshs    cc,dp,x,y,u
        ldx     10,s
        ldu     ,x
        ldd     2,x
        cmpd    #32*24
        bhs     text_bad
        pshs    d
        ldb     5,x
        cmpb    #$24
        bhs     text_bad_pop
        lda     #7
        mul
        leay    compat_glyphs,pcr
        leay    d,y
        puls    d
        pshs    b
        lsra
        rorb
        lsra
        rorb
        lsra
        rorb
        lsra
        rorb
        lsra
        rorb
        tfr     b,a
        clrb
        leau    d,u
        puls    b
        andb    #31
        leau    b,u
        ldb     #7
cell_loop
        lda     ,y+
        eora    4,x
        sta     ,u
        leau    32,u
        decb
        bne     cell_loop
        clrb
        clra
        puls    cc,dp,x,y,u,pc
text_bad_pop
        leas    2,s
        ldb     #187
        clra
        puls    cc,dp,x,y,u,pc

* TXTDPB: select the source glyph, then write its seven scanlines in source
* order. Glyphs are build-time expansions of the exact SWCTAB/SPCTAB data.
deposit
        pshs    a,b,x,y,u
        ldb     #7
        mul
        leay    compat_glyphs,pcr
        leay    d,y
        ldb     CT_CURSOR+1,x
        pshs    b
        lsrb
        lsrb
        lsrb
        lsrb
        lsrb
        tfr     b,a
        clrb
        addd    #160*32
        addd    CT_FRAME,x
        tfr     d,u
        puls    b
        andb    #31
        leau    b,u
        ldb     #7
deposit_loop
        lda     ,y+
        eora    CT_INVERSE,x
        sta     ,u
        leau    32,u
        decb
        bne     deposit_loop
        puls    a,b,x,y,u,pc

* TXTSCR: rows 1..3 move upward, then row 3 is cleared using inverse mode.
scroll_primary
        pshs    a,b,x,y,u
        lda     CT_INVERSE,x
        pshs    a
        ldu     CT_FRAME,x
        ldd     #160*32
        leau    d,u
        leax    256,u
        ldy     #384
scroll_copy
        ldd     ,x++
        std     ,u++
        leay    -1,y
        bne     scroll_copy
        puls    a
        ldy     #256
scroll_clear
        sta     ,u+
        leay    -1,y
        bne     scroll_clear
        puls    a,b,x,y,u,pc

        include "text-glyphs.asm"
        endsection
