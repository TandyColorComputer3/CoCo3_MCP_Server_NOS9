expand_one macro
        ldb     ,x+
        tfr     b,a
        lsra
        lsra
        lsra
        lsra
        lda     a,y
        sta     ,u+
        andb    #$0f
        ldb     b,y
        stb     ,u+
        endm

        section code
        export  _dag_compat_expand_strip

* Expand one original 256x32x1-bpp strip (1024 bytes) into the mapped
* 512x32 type-5 CoWin strip (2048 bytes). Each source bit becomes two adjacent
* physical bits. Stock 6809 only; no GIME/CoWin ownership in this hot leaf.
*
* CMOC ABI after saving X/Y/U: source=8,S, target=10,S. The eight-byte unroll
* amortizes loop/control work while retaining the exact P1 nibble transform.
_dag_compat_expand_strip
        pshs    x,y,u
        ldx     8,s
        ldu     10,s
        leay    doubled,pcr
        lda     #128
        pshs    a
expand_loop
        expand_one
        expand_one
        expand_one
        expand_one
        expand_one
        expand_one
        expand_one
        expand_one
        dec     ,s
        lbne    expand_loop
        leas    1,s
        puls    x,y,u,pc

doubled
        fcb     $00,$03,$0c,$0f,$30,$33,$3c,$3f
        fcb     $c0,$c3,$cc,$cf,$f0,$f3,$fc,$ff
        endsection
