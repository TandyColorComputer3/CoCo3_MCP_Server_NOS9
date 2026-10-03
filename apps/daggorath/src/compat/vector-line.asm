        section code
        export  _dag_compat_vector_line
        import  _dag_compat_vector_core

* C5 full-view adapter for the original VECTOR implementation retained in
* giant-render.asm. Context: state, frame, x0, y0, x1, y1, fade. Mutable
* VECTOR scratch is in the 256-byte-aligned caller-owned compatibility state.
FLOP    equ $0b
VDGINV  equ $2c
VCTFAD  equ $2d
Y0      equ $2f
X0      equ $31
Y1      equ $33
X1      equ $35

_dag_compat_vector_line
        pshs    cc,dp,x,y,u
        ldx     10,s
        ldd     ,x
        tstb
        bne     line_bad
        tfr     a,dp
        ldd     2,x
        std     <FLOP
        ldd     4,x
        std     <X0
        ldd     6,x
        std     <Y0
        ldd     8,x
        std     <X1
        ldd     10,x
        std     <Y1
        lda     12,x
        sta     <VCTFAD
        clr     <VDGINV
        lbsr    _dag_compat_vector_core
        clrb
        puls    cc,dp,x,y,u,pc
line_bad
        ldb     #187
        puls    cc,dp,x,y,u,pc

        endsection
