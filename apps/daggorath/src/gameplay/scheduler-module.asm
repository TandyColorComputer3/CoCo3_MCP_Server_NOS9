        section code
        export  dodsched_entry
_dodsched_execute import
* A=ABI version, B=operation, X=DagSchedulerContextV1.  As with dodcmd,
* the CMOC callee receives one stack argument and owns no retained context.
dodsched_entry
        cmpa    #1
        bne     bad
* The register operation is part of ABI v1.  Require it to agree with the
* caller-owned context before CMOC sees the context; this prevents a stale or
* mismatched host shim from silently selecting a different operation.
        cmpb    2,x
        bne     bad
        pshs    x
        pshs    y,u
        ldx     4,s
        pshs    x
        lbsr    _dodsched_execute
        leas    2,s
        puls    u
        puls    y
        puls    x,pc
bad
        ldb     #187
        orcc    #1
        rts
        endsection
