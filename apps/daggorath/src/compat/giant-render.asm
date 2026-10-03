        section code
        export  _dag_compat_render_giant

* Disposable Architecture-B P1 slice. Original provenance:
* VIEWER.ASM SETSCL/PDRAW/CMRDRW/VIEW30; COMCRE.ASM CFIND;
* VCTLST.ASM SETFAX/VCTLSX; VECTOR.ASM VECTOR/DIVIDE/INCRE;
* DTABAS.ASM FWDCRE; VARC.ASM RPEEK; D3.ASM SGINT2/SGIANT.
* Production C1 compatibility slice, derived from the cartridge source named
* above and proven against the immutable beat 019..021 cartridge captures.
* Context at 2,S: state base (256-byte aligned), framebuffer, mode.
* mode 0 = VIEW30 forward creature, mode 1 = right-side PDRAW.

* Original $0200 direct-page low-byte offsets retained.
VCNTRX  equ $05
VCNTRY  equ $07
FLOP    equ $0b
VDGINV  equ $2c
VCTFAD  equ $2d
FADCNT  equ $2e
Y0      equ $2f
X0      equ $31
Y1      equ $33
X1      equ $35
XX      equ $37
YY      equ $3a
DX      equ $3d
DY      equ $40
LENGTH  equ $43
DELTAX  equ $45
DELTAY  equ $46
VDBAS   equ $47
VDEND   equ $49
VXSCAL  equ $4f
VYSCAL  equ $50
DRWFLG  equ $51
TX      equ $52
TY      equ $54
RLIGHT  equ $6e
MLIGHT  equ $6f
HLFSTP  equ $73
BAKSTP  equ $74
MAGFLG  equ $75
DROW    equ $7c
DIR     equ $8a
RANGE   equ $8b
PDIR    equ $23
T0      equ $c1
T1      equ $c2
T2      equ $c3

* Original non-DP state offsets relative to original $0200 state image.
CCBLND_OFF equ $03d4-$0200
CCBEND_OFF equ $05f4-$0200
NEIBOR_OFF equ $09f4-$0200
CC_LEN   equ 17
CC_MGO   equ 2
CC_USE   equ 12
CC_TYP   equ 13
CC_ROW   equ 15

V_RTS equ $fa
V_JSR equ $fb
V_REL equ $fc
V_JMP equ $fd
V_END equ $fe
V_NEW equ $ff

_dag_compat_render_giant
        pshs    cc,dp,x,y,u
        ldx     10,s
        ldd     ,x
        tstb
        bne     pic_bad_align
        tfr     a,dp
        ldu     2,x
        stu     <FLOP
        ldb     4,x
        beq     viewer_front
        cmpb    #1
        bne     pic_bad
viewer_right
        lbsr    setscl
        tfr     dp,a
        clrb
        addd    #NEIBOR_OFF
        tfr     d,u
        ldb     #1
        leax    rpeek,pcr
        lbsr    pdraw
        bra     pic_ok
viewer_front
        lbsr    setscl
        ldd     <DROW
        lbsr    cfind
        beq     pic_ok
        tfr     x,y
        ldb     CC_TYP,y
        cmpb    #12
        bhs     pic_bad
        lslb
        leax    fwdcre,pcr
        ldd     b,x
        beq     pic_bad
        leax    d,x
        lbsr    cmrdrw
pic_ok  clrb
        puls    cc,dp,x,y,u,pc
pic_bad_align
pic_bad ldb     #187
        puls    cc,dp,x,y,u,pc

* VIEWER.ASM:SETSCL, with the three tables made PCR-relative.
setscl pshs    a,x
        leax    hlfscl,pcr
        tst     <HLFSTP
        bne     sscl10
        leax    bakscl,pcr
        tst     <BAKSTP
        bne     sscl10
        leax    norscl,pcr
sscl10  lda     <RANGE
        lda     a,x
        sta     <VXSCAL
        sta     <VYSCAL
        puls    a,x,pc

* VIEWER.ASM:PDRAW. MAP32's unused address result is omitted after STEP;
* position arithmetic and CFIND ordering are unchanged.
pdraw   tfr     x,y
        tst     b,u
        bne     pic_rts
        addb    <PDIR
        stb     <DIR
        ldd     <DROW
        lbsr    step
        lbsr    cfind
        beq     pic_rts
        exg     x,y
        bra     cmrdrw
pic_rts rts

* CRETUR.ASM:STEP position arithmetic; MAP32 is not needed by CFIND.
step    pshs    a,b
        ldb     <DIR
        andb    #3
        lslb
        leax    stptab,pcr
        ldd     b,x
        adda    ,s+
        addb    ,s+
        rts
stptab  fcb     $ff,$00,$00,$01,$01,$00,$00,$ff

* COMCRE.ASM:CFIND over the original-layout caller-owned CCB table.
cfind   pshs    u
        pshs    d
        tfr     dp,a
        clrb
        addd    #CCBEND_OFF
        pshs    d
        tfr     dp,a
        clrb
        addd    #CCBLND_OFF-CC_LEN
        tfr     d,x
cfnd10  leax    CC_LEN,x
        cmpx    ,s
        beq     cfnd99
        ldd     2,s
        cmpd    CC_ROW,x
        bne     cfnd10
        tst     CC_USE,x
        beq     cfnd10
cfnd99  leas    2,s
        puls    d,u,pc

* VIEWER.ASM:CMRDRW/DRAWIT; FLOP now directly names the private 6144-byte
* logical framebuffer rather than a cartridge VDB descriptor.
cmrdrw  tst     CC_MGO,y
        beq     drawit
        dec     <MAGFLG
drawit  pshs    u
        pshs    x
        lbsr    setfad
        puls    x
        ldu     <FLOP
        lbsr    vctlst
        puls    u,pc

* VCTLST.ASM:SETFAX.
setfad  lda     <RLIGHT
        tst     <MAGFLG
        beq     sfad10
        lda     <MLIGHT
        clr     <MAGFLG
sfad10  clrb
        suba    #7
        suba    <RANGE
        bge     sfad30
        decb
        cmpa    #-7
        ble     sfad30
        leax    bitmsk+8,pcr
        ldb     a,x
sfad30  stb     <VCTFAD
        rts

* VCTLST.ASM relevant command closure (absolute, relative, NEW, END).
* JSR/JMP/RTS vector-list opcodes are excluded: neither captured list uses one.
vctlst  clr     <DRWFLG
        lda     <VCTFAD
        inca
        beq     vctend
vctl10  ldb     ,x
        cmpb    #V_REL
        beq     vctrel_enter
        cmpb    #V_NEW
        beq     vctnew_code
        cmpb    #V_END
        beq     vctend_code
        cmpb    #V_RTS
        bhs     vctend_error
vctabs  tst     <DRWFLG
        bne     vabs00
        bsr     vabs20
        dec     <DRWFLG
        bra     vctl10
vabs00  bsr     vabs10
        lbsr    vector
        bra     vctl10
vabs10  bsr     newold
vabs20  ldb     ,x+
        stb     <TY
        bsr     ascaly
        addd    <VCNTRY
        std     <Y1
        ldb     ,x+
        stb     <TX
        bsr     ascalx
        addd    <VCNTRX
        std     <X1
vctend  rts
vctnew_code
        leax    1,x
        clr     <DRWFLG
        bra     vctl10
vctend_code
        leax    1,x
        rts
vctend_error
        rts
vctrel_enter
        leax    1,x
vctrel lda     ,x+
        beq     vctnew_rel
        bsr     newold
        ldb     -1,x
        asrb
        asrb
        asrb
        asrb
        lslb
        addb    <TY
        stb     <TY
        bsr     ascaly
        addd    <VCNTRY
        std     <Y1
        ldb     -1,x
        andb    #$0f
        bitb    #8
        beq     vrel20
        orb     #$f0
vrel20  lslb
        addb    <TX
        stb     <TX
        bsr     ascalx
        addd    <VCNTRX
        std     <X1
        lbsr    vector
        bra     vctrel
vctnew_rel
        clr     <DRWFLG
        bra     vctl10
newold ldd     <Y1
        std     <Y0
        ldd     <X1
        std     <X0
        rts
ascaly lda     <VYSCAL
        subb    <VCNTRY+1
        bra     asca10
ascalx lda     <VXSCAL
        subb    <VCNTRX+1
asca10 bcs     scal10
        mul
        bra     scal20
scal10 negb
        mul
        bsr     negd
scal20 lbra    asrd7

* VECTOR.ASM exact DDA arithmetic, with VDB descriptor lookup replaced by the
* caller-owned direct frame pointer and PCR bit-mask table.
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
        bpl     incr10
        bsr     negd
        bsr     divide
        bsr     negd
        bra     incr90
incr10  bsr     divide
incr90  std     ,s
        puls    a,b,x,pc

vector  pshs    a,b,x,y,u
        inc     <VCTFAD
        lbeq    vect99
        lda     <VCTFAD
        sta     <FADCNT
        ldd     <X1
        subd    <X0
        std     <DX+1
        bpl     vect12
        bsr     negd
vect12  std     <LENGTH
        ldd     <Y1
        subd    <Y0
        std     <DY+1
        bpl     vect14
        bsr     negd
vect14  cmpd    <LENGTH
        blt     vect20
        std     <LENGTH
        lbeq    vect99
vect20  ldd     <DX+1
        bsr     incre
        std     <DX+1
        tfr     a,b
        sex
        ldb     #1
        sta     <DX
        bpl     vect22
        negb
vect22  stb     <DELTAX
        ldd     <DY+1
        bsr     incre
        std     <DY+1
        tfr     a,b
        sex
        ldb     #32
        sta     <DY
        bpl     vect24
        negb
vect24  stb     <DELTAY
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
        lbsr    lsld5
        leax    d,x
        ldd     <XX
        lbsr    asrd3
        leax    d,x
        leau    bitmsk,pcr
        ldy     <LENGTH
vect30  dec     <FADCNT
        bne     vect40
        lda     <VCTFAD
        sta     <FADCNT
        tst     <XX
        bne     vect40
        cmpx    <VDBAS
        blo     vect40
        cmpx    <VDEND
        bhs     vect40
        ldb     <XX+1
        andb    #7
        lda     b,u
        tst     <VDGINV
        beq     vect32
        coma
        anda    ,x
        bra     vect33
vect32  ora     ,x
vect33  sta     ,x
vect40  lda     <XX+1
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
        beq     vect50
        ldb     <DELTAX
        leax    b,x
vect50  ldd     <YY+1
        sta     <T0
        addd    <DY+1
        std     <YY+1
        ldb     <YY
        adcb    <DY
        stb     <YY
        cmpa    <T0
        beq     vect60
        ldb     <DELTAY
        leax    b,x
vect60  leay    -1,y
        bne     vect30
vect99  dec     <VCTFAD
        puls    a,b,x,y,u,pc

asrd3   asra
        rorb
        asra
        rorb
        asra
        rorb
        rts
asrd7   asra
        rorb
        asra
        rorb
        asra
        rorb
        asra
        rorb
        asra
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

* Source VIEWER scaling tables.
norscl  fcb $c8,$80,$50,$32,$1f,$14,$0c,$08,$04,$02
* VIEWER.ASM defines the half-step tables with BAKSCL at HLFSCL+1.
* Preserve that overlap exactly: HLF range 2 is 100, as in beat 020.
hlfscl  fcb $ff
bakscl  fcb $9c,$64,$41,$28,$1a,$10,$0a,$06,$03,$01

* PIC FWDCRE entries are module-relative offsets. Only type 5 belongs to this
* bounded dependency closure; zero entries are rejected by the entry wrapper.
fwdcre  fdb 0,0,0,0,0,sgint2-fwdcre,0,0,0,0,0,0

rpeek
        fcb     $64,$e4,$fc,$4c,$22,$4e,$44,$00,$fe
sgint2
        fcb     $68,$62,$5e,$7c,$60,$7e,$6a,$64,$ff,$66,$84,$5c,$72,$66,$76,$6e
        fcb     $72,$66,$84,$fc,$02,$56,$56,$17,$ee,$02,$ea,$bb,$bb,$ea,$ea,$00
        fcb     $4e,$5c,$fc,$c2,$51,$3e,$cf,$fc,$42,$13,$00,$6a,$5a,$fc,$1e,$11
        fcb     $f3,$62,$39,$e2,$0c,$e4,$8a,$e2,$00,$56,$54,$fc,$54,$65,$2e,$ca
        fcb     $ba,$a1,$d4,$ee,$12,$d2,$13,$e1,$20,$f6,$24,$72,$58,$ee,$c5,$be
        fcb     $00,$fe

bitmsk  fcb $80,$40,$20,$10,$08,$04,$02,$01
        endsection
