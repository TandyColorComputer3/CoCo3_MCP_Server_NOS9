        section code
        export  dodcmd_entry
_dod_overlay_execute import

* Stable ABI: A=1, B=operation, X=DagOverlayContextV1.  Keep the caller's
* X/Y/U intact.  The C leaf returns 0 in B on success, or an OS-9 error code.
dodcmd_entry
        cmpa    #1
        bne     bad
        cmpb    #1
        beq     call
        cmpb    #2
        bne     bad
call    pshs    x
        pshs    y
        pshs    u
        ldx     4,s
        pshs    x
        lbsr    _dod_overlay_execute
        leas    2,s
        puls    u
        puls    y
        puls    x
        tstb
        bne     error
        clrb
        rts
bad     ldb     #187
error   orcc    #1
        rts

        endsection
