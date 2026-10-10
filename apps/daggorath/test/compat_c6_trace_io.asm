        section code
        export  _compat_trace_create,_compat_trace_close
        import  _stkcheck
i_create equ    $83
i_close equ     $8f

_compat_trace_create
        pshs    u
        lbsr    _stkcheck
        fdb     -64
        leau    ,s
        ldx     4,u                     * ordinary NUL-terminated CMOC path
        lda     #2                      * WRITE
        ldb     #3                      * owner read/write
        os9     i_create
        bcs     cdone
        ldx     6,u
        sta     ,x
        clrb
cdone   puls    u,pc

_compat_trace_close
        pshs    u
        lbsr    _stkcheck
        fdb     -64
        leau    ,s
        lda     5,u
        os9     i_close
        bcs     xdone
        clrb
xdone   puls    u,pc
        endsection
