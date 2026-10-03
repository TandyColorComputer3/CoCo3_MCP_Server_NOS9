        section code
        export  _dag_compat_render_wizard

* C3 PIC adaptation of MISC:WIZZES and the VCTLST/VECTOR leaf used by D4:WIZ1.
* The importer flattens only WIZ1's source V$JMP to WIZ0 into the ordered source
* segments below. No raster frames are stored or replayed. Context at 10,S:
* 256-byte-aligned original-layout work state, caller-owned 6144-byte frame,
* and the source fade value.
FLOP    equ $0b
VCTFAD  equ $2d
FADCNT  equ $2e
X0      equ $31
Y0      equ $2f
X1      equ $35
Y1      equ $33
XX      equ $37
YY      equ $3a
DX      equ $3d
DY      equ $40
LENGTH  equ $43
DELTAX  equ $45
DELTAY  equ $46
VDBAS   equ $47
VDEND   equ $49
T0      equ $c1
T1      equ $c2
T2      equ $c3

_dag_compat_render_wizard
        pshs    cc,dp,x,y,u
        ldx     10,s
        ldd     ,x
        tstb
        bne     wiz_bad
        tfr     a,dp
        ldu     2,x
        beq     wiz_bad
        stu     <FLOP
        ldb     4,x
        stb     <VCTFAD
        leax    wizard_segments,pcr
        ldy     #WIZARD_SEGMENT_COUNT
wiz_loop
        clra
        ldb     ,x+
        std     <X0
        clra
        ldb     ,x+
        std     <Y0
        clra
        ldb     ,x+
        std     <X1
        clra
        ldb     ,x+
        std     <Y1
        bsr     vector
        leay    -1,y
        bne     wiz_loop
        clrb
        puls    cc,dp,x,y,u,pc
wiz_bad
        ldb     #187
        puls    cc,dp,x,y,u,pc

negd    coma
        comb
        addd    #1
        rts

divide  pshs    a,b,x
        clr     ,s
        clr     1,s
        clr     <T0
        std     <T1
        beq     div99
        cmpd    2,s
        bne     div00
        inc     ,s
        bra     div99
div00   ldx     #16
div10   lsl     <T2
        rol     <T1
        rol     <T0
        lsl     1,s
        rol     ,s
        ldd     <T0
        subd    2,s
        bcs     div20
        std     <T0
        inc     1,s
div20   leax    -1,x
        bne     div10
div99   puls    a,b,x,pc

incre   pshs    a,b,x
        ldx     <LENGTH
        ldd     ,s
        bpl     inc10
        bsr     negd
        bsr     divide
        bsr     negd
        bra     inc90
inc10   bsr     divide
inc90   std     ,s
        puls    a,b,x,pc

vector  pshs    a,b,x,y,u
        inc     <VCTFAD
        lbeq    vec99
        lda     <VCTFAD
        sta     <FADCNT
        ldd     <X1
        subd    <X0
        std     <DX+1
        bpl     vec12
        bsr     negd
vec12   std     <LENGTH
        ldd     <Y1
        subd    <Y0
        std     <DY+1
        bpl     vec14
        bsr     negd
vec14   cmpd    <LENGTH
        blt     vec20
        std     <LENGTH
        lbeq    vec99
vec20   ldd     <DX+1
        bsr     incre
        std     <DX+1
        tfr     a,b
        sex
        ldb     #1
        sta     <DX
        bpl     vec22
        negb
vec22   stb     <DELTAX
        ldd     <DY+1
        bsr     incre
        std     <DY+1
        tfr     a,b
        sex
        ldb     #32
        sta     <DY
        bpl     vec24
        negb
vec24   stb     <DELTAY
        ldd     <X0
        std     <XX
        ldd     <Y0
        std     <YY
        lda     #$80
        sta     <XX+2
        sta     <YY+2
        ldx     <FLOP
        stx     <VDBAS
        leax    6144,x
        stx     <VDEND
        ldx     <VDBAS
        ldd     <YY
        bsr     lsld5
        leax    d,x
        ldd     <XX
        bsr     asrd3
        leax    d,x
        leau    bitmsk,pcr
        ldy     <LENGTH
vec30   dec     <FADCNT
        bne     vec40
        lda     <VCTFAD
        sta     <FADCNT
        tst     <XX
        bne     vec40
        cmpx    <VDBAS
        blo     vec40
        cmpx    <VDEND
        bhs     vec40
        ldb     <XX+1
        andb    #7
        lda     b,u
        ora     ,x
        sta     ,x
vec40   lda     <XX+1
        anda    #$f8
        sta     <T0
        ldd     <XX+1
        addd    <DX+1
        std     <XX+1
        ldb     <XX
        adcb    <DX
        stb     <XX
        anda    #$f8
        cmpa    <T0
        beq     vec50
        ldb     <DELTAX
        leax    b,x
vec50   ldd     <YY+1
        sta     <T0
        addd    <DY+1
        std     <YY+1
        ldb     <YY
        adcb    <DY
        stb     <YY
        cmpa    <T0
        beq     vec60
        ldb     <DELTAY
        leax    b,x
vec60   leay    -1,y
        bne     vec30
vec99   dec     <VCTFAD
        puls    a,b,x,y,u,pc

asrd3   asra
        rorb
        asra
        rorb
        asra
        rorb
        rts
lsld5   lslb
        rola
        lslb
        rola
        lslb
        rola
        lslb
        rola
        lslb
        rola
        rts

bitmsk  fcb $80,$40,$20,$10,$08,$04,$02,$01
        include "wizard-data.asm"
        endsection
